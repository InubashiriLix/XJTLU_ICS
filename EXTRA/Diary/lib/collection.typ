#let index-entries(entries) = {
  assert(type(entries) == array, message: "entries must be an array")
  let index = (:)

  for item in entries {
    assert(
      not (item.id in index),
      message: "Duplicate entry ID: " + item.id,
    )
    index.insert(item.id, item)
  }

  index
}

#let assemble(entries, comments) = {
  assert(type(comments) == array, message: "comments must be an array")

  let index = index-entries(entries)
  let grouped = (:)

  for reply in comments {
    assert(
      reply.target in index,
      message: "Unknown comment target: " + reply.target,
    )

    let target = index.at(reply.target)

    assert(
      reply.date >= target.date,
      message: "Comment date precedes entry: " + reply.target,
    )

    let replies = grouped.at(reply.target, default: ())
    grouped.insert(reply.target, replies + (reply,))
  }

  entries
    .sorted(key: item => item.date)
    .map(item => (..item, comments: grouped.at(item.id, default: ()).sorted(key: reply => reply.date)))
}
