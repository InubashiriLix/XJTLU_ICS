// note: this file is a part of the implementation of the library

// register the tag first
#import "type.typ": lib-insert-type
#lib-insert-type("tag")

#import "../utils/str-validator.typ": str-optional-validator, str-validator


// tag responding for filtering nodes
//
// id: non-blank str
// color: str
#let tag(id: none, color: none) = {
  let id = str-validator(str: id, field-name: "id", from: "tag")
  let color = str-optional-validator(str: color, field-name: "color", from: "tag")

  (id: id, color: color, kind: "tag")
}

