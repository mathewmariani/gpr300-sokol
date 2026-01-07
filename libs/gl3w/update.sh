#!/usr/bin/env bash
set -e

# Resolve directory where this script lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# URLs
GL3W_GEN_URL="https://raw.githubusercontent.com/skaslev/gl3w/refs/heads/master/gl3w_gen.py"

# Temp file for the generator
GENERATOR="$(mktemp --suffix=.py)"

echo "Downloading gl3w_gen.py..."
curl -fsSL "$GL3W_GEN_URL" -o "$GENERATOR"

echo "Running gl3w_gen.py..."
python3 "$GENERATOR" --root "$SCRIPT_DIR"

echo "Cleaning up..."
rm -f "$GENERATOR"

echo "Done. Files are in $SCRIPT_DIR"