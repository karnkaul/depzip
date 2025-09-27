#include <depzip/instance.hpp>
#include <depzip/package_info.hpp>
#include <exception>
#include <print>

namespace dz::cli {
namespace {
void run_lab() {
	auto const info = InstanceInfo{
		.working_dir = "test",
		.verbosity = Verbosity::Verbose,
	};
	auto instance = create_instance(info);
	//

	instance->add_package({.repo_uri = "karnkaul/klib"});
	instance->add_package({.repo_uri = "glfw/glfw", .remove_subpaths = {"docs", "examples"}});
	instance->create_zip();
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
