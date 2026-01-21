#pragma once
#include <string_view>

namespace depzip {
/// \brief Configuration for a vendor request.
struct Config {
	/// \brief Source directory.
	/// All repositories are cloned here.
	/// This is the directory that gets archived into a ZIP file.
	std::string_view source_dir{"src"};
	/// \brief Working directory.
	std::string_view working_dir{"."};
};
} // namespace depzip
