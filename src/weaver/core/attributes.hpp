#ifndef WEAVER_CORE_ATTRIBUTE_HPP
#define WEAVER_CORE_ATTRIBUTE_HPP

#ifndef WEAVER_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define WEAVER_EXPORT __declspec(dllexport)
#       define WEAVER_IMPORT __declspec(dllimport)
#       define WEAVER_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define WEAVER_EXPORT __attribute__((visibility("default")))
#       define WEAVER_IMPORT __attribute__((visibility("default")))
#       define WEAVER_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define WEAVER_EXPORT
#       define WEAVER_IMPORT
#       define WEAVER_HIDDEN
#   endif
#endif


#ifndef WEAVER_API
#   if defined WEAVER_API_EXPORT
#       define WEAVER_API WEAVER_EXPORT
#   elif defined WEAVER_API_IMPORT
#       define WEAVER_API WEAVER_IMPORT
#   else /* No API */
#       define WEAVER_API
#   endif
#endif


#endif
