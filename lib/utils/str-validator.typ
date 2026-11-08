// Validate a required string and return it with surrounding whitespace removed.
// Internal whitespace is preserved. Invalid arguments fail with an assertion.
//
// Named arguments:
// - str (str): Value to validate. Must be a string that is nonblank after
//   trimming. Although the default is none, omitting it is a validation error.
// - field-name (str): Field label used in error messages. Must be a string that
//   is nonblank after trimming; omitting it is a validation error.
// - from (str | none): Optional origin, such as "people". A nonblank origin is
//   trimmed and prepended to the field label with a dot. none or a blank string
//   leaves the field label unqualified. Defaults to none.
//
// Returns: The trimmed, nonblank str value.
//
// Validation errors (checked in this order):
// 1. Invalid field-name: "field-name must be str" or
//    "field-name must not be blank".
// 2. Invalid from: "from must be str or none".
// 3. Invalid str: "<field> must be str" or "<field> must not be blank",
//    where <field> is the trimmed field-name, optionally prefixed by from.
//
// Examples (inside a Typst code block):
//   str-validator(str: "  Alice  ", field-name: "name") // Returns "Alice".
//   str-validator(str: "Alice  Smith", field-name: "name") // Keeps both spaces.
//   str-validator(str: " ", field-name: "name", from: "people")
//   // Fails: "people.name must not be blank".
//   str-validator(field-name: "name") // Fails: "name must be str".
#let str-validator(str: none, field-name: none, from: none) = {
  // The `str` argument shadows Typst's built-in string type.
  let string-type = type("")
  assert(type(field-name) == string-type, message: "field-name must be str")
  let field-name = field-name.trim()
  assert(field-name != "", message: "field-name must not be blank")

  assert(from == none or type(from) == string-type, message: "from must be str or none")
  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(type(str) == string-type, message: field + " must be str")
  let str = str.trim()
  assert(str != "", message: field + " must not be blank")

  str
}

#let str-optional-validator(str: none, field-name: none, from: none) = {
  let string-type = type("")
  assert(type(field-name) == string-type, message: "field-name must be str")
  let field-name = field-name.trim()
  assert(field-name != "", message: "field-name must not be blank")

  assert(from == none or type(from) == string-type, message: "from must be str or none")
  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(str == none or type(str) == string-type, message: field + " must be str or none")
  if str == none {
    return none
  }

  str-validator(str: str, field-name: field-name, from: from)
}
