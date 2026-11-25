// The tokens at the start of a task, whose rules a regular expression can't
// express: they count only where the todo.txt spec puts them, and only when
// a space follows, which isn't part of the token.
//
// - `x` marks a done task only in the first column (`grammar.js`: done_task).
// - `(A)` is a priority only in the first column.
// - A date is a creation or completion date only in the first column or
//   after the tokens above.
//
// Anywhere else, or without the space, the same text is a plain word.

#include "tree_sitter/parser.h"

#include <stdbool.h>

// The order of `externals` in grammar.js.
enum TokenType {
  PRIORITY,
  DATE,
  DONE_MARKER,
  ERROR_SENTINEL,
};

void *tree_sitter_todotxt_external_scanner_create(void) { return NULL; }

void tree_sitter_todotxt_external_scanner_destroy(void *payload) { (void)payload; }

unsigned tree_sitter_todotxt_external_scanner_serialize(void *payload, char *buffer) {
  (void)payload;
  (void)buffer;
  return 0;
}

void tree_sitter_todotxt_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
  (void)payload;
  (void)buffer;
  (void)length;
}

static bool is_digit(int32_t c) { return c >= '0' && c <= '9'; }

// Reads `count` digits.
static bool digits(TSLexer *lexer, int count) {
  for (int i = 0; i < count; i++) {
    if (!is_digit(lexer->lookahead)) return false;
    lexer->advance(lexer, false);
  }
  return true;
}

// Reads `YYYY-MM-DD`.
static bool date(TSLexer *lexer) {
  if (!digits(lexer, 4) || lexer->lookahead != '-') return false;
  lexer->advance(lexer, false);
  if (!digits(lexer, 2) || lexer->lookahead != '-') return false;
  lexer->advance(lexer, false);
  return digits(lexer, 2);
}

// Ends the token here and accepts it as `type` when a space and more text
// follow on the line: a task needs a description, so `(A) ` alone is a word.
static bool before_space(TSLexer *lexer, enum TokenType type) {
  lexer->mark_end(lexer);
  if (lexer->lookahead != ' ') return false;
  while (lexer->lookahead == ' ' || lexer->lookahead == '\t' || lexer->lookahead == '\f') lexer->advance(lexer, false);
  if (lexer->eof(lexer) || lexer->lookahead == '\n' || lexer->lookahead == '\r') return false;
  lexer->result_symbol = type;
  return true;
}

bool tree_sitter_todotxt_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
  (void)payload;
  // Error recovery marks every token valid: leave it to the grammar's lexer.
  if (valid_symbols[ERROR_SENTINEL]) return false;

  // A priority or done marker is valid only at the start of a line, and
  // what starts a line must be in its first column.
  bool line_start = valid_symbols[PRIORITY] || valid_symbols[DONE_MARKER];
  if (line_start) {
    if (lexer->get_column(lexer) != 0) return false;
  } else {
    while (lexer->lookahead == ' ' || lexer->lookahead == '\t') lexer->advance(lexer, true);
  }

  if (line_start && lexer->lookahead == 'x') {
    lexer->advance(lexer, false);
    return before_space(lexer, DONE_MARKER);
  }

  if (valid_symbols[PRIORITY] && lexer->lookahead == '(') {
    lexer->advance(lexer, false);
    if (lexer->lookahead < 'A' || lexer->lookahead > 'Z') return false;
    lexer->advance(lexer, false);
    if (lexer->lookahead != ')') return false;
    lexer->advance(lexer, false);
    return before_space(lexer, PRIORITY);
  }

  if (valid_symbols[DATE] && is_digit(lexer->lookahead)) {
    return date(lexer) && before_space(lexer, DATE);
  }

  return false;
}
