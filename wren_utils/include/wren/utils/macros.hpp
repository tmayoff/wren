#pragma once

#include <cassert>

#define LINEIZE_(n, line) n##line
#define LINEIZE(n, line) LINEIZE_(n, line)

#ifdef WREN_DEBUG
#define WREN_ASSERT(expr) assert(expr);
#else
#define WREN_ASSERT(expr)
#endif
