#import "type.typ": lib-insert-type
#lib-insert-type("people")

#import "../utils/str-validator.typ": str-validator
#import "../utils/str-array-or-none-validate.typ": str-array-validator

#let people(
  id: none,
  name: none,
  contact-emails: none,
  contact-phones: none,
  contact-qqs: none,
  personal-pages: none,
  alias: none,
) = {
  let id = str-validator(str: id, field-name: "id", from: "people")
  let name = str-validator(str: name, field-name: "name", from: "people")
  let contact-emails = str-array-validator(strs: contact-emails, field-name: "contact-emails", from: "people")
  let contact-phones = str-array-validator(strs: contact-phones, field-name: "contact-phones", from: "people")
  let contact-qqs = str-array-validator(strs: contact-qqs, field-name: "contact-qqs", from: "people")
  let personal-pages = str-array-validator(strs: personal-pages, field-name: "personal-pages", from: "people")
  let alias = str-array-validator(strs: alias, field-name: "alias", from: "people")

  (
    id: id,
    name: name,
    contact-emails: contact-emails,
    contact-phones: contact-phones,
    contact-qqs: contact-qqs,
    personal-pages: personal-pages,
    alias: alias,
    kind: "people",
  )
}
