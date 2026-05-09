def base64_to_bigint_limbs():
    # Define the custom character set
    BASE64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
    char_to_val = {char: i for i, char in enumerate(BASE64_CHARS)}
    
    # Receive input
    try:
        user_input = input("Enter base-64 string: ").strip()
    except EOFError:
        return
    
    # Validation
    raw_len = len(user_input)
    if raw_len > 512:
        print(f"Error: String length ({raw_len}) exceeds 512 character limit.")
        return
    
    if not all(c in char_to_val for c in user_input):
        print("Error: String contains invalid characters.")
        return

    # --- Length Metadata ---
    # raw_len: The actual base-64 payload
    # +2 for "0," prefix, +1 for '\0'
    c_style_buffer_len = raw_len + 2 + 1 
    
    print("\n------------------ String Metadata ------------------")
    print(f"Raw payload length: {raw_len}")
    print(f"Total buffer length (prefix '0,' + null terminator): {c_style_buffer_len}")
    print("-----------------------------------------------------")

    # Step 1: Convert base-64 string to a single large integer
    # (Operating exclusively on the raw inputted string)
    total_value = 0
    for char in user_input:
        total_value = (total_value << 6) | char_to_val[char]

    # Step 2: Split into base 2^64 limbs
    limbs = []
    bit_mask = (1 << 64) - 1
    
    if total_value == 0:
        if raw_len > 0:
            limbs.append(0)
    else:
        temp_val = total_value
        while temp_val > 0:
            limbs.append(temp_val & bit_mask)
            temp_val >>= 64

    # Step 3: Output Formatting with Bit-Depth detection
    if limbs or raw_len > 0:
        print(f"Limb count: {len(limbs)}")
        for i, value in enumerate(limbs):
            # Determine the smallest standard fit
            if value < (1 << 8):
                bit_size = "8-bit"
            elif value < (1 << 16):
                bit_size = "16-bit"
            elif value < (1 << 32):
                bit_size = "32-bit"
            else:
                bit_size = "64-bit"
            
            print(f"[{i}]: {value} ({bit_size})")
    else:
        print("Limb count: 0")

if __name__ == "__main__":
    base64_to_bigint_limbs()
    print("-----------------------------------------------------\n")
