# main.py
from generator import run_generator
from evaluator import run_evaluator
from formatter import run_formatter

def main():
    print("=== Step 1: Input & Limb Generation ===")
    # Generates cases list and C-code string for input limbs
    cases, input_limbs_c = run_generator()
    
    print("\n=== Step 2: Evaluating Expected Results ===")
    # Appends expected results to cases and generates C-code string for exp limbs
    cases, exp_limbs_c = run_evaluator(cases)
    
    print("\n=== Step 3: Formatting C Arrays ===")
    # Asks for struct info then generates the struct array
    t_name = input("Enter the C struct type name (e.g., bi_add_case): ").strip()
    a_name = input("Enter the C array name (e.g., add_cases): ").strip()
    cases_array_c = run_formatter(cases, type_name=t_name, array_name=a_name)
    
    # Final Output Orchestration
    print("\n" + "="*60)
    print("SUCCESS: C Code Generated Successfully!")
    print("="*60 + "\n")
    
    final_output = (
        "//* ================= GENERATED LIMB ARRAYS ================= *//\n\n"
        "// INPUT LIMBS VECTORS\n"
        f"{input_limbs_c}\n"
        "// EXPECTED LIMBS VECTORS\n"
        f"{exp_limbs_c}\n"
        "//* ================= GLOBAL ARRAY OF CASES ================= *//\n\n"
        f"#define CASE_CNT {len(cases)}\n\n"
        f"{cases_array_c}\n"
    )
    
        # Save to file or print
    with open("generated_cases.sout", "w+") as f:
        f.write(final_output)
    print("Output written to 'generated_cases.sout'. Preview below:\n")
    print(final_output[:1500] + "\n\n... [Truncated for console] ...")

if __name__ == "__main__":
    main()