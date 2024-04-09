#!/bin/bash

# Navigate to the specific directory if needed
# cd /path/to/your/project

# Execute 'make clean'
make clean

# Execute 'make all'
make all

# Run the Yalnix binary with specified options
/clear/courses/comp421/pub/bin/yalnix -ly 5 yfs test/sample1
