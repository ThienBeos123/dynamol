# formatter.py
from pprint import pprint
import sys
sys.set_int_max_str_digits(123849834)

def format_struct_field(item):
    v_type = item['type']
    val = item['val']
    
    if v_type in ['uint8_t', 'size_t']:
        return str(val)
        
    elif v_type in ['bigint', 'crint']:
        # ALWAYS derive n from the current value 'val'
        # This fixes the metadata mismatch caused by swaps in generator.py
        n = (val.bit_length() + 63) // 64 if val != 0 else 0
        
        sign = -1 if val < 0 else 1
        cap = max(1, n)
        limb_ptr = item['limb_name']
        
        # Ensure 'n' matches the reality of 'val'
        base_struct = f"{{ .n = {n}, .cap = {cap}, .limbs = (limb_t*){limb_ptr}, .sign = {sign}"
        if v_type == 'crint':
            base_struct += ", .poisoned = false"
        base_struct += " }"
        return base_struct
        
    return "0"

def run_formatter(cases, type_name="test_case_t", array_name="generated_cases"):
    lines = []
    lines.append(f"static const {type_name} {array_name}[CASE_CNT] = {{")
    
    for idx, case in enumerate(cases):
        # Explicitly separate inputs and outputs to ensure nothing is skipped
        # This solves the "missing inputs" issue
        input_cnt: int = case[0]['input_count']
        output_cnt: int = case[0]['output_count']
        inputs: list = case[1:input_cnt+1]
        outputs: list = case[input_cnt+1:len(case)]
        if idx == 0:
            pprint(case)
            pprint(inputs)
            pprint(outputs)
        
        # Format input fields
        input_fields: list = [format_struct_field(item) for item in inputs]
        # Format output field
        output_fields: list = [format_struct_field(item) for item in outputs]
        
        # Combine in order: { InputA, InputB, Expected }
        if (input_cnt == 1 and output_cnt == 1): row_str = f"    {{ {input_fields[0]}, {output_fields[0]} }}"
        elif (output_cnt == 1): row_str = f"    {{ {', '.join(input_fields)}, {output_fields[0]} }}"
        elif (input_cnt == 1): row_str = f"    {{ {input_fields[0]}, {', '.join(output_fields)} }}"
        else: row_str = f"    {{ {', '.join(input_fields)}, {', '.join(output_fields)} }}"
        
        if idx < len(cases) - 1: row_str += ","
            
        row_str = f"{row_str:<100} // {idx + 1}"
        lines.append(row_str)
        
    lines.append("};")
    return "\n".join(lines)