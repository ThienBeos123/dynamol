import random
from pprint import pprint
u64_max_const = (1 << 64) - 1


def int_to_limbs(val: int):
    val = abs(val)
    if val == 0:
        return ["UINT64_C(0x0000000000000000)"]
    limbs = []
    while val > 0:
        limbs.append(f"UINT64_C(0x{val & 0xFFFFFFFFFFFFFFFF:016X})")
        val >>= 64
    return limbs

def format_limb_array(name: str, limbs) -> str:
    size = len(limbs)
    res = f"static const limb_t {name}[{size}] = {{"
    lines = []
    for i in range(0, size, 6):
        lines.append(", ".join(limbs[i:i+6]))
    
    if len(lines) > 1:
        res += "\n    " + ",\n    ".join(lines) + "\n};\n"
    else:
        res += " " + lines[0] + " };\n"
    return res

def generate_random_value(
    v_type: str, max_len: int, magnituded: bool, 
    balanced: bool, new_iter: bool, max_iter_len: int
) -> int:
    if v_type in ['bigint', 'crint']:
        # Force a random number of limbs between 2 and max_len
        if balanced and not new_iter: 
            # num_limbs = random.randint(
            #     int(float(max_iter_len / 22) * 10), # max_iter_len / 2.2
            #     int(float(max_iter_len / 18) * 10) # max_iter_len / 1.8
            # )
            num_limbs = int(max_iter_len / 2)
        else: num_limbs = random.randint(2, max_len)
        val, limb = 0, 0
        for i in range(int(num_limbs)):
            if i == num_limbs - 1: limb = random.randint(1, u64_max_const)
            else: limb = random.getrandbits(64)
            val |= (limb << (64 * i))
        return val if magnituded or random.random() < 0.5 else -val
    elif v_type == 'uint8_t': return random.choice([0, 1, 255] + [random.randint(2, 254)])
    # elif v_type == 'uint64_t': return random.choice([0, 1, 64] + [random.randint(2, 100)])
    elif v_type == 'uint64_t': return random.randint(1, u64_max_const);
    return 0


def swap_vals(case_data: list[dict], i: int) -> None:
    if abs(case_data[i]['val']) < abs(case_data[i + 1]['val']):
        case_data[i]['val'], case_data[i + 1]['val'] = case_data[i + 1]['val'], case_data[i]['val']


def run_generator():
    num_cases = int(input("Enter the amount of test cases: "))
    num_inputs = int(input("Enter the number of inputs per case: "))
    max_len = int(input("Enter the maximum amount of limbs: "))
    input_types = [input(f"Type for input {i+1} (bigInt, crint, uint64_t, uint8_t): ").strip().lower() for i in range(num_inputs)]

    magnituded = input("Magnituded inputs? [Y/n]: ")
    magnituded = magnituded.lower()
    if magnituded == 'y': magnituded = True
    else: magnituded = False

    balanced = input("Balanced inputs? [Y/n]: ")
    balanced = balanced.lower()
    if balanced == 'y': balanced = True
    else: balanced = False


    seen_combinations: set = set()
    limb_cache: dict = {}
    limb_strings: list = []
    cases: list = []
    limb_counter: int = 1
    
    # Generate the product from this larger set
    attempts = 0
    while len(cases) < num_cases:
        list_case_vals: list = []
        new_iter: bool = True
        max_iter_len: int = 0
        for i in range(len(input_types)):
            tmp: int = generate_random_value(
                v_type=input_types[i], max_len=max_len, magnituded=magnituded,
                balanced=balanced, new_iter=new_iter, max_iter_len=max_iter_len
            )
            if i == 0: new_iter = False
            current_limb_count = (tmp.bit_length() + 63) // 64 if tmp != 0 else 1
            max_iter_len = max(max_iter_len, current_limb_count)
            list_case_vals.append(tmp)
        current_case_vals: tuple = tuple(list_case_vals)

        
        if current_case_vals in seen_combinations:
            attempts += 1
            if attempts > 2000: # Circuit breaker to prevent hanging
                print(f"Space saturated, generated {len(cases)} cases.")
                break
            continue
        
        attempts = 0
        seen_combinations.add(current_case_vals)
        
        case_data = [{'input_count': num_inputs, 'output_count': 0}]
        for i, val in enumerate(current_case_vals):
            v_type: str = input_types[i]
            input_info: dict = {'type': v_type, 'val': val}
            
            if v_type in ['bigint', 'crint']:
                abs_val = abs(val)
                if abs_val not in limb_cache:
                    if (magnituded): arr_name = f"in_limb_{limb_counter}"
                    else: arr_name = f"in_limb_saw_{limb_counter}"
                    limb_cache[abs_val] = arr_name
                    limb_strings.append(format_limb_array(arr_name, int_to_limbs(abs_val)))
                    limb_counter += 1
                input_info['limb_name'] = limb_cache[abs_val]
                
            case_data.append(input_info)
        for i in range(1, num_inputs, 2):
            if magnituded and i + 1 < len(case_data): # Fixed boundary check
                if case_data[i]['type'] == case_data[i+1]['type'] and case_data[i]['type'] in ['bigint', 'crint']: swap_vals(case_data, i)
                if case_data[i]['type'] in ['bigint', 'crint'] and case_data[i+1]['type'] == 'uint64_t': swap_vals(case_data, i)
        cases.append(case_data)
        
    return cases, "".join(limb_strings)