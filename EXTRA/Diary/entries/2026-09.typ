#import "../lib/records/entry.typ": entry
#import "../lib/records/comment.typ": comment
#import "../lib/records/author.typ": author

#let me = author("InubashiriLix", contact: "https://github.com/InubashiriLix")

#let entries = (
  entry(
    [
      Okay, the first diary is bullshit I guess.
      I'm going to write something useful;
      == Symptom 1:
      === situation:
      When I'm biking home from work, I feel stressed.
      Multiple things always flash through my mind:
      - Mongoose choose to exit the program.
      - Mongoose refused multiple love expressions.
      - Mongoose used to be a good friend of mine.
        But I think I loved him.
        After multiple days of these, He admitted that he don't want to talk with me anymore;

      These things are the most painful things I have experienced.
    ],
    id: "2026-09-14-note",
    title: "2026-09-14-chores",
    date: datetime(year: 2026, month: 9, day: 14),
  ),
)

#let comments = (
  comment(
    target: "2026-09-14-note",
    author: me,
    date: datetime(year: 2026, month: 9, day: 14),
    [
      this is a test comment
    ],
  ),
)
