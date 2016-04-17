/*
 * preprocessor macro capable of counting the number of arguments by using a mask for the __VA_ARGS__
 *
 * https://stackoverflow.com/questions/2598132/length-of-va-list-when-using-variable-list-arguments/18584390#18584390
 * https://gist.github.com/mbolt35/4e60da5aaec94dcd39ca
 *
 */

#define IS_MSVC _MSC_VER && !__INTEL_COMPILER

/**
 * Define the macros to determine variadic argument lengths up to 20 arguments. The MSVC
 * preprocessor handles variadic arguments a bit differently than the GNU preprocessor,
 * so we account for that here.
 */
#if IS_MSVC
  #define MSVC_HACK(FUNC, ARGS) FUNC ARGS
  #define APPLY(FUNC, ...) MSVC_HACK(FUNC, (__VA_ARGS__))
  #define VA_LENGTH(...) APPLY(VA_LENGTH_, 0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
  #define VA_LENGTH(...) VA_LENGTH_(0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif

/**
 * Strip the processed arguments to a length variable.
 */
#define VA_LENGTH_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N

/**
 * Use the VA_LENGTH macro to determine the length of the variadic args to
 * pass in as the first parameter, and forward along the arguments after that.
 */
#define ExecVF(Func, ...) Func(VA_LENGTH(__VA_ARGS__), __VA_ARGS__)
