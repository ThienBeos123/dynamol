import math
import sys

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

    # Metadata for Terminal
    metadata = {
        "Power of 2": f"2^{bit_count} - 1",
        "Dec Digits": len(str(max_val)),
        "Hex Digits": bit_count // 4,
        "Oct Digits": math.ceil(bit_count / 3),
        "Bin Digits": bit_count
    }

    print("\n--- BigInt Metadata ---")
    for key, value in metadata.items():
        print(f"{key:15}: {value}")
    print("-----------------------\n")
    print(f"Results written to: bi_lmax_out.txt")

    # --- File Writing & Formatting ---
    def format_c_string(label, data_str):
        length = len(data_str)
        
        # Rule: If total line (label + data) <= 64, inline it
        # Otherwise, move to new line and chunk
        if len(label) + len(data_str) + 3 <= 64:
            return f"{label}: \"{data_str}\"\n"
        
        # Determine chunk size
        # Rule: < 128 digits -> 32-digit chunks. Else, try to fit 64 if divisible.
        if length < 128:
            chunk_size = 32
        else:
            # Prefer 64, but if 64 isn't trivial and 32 is better, we could adjust.
            # Here we follow the 64-digit part request for longer strings.
            chunk_size = 64 if length % 64 == 0 or length > 256 else 32
            
        output = f"{label}:\n"
        for i in range(0, length, chunk_size):
            chunk = data_str[i : i + chunk_size]
            output += f"    \"{chunk}\"\n"
        return output

    try:
        with open("bitos.py/bi_lmax_out.txt", "w") as f:
            # Base-10 (Decimal)
            f.write(format_c_string("Base-10", str(max_val)))
            f.write("\n")
            
            # Base-16 (Hex) - Lowercase to follow standard hex formatting
            f.write(format_c_string("Base-16", hex(max_val)[2:]))
            f.write("\n")
            
            # Base-8 (Octal)
            f.write(format_c_string("Base-8", oct(max_val)[2:]))
            f.write("\n")
            
            # Base-2 (Binary)
            f.write(format_c_string("Base-2", bin(max_val)[2:]))
    except IOError as e:
        print(f"File Error: {e}")

if __name__ == "__main__":
    generate_bigint_max_value()