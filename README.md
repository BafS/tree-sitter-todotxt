# tree-sitter-todotxt

A [tree-sitter](https://tree-sitter.github.io/) grammar for
[todo.txt](https://github.com/todotxt/todo.txt) files, with a highlight query.

```text
(A) 2026-11-25 Call mom +family @phone due:2026-11-30
x 2026-11-26 2026-11-20 Renew the passport +travel
```

## Nodes

| Node        | Example                 | Notes                                                    |
| ----------- | ----------------------- | -------------------------------------------------------- |
| `task`      | `(A) Call mom`          | An open task.                                            |
| `done_task` | `x 2026-11-26 Call mom` | `x` in the first column, then a space.                   |
| `priority`  | `(A)`                   | First column, then a space.                              |
| `date`      | `2026-11-25`            | Then a space. Fields `completed` and `created`.          |
| `project`   | `+family`               |                                                          |
| `context`   | `@phone`                |                                                          |
| `tag`       | `due:2026-11-30`        | The value may hold colons and slashes.                   |
| `url`       | `https://example.com`   | Anything written `scheme://…`, rather than a tag.        |

The done marker, priority and dates count only where the spec puts them and
only when a space and more text follow (`src/scanner.c`); otherwise they are
plain words, so a line of only `(A)` or `x ` is an ordinary task. Other words
are part of the task without a node of their own. Blank lines are allowed,
and the last line needs no newline.

## Rust

```rust
let mut parser = tree_sitter::Parser::new();
parser.set_language(&tree_sitter_todotxt::LANGUAGE.into())?;
let tree = parser.parse("(A) Call mom +family @phone\n", None).unwrap();
```

`HIGHLIGHTS_QUERY` holds `queries/highlights.scm`.

## Development

You need the tree-sitter CLI (`cargo install tree-sitter-cli --locked`).

```sh
tree-sitter generate     # grammar.js → src/parser.c (commit the result)
tree-sitter test         # test/corpus
tree-sitter highlight examples/todo.txt
cargo test
```

Run `tree-sitter generate` after every change to `grammar.js`, and commit the
generated `src/` files: the crate compiles `src/parser.c` and doesn't need the
CLI.

## License

MIT, see `LICENSE`.
