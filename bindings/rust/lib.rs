//! A [todo.txt](https://github.com/todotxt/todo.txt) grammar for
//! [tree-sitter](https://tree-sitter.github.io/).
//!
//! ```
//! let mut parser = tree_sitter::Parser::new();
//! parser.set_language(&tree_sitter_todotxt::LANGUAGE.into()).unwrap();
//! let tree = parser.parse("(A) Call mom +family @phone\n", None).unwrap();
//! assert!(!tree.root_node().has_error());
//! ```

use tree_sitter_language::LanguageFn;

unsafe extern "C" {
    fn tree_sitter_todotxt() -> *const ();
}

/// The tree-sitter language for todo.txt.
// SAFETY: `tree_sitter_todotxt` is the entry point of the generated parser in
// `src/parser.c`, which `build.rs` compiles; it returns a static `TSLanguage`.
pub const LANGUAGE: LanguageFn = unsafe { LanguageFn::from_raw(tree_sitter_todotxt) };

/// The node types of the grammar, as JSON.
pub const NODE_TYPES: &str = include_str!("../../src/node-types.json");

/// The highlight query: done tasks, priorities, dates, projects, contexts,
/// `key:value` tags and URLs.
pub const HIGHLIGHTS_QUERY: &str = include_str!("../../queries/highlights.scm");

#[cfg(test)]
mod tests {
    #[test]
    fn the_grammar_loads() {
        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&super::LANGUAGE.into()).expect("load the todo.txt grammar");
    }

    #[test]
    fn the_highlight_query_fits_the_grammar() {
        tree_sitter::Query::new(&super::LANGUAGE.into(), super::HIGHLIGHTS_QUERY).expect("compile the highlight query");
    }
}
