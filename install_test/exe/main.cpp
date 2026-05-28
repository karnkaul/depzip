#include "depzip/build_version.hpp"
#include "klib/version_str.hpp"
#include <print>

auto main() -> int { std::println("depzip version: {}", depzip::build_version_v); }
