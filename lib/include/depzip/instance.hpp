#pragma once
#include <depzip/package_info.hpp>
#include <depzip/verbosity.hpp>
#include <memory>
#include <span>

namespace dz {
struct InstanceInfo {
	std::string_view working_dir{"."};
	std::string_view source_dir{"src"};
	Verbosity verbosity{Verbosity::Default};
};

class Instance {
  public:
	using Info = InstanceInfo;

	Instance(Instance const&) = delete;
	Instance(Instance&&) = delete;
	auto operator=(Instance const&) = delete;
	auto operator=(Instance&&) = delete;

	Instance() = default;
	virtual ~Instance() = default;

	virtual void setup(Info const& info) = 0;
	virtual void add_package(PackageInfo const& package_info) = 0;
	virtual void create_zip() = 0;

	void vendor(std::span<PackageInfo const> package_infos);
};

[[nodiscard]] auto create_instance(InstanceInfo const& info = {}) -> std::unique_ptr<Instance>;
} // namespace dz
