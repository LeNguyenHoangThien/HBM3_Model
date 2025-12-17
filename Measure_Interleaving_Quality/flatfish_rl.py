import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.distributions import Categorical
import math
import argparse

# --- IMPORT FROM YOUR EXISTING FILE ---
from measure_interleaving_quality import (
    qualifying_matrix_optimized, 
    calculate_pixel_values,
    # Import globals if available, otherwise we define defaults below
)

super_page_size = 2048
col_bits = 5
row_bits = 14

# ==========================================
# 1. The Flatfish Environment
# ==========================================
class FlatfishEnvironment:
    def __init__(self, num_banks, img_hb, img_v, banks_per_group=4):
        self.num_banks = num_banks
        self.img_hb = img_hb
        self.img_v = img_v
        self.banks_per_group = banks_per_group
        self.num_bankgroups = num_banks // banks_per_group
        
        # --- 1. Generate Base Address from LIAM ---
        # This is the "Input Vector" for your BIM multiplication.
        print("Generating Base LIAM Addresses...")
        liam_ri, liam_bi, liam_ci = calculate_pixel_values(
            self.img_v, self.img_hb, super_page_size, self.num_banks
        )
        
        # LIAM returns transactions (bursts), usually 1/16th of total pixels
        self.num_transactions = len(liam_ri)

        # --- 2. Define Bit Widths ---
        # We need to construct the full address vector: [Row | Bank | Col]
        self.col_bits = col_bits # Usually 5
        self.bank_bits = int(math.ceil(math.log2(num_banks))) # e.g., 4 bits for 16 banks, 5 for 32
        
        # Calculate row bits needed to cover the generated row IDs
        max_row = np.max(liam_ri) if len(liam_ri) > 0 else 0
        self.row_bits = max(row_bits, int(math.ceil(math.log2(max_row + 1))))
        
        self.addr_bits = self.row_bits + self.bank_bits + self.col_bits
        
        print(f"DEBUG: Address Vector Structure -> Row:{self.row_bits} | Bank:{self.bank_bits} | Col:{self.col_bits}")
        print(f"DEBUG: BIM Size: {self.addr_bits}x{self.addr_bits}")

        # --- 3. Pack LIAM into Linear Integers ---
        # Address = (Row << (Bank+Col)) | (Bank << Col) | Col
        self.bank_shift = self.col_bits
        self.row_shift = self.col_bits + self.bank_bits
        
        self.liam_linear_addresses = (liam_ri.astype(np.int64) << self.row_shift) | \
                                     (liam_bi.astype(np.int64) << self.bank_shift) | \
                                     (liam_ci.astype(np.int64))

        # --- 4. Convert to Bit Matrix for BIM Multiplication ---
        # Shape: (addr_bits, num_transactions)
        # This represents the "LIAM's address" in bits, ready to be multiplied.
        self.addr_bit_matrix = np.zeros((self.addr_bits, self.num_transactions), dtype=np.int8)
        for i in range(self.addr_bits):
            self.addr_bit_matrix[i, :] = (self.liam_linear_addresses >> i) & 1

        # Initialize BIM (Identity Matrix)
        self.bim = np.eye(self.addr_bits, dtype=np.int8)

    def step(self, row_idx_a, row_idx_b):
        """Action: Switch two rows in the BIM."""
        self.bim[[row_idx_a, row_idx_b]] = self.bim[[row_idx_b, row_idx_a]]
        reward = self.calculate_reward()
        return self.bim.copy(), reward

    def calculate_reward(self):
        """
        1. Multiply BIM * LIAM_Address_Bits = New_Address_Bits
        2. Decode New_Address_Bits -> New LIAM (Row, Bank, Col)
        3. Expand and Score
        """
        # --- A. The Core Operation: BIM * LIAM ---
        # new_bits = BIM @ old_bits (modulo 2)
        new_bits = np.dot(self.bim, self.addr_bit_matrix) % 2
        
        # --- B. Reconstruct Integer Addresses ---
        powers_of_two = (1 << np.arange(self.addr_bits)).astype(np.int64)
        new_linear_addresses = np.dot(powers_of_two, new_bits)
        
        # --- C. Decode back to Row / Bank / Col ---
        # Mask for Column (lower bits)
        col_mask = (1 << self.col_bits) - 1
        # Mask for Bank (middle bits)
        bank_mask = (1 << self.bank_bits) - 1
        
        # Extract components
        # new_ci = new_linear_addresses & col_mask  # We don't need Col for scoring
        new_bi = (new_linear_addresses >> self.bank_shift) & bank_mask
        new_ri = (new_linear_addresses >> self.row_shift)
        
        # --- D. Expand to Pixels ---
        # LIAM gives 1 address per 16 pixels (transaction granularity).
        # We must repeat these 16 times to fill the pixel grid for the scorer.
        expanded_ri = np.repeat(new_ri, 16)
        expanded_bi = np.repeat(new_bi, 16)
        
        # --- E. Reshape & Score ---
        # Ensure exact match with image size
        total_required = self.img_v * self.img_hb
        
        # Safety truncate or pad if sizes mismatch slightly due to integer division
        if len(expanded_ri) > total_required:
            expanded_ri = expanded_ri[:total_required]
            expanded_bi = expanded_bi[:total_required]
        elif len(expanded_ri) < total_required:
            # Pad with zeros if short (unlikely with correct math)
            pad_len = total_required - len(expanded_ri)
            expanded_ri = np.pad(expanded_ri, (0, pad_len))
            expanded_bi = np.pad(expanded_bi, (0, pad_len))

        try:
            sp_grid = expanded_ri.reshape(self.img_v, self.img_hb)
            bi_grid = expanded_bi.reshape(self.img_v, self.img_hb)
            
            score = qualifying_matrix_optimized(
                sp_grid, 
                bi_grid, 
                self.img_v, 
                self.img_hb, 
                self.num_bankgroups,
                self.banks_per_group
            )
            return score
        except ValueError:
            return -1.0 # Penalize invalid shapes

