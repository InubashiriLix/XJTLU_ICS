#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

JAVAFX_JDK="${JAVAFX_JDK:-/opt/stm32cubeclt/jre}"
JAVAC_BIN="$JAVAFX_JDK/bin/javac"

if [ ! -x "$JAVAC_BIN" ]; then
  echo "JavaFX JDK not found. Set JAVAFX_JDK to a JDK that includes JavaFX."
  exit 1
fi

mkdir -p out
"$JAVAC_BIN" \
  --add-modules javafx.controls \
  -encoding UTF-8 \
  -d out \
  $(find src src-javafx -name "*.java")

echo "GUI compile successful. Class files are in ./out"
