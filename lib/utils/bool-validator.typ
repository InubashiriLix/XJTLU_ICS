#import "str-validator.typ": str-optional-validator, str-validator

// Validate a required boolean and return it unchanged.
//
// Named arguments:
// - val (bool): Value to validate. Must be a boolean. Although the default is
//   none, omitting it is a validation error.
// - field-name (str): Field label used in error messages. Must be a string that
//   is nonblank after trimming; omitting it is a validation error.
// - from (str | none): Optional origin, such as "relation". A nonblank origin
//   is trimmed and prepended to the field label with a dot. none or a blank
//   string leaves the field label unqualified. Defaults to none.
//
// Returns: The unchanged bool value.
//
// Validation errors (checked in this order):
// 1. Invalid field-name: "field-name must be str" or
//    "field-name must not be blank".
// 2. Invalid from: "from must be str or none".
// 3. Invalid val: "<field> must be bool", where <field> is the trimmed
//    field-name, optionally prefixed by from.
//
// Examples (inside a Typst code block):
//   bool-validator(val: true, field-name: "display") // Returns true.
//   bool-validator(field-name: "display") // Fails: "display must be bool".
#let bool-validator(val: none, field-name: none, from: none) = {
  let bool-type = type(true)

  // Reuse str-validator to validate and trim field-name and from.
  let field-name = str-validator(str: field-name, field-name: "field-name")
  let from = str-optional-validator(str: from, field-name: "from")

  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(type(val) == bool-type, message: field + " must be bool")

  return val
}

// Validate an optional boolean and return it unchanged, or none.
//
// Named arguments:
// - val (bool | none): Value to validate. Must be a boolean or none. Defaults
//   to none.
// - field-name (str): Field label used in error messages. Must be a string that
//   is nonblank after trimming; omitting it is a validation error.
// - from (str | none): Optional origin, such as "relation". A nonblank origin
//   is trimmed and prepended to the field label with a dot. none or a blank
//   string leaves the field label unqualified. Defaults to none.
//
// Returns: none when val is none; otherwise the unchanged bool value.
//
// Validation errors (checked in this order):
// 1. Invalid field-name: "field-name must be str" or
//    "field-name must not be blank".
// 2. Invalid from: "from must be str or none".
// 3. Invalid val: "<field> must be bool or none".
//
// Examples (inside a Typst code block):
//   optional-bool-validator(val: false, field-name: "display") // Returns false.
//   optional-bool-validator(field-name: "display") // Returns none.
#let optional-bool-validator(val: none, field-name: none, from: none) = {
  let bool-type = type(true)

  let field-name = str-validator(str: field-name, field-name: "field-name")
  let from = str-optional-validator(str: from, field-name: "from")

  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(val == none or type(val) == bool-type, message: field + " must be bool or none")
  if val == none {
    return none
  }

  bool-validator(val: val, field-name: field-name, from: from)

  return val
}
