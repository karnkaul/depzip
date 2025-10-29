#include <depzip/instance.hpp>
#include <klib/unit_test.hpp>
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

	auto const manifest = dz::Manifest{
		.packages =
			{
				dz::PackageInfo{.uri = "karnkaul/depzip.git"},
				dz::PackageInfo{
					.uri = "glfw/glfw",
					.branch = "3.4",
					.remove_subpaths = {"docs", "examples"},
					.custom_command = "cmake -P src/glfw/CMake/GenerateMappings.cmake src/glfw/src/mappings.h.in src/glfw/src/mappings.h",
				},
			},
	};

	instance->vendor(manifest, config);

	fs::current_path(pwd);

	EXPECT(fs::is_regular_file("workspace/src.zip"));
	EXPECT(fs::is_directory("workspace/src/depzip"));
	EXPECT(fs::is_directory("workspace/src/glfw"));

	fs::remove_all("workspace");
}
} // namespace
