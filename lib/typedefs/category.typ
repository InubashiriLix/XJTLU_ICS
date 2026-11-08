// note: register the types first
#import "type.typ": lib-insert-type
#lib-insert-type("category")
#lib-insert-type("category-branch")
#lib-insert-type("category-tree")
// end note

// Category tree shorthand.
//
// Declare the category tree once. `category` declares a leaf category and
// `category-branch` declares a branch with a `branch-id` plus nested children.
// `mk-category-tree` accepts `category` and `category-branch` children.
//
//   #let categories-tree = mk-category-tree(
//     category(id: "branch"),
//     category(id: "daily"),
//     category-branch(
//       branch-id: "work",
//       category(id: "school-study"),
//       category(id: "science-research"),
//       category-branch(
//         branch-id: "jupiter",
//         category(id: "fpga-development"),
//       ),
//     ),
//   )
//
// Every branch (and the root) gets an `undefined` bucket appended to its
// children. It stores nodes that only name the branch id without narrowing down
// to a specific category. The root additionally gets a `not-match` bucket for
// nodes that match no branch or category. `undefined` and `not-match` are
// reserved ids, may repeat, and are kept out of `by-id`.
//
// Every child is normalized to a branch `(root: category, children: array)`, so
// leaves end up with an empty children array. The result is
// `(root: category, children: array, by-id: dictionary)`, where `by-id` maps
// each category id to its branch node and never contains the reserved ids.

#import "../utils/str-validator.typ": str-validator

// IDs reserved for the tree root and the automatically added buckets.
#let reserved-ids = ("root", "undefined", "not-match")

// Build a reserved category record, bypassing the user-facing id check.
#let reserved-category(id) = (id: id)

// register the cat

// Category responsible for filtering nodes.
//
// Declares a leaf category. Returns `(id: id)`.
#let category(id: none) = {
  let id = str-validator(str: id, field-name: "id", from: "category")
  assert(id not in reserved-ids, message: "category ID '" + id + "' is reserved")

  (id: id, kind: "category")
}

// Normalize one child produced by `category` or `category-branch`.
//
// Leaves become branches with an empty children array; branches pass through.
#let normalize-child(child) = {
  assert(
    type(child) == dictionary,
    message: "category children must be categories or category branches",
  )

  if "root" in child or "children" in child {
    assert(
      "root" in child and "children" in child,
      message: "category children must be category branches",
    )
    assert(
      type(child.root) == dictionary,
      message: "category branch root must be a category",
    )
    assert(
      "id" in child.root,
      message: "category branch root must be a category",
    )
    assert(
      type(child.children) == array,
      message: "category branch children must be an array",
    )
    child
  } else {
    assert(
      "id" in child,
      message: "category children must be categories or category branches",
    )
    (root: child, children: ())
  }
}

// A branch in the category tree.
//
// `branch-id` names the branch category itself. Children must be results of
// `category` or `category-branch`. An `undefined` bucket is appended so nodes
// annotated with this branch id but no specific category have a home.
// Returns `(root: category, children: array)`.
#let category-branch(branch-id: none, ..children) = {
  assert(
    children.named().len() == 0,
    message: "category-branch received unknown named arguments",
  )
  let branch-id = str-validator(
    str: branch-id,
    field-name: "branch-id",
    from: "category-branch",
  )
  let children = children.pos().map(normalize-child)
  children.push((root: reserved-category("undefined"), children: ()))

  (root: category(id: branch-id), children: children, kind: "category-branch")
}

// Recursively register category ids and normalize the tree branches.
//
// Returns `(children: array, by-id: dictionary)`. `by-id` maps each category id
// to its branch node `(root:, children:)`. The reserved `"undefined"` and
// `"not-match"` buckets may repeat across branches and are omitted from
// `by-id`; every other id must be unique and must not be `"root"`.
#let index-category-children(children, by-id: (:)) = {
  assert(type(children) == array, message: "category children must be an array")

  let normalized = ()
  for child in children {
    assert(
      type(child) == dictionary,
      message: "category children must be category branches",
    )
    assert(
      "root" in child and "children" in child,
      message: "category children must be category branches",
    )
    assert(
      type(child.root) == dictionary and "id" in child.root,
      message: "category branch root must be a category",
    )

    let id = child.root.id
    let is-bucket = id == "undefined" or id == "not-match"
    if not is-bucket {
      assert(id != "root", message: "category ID 'root' is reserved for the tree root")
      assert(not (id in by-id), message: "Duplicate category ID: " + id)
    }

    let result = index-category-children(child.children, by-id: by-id)
    by-id = result.by-id
    let node = (root: child.root, children: result.children)
    if not is-bucket {
      by-id.insert(id, node)
    }
    normalized.push(node)
  }

  (children: normalized, by-id: by-id)
}

// Build the category tree.
//
// Accepts top-level `category` and `category-branch` children. The root gets an
// `undefined` bucket like every branch, plus a `not-match` bucket for nodes that
// match no branch or category. Returns
// `(root: category, children: array, by-id: dictionary)`.
#let mk-category-tree(..children) = {
  assert(
    children.named().len() == 0,
    message: "mk-category-tree received unknown named arguments",
  )

  let root = reserved-category("root")
  let children = children.pos().map(normalize-child)
  children.push((root: reserved-category("undefined"), children: ()))
  children.push((root: reserved-category("not-match"), children: ()))
  let result = index-category-children(children)

  (root: root, children: result.children, by-id: result.by-id, kind: "category-tree")
}
