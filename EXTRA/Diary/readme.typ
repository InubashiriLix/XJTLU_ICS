okay, this is a typst doc system

the overall flow is like this:
1. write a node at somewhere
2. register it to the root

for tag, we also manage it in the root

that is, all the nodes should be explicitly reigstered to the root
and all the tags should be explicitly registered to the root

I think it is a good strategy to manage the docs, and the docs that not desired to be managed can be ignored simply

= Tag tree shorthand

Declare the tag tree once. Use `group(...)` for branches and strings for leaf tags.
`group` is an import alias for `tag-branch`; groups can be nested.

```typ
#import "lib/typedefs/tag.typ": mk-tag-tree, tag-branch as group

#let tags = mk-tag-tree(
  group("life", "reading", "travel"),
  group("study", "math", group("programming", "Typst", "Python")),
  "thoughts",
)
```

Nodes reference tag IDs, for example `tag: "reading"`.
Groups are also tags, so `tag: "study"` is valid.
Omitting a tag leaves `node.tag` as `none`, corresponding to the automatically
added untagged branch.

- Tag IDs must be unique across the entire tree. Duplicates fail during construction.
- IDs do not contain parent paths, so moving a group preserves node references.
- The ID `"root"` is reserved for the tree root.
- To specify a color, declare a tag with `group("reading", color: "blue")`.
  Colors remain string metadata and are set independently for each tag.

In the result, `children` is an ordered array of branches. Each branch contains
`root` and `children`; the untagged branch has `root: none`.
Use `tags.by-id.at("reading")` to retrieve a tag record.

See `examples/tag-tree.typ` for a complete example of writing entries, explicit
registration, and displaying the tree:

```sh
typst compile --root . examples/tag-tree.typ /tmp/diary-tag-tree.pdf
```
