#!/usr/bin/python3

import math
import argparse
import numpy as np
from numba import njit
from typing import Tuple
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.colors as mcolors
import sys

# Default HBM Configuration
banks_per_group = 4

# Default Image Configuration
bytes_per_pixel = 4
page_size = 2048
pixels_per_page = page_size // bytes_per_pixel


@njit
def calculate_pixel_values(img_v: int, img_hb: int, super_page_size: int, num_banks: int) -> Tuple[np.ndarray, np.ndarray]:
    """Vectorized calculation of super page IDs and bank IDs for all pixels."""
    total_pixels = img_hb_banks * img_v_banks
    pixels = np.arange(total_pixels, dtype=np.int64)

    sp = pixels // num_banks
    bi = pixels % num_banks

    return sp.reshape(img_v_banks, img_hb_banks), bi.reshape(img_v_banks, img_hb_banks)


def generate_bankgroup_interleaving_matrix_liam(num_bankgroups: int, img_hb: int) -> Tuple[np.ndarray, np.ndarray]:
    """Generate a bankgroup interleaving matrix for LIAM using vectorized operations."""
    return calculate_pixel_values(img_v, img_hb, super_page_size, num_banks)


def generate_bankgroup_interleaving_matrix_bfam(num_bankgroups: int, img_hb: int) -> Tuple[np.ndarray, np.ndarray]:
    """Generate a bankgroup interleaving matrix for BFAM."""
    super_page_ids, bank_ids = calculate_pixel_values(img_v, img_hb, super_page_size, num_banks)

    gspn = super_page_ids // k
    mask = (gspn % 2 == 1)
    bank_ids = np.where(mask, (bank_ids + num_banks // 2) % num_banks, bank_ids)

    return super_page_ids, bank_ids


def generate_bankgroup_interleaving_matrix_bgfam(num_bankgroups: int, img_hb: int) -> Tuple[np.ndarray, np.ndarray]:
    """Generate a bankgroup interleaving matrix for BGFAM."""
    super_page_ids, bank_ids = calculate_pixel_values(img_v, img_hb, super_page_size, num_banks)

    gspn = math.floor(super_page_ids // k)
    bank_ids = (bank_ids + (gspn % num_bankgroups) * banks_per_group) % num_banks

    return super_page_ids, bank_ids


def generate_bankgroup_interleaving_matrix_proposal(num_bankgroups: int, img_hb: int) -> Tuple[np.ndarray, np.ndarray]:
    """Generate a bankgroup interleaving matrix for Proposal (fully vectorized)."""
    super_page_ids, bank_ids = calculate_pixel_values(img_v, img_hb, super_page_size, num_banks)

    image_width_banks           = num_banks * quotient
    image_width_banks_wo_int    = image_width_banks % num_banks
    row_ids                     = (bank_ids + (super_page_ids * num_banks)) // image_width_banks
    page_offset                 = (bank_ids + (super_page_ids * num_banks)) %  image_width_banks

    is_integer_page = (image_width_banks // num_banks) > 0 and (page_offset < ((image_width_banks // num_banks) * num_banks))

    image_height_rows   = page_offset // num_banks

    sum_linear_x4   = (page_offset + row_ids) * 4
    term_bg_page    = (page_offset // num_bankgroups) % num_bankgroups
    term_bg_row     = (row_ids // num_bankgroups) % num_bankgroups

    # Calculate new address
    new_row_partitial   = (row_ids // num_banks) * image_width_banks + page_offset
    new_row_integer     = ((row_ids // num_banks) * image_width_banks_wo_int) + image_height_rows + row_ids * (image_width_banks // num_banks)

    new_row             = np.where(is_integer_page, new_row_integer, new_row_partitial)
    new_bank            = (sum_linear_x4 + term_bg_page + term_bg_row) % num_banks

    return new_row, new_bank

@njit
def qualifying_matrix_optimized(super_page_ids: np.ndarray, bank_ids: np.ndarray,
                                img_v: int, img_hb: int, num_bankgroups: int) -> float:
    """Optimized quality calculation using numba for speed."""
    total_quality = 0.0

    for row in range(img_v_banks):
        for col in range(img_hb_banks):
    #for row in range(1, 5):
        #for col in range(1):
            each_quality = 0

            for per_bg in range(1, num_bankgroups):
                # Right - Different Bank
                if col + per_bg < img_hb_banks and bank_ids[row, col] != bank_ids[row, col + per_bg]:
                    each_quality += 2 if math.floor(bank_ids[row, col] // banks_per_group) != math.floor(bank_ids[row, col + per_bg] // banks_per_group) else 1
                #Right - Same Bank
                if col + per_bg < img_hb_banks and bank_ids[row, col] == bank_ids[row, col + per_bg]:
                    each_quality += 2 if super_page_ids[row, col] == super_page_ids[row, col + per_bg] else -1
                    if super_page_ids[row, col] != super_page_ids[row, col + per_bg]:
                        break
            
            for per_bg in range(1, num_bankgroups):
                # Left - Different Bank
                if col - per_bg >= 0 and bank_ids[row, col] != bank_ids[row, col - per_bg]:
                    each_quality += 2 if math.floor(bank_ids[row, col] // banks_per_group) != math.floor(bank_ids[row, col + per_bg] // banks_per_group) else 1
                # Left - Same Bank
                if col - per_bg >= 0 and bank_ids[row, col] == bank_ids[row, col - per_bg]:
                    each_quality += 2 if super_page_ids[row, col] == super_page_ids[row, col - per_bg] else -1
                    if super_page_ids[row, col] != super_page_ids[row, col - per_bg]:
                        break
            
            for per_bg in range(1, num_bankgroups):
                nr = row + (col + per_bg * img_hb_banks) // img_hb_banks
                nc = (col + per_bg * img_hb_banks) % img_hb_banks
                # Down - Different Bank
                if nr < img_v and bank_ids[row, col] != bank_ids[nr, nc]:
                    each_quality += 2 if math.floor(bank_ids[row, col] // banks_per_group) != math.floor(bank_ids[nr, nc] // banks_per_group) else 1
                # Down - Same Bank
                if nr < img_v and bank_ids[row, col] == bank_ids[nr, nc]:
                    each_quality += 2 if super_page_ids[row, col] == super_page_ids[nr, nc] else -1
                    if super_page_ids[row, col] != super_page_ids[nr, nc]:
                        #print("break down", each_quality)
                        break
            
            for per_bg in range(1, num_bankgroups):
                if col - per_bg * img_hb_banks >= 0:
                    nr = row - (col - per_bg * img_hb_banks) // img_hb_banks
                    nc = (col - per_bg * img_hb_banks) % img_hb_banks
                    # Up - Different Bank
                    if nr >= 0 and bank_ids[row, col] != bank_ids[nr, nc]:
                        each_quality += 2 if math.floor(bank_ids[row, col] // banks_per_group) != math.floor(bank_ids[nr, nc] // banks_per_group) else 1
                    # Up - Same Bank
                    if nr >= 0 and bank_ids[row, col] == bank_ids[nr, nc]:
                        each_quality += 2 if super_page_ids[row, col] == super_page_ids[nr, nc] else -1
                        if super_page_ids[row, col] != super_page_ids[nr, nc]:
                            break

            total_quality += each_quality

    return total_quality / (img_v_banks * img_hb_banks)
    #return total_quality

def check_and_print_overlaps(matrix_tuple: Tuple[np.ndarray, np.ndarray]) -> bool:
    """
    Checks for overlaps and prints the specific (Row, Bank) pairs that collide.
    """
    row_ids, bank_ids = matrix_tuple
    
    # 1. Flatten and cast to integer to ensure clean comparison
    flat_rows = row_ids.flatten().astype(np.int64)
    flat_banks = bank_ids.flatten().astype(np.int64)
    
    # 2. Stack them to create pairs: [[r1, b1], [r2, b2], ...]
    pairs = np.stack((flat_rows, flat_banks), axis=1)
    
    # 3. Find unique pairs and count their occurrences
    # axis=0 looks at the "rows" of our pairs list (the coordinates)
    unique_pairs, counts = np.unique(pairs, axis=0, return_counts=True)
    
    # 4. Identify indices where count is greater than 1
    duplicate_indices = np.where(counts > 1)[0]
    
    if len(duplicate_indices) > 0:
        print(f"\n[!] OVERLAP DETECTED: {len(duplicate_indices)} unique addresses have collisions.")
        print(f"{'Row ID':<10} | {'Bank ID':<10} | {'Count'}")
        print("-" * 35)
        
        # Print the duplicates
        for idx in duplicate_indices:
            row_val = unique_pairs[idx][0]
            bank_val = unique_pairs[idx][1]
            count = counts[idx]
            print(f"{row_val:<10} | {bank_val:<10} | {count}")
            
        return True # Overlaps exist
    else:
        print("\n[OK] No overlaps found. All addresses are unique.")
        return False # No overlaps

import numpy as np
import matplotlib.pyplot as plt
from typing import Tuple

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from typing import Tuple

def map_proposal_to_physical_memory(matrix_tuple: Tuple[np.ndarray, np.ndarray], 
                                    num_banks: int, 
                                    num_rows: int) -> np.ndarray:
    """
    Maps logical (Row, Bank) pairs to physical memory [num_banks, num_rows].
    Includes safety checks to prevent crashes from invalid Bank/Row IDs.
    """
    row_ids_logical, bank_ids_logical = matrix_tuple

    # 1. Initialize Map
    physical_memory_map = np.zeros((num_banks, num_rows), dtype=np.int32)

    # 2. Flatten for iteration
    r_flat = row_ids_logical.flatten().astype(np.int64)
    b_flat = bank_ids_logical.flatten().astype(np.int64)

    # 3. Safety Check: Filter Invalid IDs
    # This prevents the "IndexError" you encountered if the proposal algorithm 
    # generates a Bank ID >= num_banks.
    valid_mask = (r_flat >= 0) & (r_flat < num_rows) & \
                 (b_flat >= 0) & (b_flat < num_banks)
    
    total_pixels = len(r_flat)
    valid_pixels = np.sum(valid_mask)
    
    if valid_pixels < total_pixels:
        print(f"Warning: {total_pixels - valid_pixels} pixels were out of bounds (Invalid Bank or Row ID) and were skipped.")

    # Apply mask
    r_flat = r_flat[valid_mask]
    b_flat = b_flat[valid_mask]

    # 4. Populate Map
    # np.add.at handles collisions correctly by incrementing the counter
    np.add.at(physical_memory_map, (b_flat, r_flat), 1)

    return physical_memory_map

def save_heatmap(physical_map: np.ndarray, filename: str = "heatmap_legend.png"):
    """
    Saves a heatmap with a 3-block legend:
      - Black Square: Empty
      - Green Square: Allocation
      - Red Square: Collision
    """

    # Set the Font to Times New Roman
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    num_banks, num_rows = physical_map.shape
    
    # --- 1. Data Preparation ---
    # We create a display copy where all collision counts (>1) are clamped to exactly 2.
    # This creates 3 discrete states: 0, 1, and 2.
    display_map = np.clip(physical_map, 0, 2)
    
    # --- 2. Define Colors ---
    # 0 -> Black, 1 -> Lime, 2 -> Red
    colors = ['black', 'lime', 'red']
    cmap = mcolors.ListedColormap(colors)

    # --- 3. Plotting ---
    plt.figure(figsize=(12, 12))
    
    # Plot the clamped map. vmin=0, vmax=2 ensures strict mapping to our 3 colors.
    plt.imshow(display_map.T, cmap=cmap, vmin=0, vmax=2, aspect='auto', interpolation='nearest')
    
    # --- 4. Decoration ---
    plt.suptitle(f"Physical Memory Layout\nBanks: {num_banks}, Image Size: {img_hb}x{img_v}, Quotient: {quotient}", fontsize=14)
    plt.xlabel(f"Bank ID (0 - {num_banks-1})", fontsize=12)
    plt.ylabel(f"Row ID (0 - {num_rows-1})", fontsize=12)
    plt.gca().invert_yaxis() # Row 0 at top

    # --- 5. Custom Legend (The "3 Square Block") ---
    # We create manual handles for the legend
    legend_elements = [
        mpatches.Patch(color='black', label='Empty (0)'),
        mpatches.Patch(color='lime',  label='Allocation (1)'),
        mpatches.Patch(color='red',   label='Collision (≥2)')
    ]
    
    # Place legend outside the plot area (bbox_to_anchor) so it doesn't cover data
    plt.legend(handles=legend_elements, loc='upper left', bbox_to_anchor=(1.02, 1), 
               title="Status", fontsize=12, title_fontsize=14, frameon=True)
    
    # Adjust layout to make room for the side legend
    plt.tight_layout()

    # --- 6. Save ---
    print(f"Saving heatmap with legend to '{filename}'...")
    plt.savefig(filename, dpi=150)
    plt.close()

# --- Example Usage in final_result ---
# def final_result():
#     # ... (Generate Proposal) ...
#     
#     # 1. Map to Physical Grid (with safety check)
#     phy_map = map_proposal_to_physical_memory(proposal_output, num_banks, num_rows=2**15)
#
#     # 2. Save Discrete Heatmap
#     save_optimized_heatmap(phy_map, "proposal_result_discrete.png")

def qualifying_matrix(matrix_tuple: Tuple[np.ndarray, np.ndarray]) -> float:
    super_page_ids, bank_ids = matrix_tuple
    return qualifying_matrix_optimized(super_page_ids, bank_ids, img_v, img_hb, num_bankgroups)

def print_matrix(matrix_tuple: Tuple[np.ndarray, np.ndarray]):
    super_page_ids, bank_ids = matrix_tuple
    print("[Super Page ID, Bank ID]")
    rows, cols = [img_v_banks, img_hb_banks]
    for i in range(rows):
        row_data = [[int(super_page_ids[i, j]), int(bank_ids[i, j])] for j in range(cols)]
        print(row_data, ("..." if cols < img_hb else ""))
    if rows < img_v:
        print("...")
    print("\n")


def final_result():
    key = f"{num_banks}_{img_hb}_{img_v}"
    schemes = [
        ("LIAM", generate_bankgroup_interleaving_matrix_liam),
        ("BFAM", generate_bankgroup_interleaving_matrix_bfam),
        ("BGFAM", generate_bankgroup_interleaving_matrix_bgfam),
        ("Proposal", generate_bankgroup_interleaving_matrix_proposal),
    ]

    #print_matrix(generate_bankgroup_interleaving_matrix_liam(num_bankgroups, img_hb))
    #print_matrix(generate_bankgroup_interleaving_matrix_bfam(num_bankgroups, img_hb))
    #print_matrix(generate_bankgroup_interleaving_matrix_bgfam(num_bankgroups, img_hb))
    print_matrix(generate_bankgroup_interleaving_matrix_proposal(num_bankgroups, img_hb))

    # Check for Overlaps
    check_and_print_overlaps(generate_bankgroup_interleaving_matrix_proposal(num_bankgroups, img_hb))

    # Check for HeatMap
    phy_map = map_proposal_to_physical_memory(generate_bankgroup_interleaving_matrix_proposal(num_bankgroups, img_hb), num_banks, num_rows=img_v*2)
    save_heatmap(phy_map, filename=f"./heatmap/heatmap_{key}.png")

    #print("=" * 85)
    #for name, fn in schemes:
    #    q = qualifying_matrix(fn(num_bankgroups, img_hb))
    #    print(f"{key} {name}:\t {q}")
    #print("=" * 85 + "\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Script qualifying interleaving quality.")
    parser.add_argument("--num_banks", required=True, type=int)
    parser.add_argument("--img_hb",    required=True, type=int)
    parser.add_argument("--img_v",     required=True, type=int)
    args = parser.parse_args()

    if not (16 <= args.num_banks <= 64):
        print("Error: The number of banks must be from 16 to 64.")
        exit(1)

    num_banks = args.num_banks
    img_hb     = args.img_hb
    img_v      = args.img_v

    img_hb_banks = 1 if (math.floor(img_hb // pixels_per_page) == 0) else math.floor(img_hb // pixels_per_page)
    img_v_banks  = img_v
    quotient     = (img_hb * 4.0)    / (num_banks * page_size)

    num_bankgroups   = num_banks // banks_per_group
    super_page_size  = page_size * num_banks
    k                = 1 if (round((img_hb * bytes_per_pixel) / super_page_size) == 0) else round((img_hb * bytes_per_pixel) / super_page_size)
    print(f"num_banks: {num_banks}, img_hb: {img_hb}, img_v: {img_v}, num_bankgroups: {num_bankgroups}, super_page_size: {super_page_size}, k: {k}")

    final_result()
