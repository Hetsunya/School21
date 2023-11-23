#!/bin/bash

if (($# != 1)); then
    echo "No filename!"
    exit 0
fi

filename=$1
flags=(b e n s t v)

rm out1.txt out2.txt
make s21_cat

# Function to run test with a specific flag
run_specific_test() {
    local flag=$1
    cat "-${flag}" "$filename" >> out1.txt
    ./s21_cat "-${flag}" "$filename" >> out2.txt
    result=$(diff out1.txt out2.txt)
    if (($? == 0)); then
        echo "-$flag SUCCESS"
    else
        echo "-$flag FAIL"
        exit 0
    fi
}

# Function to run test with random flags
run_random_test() {
    local random_flags=$1
    cat -- "$random_flags" "$filename" >> out1.txt
    ./s21_cat -- "$random_flags" "$filename" >> out2.txt
    result=$(diff out1.txt out2.txt)
    if (($? == 0)); then
        echo "$random_flags SUCCESS"
    else
        echo "$random_flags FAIL"
        exit 0
    fi
}

# Run tests with each specific flag
for flag in "${flags[@]}"; do
    echo "-$flag"
    run_specific_test $flag
done

# Run test with random combination of 2-3 flags
random_flags=""
for i in {1..3}; do
    random_flag=${flags[$((RANDOM % ${#flags[@]}))]}
    random_flags+="-${random_flag}"
done

echo "$random_flags"
run_random_test "$random_flags"

# Clean up
rm out1.txt out2.txt
make s21_cat
