// Presentation language is explicit; record dates and prose stay unchanged.
#let get-locale(lang) = {
  assert(lang in ("en", "zh"), message: "Unknown diary language: " + lang)
  let months = ("January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December")
  let month-name(date) = months.at(date.month() - 1)
  (
    lang: lang,
    title: if lang == "en" { [Journal] } else { [日记] },
    contents: if lang == "en" { [Contents] } else { [目录] },
    comments: if lang == "en" { [Comments] } else { [评论] },
    empty: if lang == "en" { [No entries yet.] } else { [尚无记录。] },
    date: date => if lang == "en" {
      str(date.day()) + " " + month-name(date) + " " + str(date.year())
    } else {
      str(date.year()) + "年" + str(date.month()) + "月" + str(date.day()) + "日"
    },
    month: date => if lang == "en" {
      month-name(date) + " " + str(date.year())
    } else { str(date.year()) + "年" + str(date.month()) + "月" },
    day: date => if lang == "en" { str(date.day()) } else { str(date.day()) + "日" },
  )
}
