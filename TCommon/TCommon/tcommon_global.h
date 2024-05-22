#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TCOMMON_LIB)
#  define TCOMMON_EXPORT Q_DECL_EXPORT
# else
#  define TCOMMON_EXPORT Q_DECL_IMPORT
# endif
#else
# define TCOMMON_EXPORT
#endif
