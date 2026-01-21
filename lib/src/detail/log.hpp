#pragma once
#include <klib/log.hpp>

namespace dz::detail {
auto const log = klib::TaggedLogger{"depzip"};

namespace shell {
auto const log = klib::TaggedLogger{"depzip::shell"};
} // namespace shell

namespace util {
auto const log = klib::TaggedLogger{"depzip::util"};
} // namespace util
} // namespace dz::detail
