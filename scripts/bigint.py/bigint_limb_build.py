import os
from pathlib import Path

def truncate_string(s):
    """Truncates string to '12 digits...12 digits' if longer than 34 chars."""
    if len(s) <= 34:
        return s
    return f"{s[:12]}...{s[-12:]}"

def format_large_string(label, value_str):
    """Formats strings with C-style concatenation for the log file."""
    if len(value_str) <= 64:
        return f"{label}: \"{value_str}\"\n"
    
    # Logic: <= 128 chars -> 32 part chunks, else 64 part chunks
    part_size = 32 if len(value_str) <= 128 else 64
    
    output = f"{label}:\n"
    for i in range(0, len(value_str), part_size):
        chunk = value_str[i : i + part_size]
        output += f"    \"{chunk}\"\n"
    return output

def main():
    try:
        length = int(input("Enter the number of 2^64 limbs: "))
    except ValueError:
        print("Error: Length must be an integer.")
        return

    total_value = 0
    
    for i in range(length):
        limb_hex = input(f"Enter limb {i} (hex with 0x prefix): ").strip()
        
        if not limb_hex.startswith("0x"):
            print("Error: Limb must start with '0x' prefix.")
            return
        
        try:
            val = int(limb_hex, 16)
        except ValueError:
            print(f"Error: '{limb_hex}' is not valid hex.")
            return

        total_value += val * (pow(2, 64 * i))

    # Generate the raw strings
    data = {
        "Base-16": hex(total_value)[2:],
        "Base-10": str(total_value),
        "Base-8": oct(total_value)[2:],
        "Base-2": bin(total_value)[2:]
    }
    # ------ Print String Metadata
    print("\n" + "="*30)
    print("      STRING METADATA")
    print("="*30)
    for label, val_str in data.items():
        print(f"{label}: {len(val_str)}")
    print("="*30)

    # --- Terminal Output Section ---
    print("\n" + "="*30)
    print("      TERMINAL SUMMARY")
    print("="*30)
    for label, val_str in data.items():
        display_val = truncate_string(val_str)
        print(f"{label}: {display_val}")
    print("="*30 + "\n")

    # --- Log File Section ---
    log_content = ""
    for label, val_str in data.items():
        log_content += format_large_string(label, val_str)

    try:
        # Gets the absolute directory of the current script
        script_dir = Path(__file__).resolve().parent
        with open(str(script_dir) + os.sep + "bi_limbs.txt", "w") as f:
            f.write(log_content)
        print("Full data successfully logged to 'bi_limbs.txt'.")
    except Exception as e:
        print(f"Error writing to file: {e}")

if __name__ == "__main__":
    main()