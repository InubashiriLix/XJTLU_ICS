#import "../typedefs/category.typ"
#import "../typedefs/node.typ"

#import "../utils/str-validator.typ": str-optional-validator, str-validator
#import "../utils/int-validator.typ": int-optional-validator, int-validator

#let from-name = "mk-title"

#let mk-title(title: none, id: none, categories: none, tags: none) = {
  let title = str-validator(str: node.title, field-name: "node.title", from: from-name)
  let id = str-validator(str: node.id, field-name: "node.id", from: from-name)
}
