#import "../utils/date.typ": date-only

// body: content; date: datetime containing a calendar date (required).
// id: nonblank str (required); title: str | none.
// Example: entry(id: "first-day", date: day)[Today.]
// Returns {id, date, title, body}; assemble attaches comments by ID.
#let entry(body, id: none, date: none, title: none) = {
  assert(type(body) == content, message: "entry.body must be content")
  assert(id != none, message: "entry.id is required")
  assert(type(id) == str, message: "entry.id must be str")
  let id = id.trim()
  assert(id != "", message: "entry.id must not be blank")
  assert(date != none, message: "entry.date is required")
  assert(type(date) == datetime, message: "entry.date must be datetime")
  assert(date.year() != none, message: "entry.date must contain year, month, and day")
  assert(title == none or type(title) == str, message: "entry.title must be str or none")

  let date = date-only(date)
  if title != none and title.trim() == "" { title = none }

  (id: id, date: date, title: title, body: body)
}
