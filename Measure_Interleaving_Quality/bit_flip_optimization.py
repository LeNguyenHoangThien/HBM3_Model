#!/usr/bin/python3

import math
import argparse
import numpy as np
from itertools import combinations
from numba import njit, prange

# --- Configuration ---
banks_per_group = 4
col_num = 32
row_bits = 13
col_bits = 5
bank_bits = 0
bytes_per_pixel = 4
page_size = 2048

# ==========================================
# 1. Trace Generation (Vectorized & Memory Optimized)
# ==========================================

def calculate_pixel_values(img_v: int, img_hb: int, num_banks: int):
    total_pixels = img_hb * img_v
    pixels = np.arange(total_pixels, dtype=np.uint32)
    
    div_col = pixels // col_num
    ri = (div_col // num_banks).astype(np.uint16)
    bi = (div_col % num_banks).astype(np.uint8)
    ci = (pixels % col_num).astype(np.uint8)
    
    return ri, bi, ci

def get_flat_trace(ri, bi, ci, img_v, img_hb, include_bank_col=False, row_major=False, col_major=False):
    if row_major:
        # 1. Row-Major Trace
        r = ri
        b = bi
        c = ci

    if col_major:
        # Generate Column-Major indices
        r_p = ri.reshape(img_v, img_hb).flatten(order='F')
        b_p = bi.reshape(img_v, img_hb).flatten(order='F')
        c_p = ci.reshape(img_v, img_hb).flatten(order='F')
        
        r = np.concatenate((r, r_p)) if row_major else r_p
        b = np.concatenate((b, b_p)) if row_major else b_p
        c = np.concatenate((c, c_p)) if row_major else c_p

    if not include_bank_col:
        return r.astype(np.uint32)

    # Reconstruct Physical Address
    shift_bank = col_bits
    shift_row = int(math.log2(max(b) + 1)) + col_bits 
    if shift_row < (bank_bits + col_bits): 
        shift_row = bank_bits + col_bits 

    full_address = (r.astype(np.uint32) << shift_row) | \
                   (b.astype(np.uint32) << shift_bank) | \
                   c.astype(np.uint32)
                   
    return full_address

def get_diff_histogram(trace):
    diffs = trace[:-1] ^ trace[1:]
    diffs = diffs[diffs != 0]
    return np.unique(diffs, return_counts=True)

# ==========================================
# 2. Fast Numba Kernels
# ==========================================

@njit(fastmath=True)
def calc_union_size_numba(unique_diffs, counts, mask):
    total = 0
    for i in range(len(unique_diffs)):
        if (unique_diffs[i] & mask) != 0:
            total += counts[i]
    return total

@njit(fastmath=True)
def _calc_block_score(block):
    """
    Helper: Sorts and counts uniques in a block.
    Extracted to prevent Numba 'unexpected cycle' errors.
    """
    block.sort()
    length = len(block)
    if length == 0:
        return 0
    unique_count = 1
    for j in range(1, length):
        if block[j] != block[j-1]:
            unique_count += 1
    return length - unique_count

@njit(parallel=True, fastmath=True)
def find_best_mask_batch(trace, masks, window_size):
    """
    Parallel Batch Solver:
    Evaluates ALL masks in parallel. Each thread takes a mask 
    and scans the trace to compute its score.
    """
    n_masks = len(masks)
    n_trace = len(trace)
    num_blocks = n_trace // window_size
    
    if num_blocks == 0:
        return 0, 0
    
    scores = np.zeros(n_masks, dtype=np.uint64)
    
    # Parallelize over the MASKS (Search Space)
    for m in prange(n_masks):
        mask = masks[m]
        total_score = 0
        
        # Scan trace sequentially for this mask
        # (This is safe inside a parallel loop)
        for i in range(num_blocks):
            start = i * window_size
            end = start + window_size
            
            # Extract and mask
            block = trace[start:end] & mask
            
            # Calculate score
            total_score += _calc_block_score(block)
            
        scores[m] = total_score
        
    # Find best result
    best_idx = 0
    min_score = scores[0]
    for m in range(1, n_masks):
        if scores[m] < min_score:
            min_score = scores[m]
            best_idx = m
            
    return best_idx, min_score

# ==========================================
# 3. Solvers
# ==========================================

def solve_min_k_union(unique_diffs, counts, num_bits, k):
    min_size = float('inf')
    best_combo = None
    bit_masks = np.array([1 << i for i in range(num_bits)], dtype=np.uint32)
    
    for combo in combinations(range(num_bits), k):
        mask = 0
        for bit in combo:
            mask |= bit_masks[bit]
        current_size = calc_union_size_numba(unique_diffs, counts, mask)
        if current_size < min_size:
            min_size = current_size
            best_combo = combo
    return best_combo, min_size

def get_high_entropy_bits(unique_diffs, counts, total_bits):
    entropy = []
    for bit in range(total_bits):
        mask = 1 << bit
        flips = calc_union_size_numba(unique_diffs, counts, mask)
        entropy.append((bit, flips))
    entropy.sort(key=lambda x: x[1], reverse=True)
    return [x[0] for x in entropy]

def solve_near_optimal_mapping(trace, num_bg, num_bk, num_col, total_width):
    n_total = num_bg + num_bk + num_col
    
    # 1. Entropy Pre-calc
    udiffs, ucounts = get_diff_histogram(trace)
    
    #print(f"Selecting top {n_total} high-entropy bits...")
    pool = get_high_entropy_bits(udiffs, ucounts, total_width)[:n_total]
    remaining = set(pool)
    mapping = {}
    
    # 2. Assign Columns (Highest Entropy)
    #print("Assigning Columns (Max Entropy)...")
    col_bits = [b for b in pool if b in remaining][:num_col]
    mapping['Column'] = col_bits
    remaining -= set(col_bits)
    
    # 3. Assign Bank Groups (Min Repetitive) - PARALLELIZED
    if num_bg > 0:
        #print("Optimizing Bank Groups (Min Repetitive)...")
        bg_win = 1 << num_bg
        
        # A. Generate ALL combinations
        combos = list(combinations(remaining, num_bg))
        masks_list = []
        for combo in combos:
            mask = 0
            for b in combo: mask |= (1 << b)
            masks_list.append(mask)
        masks_array = np.array(masks_list, dtype=np.uint32)
        
        # B. Solve all in parallel
        best_idx, _ = find_best_mask_batch(trace, masks_array, bg_win)
        
        best_combo = combos[best_idx]
        mapping['BankGroup'] = list(best_combo)
        remaining -= set(best_combo)
    else:
        mapping['BankGroup'] = []
        
    # 4. Assign Banks (Min Repetitive) - PARALLELIZED
    if num_bk > 0:
        #print("Optimizing Banks (Min Repetitive)...")
        bk_win = 1 << num_bk
        
        # A. Generate ALL combinations (from remaining bits)
        combos = list(combinations(remaining, num_bk))
        masks_list = []
        for combo in combos:
            mask = 0
            for b in combo: mask |= (1 << b)
            masks_list.append(mask)
        masks_array = np.array(masks_list, dtype=np.uint32)
        
        # B. Solve all in parallel
        best_idx, _ = find_best_mask_batch(trace, masks_array, bk_win)
        
        best_combo = combos[best_idx]
        mapping['Bank'] = list(best_combo)
        remaining -= set(best_combo)
    else:
        mapping['Bank'] = []
    
    # 5. Assign Rows
    all_bits = set(range(total_width))
    mapped_bits = set(mapping['Column'] + mapping['BankGroup'] + mapping['Bank'])
    mapping['Row'] = list(all_bits - mapped_bits)
        
    return mapping

# ==========================================
# Main
# ==========================================

def final_result(args):
    num_banks   = args.num_banks
    img_hb      = args.img_hb
    img_v       = args.img_v
    col_major   = args.col_major
    row_major   = args.row_major
    
    ri, bi, ci = calculate_pixel_values(img_v, img_hb, num_banks)
    
    # Min-K-Union
    #print("--- Solving Min-K-Union ---")
    row_trace = get_flat_trace(ri, bi, ci, img_v, img_hb, include_bank_col=False, row_major=row_major, col_major=col_major)
    udiffs, ucounts = get_diff_histogram(row_trace)
    best_bits, misses = solve_min_k_union(udiffs, ucounts, row_bits, row_bits)
    print(f"Min-K-Union:\t\tRow Bits={best_bits} - Union-K={len(best_bits)} - Misses={misses}")
    
    # Near-Optimal Mapping
    #print("--- Solving Near-Optimal Mapping ---")
    full_trace = get_flat_trace(ri, bi, ci, img_v, img_hb, include_bank_col=True, row_major=row_major, col_major=col_major)
    
    total_bank_bits = int(math.log2(num_banks))
    n_bg = max(0, total_bank_bits - 4)
    n_bk = total_bank_bits - n_bg
    
    mapping = solve_near_optimal_mapping(full_trace, 
                                         num_bg=n_bg, 
                                         num_bk=n_bk, 
                                         num_col=col_bits, 
                                         total_width=(row_bits + col_bits + total_bank_bits))
    print(f"Near-Optimal:\t\tCol={mapping['Column']} - BankGroup={mapping['BankGroup']} - Bank={mapping['Bank']} - Row={mapping['Row']}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--num_banks", required=True, type=int)
    parser.add_argument("--img_hb",    required=True, type=int)
    parser.add_argument("--img_v",     required=True, type=int)
    parser.add_argument("--col_major", action="store_true", default=False)
    parser.add_argument("--row_major", action="store_true", default=False)
    args = parser.parse_args()
    
    print("=" * 150)
    print(f"Column-Wise: {args.col_major} + Row-Wise: {args.row_major} - num_banks: {args.num_banks}, img_hb: {args.img_hb}, img_v: {args.img_v}")
    final_result(args)