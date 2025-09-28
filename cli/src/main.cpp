#include <depzip/instance.hpp>
#include <depzip/json_io.hpp>
#include <depzip/package_info.hpp>
#include <depzip/panic.hpp>
#include <klib/args/parse.hpp>
#include <array>
#include <exception>
#include <print>

namespace dz::cli {
namespace {
class App {
  public:
	auto run(int const argc, char const* const* argv) -> int {
		auto const parse_result = parse_args(argc, argv);
		if (parse_result.early_return()) { return parse_result.get_return_code(); }

		read_manifest();
		m_instance = create_instance();
		m_instance->vendor(m_manifest.packages, m_config);

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
		auto result = dj::Json::from_file(m_manifest_path);
		if (!result) { throw Panic{std::format("Failed to read manifest {}", m_manifest_path)}; }
		m_manifest_json = std::move(*result);
		from_json(m_manifest_json, m_manifest);
	}

	std::string_view m_manifest_path{"depzip.json"};
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
