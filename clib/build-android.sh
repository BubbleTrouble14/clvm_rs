#!/bin/bash

# Set the Android NDK path (change this to your NDK path)
ANDROID_NDK_PATH=~/Library/Android/sdk/ndk/23.1.7779620

# Set the Rust targets for Android
RUST_TARGETS=(
  aarch64-linux-android
  armv7-linux-androideabi
  x86_64-linux-android
  i686-linux-android
)

# Set the target Android API level (e.g., 21 for Android 5.0)
ANDROID_API_LEVEL=21

# Set the Rust toolchain (e.g., nightly, stable, etc.)
RUST_TOOLCHAIN=nightly

# Set the path to your Rust toolchain (usually installed in ~/.rustup/toolchains/)
RUST_TOOLCHAIN_PATH=~/.rustup/toolchains/$RUST_TOOLCHAIN

# Set the project directory
PROJECT_DIR=..
WORKSPACE_DIR=$PWD
# # Create the libs directory
LIBS_DIR=$WORKSPACE_DIR/libs
mkdir -p $LIBS_DIR

# Ensure the NDK path is set
if [ -z "$ANDROID_NDK_PATH" ]; then
  echo "Error: Please set the ANDROID_NDK_PATH variable to your Android NDK path."
  exit 1
fi

# Ensure the Android API level is set
if [ -z "$ANDROID_API_LEVEL" ]; then
  echo "Error: Please set the ANDROID_API_LEVEL variable to the target Android API level."
  exit 1
fi

# Ensure the Rust toolchain is set
if [ -z "$RUST_TOOLCHAIN" ]; then
  echo "Error: Please set the RUST_TOOLCHAIN variable to your desired Rust toolchain."
  exit 1
fi

# Build the Rust code for each target
for RUST_TARGET in "${RUST_TARGETS[@]}"; do
  echo "🏗  Building libclvm_c.a for $RUST_TARGET"

  # Handle special naming case for armv7
  if [ "$RUST_TARGET" == "armv7-linux-androideabi" ]; then
      LINKER_TARGET="armv7a-linux-androideabi"
  else
      LINKER_TARGET="$RUST_TARGET"
  fi

  # Set up the Rust environment
  export RUSTFLAGS="-C linker=$ANDROID_NDK_PATH/toolchains/llvm/prebuilt/darwin-x86_64/bin/$LINKER_TARGET$ANDROID_API_LEVEL-clang"

  # Build the Rust code
  cargo build --target $RUST_TARGET --release

  echo "✅  Library build complete for $RUST_TARGET."
done

# Copy the .a files to the respective ABI folders
for RUST_TARGET in "${RUST_TARGETS[@]}"; do
    if [ "$RUST_TARGET" == "aarch64-linux-android" ]; then
        ABI_DIR="arm64-v8a"
    elif [ "$RUST_TARGET" == "armv7-linux-androideabi" ]; then
        ABI_DIR="armeabi-v7a"
    elif [ "$RUST_TARGET" == "x86_64-linux-android" ]; then
        ABI_DIR="x86_64"
    elif [ "$RUST_TARGET" == "i686-linux-android" ]; then
        ABI_DIR="x86"
    else
        echo "Unknown RUST_TARGET: $RUST_TARGET"
        continue
    fi

    mkdir -p $LIBS_DIR/$ABI_DIR
    echo $ABI_DIR    $RUST_TARGET
    cp $PROJECT_DIR/target/$RUST_TARGET/release/lib*.a $LIBS_DIR/$ABI_DIR/
done

# Generate the header file using cbindgen
cbindgen --config cbindgen.toml --crate clvm --output $LIBS_DIR/clvm.h

echo "✅  All .a files and the header file have been copied to the libs directory."
