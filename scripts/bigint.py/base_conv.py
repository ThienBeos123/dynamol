# Custom Base-64 character set
B64_CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./"

def string_to_int(s, base_n):
    """Converts a string in base-n to a Python integer."""
    is_negative = s.startswith('-')
    if is_negative:
        s = s[1:]
    
    val = 0
    char_map = {char: i for i, char in enumerate(B64_CHARS[:base_n])}
    
    for char in s:
        if char not in char_map:
            raise ValueError(f"Character '{char}' is not valid for base {base_n}.")
        val = val * base_n + char_map[char]
        
    return -val if is_negative else val

def int_to_string(n, base_b):
    """Converts a Python integer to a string in base-b."""
    if n == 0:
        return B64_CHARS[0]
    
    is_negative = n < 0
    n = abs(n)
    
    digits = []
    while n > 0:
        digits.append(B64_CHARS[n % base_b])
        n //= base_b
    
    res = "".join(reversed(digits))
    return "-" + res if is_negative else res

def main():
    print("--- Universal Base Converter (Base 2 to 64) ---")
    
    try:
        # Input Base Selection
        base_n = int(input("Enter source base (n): "))
        base_b = int(input("Enter target base (b): "))
        
        if not (2 <= base_n <= 64 and 2 <= base_b <= 64):
            print("Error: Bases must be between 2 and 64.")
            return
            
        num_str = input(f"Enter the numerical string (Base {base_n}): ").strip()
        
        if not num_str:
            print("Error: Input string cannot be empty.")
            return

        # Perform Conversion
        # 1. Convert source string to a Python BigInt
        decimal_value = string_to_int(num_str, base_n)
        
        # 2. Convert BigInt to target base string
        result_str = int_to_string(decimal_value, base_b)
        
        print("\n" + "="*30)
        print(f"Result length: {len(result_str)}")
        print(f"Result (Base {base_b}):")
        print(result_str)
        print("="*30)

    except ValueError as e:
        print(f"Input Error: {e}")
    except KeyboardInterrupt:
        print("\nProgram exited.")
    except Exception as e:
        print(f"Unexpected Error: {e}")

if __name__ == "__main__":
    main()