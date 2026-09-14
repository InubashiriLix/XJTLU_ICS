#import "entry.typ": render-entry

// Compatibility presentation helper: accepts the same assembled entry as render-entry.
#let render-novel(item, theme) = render-entry(item, theme, variant: "prose")
