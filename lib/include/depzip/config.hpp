#pragma once
#include <depzip/verbosity.hpp>
#include <string_view>

namespace dz {
/// \brief Configuration for a vendor request.
struct Config {
	/// \brief Source directory.
	/// All repositories are cloned here.
	/// This is the directory that gets archived into a ZIP file.
	std::string_view source_dir{"src"};
	/// \brief Working directory.
	std::string_view working_dir{"."};
	/// \brief Output verbosity.
	Verbosity verbosity{Verbosity::Default};
};
} // namespace dz
