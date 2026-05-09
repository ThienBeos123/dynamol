import secrets

def randomize_and_convert_base64():
    # Define the custom character set
    BASE64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
    char_to_val = {char: i for i, char in enumerate(BASE64_CHARS)}
    
    # Receive desired length
    try:
        input_len = int(input("Enter the desired length for the random base-64 string: "))
    except ValueError:
        print("Error: Please enter a valid integer.")
        return
    
    # Validation
    if input_len > 512:
        print(f"Error: Length {input_len} exceeds the 512 character limit.")
        return
    if input_len <= 0:
        print("Error: Length must be greater than 0.")
        return

    # Generate random base-64 string
    random_str = ''.join(secrets.choice(BASE64_CHARS) for _ in range(input_len))

    # --- Metadata Output ---
    c_style_buffer_len = input_len + 2 + 1  # +2 for "0," and +1 for null terminator
    print("\n--- String Metadata ---")
    print(f"Generated String: {random_str}")
    print(f"Raw payload length: {input_len}")
    print(f"Total buffer length (prefix '0,' + null terminator): {c_style_buffer_len}")
    print("-----------------------\n")

    # Step 1: Convert generated string to a single large integer
    total_value = 0
    for char in random_str:
        total_value = (total_value << 6) | char_to_val[char]

    # Step 2: Split into base 2^64 limbs
    limbs = []
    bit_mask = (1 << 64) - 1
    
    temp_val = total_value
    if temp_val == 0:
        limbs.append(0)
    else:
        while temp_val > 0:
            limbs.append(temp_val & bit_mask)
            temp_val >>= 64

    # Step 3: Output limbs and bit-depth
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

if __name__ == "__main__":
    randomize_and_convert_base64()