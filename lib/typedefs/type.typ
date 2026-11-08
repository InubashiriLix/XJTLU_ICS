#import "../utils/str-validator.typ": str-validator

// Registry of the lib's kinds, shared across the whole document.
//
// The registry is backed by a document-wide state. Register kinds with
// `lib-insert-type`; read the registry inside a `context` block, for example
// `#context type-reg.get()`. Reading a state outside `context` fails, so
// registration never reads it back.
#let type-reg = state("__lib_type_reg", (:))

// Return the kind of a value as a string.
//
// Dictionaries carrying a `kind` field report that field. Everything else
// reports its built-in type name, such as "integer", "string" or "dictionary".
//
// Named arguments:
// - val: The value to inspect.
#let kind-of(val) = {
  if type(val) == dictionary and "kind" in val {
    val.kind
  } else {
    str(type(val))
  }
}

// Register a kind in `type-reg`.
//
// Named arguments:
// - kind (str): Required, nonblank kind name. Registering the same kind twice
//   fails with an assertion.
//
// State updates are evaluated lazily, so a duplicate only fails once the
// registry is actually read (for example `#context type-reg.get()`).
// Registration is append-only and never reads `type-reg` itself, so it is safe
// to call outside `context`.
#let lib-insert-type(kind) = {
  let kind = str-validator(str: kind, field-name: "kind", from: "type-append")

  type-reg.update(prev => {
    assert(kind not in prev, message: "type-append: kind is not unique: " + kind)
    let next = prev
    next.insert(kind, (kind: kind))
    next
  })
}
