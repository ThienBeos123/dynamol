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

# Find the files and loop through them safely
find . -type f \( \
    -name "*.c" -or -name "*.h" \
    -or -name "*.hpp" -or -name "*.hxx" -or -name "*.hh" -or -name "*.h++" \
    -or -name "*.rs" \
    -or -name "*.S" \
\) | while read -r file; do

    # CASE 1: File is completely empty (ignoring white spaces)
    if [[ ! -s "$file" || -z $(tr -d '[:space:]' < "$file") ]]; then
        printf "%s" "$NEW_HEADER_TEXT" > "$file"
        echo "➕ ADDED (Empty File): $file"
        continue
    fi

    # CASE 2: File has no license block at all
    if ! grep -q "Licensed under the Apache License" "$file" && ! grep -q "The ASF licenses this file" "$file"; then
        if grep -q "Copyright" "$file"; then
            echo "⚠️ SKIPPED (No Apache license, but contains custom Copyright text): $file"
            continue
        fi
        
        # Prepend the header cleanly using a subshell stream redirect
        (printf "%s" "$NEW_HEADER_TEXT"; cat "$file") > "$file.tmp" && mv "$file.tmp" "$file"
        echo "➕ ADDED (New Header): $file"
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

        # Streams substitution: Strips the first 16 lines (the old header size) and prepends the new one
        (printf "%s" "$NEW_HEADER_TEXT"; tail -n +17 "$file") > "$file.tmp" && mv "$file.tmp" "$file"
        echo "✅ UPDATED (Fixed ASF Header): $file"
        
    else
        # Determine if it was skipped due to a structural mismatch or if it's already clean
        if grep -q "Licensed under the Apache License" "$file" || grep -q "The ASF licenses this file" "$file"; then
            if [[ $(head -n 2 "$file") == *"@Poly-glon"* ]]; then
                echo "ℹ️ NO ACTION (Already using clean header): $file"
            else
                echo "⚠️ SKIPPED (ASF phrase found, but header structure mismatched): $file"
            fi
        fi
    fi
done
