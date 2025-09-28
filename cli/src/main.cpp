#include <depzip/instance.hpp>
#include <depzip/package_info.hpp>
#include <array>
#include <exception>
#include <print>

namespace dz::cli {
namespace {
void run_lab() {
	auto instance = create_instance();

	auto const packages = std::array{
		PackageInfo{.repo_uri = "karnkaul/klib"},
		PackageInfo{.repo_uri = "glfw/glfw", .remove_subpaths = {"docs", "examples"}},
	};

	auto const config = Config{
		.working_dir = "test",
		.verbosity = Verbosity::Verbose,
	};

	instance->vendor(packages, config);
}
} // namespace
} // namespace dz::cli

auto main() -> int {
	try {
		dz::cli::run_lab();
	} catch (std::exception const& e) {
		std::println(stderr, "PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		std::println(stderr, "PANIC!");
		return EXIT_FAILURE;
	}
}
