# Teal Language

[Teal Lang](https://teal-language.org/)

## Prologue

<p>Things are like, when I was writing my neovim plugin, I'm just thinking, "gesus this lang sucks!". </p>

<p>I have tried multiple style to write a class including </p>

- prototype chain (the most sucks one, cause' it will have you to **GUESS** the type of fields. and somehow lsp and completion won't work.)
- fat style (like all the field, functions are copied and paste into `self` table that every instance holds)
  - seems shit. but actually it is the most fast and stra
- metatable style (lsp still can't infer type sometimes, but overall better than prototype chain.)

## What is this

this is a dir recording some fuzzy ideas about what can we do if we have a compiling time type checking system in lua, that is (partially), teal.
some ideas are really stupid or wishful-thinking.

## Ideas

### Err.tl

[self made err type](Err.tl)
