def base64_to_bounded_bigint():
    # Custom character set
    BASE64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
    char_to_val = {char: i for i, char in enumerate(BASE64_CHARS)}
    
    try:
        user_input = input("Enter base-64 string: ").strip()
        capacity = int(input("Enter bigint capacity (number of 64-bit limbs): "))
    except (EOFError, ValueError):
        print("Error: Invalid input. Capacity must be an integer.")
        return
    
    raw_len = len(user_input)
    if raw_len > 512 or not all(c in char_to_val for c in user_input):
        print("Error: Invalid string or length exceeds 512.")
        return

    if capacity <= 0:
        print("Error: Capacity must be at least 1.")
        return

    # --- String Metadata ---
    truncated_str = user_input if raw_len <= 24 else f"{user_input[:12]}...{user_input[-12:]}"
    print("\n--- String Metadata ---")
    print(f"Truncated Preview: {truncated_str}")
    print(f"Raw payload len:   {raw_len}")
    print(f"Total buffer len:  {raw_len + 2} (includes '0,' prefix)")
    print(f"Target Capacity:   {capacity} limbs ({capacity * 64} bits)")
    print("-----------------------\n")

    # Step 1: Convert base-64 string to a single large integer
    total_value = 0
    for char in user_input:
        total_value = (total_value << 6) | char_to_val[char]

    # Step 2: Split into base 2^64 limbs
    limbs = []
    bit_mask = (1 << 64) - 1
    
    temp_val = total_value
    # We continue until we run out of value OR we hit the capacity limit
    while temp_val > 0 and len(limbs) < capacity:
        limbs.append(temp_val & bit_mask)
        temp_val >>= 64

    # Handle the case where the input was effectively zero
    if not limbs and raw_len > 0:
        limbs.append(0)

    # Check if truncation occurred
    overflowed = temp_val > 0

    # Step 3: Output Results
    print(f"Limb count: {len(limbs)} / {capacity}")
    if overflowed:
        print("(!) WARNING: Value exceeds capacity. Most significant limbs were truncated.")
    
    for i, val in enumerate(limbs):
        # Threshold Logic
        if val == (1 << 64) - 1:   size, note = "64-bit", " [MAX UINT64]"
        elif val >= (1 << 32):     size, note = "64-bit", ""
        elif val == (1 << 32) - 1: size, note = "32-bit", " [MAX UINT32]"
        elif val >= (1 << 16):     size, note = "32-bit", ""
        elif val == (1 << 16) - 1: size, note = "16-bit", " [MAX UINT16]"
        elif val >= (1 << 8):      size, note = "16-bit", ""
        elif val == (1 << 8) - 1:  size, note = "8-bit",  " [MAX UINT8]"
        else:                      size, note = "8-bit",  ""
        
        print(f"[{i}]: {val} ({size}){note}")

if __name__ == "__main__":
    base64_to_bounded_bigint()