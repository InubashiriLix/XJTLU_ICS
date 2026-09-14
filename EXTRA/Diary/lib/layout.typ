#import "collection.typ": assemble
#import "themes.typ": get-theme
#import "components/entry.typ": render-entry

#let diary(entries, comments, mode: "screen", variant: "prose", lang: "en", title: none, entry-break: true) = {
  assert(type(entry-break) == bool, message: "entry-break must be bool")
  let theme = get-theme(mode, lang: lang)
  let locale = theme.locale
  if title == none { title = locale.title }
  assert(variant in ("journal", "prose"), message: "Unknown entry variant: " + variant)
  let items = assemble(entries, comments)

  set document(title: title)
  set page(
    paper: theme.paper,
    margin: theme.margin,
    fill: theme.background,
    header: context if counter(page).get().first() > 1 {
      let current-page = counter(page).get().first()
      let months = query(heading.where(level: 1, outlined: true)).filter(it => it.location().page() <= current-page)
      text(size: 8pt, fill: theme.colors.muted)[
        #grid(
          columns: (1fr, auto),
          column-gutter: 12pt,
          title, if months.len() > 0 { months.last().body } else { [] },
        )
      ]
    },
    footer: context align(center)[
      #text(size: 8pt, fill: theme.colors.muted)[#counter(page).display("1")]
    ],
  )
  set text(
    font: theme.font,
    size: theme.body-size,
    fill: theme.colors.ink,
    lang: lang,
    hyphenate: mode == "book" and lang == "en",
  )
  // Paragraph separation includes the normal line gap plus extra breathing room.
  set par(leading: theme.leading, spacing: theme.leading + theme.paragraph-gap, justify: mode == "book")
  set heading(numbering: none)
  show heading: set text(font: theme.font, weight: "regular")
  show heading: set par(justify: false, leading: .4em)
  show heading.where(level: 1): it => block(sticky: true, above: 20pt, below: 0pt)[
    #text(size: 10pt, fill: theme.colors.accent)[#it.body]
  ]
  show heading.where(level: 2): set text(size: 16pt)
  show heading.where(level: 2): set block(above: 0pt, below: 6pt)
  show heading.where(level: 3): set text(size: 13pt)
  show heading.where(level: 3): set block(above: 18pt, below: 8pt)
  show heading.where(level: 4): set text(size: 11pt, style: "italic")
  show heading.where(level: 4): set block(above: 12pt, below: 6pt)
  show link: set text(fill: theme.colors.accent)

  // Month groups, then day / title / page. Each complete row is clickable.
  show outline.entry.where(level: 1): it => block(sticky: true, above: 18pt, below: 9pt)[
    #link(it.element.location(), text(size: 10pt, fill: theme.colors.accent, it.body()))
  ]
  show outline.entry.where(level: 2): it => {
    let item = items.find(item => it.element.label == label("entry:" + item.id))
    let caption = if item == none { it.body() } else if item.title == none {
      (locale.date)(item.date)
    } else { item.title }
    block(above: 0pt, below: 7pt)[
      #set par(justify: false, leading: .5em)
      #link(it.element.location(), grid(
        columns: (22pt, 1fr, 22pt),
        column-gutter: 10pt,
        text(size: 9pt, fill: theme.colors.muted, if item == none { [] } else { (locale.day)(item.date) }),
        text(size: 11pt, fill: theme.colors.ink, caption),
        align(right, text(size: 9pt, fill: theme.colors.muted, it.page())),
      ))
    ]
  }

  if mode == "book" {
    page(header: none, footer: none)[
      #set par(justify: false, spacing: 0pt)
      #v(27%)
      #text(size: 42pt, weight: "regular")[#title]
      #v(18pt)
      #if items.len() > 0 [
        #text(size: 10pt, fill: theme.colors.muted)[
          #(locale.date)(items.first().date)
          #if items.first().date != items.last().date { text(" / " + (locale.date)(items.last().date)) }
        ]
      ]
    ]
  } else {
    block(above: 4pt, below: 22pt)[#text(size: 30pt)[#title]]
  }

  if items.len() == 0 {
    text(fill: theme.colors.muted, locale.empty)
  } else {
    block(above: 6pt, below: 18pt, sticky: true)[
      #text(size: if mode == "book" { 24pt } else { 15pt })[#locale.contents]
    ]
    outline(title: none, depth: 2, indent: 0pt)
    if mode == "book" { pagebreak(weak: true) } else { v(12pt) }

    let month = none
    for (index, item) in items.enumerate() {
      if index > 0 and entry-break { pagebreak(weak: true) }
      let next-month = item.date.display("[year]-[month]")
      if next-month != month {
        if month != none and theme.month-break and not entry-break { pagebreak(weak: true) }
        heading(level: 1)[#(locale.month)(item.date)]
        month = next-month
      }
      render-entry(item, theme, variant: variant)
    }
  }
}
