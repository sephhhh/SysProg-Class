#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file


@test "Exit command terminates the shell" {
    run "./dsh" <<EOF
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "List directory contents" {
    current=$(pwd)
    
    cd /tmp
    mkdir -p dsh-test && cd dsh-test
    touch file1.txt file2.txt

    run "${current}/dsh" <<EOF
ls
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="file1.txtfile2.txtdsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Print working directory" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test && cd dsh-test

    run "${current}/dsh" <<EOF
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/tmp/dsh-testdsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo with special characters" {
    run "./dsh" <<EOF
echo Hello, \$USER!
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,\$USER!dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo with multiple arguments" {
    run "./dsh" <<EOF
echo Hello World, from dsh!
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HelloWorld,fromdsh!dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}