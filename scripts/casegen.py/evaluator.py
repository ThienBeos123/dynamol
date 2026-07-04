# evaluator.py
from generator import int_to_limbs, format_limb_array

def evaluate_logic(inputs):
    """
    MANUAL TWEAK ZONE: Modify this function based on the operation you are testing.
    'inputs' is a list of integer values corresponding to the current case.
    """
    return inputs[0] - inputs[1]

def run_evaluator(cases):
    exp_limb_cache = {}
    exp_limb_strings = []
    limb_counter = 1
    magnituded = input("Magnituded inputs (for evaluator.py? [Y/n]: ")
    magnituded = magnituded.lower()
    if magnituded == 'y': magnituded = True
    else: magnituded = False
    
    for case in cases:
        # Extract raw values for evaluation
        raw_vals = [c['val'] for c in case]
        
        # Calculate expected result
        exp_val = evaluate_logic(raw_vals)
        
        abs_exp = abs(exp_val)
        if abs_exp not in exp_limb_cache:
            if magnituded: arr_name = f"exp_limb_{limb_counter}"
            else: arr_name = f"exp_limb_saw_{limb_counter}"
            exp_limb_cache[abs_exp] = arr_name
            exp_limb_strings.append(format_limb_array(arr_name, int_to_limbs(abs_exp)))
            limb_counter += 1
            
        case.append({
            'type': 'bigInt', # Expected output is usually a bigInt
            'val': exp_val,
            'limb_name': exp_limb_cache[abs_exp]
        })
        
    return cases, "".join(exp_limb_strings)