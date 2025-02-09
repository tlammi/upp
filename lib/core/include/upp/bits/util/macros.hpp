#pragma once

namespace upp {

// NOLINTBEGIN(*-macro-usage)
#if defined(NDEBUG)
#define UPP_IF_NO_NDEBUG(stmt)
#define UPP_IF_NDEBUG(stmt) stmt
#else
#define UPP_IF_NO_NDEBUG(stmt) stmt
#define UPP_IF_NDEBUG(stmt)
#endif
// NOLINTEND(*-macro-usage)
}  // namespace upp
