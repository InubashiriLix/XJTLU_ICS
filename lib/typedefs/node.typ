#import "type.typ": lib-insert-type
#lib-insert-type("node")

#import "../utils/str-validator.typ": str-validator
#import "../utils/str-array-or-none-validate.typ": str-array-validator
#import "../typedefs/category.typ": category
#import "../typedefs/tag.typ": tag

// Build a document node and validate its metadata.
//
// `categories` and `tags` accept either one record or an array of records. Both
// values are returned as arrays so callers can iterate over them consistently.
#let node(body, id: none, title: none, authors: none, categories: none, tags: none) = {
  assert(type(id) == str, message: "node.id must be str")
  let id = id.trim()
  assert(id != "", message: "node.id must not be blank")

  assert(type(title) == content, message: "node.title must be content")
  assert(type(body) == content, message: "node.body must be content")

  let normalize-records = (value, field-name, constructor) => {
    if value == none {
      return none
    }

    let values = if type(value) == array { value } else { (value,) }
    values
      .enumerate()
      .map(((index, item)) => {
        assert(
          type(item) == dictionary,
          message: field-name + "[" + str(index) + "] must be a dictionary",
        )
        assert(
          "id" in item,
          message: field-name + "[" + str(index) + "] must contain id",
        )
        let color = if "color" in item { item.color } else { none }
        constructor(item.id, color)
      })
  }

  let categories = normalize-records(
    categories,
    "node.categories",
    (id, color) => category(id: id),
  )
  let tags = normalize-records(
    tags,
    "node.tags",
    (id, color) => tag(id: id, color: color),
  )

  lib-insert-type("node")

  (
    body: body,
    id: id,
    title: title,
    authors: authors,
    categories: categories,
    tags: tags,
    kind: "node",
  )
}
