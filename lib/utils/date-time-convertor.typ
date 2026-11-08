#import "str-validator.typ": str-validator

// Parse YYYY-MM-DD HH:mm, trimming surrounding whitespace and setting seconds to zero.
// Typst's datetime constructor validates calendar dates and time ranges.
#let date-time-convertor(str: none, field-name: none, from: none) = {
  let value = str-validator(str: str, field-name: field-name, from: from)
  let field = if from == none or from.trim() == "" {
    field-name.trim()
  } else {
    from.trim() + "." + field-name.trim()
  }

  assert(
    value.contains(regex("^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}$")),
    message: field + " must use YYYY-MM-DD HH:mm",
  )

  datetime(
    year: int(value.slice(0, 4)),
    month: int(value.slice(5, 7)),
    day: int(value.slice(8, 10)),
    hour: int(value.slice(11, 13)),
    minute: int(value.slice(14, 16)),
    second: 0,
  )
}
