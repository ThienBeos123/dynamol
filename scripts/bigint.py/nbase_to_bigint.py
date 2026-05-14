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

def main():
    input_str = input("Enter the large number string: ").strip()
    
    print("\nSelect the base of the input:")
    print("1. Base 10 (Decimal)")
    print("2. Base 8  (Octal)")
    print("3. Base 2  (Binary)")
    print("4. Base 16 (Hex)")
    
    choice = input("Choice (1-4): ")
    
    bases = {"1": 10, "2": 8, "3": 2, "4": 16}
    base = bases.get(choice)
    
    if not base:
        print("Error: Invalid choice.")
        return

    try:
        # Convert the full string to one massive integer
        big_int_val = int(input_str, base)
    except ValueError:
        print(f"Error: The input string is not valid for base {base}.")
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