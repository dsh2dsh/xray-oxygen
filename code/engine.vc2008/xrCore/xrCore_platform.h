#pragma once

#define VC_EXTRALEAN				// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers

// windows.h
#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0600
#endif

#define NOGDICAPMASKS
//#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
//#define NOMETAFILE
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX
#define DOSWIN32
#define _WIN32_DCOM
#define NOMINMAX

#pragma warning(push)
#pragma warning(disable:4005)
#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)

#ifdef _M_AMD64
#pragma warning (disable : 4512 )
#endif