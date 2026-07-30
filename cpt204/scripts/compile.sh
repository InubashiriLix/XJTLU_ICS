#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

mkdir -p out
javac -encoding UTF-8 -d out $(find src -name "*.java")

echo "Compile successful. Class files are in ./out"
