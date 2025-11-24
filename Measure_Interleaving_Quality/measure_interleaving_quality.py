#!/usr/bin/python3

import math
import argparse
import numpy as np
from numba import njit
from typing import Tuple

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
    pixels = np.arange(total_pixels, dtype=np.int16)

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


#def generate_bankgroup_interleaving_matrix_proposal(num_bankgroups: int, img_hb: int) -> Tuple[np.ndarray, np.ndarray]:
#    """Generate a bankgroup interleaving matrix for Proposal (fully vectorized)."""
#    super_page_ids, bank_ids = calculate_pixel_values(img_v, img_hb, super_page_size, num_banks)
#
#    # Vectorize successive row-wise shifts: row i = row0 shifted left by i
#    row0 = bank_ids[0].copy()
#    row_indices = np.arange(img_v, dtype=np.int32)[:, None]
#    col_indices = np.arange(img_hb, dtype=np.int32)[None, :]
#    bank_ids = row0[(col_indices + row_indices) % img_hb]
#
#    return super_page_ids, bank_ids


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


def qualifying_matrix(matrix_tuple: Tuple[np.ndarray, np.ndarray]) -> float:
    super_page_ids, bank_ids = matrix_tuple
    return qualifying_matrix_optimized(super_page_ids, bank_ids, img_v, img_hb, num_bankgroups)


def print_matrix(matrix_tuple: Tuple[np.ndarray, np.ndarray]):
    super_page_ids, bank_ids = matrix_tuple
    print("[Super Page ID, Bank ID]")
    rows, cols = min(20, 20), min(5, 5)
    for i in range(rows):
        row_data = [[int(super_page_ids[i, j]), int(bank_ids[i, j])] for j in range(cols)]
        print(row_data, ("..." if cols < img_hb else ""))
    if rows < img_v:
        print("...")
    print("\n\n\n")


def final_result():
    key = f"{num_banks}_{img_hb}_{img_v}"
    schemes = [
        ("LIAM", generate_bankgroup_interleaving_matrix_liam),
        ("BFAM", generate_bankgroup_interleaving_matrix_bfam),
        ("BGFAM", generate_bankgroup_interleaving_matrix_bgfam),
        #("Proposal", generate_bankgroup_interleaving_matrix_proposal),
    ]

    #print_matrix(generate_bankgroup_interleaving_matrix_liam(num_bankgroups, img_hb))
    #print_matrix(generate_bankgroup_interleaving_matrix_bfam(num_bankgroups, img_hb))
    #print_matrix(generate_bankgroup_interleaving_matrix_bgfam(num_bankgroups, img_hb))


    print("=" * 85)
    for name, fn in schemes:
        q = qualifying_matrix(fn(num_bankgroups, img_hb))
        print(f"{key} {name}:\t {q}")
    print("=" * 85 + "\n")


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

    num_bankgroups   = num_banks // banks_per_group
    super_page_size  = page_size * num_banks
    k                = 1 if (round((img_hb * bytes_per_pixel) / super_page_size) == 0) else round((img_hb * bytes_per_pixel) / super_page_size)
    print(f"num_banks: {num_banks}, img_hb: {img_hb}, img_v: {img_v}, num_bankgroups: {num_bankgroups}, super_page_size: {super_page_size}, k: {k}")

    final_result()
