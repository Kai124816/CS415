#!/bin/bash

# Build the project
echo "Building project with make..."
make park || { echo "Build failed. Exiting."; exit 1; }

echo ""
echo "Starting simulation runs..."

# Define test runs with different combinations
runs=(
  "-n 10 -c 2 -p 4 -w 5 -r 6"
  "-n 20 -c 3 -p 5 -w 7 -r 9"
  "-n 15 -c 2 -p 6 -w 4 -r 8"
  "-n 25 -c 4 -p 3 -w 6 -r 10"
  "-n 30 -c 5 -p 5 -w 8 -r 12"
)

# Execute each test run
for i in {0..4}; do
  echo ""
  echo "-------------------"
  echo "Run $((i + 1)): ./park ${runs[$i]}"
  echo "-------------------"
  ./park ${runs[$i]}
  echo ""
  sleep 2  # brief pause between runs (optional)
done

echo "All test runs complete."
