// Pure conversion: callers must first validate a datetime containing a date.
// Explicitly discards time-of-day; does not perform record-specific validation.
#let date-only(value) = datetime(
  year: value.year(),
  month: value.month(),
  day: value.day(),
)
