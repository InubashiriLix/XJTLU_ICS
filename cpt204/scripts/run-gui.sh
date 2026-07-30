#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

JAVAFX_JDK="${JAVAFX_JDK:-/opt/stm32cubeclt/jre}"
JAVA_BIN="$JAVAFX_JDK/bin/java"

if [ ! -x "$JAVA_BIN" ]; then
  echo "JavaFX JDK not found. Set JAVAFX_JDK to a JDK that includes JavaFX."
  exit 1
fi

if [ ! -f out/cpt204/inspection/gui/GuiMain.class ]; then
  bash scripts/compile-gui.sh
fi

"$JAVA_BIN" \
  --add-modules javafx.controls \
  -cp out \
  cpt204.inspection.gui.GuiMain
