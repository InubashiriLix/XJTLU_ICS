#import "../utils/date.typ": date-only
#import "author.typ": author as make-author

// body: content; author: {name: str, contact: str | none}; date: datetime.
// target: nonblank entry ID (required).
// Example: comment(target: "first-day", author: person, date: day)[A reply.]
// Returns {target, date, author, body}; assemble resolves ownership.
#let comment(body, target: none, author: none, date: none) = {
  assert(type(body) == content, message: "comment.body must be content")
  assert(target != none, message: "comment.target is required")
  assert(type(target) == str, message: "comment.target must be str")
  let target = target.trim()
  assert(target != "", message: "comment.target must not be blank")
  assert(date != none, message: "comment.date is required")
  assert(type(date) == datetime, message: "comment.date must be datetime")
  assert(date.year() != none, message: "comment.date must contain year, month, and day")
  assert(type(author) == dictionary, message: "comment.author must be an author dictionary")
  assert("name" in author, message: "comment.author.name is required")
  assert("contact" in author, message: "comment.author.contact is required")

  // Reuse the author's own constructor so its rules live in one place.
  let author = make-author(author.name, contact: author.contact)
  let date = date-only(date)
  (
    target: target,
    date: date,
    author: author,
    body: body,
  )
}
