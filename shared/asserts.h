#pragma once

// comment to disable
#define DASSERTIONS_ENABLED

#ifdef DASSERTIONS_ENABLED

void report_assertion_failure(const char* expression,
                              const char* message,
                              const char* file,
                              int line);

#define debugBreak() __builtin_trap()

#define DASSERT(expr) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, "", __FILE__, __LINE__);  \
          debugBreak();\
      } \
  }

#define DASSERT_MSG(expr, message) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, message, __FILE__, __LINE__); \
          debugBreak(); \
      } \
  }

#ifdef _DEBUG
#define DASSERT_DEBUG(expr) \
  { \
      if (expr) { \
      } else { \
          report_assertion_failure(#expr, message, __FILE__, __LINE__); \
          debugBreak(); \
      } \
  }
#else
#define DASSERT_DEBUG(expr)
#endif

#else
#define DASSERT(expr)
#define DASSERT_MSG(expr, message)
#endif
