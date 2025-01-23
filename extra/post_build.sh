#!/bin/bash

# Define source and destination directories
SOURCE_DIR="./build/assignments/Debug"
DEST_DIR="./public/assets/demos"

# Create the destination directory if it doesn't exist
mkdir -p "$DEST_DIR"

# Copy all .js and .wasm files from source to destination
cp "$SOURCE_DIR"/*.js "$SOURCE_DIR"/*.wasm "$DEST_DIR" 2>/dev/null

# Check if the copy command was successful
if [ $? -eq 0 ]; then
    echo "Files copied successfully to $DEST_DIR"
else
    echo "No .js or .wasm files found in $SOURCE_DIR"
fi