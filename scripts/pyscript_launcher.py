import os
import sys
import json

CACHE_FILE = ".program_cache.json"

def scan_folders():
    """Scans for directories ending in .py and finds .py files inside them."""
    print("Scanning directories...")
    programs = {}
    
    # Iterate through items in the current directory
    for entry in os.scandir('.'):
        if entry.is_dir() and entry.name.endswith('.py'):
            # Look for python files inside these specific folders
            for file in os.scandir(entry.path):
                if file.is_file() and file.name.endswith('.py'):
                    # Store as 'FolderName/FileName'
                    prog_name = f"{entry.name}/{file.name}"
                    programs[prog_name] = os.path.abspath(file.path)
    
    # Save to cache
    with open(CACHE_FILE, 'w') as f:
        json.dump(programs, f)
    return programs

def load_cache():
    """Loads the program list from the JSON cache."""
    if not os.path.exists(CACHE_FILE):
        return None
    with open(CACHE_FILE, 'r') as f:
        return json.load(f)

def main():
    programs = None
    
    # Check for cache first
    cached_data = load_cache()
    
    if cached_data:
        choice = input("Cache found. Use cached list? (y/n): ").lower()
        if choice == 'y':
            programs = cached_data
    
    # Cold boot or user chose to rescan
    if not programs:
        programs = scan_folders()

    if not programs:
        print("No programs found in folders ending with '.py'.")
        return

    # Display list to user
    print("\n--- Available Programs ---")
    prog_list = list(programs.keys())
    for i, name in enumerate(prog_list, 1):
        print(f"{i}. {name}")

    try:
        user_choice = int(input("\nSelect a program number to run: "))
        selected_key = prog_list[user_choice - 1]
        script_path = programs[selected_key]
        
        if not os.path.exists(script_path):
            print(f"Error: {selected_key} no longer exists at {script_path}")
            return

        print(f"Launching {selected_key} and exiting launcher...")
        print("-------------------------------------------------")
        
        # os.execv replaces the current process. 
        # sys.executable is the path to the python interpreter.
        os.execv(sys.executable, [sys.executable, script_path])

    except (ValueError, IndexError):
        print("Invalid selection. Exiting.")

if __name__ == "__main__":
    main()