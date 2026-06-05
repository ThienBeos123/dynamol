#!/bin/bash

# We define the literal text blocks to match your python script
OLD_HEADER_TEXT="/*
Copyright (C) 2026 @ThienBeos123

Licensed under the Apache License, Version 2.0 (the \"License\");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an \"AS IS\" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



"

NEW_HEADER_TEXT="/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the \"License\");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an \"AS IS\" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



"

# Export the headers so awk can read them safely as environment variables
export OLD_HEADER_TEXT
export NEW_HEADER_TEXT

# Helper function to enforce a single trailing newline format using a temporary streaming pipe
enforce_formatting_inline() {
    local target_file="$1"
    # Read file, remove all trailing whitespaces/newlines, and output exactly one trailing newline
    # Using awk to avoid pulling massive files completely into shell memory arrays
    awk '{sub(/[ \t\r\n]+$/, "", $0); print}' ORS="\n" "$target_file" > "$target_file.fmt"
    
    # Check if a transformation actually occurred to report correctly
    if ! cmp -s "$target_file" "$target_file.fmt"; then
        mv "$target_file.fmt" "$target_file"
        return 0 # True: formatting was modified
    else
        rm "$target_file.fmt"
        return 1 # False: formatting was already clean
    fi
}

# Find the files and loop through them safely
find . -type f \( \
    -name "*.c" -or -name "*.h" \
    -or -name "*.hpp" -or -name "*.hxx" -or -name "*.hh" -or -name "*.h++" \
    -or -name "*.rs" \
    -or -name "*.S" -or -name "*.s" \
\) | while read -r file; do

    # Exclude the script tools themselves from self-modification loops
    if [[ "$file" == *"license_header.sh" || "$file" == *"license_header.py" ]]; then
        continue
    fi

    # CASE 1: File is completely empty
    if [ ! -s "$file" ]; then
        printf "%s" "$NEW_HEADER_TEXT" > "$file"
        enforce_formatting_inline "$file" > /dev/null
        echo "🆕 POPULATED (Empty file initialized with Header): $file"
        continue
    fi

    # CASE 2: File has content but missing an Apache-style license completely
    if ! grep -q "Licensed under the Apache License" "$file" && ! grep -q "The ASF licenses this file" "$file"; then
        if grep -q "Copyright" "$file"; then
            echo "⚠️ SKIPPED (No ASF keywords found, but contains custom Copyright text): $file"
            continue
        fi

        # Prepend new header inline using a subshell layout
        (printf "%s" "$NEW_HEADER_TEXT"; cat "$file") > "$file.tmp" && mv "$file.tmp" "$file"
        enforce_formatting_inline "$file" > /dev/null
        echo "➕ ADDED (New Header & Formatting Fixed): $file"
        continue
    fi

    # CASE 3: File contains the old ASF style header
    # Instead of reading the whole file into a Bash variable, we let awk handle the streaming match
    if awk 'BEGIN {gsub(/\r/, "", ENVIRON["OLD_HEADER_TEXT"]); len=length(ENVIRON["OLD_HEADER_TEXT"])} 
            {chars = chars $0 "\n"} 
            NR==50 {exit} 
            END {if (index(chars, ENVIRON["OLD_HEADER_TEXT"]) == 1) exit 0; else exit 1}' "$file"; then
        
        # Safety check: count occurrences of "Copyright" using a fast grep stream
        cp_count=$(grep -c "Copyright" "$file")
        if [ "$cp_count" -gt 1 ]; then
            echo "⚠️ SKIPPED (Multiple copyrights detected): $file"
            continue
        fi

        # Streams substitution: Strips the first 19 lines (the old header size with its lines) and prepends the new one
        (printf "%s" "$NEW_HEADER_TEXT"; tail -n +20 "$file") > "$file.tmp" && mv "$file.tmp" "$file"
        enforce_formatting_inline "$file" > /dev/null
        echo "✅ UPDATED (Fixed ASF Header & Formatting Fixed): $file"
        
    else
        # CASE 4: Header is already clean, evaluate and fix formatting rules exclusively
        if enforce_formatting_inline "$file"; then
            echo "✨ FORMATTED (Enforced trailing newline layout): $file"
        else
            echo "ℹ️ NO ACTION (Already clean with correct trailing newline): $file"
        fi
    fi
done
