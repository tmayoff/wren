#pragma once

#include <cassert>

#define WREN_STATEMENT_BEGIN do {
#define WREN_STATEMENT_END \
  }                        \
  while (false);

#define LINEIZE_(n, line) n##line
#define LINEIZE(n, line) LINEIZE_(n, line)

#ifdef WREN_DEBUG
#define WREN_ASSERT(expr, msg) assert(expr && (msg));
#else
#define WREN_ASSERT(expr, msg)
#endif
