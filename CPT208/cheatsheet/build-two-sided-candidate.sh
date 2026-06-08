#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

sources=(
  "exam.requirement.org"
  "01_DiscoveringRequirements/main.org"
  "02_Prototyping/main.org"
  "03_ConceptualPrototypingAndPracticalGuide/main.org"
  "04_HeuristicEvaluationQuestionnairAndInterview/HeuristicEvaluation.org"
  "04_HeuristicEvaluationQuestionnairAndInterview/QuestionnairInterview.org"
  "05_PrototypingFidelity/main.org"
  "09_UsabilityTesting&ExperimentDesign/main.org"
  "10_InterfacesAndReserachCOnsiderations/main.org"
  "11_FieldStudyAndAnalytics/main.org"
)

label_for() {
  case "$1" in
    "exam.requirement.org") printf '%s\n' "Exam Requirements" ;;
    "01_DiscoveringRequirements/main.org") printf '%s\n' "Week 2 - Discovering Requirements" ;;
    "02_Prototyping/main.org") printf '%s\n' "Week 3 - Prototyping" ;;
    "03_ConceptualPrototypingAndPracticalGuide/main.org") printf '%s\n' "Week 4 - Conceptual Prototyping and Practical Guide" ;;
    "04_HeuristicEvaluationQuestionnairAndInterview/HeuristicEvaluation.org") printf '%s\n' "Week 5 - Heuristic Evaluation" ;;
    "04_HeuristicEvaluationQuestionnairAndInterview/QuestionnairInterview.org") printf '%s\n' "Week 5 - Questionnaire and Interview" ;;
    "05_PrototypingFidelity/main.org") printf '%s\n' "Week 6 - Prototyping Fidelity" ;;
    "09_UsabilityTesting&ExperimentDesign/main.org") printf '%s\n' "Week 9 - Usability Testing and Experimental Design" ;;
    "10_InterfacesAndReserachCOnsiderations/main.org") printf '%s\n' "Week 10 - Interfaces and Research Considerations" ;;
    "11_FieldStudyAndAnalytics/main.org") printf '%s\n' "Week 11 - Field Study and Analytics" ;;
    *) printf '%s\n' "$1" ;;
  esac
}

color_for() {
  case "$1" in
    "exam.requirement.org") printf '%s\n' "cptExam" ;;
    "01_DiscoveringRequirements/main.org") printf '%s\n' "cptW2" ;;
    "02_Prototyping/main.org") printf '%s\n' "cptW3" ;;
    "03_ConceptualPrototypingAndPracticalGuide/main.org") printf '%s\n' "cptW4" ;;
    "04_HeuristicEvaluationQuestionnairAndInterview/HeuristicEvaluation.org") printf '%s\n' "cptW5" ;;
    "04_HeuristicEvaluationQuestionnairAndInterview/QuestionnairInterview.org") printf '%s\n' "cptW5" ;;
    "05_PrototypingFidelity/main.org") printf '%s\n' "cptW6" ;;
    "09_UsabilityTesting&ExperimentDesign/main.org") printf '%s\n' "cptW9" ;;
    "10_InterfacesAndReserachCOnsiderations/main.org") printf '%s\n' "cptW10" ;;
    "11_FieldStudyAndAnalytics/main.org") printf '%s\n' "cptW11" ;;
    *) printf '%s\n' "cptExam" ;;
  esac
}

emit_source() {
  local src="$1"
  awk '
    /^#\+(TITLE|AUTHOR|LANGUAGE|OPTIONS|STARTUP|source_pdf|SOURCE_PDF):/ { next }
    { print }
  ' "$src" |
    awk -f cheatsheet/filter-template-subtrees.awk |
    awk -f cheatsheet/filter-cheatsheet-compact.awk
}

out_org="cheatsheet/CPT208-two-sided-candidate.org"
{
  for src in "${sources[@]}"; do
    label="$(label_for "$src")"
    color="$(color_for "$src")"
    printf '#+begin_export latex\n\\cptweekcolor{%s}{%s}\n#+end_export\n' "$color" "$label"
    if [[ "$src" == "10_InterfacesAndReserachCOnsiderations/main.org" ]]; then
      emit_source "$src" | awk -f cheatsheet/filter-week10-interface.awk
      printf '\n'
      sed -n '1,220p' cheatsheet/week10-interface-compact.org
    elif [[ "$src" == "11_FieldStudyAndAnalytics/main.org" ]]; then
      sed -n '1,220p' cheatsheet/week11-compact.org
    else
      emit_source "$src"
    fi
    printf '\n'
  done
  sed -n '1,140p' cheatsheet/image-appendix.org
} > "$out_org"

pandoc "$out_org" \
  --from=org \
  --pdf-engine=xelatex \
  --metadata-file=cheatsheet/two-sided-portrait-layout.yaml \
  --lua-filter=cheatsheet/compact-tables-tiny.lua \
  --resource-path=.:01_DiscoveringRequirements:01_DiscoveringRequirements/assets \
  -o cheatsheet/CPT208-two-sided-candidate.pdf
