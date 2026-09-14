// A signature stays with the first paragraph; the rest may span pages.
#let render-comment(reply, theme) = block(
  width: 100%, above: theme.spacing.section, below: 0pt, breakable: true,
  fill: theme.colors.header-fill,
  inset: 12pt,
  stroke: (left: 2pt + theme.colors.accent),
)[
  #set text(size: theme.comment-size)
  #set par(justify: false)
  #block(sticky: true, below: 5pt)[
    #text(fill: theme.colors.muted, size: 8.5pt)[
      #reply.author.name · #(theme.locale.date)(reply.date)
      #if reply.author.contact != none [ · #reply.author.contact]
    ]
  ]
  #{
    set heading(offset: 3, outlined: false, bookmarked: false)
    reply.body
  }
]
