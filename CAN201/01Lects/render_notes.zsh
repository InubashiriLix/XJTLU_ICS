#!/usr/bin/env zsh

set -euo pipefail
setopt extended_glob null_glob

usage() {
  cat <<'EOF'
Usage: ./render_notes.zsh [-s font_size] [-b baseline_skip] [-m margin]

  -s  Base font size in points (default: 5)
  -b  Baseline skip in points (default: 1.1 × font size)
  -m  Page margin (passed to geometry, default: 1.5cm)

Examples:
  ./render_notes.zsh -s 4.5
  ./render_notes.zsh -s 6 -b 6.8 -m 1.2cm
EOF
}

FONT_SIZE="8"
BASELINE=""
MARGIN="1.5cm"

while getopts "s:b:m:h" opt; do
  case "$opt" in
    s) FONT_SIZE="$OPTARG" ;;
    b) BASELINE="$OPTARG" ;;
    m) MARGIN="$OPTARG" ;;
    h)
      usage
      exit 0
      ;;
    *)
      usage >&2
      exit 1
      ;;
  esac
done

if [[ -z "$BASELINE" ]]; then
  BASELINE="$(python3 - "$FONT_SIZE" <<'PY'
import sys
font_size = float(sys.argv[1])
print(f"{font_size * 1.1:.2f}")
PY
)"
fi

SCRIPT_DIR=$(cd -- "$(dirname -- "${0}")" && pwd)
cd "$SCRIPT_DIR"

HEADER="pandoc-header.tex"
if [[ ! -f "$HEADER" ]]; then
  echo "Cannot find $HEADER in $SCRIPT_DIR" >&2
  exit 1
fi

TEMP_HEADER=$(mktemp)
trap 'rm -f "$TEMP_HEADER"' EXIT

perl -0pe "s/\\\\AtBeginDocument\\{\\\\fontsize\\{[0-9.]+\\}\\{[0-9.]+\\}\\\\selectfont\\}/\\\\AtBeginDocument{\\\\fontsize{$FONT_SIZE}{$BASELINE}\\\\selectfont}/" \
  "$HEADER" > "$TEMP_HEADER"

export XDG_CACHE_HOME="$SCRIPT_DIR/.cache"
export FONTCONFIG_FILE="$SCRIPT_DIR/fontconfig-local.conf"
mkdir -p "$XDG_CACHE_HOME"

PANDOC_ARGS=(
  --pdf-engine=xelatex
  -H "$TEMP_HEADER"
  -V "mainfont=JetBrainsMono Nerd Font Mono"
  -V "sansfont=JetBrainsMono Nerd Font Mono"
  -V "monofont=JetBrainsMono Nerd Font Mono"
  -V "CJKmainfont=Noto Sans CJK SC"
  -V "geometry:margin=${MARGIN}"
  -V classoption=twocolumn
  -V papersize=a4
)

org_files=( *.org )
if (( ${#org_files[@]} == 0 )); then
  echo "No .org files found in $SCRIPT_DIR" >&2
  exit 1
fi

for org in "${org_files[@]}"; do
  pdf="${org%.org}.pdf"
  echo "Converting $org -> $pdf (font ${FONT_SIZE}pt / baseline ${BASELINE}pt)"
  pandoc "$org" -o "$pdf" "${PANDOC_ARGS[@]}"
done
