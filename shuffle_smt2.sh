#!/bin/bash

set -e  # 遇错即退，可以保留

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <input_file> <iterations> [--verbose]"
    exit 1
fi

INPUT_FILE=$1
ITERATIONS=$2
VERBOSE=0

# 如果有 --verbose
if [ "$#" -eq 3 ] && [ "$3" = "--verbose" ]; then
    VERBOSE=1
fi

BITWUZLA_PATH="bitwuzla"

# ---- 1) 先把整个文件一次性读到数组 all_lines 中 ----
declare -a all_lines
while IFS= read -r line; do
    all_lines+=("$line")
done < "$INPUT_FILE"

# Fisher-Yates 洗牌函数（不变）
fisher_yates_shuffle() {
    local count=$1      # 这原本是 i=$1
    local arr_name=$2
    local j temp
    
    # 用 idx 来做循环
    local idx
    for ((idx = count - 1; idx > 0; idx--)); do
        j=$((RANDOM % (idx + 1)))
        eval "temp=\${$arr_name[$idx]}"
        eval "$arr_name[$idx]=\${$arr_name[$j]}"
        eval "$arr_name[$j]=\$temp"
    done
}

process_smt2() {
    local iteration=$1
    # 2) 从 all_lines 数组中再分别拆分 assertions 和 other_content
    declare -a assertions
    declare -a other_content
    local assert_count=0

    for line in "${all_lines[@]}"; do
        if [[ "$line" =~ ^\s*\(assert ]]; then
            assertions+=("$line")
            ((assert_count++))
        else
            other_content+=("$line")
        fi
    done

    # 洗牌
    fisher_yates_shuffle ${#assertions[@]} assertions

    echo "Iteration $iteration:"
    if [ $VERBOSE -eq 1 ]; then
        echo "Assertion order:"
        local idx=1
        for asst in "${assertions[@]}"; do
            echo "$idx. $asst"
            ((idx++))
        done
        echo "Total assertions: $assert_count"
        echo "Running bitwuzla with shuffled assertions..."
    fi

    # 临时文件
    local temp_file
    temp_file=$(mktemp)

    {
        # 3) 把 other_content 里的行写到文件里，碰到 (check-sat) 前插入洗牌后的 (assert ...)
        for line in "${other_content[@]}"; do
            if [[ "$line" =~ ^\s*\(check-sat ]]; then
                printf '%s\n' "${assertions[@]}"
            fi
            echo "$line"
        done
    } > "$temp_file"

    # 4) 调用 bitwuzla
    "$BITWUZLA_PATH" "$temp_file" || true

    rm -f "$temp_file"
}

# 5) 主循环
echo "Running $ITERATIONS iterations..."
if [ $VERBOSE -eq 1 ]; then
    echo "Verbose mode enabled"
    echo "Input file: $INPUT_FILE"
fi

for ((i=1; i<=$ITERATIONS; i++)); do
    process_smt2 "$i"
    echo "----------------------------------------"
done

echo "All iterations completed"