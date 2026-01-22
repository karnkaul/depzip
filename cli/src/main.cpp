#include "app.hpp"
#include <exception>
#include <print>

auto main(int argc, char** argv) -> int {
	try {
		auto app = depzip::cli::App{};
		return app.run(argc, argv);
	} catch (std::exception const& e) {
		std::println(stderr, "PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		std::println(stderr, "PANIC!");
		return EXIT_FAILURE;
	}
}
