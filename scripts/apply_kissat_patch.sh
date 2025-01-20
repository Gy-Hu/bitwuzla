#!/bin/bash

# Check if patch exists
if [ ! -f "patches/kissat-unsat-mode.patch" ]; then
    echo "Error: kissat-unsat-mode.patch not found in patches directory"
    exit 1
fi

# Apply the patch
patch -p1 < patches/kissat-unsat-mode.patch

if [ $? -eq 0 ]; then
    echo "Successfully applied kissat unsat mode patch"
else
    echo "Failed to apply kissat unsat mode patch"
    exit 1
fi
