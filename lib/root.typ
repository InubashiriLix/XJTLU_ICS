#let mk-root(nodes: (), tags: (blank: ())) = {
  (nodes: nodes, tags: tags)
}

#let search-node(root: none, id: none) = {
  assert(type(root) == dictionary, message: "root must be a dictionary")
  assert(type(id) == string, message: "id must be a string")

  let nodes = root.nodes
  // simple iteration search O(n), maybe perf required?
  for node in nodes.values() {
    if node.id == id {
      return node
    }
  }
  return none
}

#let root = mk-root()