# ==========================================
# 2. The Policy Network
# ==========================================
class PolicyNetwork(nn.Module):
    def __init__(self, input_size, action_size):
        super(PolicyNetwork, self).__init__()
        self.fc1 = nn.Linear(input_size, 128)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(128, action_size)
        self.softmax = nn.Softmax(dim=1)

    def forward(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        return self.softmax(x)

# ==========================================
# 3. Training Loop
# ==========================================
def train_flatfish(args):
    env = FlatfishEnvironment(args.num_banks, args.img_hb, args.img_v)
    
    input_size = env.addr_bits * env.addr_bits
    action_size = env.addr_bits 
    policy = PolicyNetwork(input_size, action_size)
    optimizer = optim.Adam(policy.parameters(), lr=1e-3)
    
    num_episodes = 200 
    best_reward = -float('inf')
    best_bim = env.bim.copy()

    print(f"--- Starting Optimization ---")
    print(f"Image: {args.img_v}x{args.img_hb}")
    print(f"Initial Reward (LIAM Base): {env.calculate_reward():.5f}")

    for episode in range(num_episodes):
        state = torch.FloatTensor(env.bim.flatten()).unsqueeze(0)
        
        probs = policy(state)
        m = Categorical(probs)
        action = m.sample()
        target_row = action.item()
        
        new_bim, reward = env.step(0, target_row)
        
        if reward > best_reward:
            best_reward = reward
            best_bim = new_bim.copy()
            print(f"Ep {episode}: New Best: {best_reward:.5f} (Swap 0 <-> {target_row})")

        loss = -m.log_prob(action) * reward
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

    print("--- Done ---")
    return best_bim

def analyze_bim_mapping(bim, row_bits, bank_bits, col_bits):
    """
    Analyzes the BIM to show exactly how LIAM bits are mapped.
    """
    total_bits = bim.shape[0]
    
    # helper to name the bits based on your architecture
    def get_bit_name(idx):
        # Packing order was: [ROW | BANK | COL]
        # Ranges determined by the shifts in FlatfishEnvironment
        # COL: 0 to col_bits-1
        # BANK: col_bits to col_bits+bank_bits-1
        # ROW: col_bits+bank_bits to total_bits-1
        
        if 0 <= idx < col_bits:
            return f"Col[{idx}]"
        elif col_bits <= idx < col_bits + bank_bits:
            return f"Bank[{idx - col_bits}]"
        else:
            return f"Row[{idx - (col_bits + bank_bits)}]"

    print("\n" + "="*40)
    print("      FINAL BIT MAPPING (NEW ORDER)      ")
    print("="*40)
    print(f"BIM Shape: {total_bits}x{total_bits}")
    
    # Iterate over every Output Bit (Row of BIM)
    for i in range(total_bits):
        # Find which input bits contribute to this output bit
        input_indices = np.where(bim[i] == 1)[0]
        
        # Format the XOR string
        formula = " XOR ".join([get_bit_name(idx) for idx in input_indices])
        
        new_name = get_bit_name(i) # The position in the new address
        print(f"New {new_name:<10} <==  {formula}")
    print("="*40 + "\n")

def get_bim_order_list(bim):
    """
    Extracts the permutation order from the BIM.
    Returns a list where list[i] is the index of the old bit mapped to new bit i.
    """
    order_list = []
    num_rows = bim.shape[0]
    
    for r in range(num_rows):
        # Find the column index where the value is 1
        # Since this is a permutation matrix, there should be exactly one '1' per row.
        indices = np.where(bim[r] == 1)[0]
        
        if len(indices) == 1:
            order_list.append(int(indices[0]))
        elif len(indices) > 1:
            # If you later enable XOR logic (multiple 1s), this handles it roughly
            # by listing them as a tuple, but for now we assume integer.
            order_list.append(tuple(indices))
        else:
            order_list.append(None) # Should not happen in a valid BIM
            
    return order_list[::-1]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--num_banks", type=int, default=32)
    parser.add_argument("--img_hb", type=int, default=1024)
    parser.add_argument("--img_v", type=int, default=256)
    args = parser.parse_args()

    # Run Training
    optimized_bim = train_flatfish(args)
    print("\nOptimized BIM:")
    print(optimized_bim)
        
    # Recalculate bit widths exactly as Env does
    liam_ri, _, _ = calculate_pixel_values(args.img_v, args.img_hb, super_page_size, args.num_banks)
    c_bits = col_bits
    b_bits = int(math.ceil(math.log2(args.num_banks)))
    r_bits = row_bits
    
    # Print the Analysis
    analyze_bim_mapping(optimized_bim, r_bits, b_bits, c_bits)

    order = get_bim_order_list(optimized_bim)
    print(f"New Order List - Bank Number = {args.num_banks}, Image Width = {args.img_hb} and Image Height = {args.img_v}: {order}")