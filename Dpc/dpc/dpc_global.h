#pragma once
//#define _MEMORY_LEAK
#ifdef _MEMORY_LEAK
	#include <vld.h>
#endif

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DPC_LIB)
#  define DPC_EXPORT Q_DECL_EXPORT
# else
#  define DPC_EXPORT Q_DECL_IMPORT
# endif
#else
# define DPC_EXPORT
#endif
