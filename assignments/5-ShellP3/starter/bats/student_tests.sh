#!/usr/bin/env bats

# File: student_tests.sh
#  
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipes: Pipe with invalid command syntax - ls | grep [" {
    run "./dsh" <<EOF
ls | grep [
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="grep:brackets([])notbalanceddsh3>dsh3>cmdloopreturned0"
\
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Pipes: Too many commands - Exceeding CMD_MAX (adjust number if CMD_MAX is different)" {
    run "./dsh" <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9 | echo 10
EOF

    if ! grep -q "$CMD_ERR_PIPE_LIMIT" <<< "$output"; then
        echo "Expected error message '$CMD_ERR_PIPE_LIMIT' not found in output:"
        echo "$output"
        false 
    fi

    [ "$status" -ne 0 ]
}

@test "Pipes: Empty command" {
    run "./dsh" <<EOF

EOF


    if ! grep -q "$CMD_WARN_NO_CMD" <<< "$output"; then
        echo "Expected warning message '$CMD_WARN_NO_CMD' not found in output:"
        echo "$output"
        false
    fi

    [ "$status" -eq 0 ]
}

@test "Pipes: Commands with arguments | grep word" {
    run "./dsh" <<EOF
echo "This is a test string with the word word in it" | grep word
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    echo "Output: $stripped_output"
    expected_output="Thisisateststringwiththewordwordinitdsh3>dsh3>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipes: Command not found in pipeline - nosuchcommand | cat" {
    run "./dsh" <<EOF
nosuchcommand | cat
EOF

    if ! grep -q 'No such file or directory' <<< "$output"; then
        echo "Error message 'No such file or directory' not found in output:"
        echo "$output"
        false 
    fi

    [ "$status" -eq 0 ]
}

@test "Pipes: Multiple pipes - ls -l | grep dsh | wc -l" {
    run "./dsh" <<EOF
ls -l | grep dsh | wc -l
EOF

    echo "Raw output: '$output'"

    actual_output=$(echo "$output" | grep -Eo '^[[:space:]]*[0-9]+' | tail -n1 | tr -d '[:space:]')
    expected_output=$(ls -l | grep dsh | wc -l | tr -d '[:space:]')

    echo "Expected: '$expected_output'"
    echo "Actual: '$actual_output'"

    [ "$actual_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Pipes: Too many pipes" {
    run "./dsh" <<EOF
echo a | echo b | echo c | echo d | echo e | echo f | echo g | echo h | echo i | echo j | echo k | echo l | echo m | echo n | echo o | echo p | echo q
EOF

    if ! grep -q "$CMD_ERR_PIPE_LIMIT" <<< "$output"; then
        echo "Expected error '$CMD_ERR_PIPE_LIMIT' not found in output:"
        echo "$output"
        false
    fi

    [ "$status" -ne 0 ]
}

@test "Pipes: Memory allocation failure" {
    run "./dsh" <<EOF
some_command_that_causes_memory_failure
EOF

    echo "Actual exit status: $status"

    [ "$status" -eq 0 ]
}