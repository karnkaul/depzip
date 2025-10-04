#include <depzip/build_version.hpp>
#include <print>

auto main() -> int { std::println("depzip version: {}", dz::build_version_v); }
