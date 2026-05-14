import math
import os
from pathlib import Path

def truncate_string(s):
    """Truncates string to '12 digits...12 digits' if longer than 34 chars."""
    if len(s) <= 34:
        return f'"{s}"'
    return f"{s[:12]}...{s[-12:]}"

def generate_bigint_max_value():
    # --- Input Handling ---
    try:
        num_limbs = int(input("Enter number of limbs (1-48): "))
    except ValueError:
        print("Error: Please enter a valid integer.")
        return

    if not (1 <= num_limbs <= 48):
        print("Error: Limit is 48 limbs.")
        return

    # --- Calculations ---
    bit_count = num_limbs * 64
    max_val = (1 << bit_count) - 1

    # Raw strings for processing
    str_dec = str(max_val)
    str_hex = hex(max_val)[2:]
    str_oct = oct(max_val)[2:]
    str_bin = bin(max_val)[2:]

    # Metadata & Truncated Summary for Terminal
    results_map = [
        ("Base-10", str_dec, len(str_dec)),
        ("Base-16", str_hex, len(str_hex)),
        ("Base-8",  str_oct, len(str_oct)),
        ("Base-2",  str_bin, len(str_bin))
    ]

    print("\n--- BigInt Metadata & Summary ---")
    print(f"{'Base':10} | {'Digits':<10} | {'Preview'}")
    print("-" * 55)
    
    for label, raw_str, count in results_map:
        preview = truncate_string(raw_str)
        print(f"{label:10} | {count:<10} | {preview}")
    
    print("-" * 55)
    print(f"Power of 2: 2^{bit_count} - 1")
    print(f"Results written to: bi_lmax_out.txt\n")

    # --- File Writing & Formatting ---
    def format_c_string(label, data_str):
        length = len(data_str)
        
        # Rule: If total line (label + data) <= 64, inline it
        if len(label) + len(data_str) + 3 <= 64:
            return f"{label}: \"{data_str}\"\n"
        
        # Determine chunk size: < 128 digits -> 32-digit chunks, else 64
        chunk_size = 32 if length < 128 else 64
            
        output = f"{label}:\n"
        for i in range(0, length, chunk_size):
            chunk = data_str[i : i + chunk_size]
            output += f"    \"{chunk}\"\n"
        return output

    try:
        script_dir = Path(__file__).resolve().parent
        with open(script_dir / "bi_lmax_out.txt", "w") as f:
            f.write(format_c_string("Base-10", str_dec))
            f.write("\n")
            f.write(format_c_string("Base-16", str_hex))
            f.write("\n")
            f.write(format_c_string("Base-8",  str_oct))
            f.write("\n")
            f.write(format_c_string("Base-2",  str_bin))
    except IOError as e:
        print(f"File Error: {e}")

if __name__ == "__main__":
    generate_bigint_max_value()