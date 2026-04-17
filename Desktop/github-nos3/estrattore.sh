#!/bin/bash
# look for IRT CSS pattern

extract_function_name() {
    local file="$1"
    local line_number="$2"

    ctags -x "$file" 2>/dev/null | sort -k2n | tac  |\
    awk -v line="$line_number" '$2 <= line { print $1 }' | \
    head -n 1
}



rm /tmp/_listModif

find . -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do

    # look for IRT CSS pattern

    grep -n -E 'IRT.*CSS' "$file" | while IFS=: read -r line_number line_content; do
    #grep -n "IRT CSS" "$file" | while IFS=: read -r line_number line_content; do

        func_name=$(extract_function_name "$file" "$line_number")

        echo "$file : $func_name" >> /tmp/_listModif

    done    

done

cat /tmp/_listModif | uniq
#rm /tmp/_listModif

