#import "str-validator.typ": str-validator

// Validate an optional array of nonblank strings.
//
// Named arguments:
// - strs (array | none): Values to validate. Defaults to none. Empty arrays
//   are accepted; every element of a nonempty array must be a nonblank string.
// - field-name (str): Required, nonblank field label for error messages.
// - from (str | none): Optional origin prefix, such as "people".
//   none or a blank string leaves the field label unqualified.
//
// Returns: none when strs is none; otherwise a new array of trimmed strings.
// Order, duplicates, and internal whitespace are preserved.
// Invalid arguments fail with an assertion, including invalid field-name or
// from arguments when strs is none or empty. Element errors use zero-based
// indices, for example "people.alias[1] must not be blank".
//
// Examples (inside a Typst code block):
//   str-array-validator(strs: (" Alice ", " Bob "), field-name: "alias")
//   // Returns ("Alice", "Bob").
//   str-array-validator(field-name: "alias") // Returns none.
//   str-array-validator(strs: (), field-name: "alias") // Returns ().
#let str-array-validator(strs: none, field-name: none, from: none) = {
  assert(type(field-name) == str, message: "field-name must be str")
  let field-name = field-name.trim()
  assert(field-name != "", message: "field-name must not be blank")

  assert(from == none or type(from) == str, message: "from must be str or none")
  let field = if from == none or from.trim() == "" {
    field-name
  } else {
    from.trim() + "." + field-name
  }

  assert(strs == none or type(strs) == array, message: field + " must be array or none")
  if strs == none {
    return none
  }

  strs
    .enumerate()
    .map(((index, value)) => str-validator(
      str: value,
      field-name: field + "[" + str(index) + "]",
    ))
}

