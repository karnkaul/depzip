#include "depzip/instance.hpp"
#include "depzip/json_io.hpp"
#include "depzip/panic.hpp"
#include <klib/args/parse.hpp>
#include <array>
#include <cassert>
#include <exception>
#include <filesystem>
#include <print>

namespace dz::cli {
namespace {
namespace fs = std::filesystem;

[[nodiscard]] auto locate_manifest(fs::path path) -> fs::path {
	if (!path.empty()) {
		if (fs::is_symlink(path)) { path = fs::read_symlink(path); }
		if (!fs::is_regular_file(path)) { throw Panic{std::format("Invalid depzip manifest: {}", path.string())}; }
		return path;
	}

	using namespace std::string_view_literals;
	static constexpr auto default_paths_v = std::array{
		"depzip.json"sv,
		"depzip.jsonc"sv,
	};
	for (auto const path : default_paths_v) {
		if (fs::is_regular_file(path)) { return path; }
	}

	throw Panic{"Failed to locate depzip manifest"};
}

class App {
  public:
	auto run(int const argc, char const* const* argv) -> int {
		auto const parse_result = parse_args(argc, argv);
		if (parse_result.early_return()) { return parse_result.get_return_code(); }

		run();
		return EXIT_SUCCESS;
	}

  private:
	struct VerbosityInput {
		std::string_view text{to_string_view(Verbosity::Default)};
		bool was_set{};
	};

	[[nodiscard]] auto parse_args(int const argc, char const* const* argv) -> klib::args::ParseResult {
		auto const parse_info = klib::args::ParseInfo{
			.version = build_version_v,
		};
		auto const args = std::array{
			klib::args::named_option(m_config.source_dir, "s,src", "source directory"),
			klib::args::named_option(m_config.working_dir, "w,pwd", "working directory"),
			klib::args::named_option(m_verbosity.text, "v,verbosity", "verbosity", &m_verbosity.was_set),
			klib::args::positional_optional(m_manifest_path, "manifest", "path to manifest"),
		};
		auto const ret = klib::args::parse_main(parse_info, args, argc, argv);
		if (ret && m_verbosity.was_set) { m_config.verbosity = to_verbosity(m_verbosity.text); }
		return ret;
	}

	void read_manifest() {
		auto const manifest_path = locate_manifest(m_manifest_path);
		assert(!manifest_path.empty());

		auto result = dj::Json::from_file(manifest_path.string(), dj::ParseMode::Jsonc);
		if (!result) { throw Panic{std::format("Failed to read manifest: {}", manifest_path.string())}; }
		m_manifest_json = std::move(*result);
		from_json(m_manifest_json, m_manifest);
	}

	void run() {
		if (m_config.verbosity != Verbosity::Silent) { std::println("depzip v{}", build_version_v); }

		read_manifest();
		if (m_manifest.packages.empty()) {
			std::println("Nothing to vendor");
			return;
		}

		m_instance = create_instance();
		m_instance->vendor(m_manifest, m_config);
	}

	std::string_view m_manifest_path{};
	VerbosityInput m_verbosity{};
	Config m_config{};

	dj::Json m_manifest_json{};
	Manifest m_manifest{};

	std::unique_ptr<Instance> m_instance{};
};
} // namespace
} // namespace dz::cli

auto main(int argc, char** argv) -> int {
	try {
		auto app = dz::cli::App{};
		return app.run(argc, argv);
	} catch (std::exception const& e) {
		std::println(stderr, "PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		std::println(stderr, "PANIC!");
		return EXIT_FAILURE;
	}
}
