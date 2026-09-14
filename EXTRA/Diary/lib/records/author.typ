// name: nonblank str; contact: str | none.
// Always returns {name, contact}, including when no contact is provided.
#let author(name, contact: none) = {
  assert(type(name) == str, message: "author.name must be str")
  assert(contact == none or type(contact) == str,
    message: "author.contact must be str or none")

  let name = name.trim()
  assert(name != "", message: "author.name must not be blank")
  if contact != none {
    contact = contact.trim()
    if contact == "" { contact = none }
  }

  (name: name, contact: contact)
}
