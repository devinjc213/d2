#pragma once

// comment to disable
#define GASSERTIONS_ENABLED

#ifdef GASSERTIONS_ENABLED

void report_assertion_failure(const char* expression,
                              const char* message,
                              const char* file,
                              int line);

#define debugBreak() __builtin_trap()

#define GASSERT(expr) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, "", __FILE__, __LINE__);  \
          debugBreak();\
      } \
  }

#define GASSERT_MSG(expr, message) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, message, __FILE__, __LINE__); \
          debugBreak(); \
      } \
  }

#ifdef _DEBUG
#define GASSERT_DEBUG(expr) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, message, __FILE__, __LINE__); \
          debugBreak(); \
      } \
  }
#else
#define GASSERT_DEBUG(expr)
#endif

#else
#define GASSERT(expr)
#define GASSERT_MSG(expr, message)
#endif
