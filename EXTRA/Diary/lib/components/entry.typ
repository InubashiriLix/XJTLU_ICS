#import "comment.typ": render-comment

// Accepts assembled entries. Variants affect presentation, never the records.
#let render-entry(item, theme, variant: "prose") = {
  assert(variant in ("journal", "prose"), message: "Unknown entry variant: " + variant)
  let date = (theme.locale.date)(item.date)
  let title = if item.title == none { date } else { item.title }
  let headline = if variant == "journal" { date } else { title }

  [
    #block(sticky: true, above: 24pt, below: 15pt)[
      #heading(level: 2, numbering: none)[#headline]#label("entry:" + item.id)
      #if item.title != none [
        #text(fill: theme.colors.muted, size: 9pt)[
          #(if variant == "journal" { item.title } else { date })
        ]
      ]
    ]

    #{
      set heading(offset: 2)
      item.body
    }

    #if item.comments.len() > 0 [
      #pagebreak(weak: false)
      #block(sticky: true, above: 18pt, below: 0pt)[
        #text(size: 13pt, weight: "bold", fill: theme.colors.accent)[#theme.locale.comments]
        #v(6pt)
        #text(size: 10pt, fill: theme.colors.muted)[#title]
      ]

      #for reply in item.comments [
        #render-comment(reply, theme)
      ]
    ]
  ]
}
