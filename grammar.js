/**
 * @file todo.txt grammar for tree-sitter
 * @license MIT
 * @see https://github.com/todotxt/todo.txt
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

// Tokens that can stand for a whole word win over a plain word of the same
// length; a longer plain word still wins.
const SPECIFIC = 1;
const PLAIN = -1;
// A URL wins over the tag its scheme and colon would make.
const URL = 2;

module.exports = grammar({
  name: 'todotxt',

  // Spaces and tabs separate words. Newlines end tasks, so they are tokens.
  extras: (_) => [/[ \t\f\r]/],

  // `src/scanner.c`: the priority, dates and done marker count only at the
  // start of a task, before a space and more text; elsewhere they are plain
  // words.
  externals: ($) => [$.priority, $.date, $._done_marker, $._error_sentinel],

  rules: {
    // Lines separated by newlines; any line may be blank, and the last one
    // needs no newline.
    source_file: ($) => seq(optional($._line), repeat(seq('\n', optional($._line)))),

    _line: ($) => choice($.task, $.done_task),

    // `(A) 2026-11-25 Call mom +family @phone due:2026-11-30`
    task: ($) => seq(
      optional($.priority),
      optional(field('created', $.date)),
      $._description,
    ),

    // `x 2026-11-26 2026-11-25 Call mom +family @phone`: `x` in the first
    // column and a space. The completion date comes first, and a creation
    // date only follows one.
    done_task: ($) => seq(
      alias($._done_marker, 'x'),
      optional(seq(field('completed', $.date), optional(field('created', $.date)))),
      $._description,
    ),

    _description: ($) => repeat1(choice($.project, $.context, $.tag, $.url, $._word)),

    project: (_) => token(prec(SPECIFIC, /\+\S+/)),

    context: (_) => token(prec(SPECIFIC, /@\S+/)),

    // `key:value`: the key holds no colon, the value may (`start:09:30`,
    // `path:/`). A key starting with `+` or `@` makes a project or context.
    tag: (_) => token(prec(SPECIFIC, /[^\s:+@][^\s:]*:\S+/)),

    // `scheme://…`. The format can't tell `https://example.com` from a tag
    // with a value starting with `//`, so anything written like a URL is one.
    url: (_) => token(prec(URL, /[A-Za-z][A-Za-z0-9+.\-]*:\/\/\S*/)),

    _word: (_) => token(prec(PLAIN, /\S+/)),
  },
});
