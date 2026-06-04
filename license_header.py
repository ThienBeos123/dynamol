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


\n"""

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


\n"""  # Contains exactly 3 endlines after the closing tag

def process_file(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    # CASE 1: File is completely empty
    if not content.strip():
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(NEW_HEADER)
        print(f"➕ ADDED (Empty File): {file_path}")
        return

    # CASE 2: File has no license block at all
    if "Licensed under the Apache License" not in content and "The ASF licenses this file" not in content:
        # Safety check: Make sure it doesn't already have a generic copyright statement
        if "Copyright" in content:
            print(f"⚠️ SKIPPED (No Apache license, but contains custom Copyright text): {file_path}")
            return
        
        # Prepend the header to the existing file contents
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(NEW_HEADER + content)
        print(f"➕ ADDED (New Header): {file_path}")
        return

    # CASE 3: File contains the old ASF style header
    if OLD_HEADER in content:
        # Safety: Do not modify if there are multiple copyright lines stacked
        if content.count("Copyright") > 1:
            print(f"⚠️ SKIPPED (Multiple copyrights detected): {file_path}")
            return
            
        if content.startswith(OLD_HEADER):
            updated_content = content.replace(OLD_HEADER, NEW_HEADER, 1)
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(updated_content)
            print(f"✅ UPDATED (Fixed ASF Header): {file_path}")
        else:
            print(f"⚠️ SKIPPED (ASF phrase found, but header structure mismatched): {file_path}")
    else:
        print(f"ℹ️ NO ACTION (Already using clean header): {file_path}")

# Walk through project directory
for root, _, files in os.walk('.'):
    for file in files:
        if file.endswith((
            '.c', '.h', # All of lib-dnml code files
            '.hpp', '.hxx', '.hh', 'h++', # C++ Header files (Convenient C++ port)
            '.rs', # Rust Files (test files)
            '.S', # Core Assembly Intrinsics files
        )): process_file(os.path.join(root, file))
