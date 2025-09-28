#include <depzip/instance.hpp>
#include <klib/unit_test.hpp>
#include <array>
#include <filesystem>

namespace {
namespace fs = std::filesystem;

TEST(instance) {
	auto const pwd = fs::current_path();

	auto instance = dz::create_instance();

	auto const config = dz::Config{
		.working_dir = "workspace",
		.verbosity = dz::Verbosity::Verbose,
	};

	auto const packages = std::array{
		dz::PackageInfo{.repo_uri = "karnkaul/depzip"},
		dz::PackageInfo{.repo_uri = "glfw/glfw", .git_tag = "3.4", .remove_subpaths = {"docs", "examples"}},
	};

	instance->vendor(packages, config);

	fs::current_path(pwd);

	EXPECT(fs::is_regular_file("workspace/src.zip"));
	EXPECT(fs::is_directory("workspace/src/depzip"));
	EXPECT(fs::is_directory("workspace/src/glfw"));

	fs::remove_all("workspace");
}
} // namespace
