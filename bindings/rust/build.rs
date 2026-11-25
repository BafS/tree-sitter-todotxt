fn main() {
    let src = std::path::Path::new("src");
    let parser = src.join("parser.c");
    let scanner = src.join("scanner.c");
    println!("cargo:rerun-if-changed={}", parser.display());
    println!("cargo:rerun-if-changed={}", scanner.display());

    let mut build = cc::Build::new();
    build.std("c11").include(src).file(&parser).file(&scanner);
    #[cfg(target_env = "msvc")]
    build.flag("-utf-8");
    build.compile("tree-sitter-todotxt");
}
