import random
import itertools
from pprint import pprint

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
    if v_type in ['bigInt', 'crint']:
        # Force a random number of limbs between 2 and max_len
        if balanced and not new_iter: num_limbs = random.randint(int(max_iter_len // 3 * 2), max_iter_len)
        else: num_limbs = random.randint(2, max_len)
        val, limb = 0, 0
        for i in range(int(num_limbs)):
            limb = 0
            if i != num_limbs - 1: limb = random.getrandbits(64)
            else: 
                while limb == 0: limb = random.getrandbits(64)
            if i == num_limbs - 1: limb |= 0x1000000000000000 
            val |= (limb << (64 * i))
        return val if magnituded or random.random() < 0.5 else -val
    elif v_type == 'uint8_t': return random.choice([0, 1, 255] + [random.randint(2, 254)])
    elif v_type == 'size_t': return random.choice([0, 1, 64] + [random.randint(2, 100)])
    return 0

def run_generator():
    num_cases = int(input("Enter the amount of test cases: "))
    num_inputs = int(input("Enter the number of inputs per case: "))
    max_len = int(input("Enter the maximum amount of limbs: "))
    input_types = [input(f"Type for input {i+1} (bigInt, crint, size_t, uint8_t): ").strip() for i in range(num_inputs)]

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

    # Pre-generate an exhaustive pool of edge case combinations to prevent stalling
    # We take a sample of edge values to create an initial unique pool
    # Use a much larger pool of "deterministic" values 
    # instead of just 3 hardcoded values.
    # This creates 20*20 = 400 unique guaranteed cases for 2 inputs.
    large_edge_set = [
        0, 1, 2, 3, 0xFFFFFFFFFFFFFFFF, 0x5555555555555555, 
        0xAAAAAAAAAAAAAAAA, 0x1234567890ABCDEF, 0x8000000000000000, 
        0x000000000000FFFF, 0xFFFFFFFF00000000, 0x1111111111111111,
        0x2222222222222222, 0x3333333333333333, 0xBBBBBBBBBBBBBBBB,
        0x4000000000000000, 0x7FFFFFFFFFFFFFFF, 0x00000000FFFFFFFF,
        0x00000000000000FF, 0xFFFFFFFFFFFFFFFE
    ]
    
    # Generate the product from this larger set
    edge_pool = list(itertools.product(large_edge_set, repeat=num_inputs))
    random.shuffle(edge_pool)

    attempts = 0
    while len(cases) < num_cases:
        # Use pool first, then fallback to random generation
        if edge_pool and random.random() < 0.5:
            current_case_vals: tuple = edge_pool.pop()
        else:
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
        
        case_data = []
        for i, val in enumerate(current_case_vals):
            v_type: str = input_types[i]
            input_info: dict = {'type': v_type, 'val': val}
            
            if v_type in ['bigInt', 'crint']:
                abs_val = abs(val)
                if abs_val not in limb_cache:
                    if (magnituded): arr_name = f"in_limb_{limb_counter}"
                    else: arr_name = f"in_limb_saw_{limb_counter}"
                    limb_cache[abs_val] = arr_name
                    limb_strings.append(format_limb_array(arr_name, int_to_limbs(abs_val)))
                    limb_counter += 1
                input_info['limb_name'] = limb_cache[abs_val]
                
            case_data.append(input_info)
        if magnituded and case_data[0]['val'] < case_data[1]['val']:
            tmp = case_data[0] # Swap the entire dictionary, not just the value
            case_data[0] = case_data[1]
            case_data[1] = tmp
        cases.append(case_data)

    return cases, "".join(limb_strings)