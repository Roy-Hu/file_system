#!/bin/bash

# Navigate to the specific directory if needed
# cd /path/to/your/project


make clean

# Build all components
make all

# List of test files to run with the Yalnix binary
test_files=(
    "test/topen2"
    "test/tcreate2"
    "test/writeread"
    "test/sample1"
    # "test/communicate"
    "test/tlink"
    "test/tunlink2"
    "test/trmdir"
    "test/tchdir"
    "test/tinderect"
    "test/tinderect2"
    "test/tstat"
    "test/trelative"
)

# Loop over each test file and run it with Yalnix
for test_file in "${test_files[@]}"; do
        # Clean previous builds

    /clear/courses/comp421/pub/bin/mkyfs
    echo "Running Yalnix on $test_file..."
    /clear/courses/comp421/pub/bin/yalnix -ly 5 yfs $test_file
    # Capture the return code of the test
    rc=$?
    if [ $rc -ne 0 ]; then
        echo "$test_file failed with return code $rc"
    else
        echo "$test_file executed successfully"
    fi
    echo "--------------------------------------------------"
done
