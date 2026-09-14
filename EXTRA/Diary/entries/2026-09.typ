#import "../lib/records/entry.typ": entry
#import "../lib/records/comment.typ": comment
#import "../lib/records/author.typ": author

#let me = author("someone")

#let note = entry(
  [

  ],
  id: "2026-09-14-note",
  title: "note",
  date: datetime(year: 2026, month: 9, day: 14),
)

#let comment1 = comment(
  target: "prologue",
  author: me,
  date: datetime(year: 2026, month: 9, day: 14),
  [
    this is a test comment
  ],
)


#let entries = (note,)
#let comments = (comment1,)
