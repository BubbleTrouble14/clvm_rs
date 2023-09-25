#!/bin/bash

# Step 1: Build the Rust project
cargo build --target=aarch64-apple-darwin --release

cbindgen --config cbindgen.toml --crate clvm --output ./tests/clvm.h

# Check if the Rust build was successful
if [ $? -ne 0 ]; then
    echo "Rust build failed!"
    exit 1
fi

# Step 2: Compile the test app
g++ -o ./tests/main ./tests/main.cpp -L ../target/aarch64-apple-darwin/release -lclvm -lpthread -ldl

# Check if the GCC compilation was successful
if [ $? -ne 0 ]; then
    echo "GCC compilation failed!"
    exit 1
fi

# Step 3: Run the compiled application
./tests/main

# Check if the application ran successfully
if [ $? -ne 0 ]; then
    echo "Application execution failed!"
    exit 1
fi

echo "Build and execution completed successfully!"
