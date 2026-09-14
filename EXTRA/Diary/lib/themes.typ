#import "locale.typ": get-locale

// Shared tokens remain available to standalone component previews.
#let colors = (
  ink: rgb("#292622"),
  muted: rgb("#777069"),
  border: rgb("#c9beb1"),
  accent: rgb("#785a43"),
  header-fill: rgb("#f3efe8"),
)
#let spacing = (table-cell: 7pt, section: 12pt)

#let get-theme(mode, lang: "en") = {
  assert(mode in ("book", "screen"), message: "Unknown diary mode: " + mode)
  let book = mode == "book"
  (
    mode: mode,
    locale: get-locale(lang),
    colors: colors,
    spacing: spacing,
    paper: "a4",
    margin: 24mm,
    background: if book { white } else { rgb("#faf8f3") },
    font: ("Libertinus Serif", "Noto Serif CJK SC"),
    body-size: 10.5pt,
    leading: .65em,
    paragraph-gap: .65em,
    comment-size: 9.5pt,
    month-break: book,
  )
}
