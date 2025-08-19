#!/bin/bash

program="./$(grep 'NAME :=' ./Makefile | cut -d'=' -f2 | xargs)"

make
build_status="$?"

if (( build_status != 0 )); then
    exit $build_status;
fi

if ! [ -x "$program" ]; then
    echo "missing $program executable, check if right NAME on Makefile"
    exit 1;
fi

setup_test_with_numbers() {
    expected=$(printf "%s\n" "${numbers[@]}" | sort -n | xargs)
    command=("$program" ${numbers[@]})
    expected="Before:  ${numbers[@]}
After:   $expected
Time to process a range of ${#numbers[@]} elements with std::vector<int> : [0-9]+us
Time to process a range of ${#numbers[@]} elements with std::deque<int> : [0-9]+us"
    expected_status='0'
}

run_test_command() {

    actual=$("${command[@]}" 2>&1)
    status="$?"
    if [[ ! "$actual" =~ $expected ]]; then
        echo "[BAD] test: ${command[*]}";
        echo "actual:"
        echo "$actual";
        echo ""
        echo "expected:"
        echo "$expected"
        echo ""
        #exit 42;
    elif (( status != expected_status )); then
        echo "[BAD] test: ${command[*]}";
        echo "actual status: $status"
        echo "expected status: $expected_status"
        # exit $status
    else
         echo "[GOOD] test: ${command[*]}";	
    fi
}

## BEGIN CASES

command=("$program")
expected="Usage: ./PmergeMe \(list of natural numbers to be sorted\)
ex: ./PmergeMe 10 0 12 22 55 13"
expected_status='11'
run_test_command

command=("$program" "-10")
expected="Error"
expected_status='22'
run_test_command

numbers=("10" "20")
setup_test_with_numbers
run_test_command

numbers=("10" "20" "30")
setup_test_with_numbers
run_test_command

numbers=("10" "20" "30" "40")
setup_test_with_numbers
run_test_command

numbers=("10" "20" "30" "40" "50")
setup_test_with_numbers
run_test_command


numbers=("20" "10")
setup_test_with_numbers
run_test_command

numbers=("30" "10" "20")
setup_test_with_numbers
run_test_command

numbers=("20" "10" "40" "30")
setup_test_with_numbers
run_test_command

numbers=("40" "50" "20" "30" "10"  )
setup_test_with_numbers
run_test_command

numbers=("10" "40" "30" "60" "20" "90" "70" "80" "50" )
setup_test_with_numbers
run_test_command

numbers=("30" "10" "50" "60" "40" "80" "20" "90" "70" )
setup_test_with_numbers
run_test_command

numbers=(88 40 69 95 13 7 61 59)
setup_test_with_numbers
run_test_command

numbers=(59 89 83 75 56 45 14 59 42)
setup_test_with_numbers
run_test_command

numbers=(18 21 91 0 31 42 76 19 86 79)
setup_test_with_numbers
run_test_command

numbers=(86 29 69 64 11 10 81 47 45 14)
setup_test_with_numbers
run_test_command

numbers=(74 49 1 65 72 18 67 66)
setup_test_with_numbers
run_test_command

numbers=(75 51 2 93 63 31 28 18 33)
setup_test_with_numbers
run_test_command

numbers=(52 97 40 41 83 31 19 26 83 33)
setup_test_with_numbers
run_test_command

numbers=(90 13 0 2 26 54 57 47 81 78 58 76 15 30 11 0 23)
setup_test_with_numbers
run_test_command

numbers=(53 22 94 29 44 29 42 78 18 55 60 71 29 1 10 26 10 4 43)
setup_test_with_numbers
run_test_command

numbers=(37 13 38 97 5 67 48 77 7 15 8 36 99 29 59 31 91 40 78 44)
setup_test_with_numbers
run_test_command

numbers=(36 43 35 62 30 72 85 97 89 82 91 82 95 73 67 53 8 22 93 70 39)
setup_test_with_numbers
run_test_command

numbers=(87 81 70 84 86 85 55 52 9 16 72 26 79 30 64 90 69 25 47 66 8 46)
setup_test_with_numbers
run_test_command

numbers=(40 43 13 15 87 20 28 32 94 2 84 19 60 56 95 14 80 83 93 35 32 24)
setup_test_with_numbers
run_test_command

numbers=(19 12 65 26 96 80 49 49 85 60 89 12 73 81 92 29 26 41 99 53 23 53 91)
setup_test_with_numbers
run_test_command

numbers=(71 20 38 18 41 39 87 14 42 46 31 80 62 64 27 3 85 61 28 92 31 60 78)
setup_test_with_numbers
run_test_command

numbers=(74 79 88 35 40 24 2 43 49 11 20 93 42 50 59 20 6 24 10 44 48 9 5)
setup_test_with_numbers
run_test_command

numbers=(95 92 61 52 72 45 58 95 20 27 56 12 89 1 45 4 42 48 77 46 41 30 57)
setup_test_with_numbers
run_test_command

numbers=(0 0 10 1 1000 0 1 1 71 1 21 1)
setup_test_with_numbers
run_test_command

numbers=(0 1 10 1 1000 0 2 0 0 0 0 0)
setup_test_with_numbers
run_test_command

numbers=(1 1 10 1 1000 1  1 1 1 1 1 1 1 1)
setup_test_with_numbers
run_test_command
 
for size in {3..500}; do # testSize
    for run in {1..20}; do  # how many tests with that size
	numbers=();
	for ((i=0; i < size; i++)); do
            numbers+=($((RANDOM % 100)) )
	done
        setup_test_with_numbers
	run_test_command
    done
done
