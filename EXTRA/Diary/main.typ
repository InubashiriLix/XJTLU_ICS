#import "lib/layout.typ": diary
#import "entries/Prologue.typ" as prologue
#import "entries/2026-09.typ" as september

#let entries = prologue.entries + september.entries
#let comments = prologue.comments + september.comments

#diary(
  entries,
  comments,
  mode: sys.inputs.at("mode", default: "book"),
  variant: sys.inputs.at("variant", default: "prose"),
  lang: sys.inputs.at("lang", default: "en"),
  entry-break: sys.inputs.at("entry-break", default: "true") == "true",
)
