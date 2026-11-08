#import "type.typ": lib-insert-type
#lib-insert-type("relation")

#import "../utils/str-validator.typ": str-validator
#import "../utils/bool-validator.typ": bool-validator

#let from-name = "relation"

#let relation(src-id: none, dest-id: none, display: true, level: 2) = {
  str-validator(field-name: "src-id", from: from-name, str: src-id)
  str-validator(field-name: "dest-id", from: from-name, str: dest-id)

  display = bool-validator(val: display, field-name: "display", from: from-name)
  level = int-validator(val: level, field-name: "level", from: from-name)

  (src-id: src-id, dest-id: dest-id, display: display, level: level, kind: "relation")
}


