import secrets

def randomize_and_convert_base64():
    BASE64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
    
    try:
        input_len = int(input("Enter the desired length: "))
    except ValueError: return
    
    if not (0 < input_len <= 512):
        print("Error: Length must be between 1 and 512.")
        return

    random_str = ''.join(secrets.choice(BASE64_CHARS) for _ in range(input_len))
    truncated = random_str if input_len <= 24 else f"{random_str[:12]}...{random_str[-12:]}"

    print("\n--- String Metadata ---")
    print(f"Full String:      {random_str}")
    print(f"Truncated:        {truncated}")
    print(f"Raw payload len:  {input_len}")
    print(f"Total buffer len: {input_len + 3}")
    print("-----------------------\n")

    char_to_val = {char: i for i, char in enumerate(BASE64_CHARS)}
    total_value = 0
    for char in random_str:
        total_value = (total_value << 6) | char_to_val[char]

    limbs = []
    bit_mask = (1 << 64) - 1
    temp_val = total_value
    while temp_val > 0:
        limbs.append(temp_val & bit_mask)
        temp_val >>= 64
    if not limbs: limbs.append(0)

    print(f"Limb count: {len(limbs)}")
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
    randomize_and_convert_base64()