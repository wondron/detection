#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(WGRAPHICS_LIB)
#  define WGRAPHICS_EXPORT Q_DECL_EXPORT
# else
#  define WGRAPHICS_EXPORT Q_DECL_IMPORT
# endif
#else
# define WGRAPHICS_EXPORT
#endif
