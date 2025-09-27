#pragma once
#include <depzip/package_info.hpp>
#include <depzip/verbosity.hpp>
#include <memory>

namespace dz {
struct InstanceInfo {
	std::string_view working_dir{"."};
	std::string_view source_dir{"src"};
	Verbosity verbosity{Verbosity::Default};
};

class Instance {
  public:
	Instance(Instance const&) = delete;
	Instance(Instance&&) = delete;
	auto operator=(Instance const&) = delete;
	auto operator=(Instance&&) = delete;

	Instance() = default;
	virtual ~Instance() = default;

	virtual void add_package(PackageInfo const& info) = 0;
	virtual void create_zip() = 0;
};

[[nodiscard]] auto create_instance(InstanceInfo const& info = {}) -> std::unique_ptr<Instance>;
} // namespace dz
