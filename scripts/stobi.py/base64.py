def base64_to_bigint_limbs():
    BASE64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"
    char_to_val = {char: i for i, char in enumerate(BASE64_CHARS)}
    
    try:
        user_input = input("Enter base-64 string: ").strip()
    except EOFError: return
    
    raw_len = len(user_input)
    if raw_len > 512 or not all(c in char_to_val for c in user_input):
        print("Error: Invalid input or length exceeds 512.")
        return

    # Metadata
    truncated = user_input if raw_len <= 24 else f"{user_input[:12]}...{user_input[-12:]}"
    print("\n--- String Metadata ---")
    print(f"Truncated:        {truncated}")
    print(f"Raw payload len:  {raw_len}")
    print(f"Total buffer len: {raw_len + 2} (includes '0,' prefix)")
    print("-----------------------\n")

    # Math
    total_value = 0
    for char in user_input:
        total_value = (total_value << 6) | char_to_val[char]

    limbs = []
    bit_mask = (1 << 64) - 1
    temp_val = total_value
    if temp_val == 0 and raw_len > 0: limbs.append(0)
    while temp_val > 0:
        limbs.append(temp_val & bit_mask)
        temp_val >>= 64

    # Output with Threshold Detection
    print(f"Limb count: {len(limbs)}")
    for i, val in enumerate(limbs):
        if val == (1 << 64) - 1: size, note = "64-bit", " [MAX UINT64]"
        elif val >= (1 << 32):   size, note = "64-bit", ""
        elif val == (1 << 32) - 1: size, note = "32-bit", " [MAX UINT32]"
        elif val >= (1 << 16):   size, note = "32-bit", ""
        elif val == (1 << 16) - 1: size, note = "16-bit", " [MAX UINT16]"
        elif val >= (1 << 8):    size, note = "16-bit", ""
        elif val == (1 << 8) - 1:  size, note = "8-bit",  " [MAX UINT8]"
        else:                      size, note = "8-bit",  ""
        
        print(f"[{i}]: {val} ({size}){note}")

if __name__ == "__main__":
    base64_to_bigint_limbs()
    print("-----------------------------------------------------\n")
