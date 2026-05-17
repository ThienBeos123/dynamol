import math

# Custom Base-64 character set used as the master template for bases 2 to 64
B64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"

def any_base_to_int(s, base, char_map):
    """Converts a string in an arbitrary base (2-64) into a Python integer."""
    val = 0
    for char in s:
        val = val * base + char_map[char]
    return val

def main():
    try:
        input_str = input("Enter the large number string: ").strip()
    except EOFError:
        return
    
    try:
        base = int(input("Enter the base of the input (2-64): "))
    except ValueError:
        print("Error: Base must be an integer.")
        return

    if not (2 <= base <= 64):
        print("Error: Base must be between 2 and 64.")
        return

    # Slice character set dynamically to match only the allowed digits for the chosen base
    char_map = {char: i for i, char in enumerate(B64_CHARS[:base])}
    
    # Dynamically scale the max allowed string length based on a 48-limb (3072-bit) budget
    # Formula: Max Length = 3072 bits / bits per character in that base
    max_len = int(3072 / math.log2(base))
    raw_len = len(input_str)
    
    if raw_len > max_len or not all(c in char_map for c in input_str):
        print(f"Error: Invalid input for base {base} or length exceeds {max_len} characters.")
        return

    # Metadata printing from base64.py
    truncated = input_str if raw_len <= 24 else f"{input_str[:12]}...{input_str[-12:]}"
    print("\n--- String Metadata ---")
    print(f"Truncated:        {truncated}")
    print(f"Raw payload len:  {raw_len}")
    print(f"Total buffer len: {raw_len + 2} (includes '0,' prefix)")
    print("-----------------------\n")

    # Math conversion using the selected base
    big_int_val = any_base_to_int(input_str, base, char_map)

    # Chunking into 64-bit limbs
    limbs = []
    temp_val = big_int_val
    bit_mask = (1 << 64) - 1
    
    if temp_val == 0 and raw_len > 0:
        limbs.append(0)
    while temp_val > 0:
        limbs.append(temp_val & bit_mask)
        temp_val >>= 64

    # Output with advanced Threshold Detection from base64.py
    print(f"Limb count: {len(limbs)}")
    for i, val in enumerate(limbs):
        if val == (1 << 64) - 1:     size, note = "64-bit", " [MAX UINT64]"
        elif val >= (1 << 32):       size, note = "64-bit", ""
        elif val == (1 << 32) - 1:   size, note = "32-bit", " [MAX UINT32]"
        elif val >= (1 << 16):       size, note = "32-bit", ""
        elif val == (1 << 16) - 1:   size, note = "16-bit", " [MAX UINT16]"
        elif val >= (1 << 8):        size, note = "16-bit", ""
        elif val == (1 << 8) - 1:    size, note = "8-bit",  " [MAX UINT8]"
        else:                        size, note = "8-bit",  ""
        
        print(f"[{i}]: {val} ({size}){note}")
        
    print("-----------------------------------------------------\n")

if __name__ == "__main__":
    main()