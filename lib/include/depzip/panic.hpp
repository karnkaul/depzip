#pragma once
#include <stdexcept>

namespace dz {
class Panic : public std::runtime_error {
  public:
	using std::runtime_error::runtime_error;
};
} // namespace dz
