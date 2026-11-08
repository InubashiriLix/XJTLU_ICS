#import "../typedefs/category.typ": category, category-branch, mk-category-tree

#let category-tree = mk-category-tree(
  // NOTE: the `not-match`, `root` and `undefined` are RESERVED IDs.
  // ================================================
  category-branch(
    branch-id: "xjtlu", //
    category(id: "CPT101"), //
    category(id: "CSI301"), //
  ),
  // ================================================
  category-branch(
    branch-id: "research", //
    category(id: "fwork"), //
    category(id: "wwork"), //
    category(id: "zwork"), //
    category(id: ""),
  ),
  // ================================================
  category-branch(
    branch-id: "diary", //
    category(id: "novel"), //
    category(id: "records"),
    category(id: "dianotics"),
  ),
  // ================================================
  category-branch(
    branch-id: "work",
    category-branch(
      branch-id: "jupiter", //
      category-branch(
        branch-id: "fpga-development", //
        category("reports"),
        category("fpga-documents"),
      ),
    ),
    category-branch(
      branch-id: "hci",
      category("hci-bked"),
      category("hci-android-app"),
      category("hci-ios-app"),
      category("hci-admin-web"),
    ),
  ),
  // ================================================
  category-branch(branch-id: "mental-health"),
)

