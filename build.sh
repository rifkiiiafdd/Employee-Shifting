#!/bin/bash

echo "Cleaning project..."
make clean

echo "Generating compile_commands.json and building project..."
bear -- make

if [ $? -eq 0 ]; then
    echo "Build successful. Running doctor_scheduler..."
   ./build/doctor_scheduler
else
    echo "Build failed."
fi
