#import "str-validator.typ": str-optional-validator, str-validator

// Validate a required integer and return it unchanged.
//
// Named arguments:
// - int (int): Value to validate. Must be an integer. Although the default is
//   none, omitting it is a validation error.
// - field-name (str): Field label used in error messages. Must be a string that
//   is nonblank after trimming; omitting it is a validation error.
// - from (str | none): Optional origin, such as "people". A nonblank origin is
//   trimmed and prepended to the field label with a dot. none or a blank string
//   leaves the field label unqualified. Defaults to none.
//
// Returns: The unchanged int value.
//
// Validation errors (checked in this order):
// 1. Invalid field-name: "field-name must be str" or
//    "field-name must not be blank".
// 2. Invalid from: "from must be str or none".
// 3. Invalid int: "<field> must be int", where <field> is the trimmed
//    field-name, optionally prefixed by from.
//
// Examples (inside a Typst code block):
//   int-validator(int: 3, field-name: "age") // Returns 3.
//   int-validator(int: 3.5, field-name: "age") // Fails: "age must be int".
//   int-validator(field-name: "age") // Fails: "age must be int".
#let int-validator(val: none, field-name: none, from: none) = {
  // The `int` argument shadows Typst's built-in integer type.
  let int-type = type(1)

  // Reuse str-validator to validate and trim field-name and from.
  let field-name = str-validator(str: field-name, field-name: "field-name")
  let from = str-optional-validator(str: from, field-name: "from")

  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(type(val) == int-type, message: field + " must be int")

  return val
}

// Validate an optional integer and return it unchanged, or none.
//
// Named arguments:
// - int (int | none): Value to validate. Must be an integer or none. Defaults
//   to none.
// - field-name (str): Field label used in error messages. Must be a string that
//   is nonblank after trimming; omitting it is a validation error.
// - from (str | none): Optional origin, such as "people". A nonblank origin is
//   trimmed and prepended to the field label with a dot. none or a blank string
//   leaves the field label unqualified. Defaults to none.
//
// Returns: none when int is none; otherwise the unchanged int value.
//
// Validation errors (checked in this order):
// 1. Invalid field-name: "field-name must be str" or
//    "field-name must not be blank".
// 2. Invalid from: "from must be str or none".
// 3. Invalid int: "<field> must be int or none".
//
// Examples (inside a Typst code block):
//   optional-int-validator(int: 3, field-name: "age") // Returns 3.
//   optional-int-validator(field-name: "age") // Returns none.
#let int-optional-validator(val: none, field-name: none, from: none) = {
  let int-type = type(1)

  let field-name = str-validator(str: field-name, field-name: "field-name")
  let from = str-optional-validator(str: from, field-name: "from")

  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(val == none or type(val) == int-type, message: field + " must be int or none")
  if val == none {
    return none
  }

  int-validator(int: val, field-name: field-name, from: from)

  return val
}
