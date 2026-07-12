# evaluator.py
from generator import int_to_limbs, format_limb_array
from pprint import pprint

def evaluate_logic(inputs) -> tuple:
    """
    MANUAL TWEAK ZONE: Modify this function based on the operation you are testing.
    'inputs' is a list of integer values corresponding to the current case.
    """
    return (inputs[0] * inputs[1], inputs[2] * inputs[3]);

def run_evaluator(cases):
    exp_limb_cache: dict = {}
    exp_limb_strings: list = []
    limb_counter: int = 1
    magnituded = input("Magnituded inputs (for evaluator.py? [Y/n]: ")
    magnituded = magnituded.lower()
    if magnituded == 'y': magnituded = True
    else: magnituded = False
    
    for case in cases:
        # Extract raw values for evaluation
        raw_vals = [case[i]['val'] for i in range(1, len(case), 1)]
        
        # Calculate expected result
        exp_val = evaluate_logic(raw_vals)
        case[0]['output_count'] = len(exp_val)
        
        abs_exp = tuple(abs(val) for val in exp_val)
        for abs_exp_val in abs_exp:
            if abs_exp_val not in exp_limb_cache:
                if magnituded: arr_name = f"exp_limb_{limb_counter}"
                else: arr_name = f"exp_limb_saw_{limb_counter}"
                exp_limb_cache[abs_exp_val] = arr_name
                exp_limb_strings.append(format_limb_array(arr_name, int_to_limbs(abs_exp_val)))
                limb_counter += 1
        
        for i in range(len(exp_val)):
            case.append({
                'type': 'bigint', # Expected output is usually a bigint
                'val': exp_val[i],
                'limb_name': exp_limb_cache[abs_exp[i]]
            })
        
        
    return cases, "".join(exp_limb_strings)