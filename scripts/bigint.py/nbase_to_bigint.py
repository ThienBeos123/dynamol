# Custom Base-64 character set
B64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"

def get_metadata(val):
    """Determines the smallest standard bit-size category for the limb."""
    if val == (2**64 - 1):
        return "(UINT64_MAX)"
    
    # Calculate bits needed
    bit_length = val.bit_length()
    
    if bit_length <= 8:
        return "[8-bit]"
    elif bit_length <= 16:
        return "[16-bit]"
    elif bit_length <= 32:
        return "[32-bit]"
    else:
        return "[64-bit]"

def any_base_to_int(s, base):
    """Converts a string in an arbitrary base (2-64) into a Python integer."""
    val = 0
    # Slice character set to match only the allowed digits for the chosen base
    char_map = {char: i for i, char in enumerate(B64_CHARS[:base])}
    
    for char in s:
        if char not in char_map:
            raise ValueError(f"Character '{char}' is not valid for base {base}.")
        val = val * base + char_map[char]
    return val

def main():
    input_str = input("Enter the large number string: ").strip()
    
    # Arbitrary base selection handling
    try:
        base = int(input("Enter the base of the input (2-64): "))
    except ValueError:
        print("Error: Base must be an integer.")
        return

    if not (2 <= base <= 64):
        print("Error: Base must be between 2 and 64.")
        return

    try:
        # Dynamically parse the big integer value based on user base configuration
        big_int_val = any_base_to_int(input_str, base)
    except ValueError as e:
        print(f"Error: {e}")
        return

    limbs = []
    temp_val = big_int_val
    
    # Extract 64-bit chunks
    mask = (1 << 64) - 1
    
    if temp_val == 0:
        limbs.append(0)
    else:
        while temp_val > 0:
            limb = temp_val & mask
            limbs.append(limb)
            temp_val >>= 64

    # Output section
    print("\n" + "="*40)
    print(f"Total BigInt Length: {len(limbs)} limb(s)")
    print("="*40)
    print("--- Deconstructed Limbs (Bottom to Top) ---")
    
    for i, limb_val in enumerate(limbs):
        # Displaying limb_val as a standard decimal string
        metadata = get_metadata(limb_val)
        print(f"Limb {i}: {limb_val} {metadata}")
    print("="*40)

if __name__ == "__main__":
    main()