import os

OLD_HEADER = """/*
Copyright (C) 2026 @ThienBeos123

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



"""

# The clean layout with 3 trailing newlines
NEW_HEADER = """/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



"""  # Contains exactly 3 endlines after the closing tag

def enforce_formatting(content):
    """
    Enforces structural formatting constraints:
    Ensures the file ends with exactly one trailing newline character 
    after the final instruction statement or brace block.
    """
    # Strip any trailing whitespaces or extra newlines from the absolute end
    cleaned = content.rstrip()
    # Re-append exactly one trailing newline
    return cleaned + "\n"

def process_file(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    # CASE 1: File is completely empty
    if not content.strip():
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(enforce_formatting(NEW_HEADER))
        print(f"🆕 POPULATED (Empty file initialized with Header): {file_path}")
        return

    # CASE 2: File has content but missing an Apache-style license completely
    if "Licensed under the Apache License" not in content and "The ASF licenses this file" not in content:
        if "Copyright" in content:
            print(f"⚠️ SKIPPED (No ASF keywords found, but contains custom Copyright text): {file_path}")
            return
        
        # Prepend the header to the existing file contents and enforce formatting rules
        updated_content = enforce_formatting(NEW_HEADER + content)
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(updated_content)
        print(f"➕ ADDED (New Header & Formatting Fixed): {file_path}")
        return

    # CASE 3: File contains the old ASF style header
    if OLD_HEADER in content:
        # Safety: Do not modify if there are multiple copyright lines stacked
        if content.count("Copyright") > 1:
            print(f"⚠️ SKIPPED (Multiple copyrights detected): {file_path}")
            return
            
        if content.startswith(OLD_HEADER):
            updated_content = content.replace(OLD_HEADER, NEW_HEADER, 1)
            updated_content = enforce_formatting(updated_content)
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(updated_content)
            print(f"✅ UPDATED (Fixed ASF Header & Formatting Fixed): {file_path}")
        else:
            print(f"⚠️ SKIPPED (ASF phrase found, but header structure mismatched): {file_path}")
    else:
        # CASE 4: Header is already up to date, but we still check and fix the formatting rules
        formatted_content = enforce_formatting(content)
        if formatted_content != content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(formatted_content)
            print(f"✨ FORMATTED (Enforced trailing newline layout): {file_path}")
        else:
            print(f"ℹ️ NO ACTION (Already clean with correct trailing newline): {file_path}")

# Walk through project directory
for root, _, files in os.walk('.'):
    for file in files:
        if file.endswith((
            '.c', '.h', # All of lib-dnml code files
            '.hpp', '.hxx', '.hh', 'h++', # C++ Header files (Convenient C++ port)
            '.rs', # Rust Files (test files)
            '.S', '.s' # Assembly source structures
        )):
            # Protect the processing script self-references
            if file in ['license_header.py', 'license_header.sh']:
                continue
            process_file(os.path.join(root, file))
