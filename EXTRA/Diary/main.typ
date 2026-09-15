#import "lib/layout.typ": diary

#import "entries/Prologue.typ" as prologue
#import "entries/2026-09.typ" as september
#import "entries/2026-08.typ" as august
#import "entries/novels/what_have_i_done.typ" as what_have_i_done

#let entries = prologue.entries + september.entries + august.entries + what_have_i_done.entries
#let comments = prologue.comments + september.comments + august.comments + what_have_i_done.comments

#diary(
  entries,
  comments,
  mode: sys.inputs.at("mode", default: "screen"),
  variant: sys.inputs.at("variant", default: "prose"),
  lang: sys.inputs.at("lang", default: "en"),
  entry-break: sys.inputs.at("entry-break", default: "true") == "true",
)
