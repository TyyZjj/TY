#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LOGINMANAGER_LIB)
#  define LOGINMANAGER_EXPORT Q_DECL_EXPORT
# else
#  define LOGINMANAGER_EXPORT Q_DECL_IMPORT
# endif
#else
# define LOGINMANAGER_EXPORT
#endif
