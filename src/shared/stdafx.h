#pragma once

#include <queue>

/* Identify environment */

// current platform
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

#define UNIX_FLAVOUR_LINUX 1
#define UNIX_FLAVOUR_BSD 2
#define UNIX_FLAVOUR_OTHER 3
#define UNIX_FLAVOUR_OSX 4

/* Define the platform */
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)

#	define PLATFORM PLATFORM_WIN32
#	define PLATFORM_TEXT "Win32"
#	define CONFIG_USE_IOCP

#	define VC_EXTRALEAN
#	define WIN32_LEAN_AND_MEAN

#	include <Windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	define THREADCALL WINAPI
#	define STRCASECMP _stricmp

#	define I64FMT "%016I64X"
#	define I64FMTD "%I64u"
#	define SI64FMTD "%I64d"

#else /* not a Windows environment */

//	Include generated config file from ./configure
#	if defined(HAVE_CONFIG_H)
#		include "config.h"
#	endif

#	include <sys/time.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <unistd.h>
#	include <signal.h>
#	include <netdb.h>
#	include <climits>

#	if defined(__APPLE_CC__)
#		define PLATFORM PLATFORM_APPLE
#	else
#		define PLATFORM PLATFORM_UNIX
#	endif

	/* Define the Unix flavour */
#	if defined(HAVE_DARWIN)
#		define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#		define PLATFORM_TEXT "MacOSX"
#		define CONFIG_USE_KQUEUE
#	elif defined(__FreeBSD__)
#		define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#		define PLATFORM_TEXT "BSD"
#		define CONFIG_USE_KQUEUE
#	else
#		define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#		define PLATFORM_TEXT "Linux"
#		define CONFIG_USE_EPOLL
#	endif

#	define THREADCALL 
#	define STRCASECMP strcasecmp

#	define I64FMT "%016llX"
#	define I64FMTD "%llu"
#	define SI64FMTD "%lld"

#	define SetConsoleTitle(title) /* unsupported & unnecessary */

#endif

/* Define the compiler we're using */
#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1
#define COMPILER_BORLAND   2

#ifdef _MSC_VER
#  define COMPILER COMPILER_MICROSOFT
#elif defined( __BORLANDC__ )
#  define COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

/* Define the build we're compiling */
#ifdef _DEBUG
#		define BUILD_TYPE "Debug"
#		include <cassert>
#		include "DebugUtils.h"

#		define ASSERT assert
#		define TRACE FormattedDebugString
#else
#		define BUILD_TYPE "Release"
#		define ASSERT 
#		define TRACE 
#endif

/* Define the architecture we're compiling for */
#ifdef X64
#	define ARCH "X64"
#else
#	define ARCH "X86"
#endif

// Ignore the warning "nonstandard extension used: enum '<enum name>' used in qualified name"
// Sometimes it's necessary to avoid collisions, but aside from that, specifying the enumeration helps make code intent clearer.
#pragma warning(disable: 4482)

#define STR(str) #str
#define STRINGIFY(str) STR(str)

// If we support C++11, use experimental std::thread implementation
#if (__cplusplus >= 201103L)	/* C++11 */  \
	|| (_MSC_VER >= 1700)		/* VS2012 */ \
	|| defined(HAVE_CXX11)		/* generated by config */

#	include <thread>
#	include <chrono>
#	include <atomic>

// Use new portable C++11 functionality.
#	define USE_STD_THREAD
#	define USE_STD_MUTEX
#	define USE_STD_ATOMIC
#	ifdef USE_STD_MUTEX
#		define USE_STD_CONDITION_VARIABLE
#	endif

// Portable C++11 thread sleep call.
#	define sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// Retain support for older Windows compilers (for now).
// Other platforms rely on C++11 support.
#else
#	define sleep(ms) Sleep(ms)
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// define compiler-specific types
#include "types.h"

#ifndef WIN32
// 	For working 64-bit support we need unixODBX 2.3.1 (Nov 2011)
//	Unfortunately, 2.3.0 (2010) is still available in most repositories
//	This hack will force SQLLEN/SQLULEN to use 32-bit types (if we're building 64-bit code).
//	It will throw a warning during compile, but it should still run fine.
#	define BUILD_LEGACY_64_BIT_MODE

//	The above is not sufficient for some distros.
//	In this case, we need to override the base types used, with 32-bit types.
#	define ODBCINT64	int32
#	define UODBCINT64	uint32
#	define SIZEOF_LONG_INT	4

#	include <sqltypes.h> /* this will define the SQL & required Windows types on a non-Windows system */
#endif

#include <memory>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

#include "tstring.h"
#include "globals.h"
#include "Atomic.h"
#include "Thread.h"
#include "Network.h"
#include "TimeThread.h"
