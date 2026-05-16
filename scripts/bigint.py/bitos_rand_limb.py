import os
import secrets
from pathlib import Path

# Custom Base-64 character set
limb_limit = 18446744073709551615
limb_base = 18446744073709551616
B64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
def int_to_b64(n):
    """Converts a Python integer to a string in custom base-64."""
    if n == 0:
        return B64_CHARS[0]
    
    digits = []
    while n > 0:
        digits.append(B64_CHARS[n % 64])
        n //= 64
    
    return "".join(reversed(digits))


def truncate_string(s):
    """Truncates string to '12 digits...12 digits' if longer than 34 chars."""
    if len(s) <= 34:
        return f'"{s}"'
    return f"{s[:12]}...{s[-12:]}"


def randomize_limb():
    secrets.randbelow(limb_limit)
    return secrets.randbelow(limb_limit)

def thres(n):
    if n <= 255: return "8-bit"
    elif n <= 65535: return "16-bit"
    elif n <= 2147483647: return "32-bit"
    else: return "64-bit"

def main():
    try:
        limb_count = int(input("Enter the number of 2^64 limbs: "))
    except ValueError:
        print("Error: Length must be an integer")
        return
    if limb_count > 48: limb_count = 48

    total_value = 0
    limb_list = []
    for i in range(limb_count):
        curr_val = randomize_limb()
        total_value *= limb_base
        total_value += curr_val
        limb_list.append(curr_val)

    results_map = [
        ("Base-16", hex(total_value)[2:], len(hex(total_value)[2:])),
        ("Base-10", str(total_value), len(str(total_value))),
        ("Base-8", oct(total_value)[2:], len(oct(total_value)[2:])),
        ("Base-2", bin(total_value)[2:], len(bin(total_value)[2:])),
        ("Base-64", int_to_b64(total_value), len(int_to_b64(total_value)))
    ]

    print("\n------------------ BigInt Internal Limbs Output ------------------")
    print(f"{'Index':5} | {'Decimal':<20} | {'Hexadecimal':<18} | {'Bit Threshold':<13}")
    print("-" * 66)
    for limb_index, limb in enumerate(limb_list):
        print(f"{limb_index:5} | {str(limb):<20} | {hex(limb):<18} | {thres(limb):<13}")
    print("\n----------- BigInt String Metadata & Summary ----------")
    print(f"{'Base':10} | {'Digits':<10} | {'Preview'}")
    print("-" * 55)
    
    for label, raw_str, count in results_map:
        preview = truncate_string(raw_str)
        print(f"{label:10} | {count:<10} | {preview}")
    
    print("-" * 55)
    print(f"Results written to: bi_lrand_out.txt\n")

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
        with open(script_dir / "bi_lrand_out.txt", "w") as f:
            f.write(format_c_string("Base-10", results_map[1][1]))
            f.write("\n")
            f.write(format_c_string("Base-16", results_map[0][1]))
            f.write("\n")
            f.write(format_c_string("Base-8",  results_map[2][1]))
            f.write("\n")
            f.write(format_c_string("Base-2",  results_map[3][1]))
            f.write("\n")
            f.write(format_c_string("Base-64", results_map[4][1]))
    except IOError as e:
        print(f"File Error: {e}")

if __name__ == "__main__":
    print("================= BIGINT RANDOM GENERATION =================")
    main()




