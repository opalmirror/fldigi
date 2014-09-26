# CMake configuration written by Richard Shaw, KF5OIM (hobbes1069 AT gmail DOT com)

# Verbose Makefile by default
option(CMAKE_VERBOSE_MAKEFILE "Verbose make?" ON)

project(fldigi)
cmake_minimum_required(VERSION 2.8)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified, defaulting to ${CMAKE_BUILD_TYPE}")
endif()

# Add local cmake modules to the search path.
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_BINARY_DIR}/cmake
)


# Setup options with defaults.
option(WANT_FLDIGI "Build Fldigi." ON)
option(WANT_FLARQ "Build Flarq." ON)
option(ENABLE_NLS "Use Native Language Support" ON)
option(ENABLE_HAMLIB "Enable hamlib for rig control." ON)
option(ENABLE_MAC_UNIVERSAL "Build a universal binary on Mac OS X" OFF)
option(ENABLE_STATIC "Use static linking for some libraries." OFF)

# Setup default CXX flags
set(CMAKE_CXX_FLAGS "-pipe -Wall -fexceptions -ffast-math -finline-functions")

# Setup default include directories.
include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/irrxml
    ${CMAKE_SOURCE_DIR}/src/libtiniconv
    ${CMAKE_SOURCE_DIR}/src/fileselector
    ${CMAKE_SOURCE_DIR}/src/xmlrpcpp
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
)

# Setup standard install location variables for *nix systems.
include(GNUInstallDirs)
# Make this visible to the cmake gui.
mark_as_advanced(CLEAR
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_DATADIR
    CMAKE_INSTALL_DOCDIR
    CMAKE_INSTALL_LOCALEDIR
)

### Version Info
# major and minor must be integers; patch may
# contain other characters or be empty
# FIXME: Probably should be pulled from configure.ac
set(FLDIGI_VERSION_MAJOR 3)
set(FLDIGI_VERSION_MINOR 22)
set(FLDIGI_VERSION_PATCH .0CN)
string(REPLACE "." "" FLDIGI_VERSION_PATCH_NODOT "${FLDIGI_VERSION_PATCH}")
set(FLARQ_VERSION_MAJOR 4)
set(FLARQ_VERSION_MINOR 3)
set(FLARQ_VERSION_PATCH .3)
string(REPLACE "." "" FLARQ_VERSION_PATCH_NODOT "${FLARQ_VERSION_PATCH}")

set(FLDIGI_VERSION ${FLDIGI_VERSION_MAJOR}.${FLDIGI_VERSION_MINOR}${FLDIGI_VERSION_PATCH})
set(FLARQ_VERSION ${FLDIGI_VERSION_MAJOR}.${FLDIGI_VERSION_MINOR}${FLDIGI_VERSION_PATCH})

# FIXME: Do we need to implement this?
#AM_MAINTAINER_MODE

# Set string variables that get set in config.h
set(COPYRIGHT "Copyright (C) 2007, 2008, 2009 Stelios Bounanos, M0GLD (m0gld AT enotty DOT net)")
set(FLARQ_AUTHORS "Dave Freese")
set(FLARQ_DOCS "http://www.w1hkj.com/FlarqHelpFiles/flarq.html")
set(FLDIGI_AUTHORS "Dave Freese, Stelios Bounanos, Leigh Klotz, Remi Chateauneu, and others")
set(PACKAGE "${CMAKE_PROJECT_NAME}")
set(PACKAGE_NAME "${PACKAGE}")
set(PACKAGE_VERSION "${FLDIGI_VERSION}")
set(VERSION "${PACKAGE_VERSION}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE_NAME}")
set(PACKAGE_BUGREPORT "fldigi-devel@lists.berlios.de")
set(PACKAGE_HOME "http://www.w1hkj.com/Fldigi.html")
set(PACKAGE_DL "http://www.w1hkj.com/download.html")
set(PACKAGE_PROJ "http://developer.berlios.de/project/showfiles.php?group_id=9149")
set(PACKAGE_NEWBUG "https://fedorahosted.org/fldigi/newticket")
set(PACKAGE_DOCS "http://www.w1hkj.com/FldigiHelp/index.html")
set(PACKAGE_GUIDE "http://www.w1hkj.com/beginners.html")

# Grok full date and time
if(UNIX OR MSYS OR (MINGW AND CMAKE_CROSSCOMPILING))
    execute_process(COMMAND date OUTPUT_VARIABLE BUILD_DATE)
    string(STRIP "${BUILD_DATE}" BUILD_DATE)
elseif(WIN32 AND NOT MSYS)
    # Assume native windows build and cmd.exe shell.
    execute_process(COMMAND date /t OUTPUT_VARIABLE _date)
    execute_process(COMMAND time /t OUTPUT_VARIABLE _time)
    if(_date AND _time)
        set(BUILD_DATE "${_date} ${_time}")
    else()
        set(BUILD_DATE "unknown")
    endif()
    unset(_date)
    unset(_time)
endif()

# Grok hostname, set to unknown first in case process fails.
set(BUILD_HOST "Unknown")
execute_process(COMMAND hostname OUTPUT_VARIABLE BUILD_HOST)
string(STRIP "${BUILD_HOST}" BUILD_HOST)

# Checks for programs.
# What is this and do we need it?
#AC_USE_SYSTEM_EXTENSIONS

# Checks for header files.
include(CheckIncludeFileCXX)
set(CHECK_HEADERS "arpa/inet.h execinfo.h fcntl.h limits.h memory.h netdb.h netinet/in.h regex.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h sys/utsname.h termios.h unistd.h values.h linux/ppdev.h dev/ppbus/ppi.h")
separate_arguments(CHECK_HEADERS)
foreach(HEADER ${CHECK_HEADERS})
    string(REPLACE "/" "_" REP1_${HEADER} ${HEADER})
    string(REPLACE "." "_" REP2_${HEADER} ${REP1_${HEADER}})
    string(TOUPPER ${REP2_${HEADER}} UPPER_${HEADER})
    check_include_file_cxx(${HEADER} HAVE_${UPPER_${HEADER}})
endforeach()

# Checks for library functions.
include(CheckFunctionExists)
#check_function_exists(error_at_line HAVE_ERROR_AT_LINE)
set(CHECK_FUNCS "error_at_line getaddrinfo gethostbyname hstrerror gmtime_r localtime_r memmove memset mkdir select setenv snprintf socket socketpair strcasecmp strcasestr strchr strdup strerror strlcpy strncasecmp strrchr strstr strtol uname unsetenv vsnprintf")
separate_arguments(CHECK_FUNCS)
foreach(FUNC ${CHECK_FUNCS})
    string(TOUPPER ${FUNC} UPPER_${FUNC})
    check_function_exists(${FUNC} HAVE_${UPPER_${FUNC}})
endforeach()

# Check for external library functions.
include(CheckLibraryExists)
if(UNIX)
    check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
    if(HAVE_CLOCK_GETTIME)
        list(APPEND COMMON_LINK_LIBS rt)
    endif()
else()
    check_library_exists(pthread clock_gettime "" HAVE_CLOCK_GETTIME)
    check_library_exists(pthread sem_unlink "" HAVE_SEM_UNLINK)
    check_library_exists(pthread sem_timedwait "" HAVE_SEM_TIMEDWAIT)
endif()

check_library_exists(dl dlopen "" HAVE_DLOPEN)
if(HAVE_DLOPEN)
    list(APPEND FLDIGI_LINK_LIBS ${CMAKE_DL_LIBS})
endif()

# AC_HEADER_TIME / TIME_WITH_SYS_TIME CMake conversion workaround.
# This autoconf macro is considered obsolete. All modern systems should be able
# to include both time.h and sys/time.h without issue.
# Only used in src/misc/timeops.cxx
set(TIME_WITH_SYS_TIME TRUE)

# Check for O_CLOEXEC
# Do we need to check for this?
#AC_FCNTL_FLAGS

# Pthread Library
find_package(Threads REQUIRED)
#message("CMAKE_THREAD_LIBS_INIT: ${CMAKE_THREAD_LIBS_INIT}")
#message("CMAKE_USE_WIN32_THREADS_INIT: ${CMAKE_USE_WIN32_THREADS_INIT}")
#message("CMAKE_USE_PTHREADS_INIT: ${CMAKE_USE_PTHREADS_INIT}")
list(APPEND COMMON_LINK_LIBS ${CMAKE_THREAD_LIBS_INIT})

### Internationalization
find_package(Gettext)
if(GETTEXT_FOUND AND ENABLE_NLS)
    set(HAVE_GETTEXT TRUE)
    add_definitions(-DUSE_NLS -DLOCALEDIR="${CMAKE_INSTALL_FULL_LOCALEDIR}")
else()
    message(WARNING "Native language support requested but gettext was not found.")
endif()

###### OS support

# Setup defaults for non-posix systems.
if(NOT UNIX)
    # Force use of internal regex.
    set(HAVE_REGEX_H FALSE)
    set(COMPAT_REGEX TRUE)
endif()

### OSX
# Set ac_cv_mac_universal to yes/no
# Set DARWIN Makefile conditional
# Substitute MAC_UNIVERSAL_CFLAGS and MAC_UNIVERSAL_LDFLAGS in Makefile
if(APPLE)
    # Do the right thing at some point.
endif()

### win32
if(WIN32)
    set(__WOE32__ TRUE)
    set(__WIN32__ TRUE)
    set(__MINGW32__ TRUE)
    set(_WINDOWS TRUE)
    set(HAVE_WINDRES TRUE)
    set(RES_FILES
        src/fldigirc.rc
        src/flarq-src/flarqrc.rc
        src/common.rc
        src/flsynop.rc
    )
    include_directories(data/win32)
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    execute_process(COMMAND date +%s OUTPUT_VARIABLE RC_BUILD_TIME)
    string(STRIP "${RC_BUILD_TIME}" RC_BUILD_TIME)
    add_definitions(-DRC_BUILD_TIME="\\\"${RC_BUILD_TIME}\\\"")
    set(CMAKE_RC_COMPILE_OBJECT
        "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif()

### static flag
# Set ac_cv_static to yes/no
# Substitute RTLIB in Makefile
#AC_FLDIGI_STATIC

### benchmark mode
option(ENABLE_BENCHMARK "Enable benchmarking?" OFF)

### TLS flag
# Define USE_TLS in config.h
include(CheckThreadLocalStorage)
check_thread_local_storage()
if(HAVE_TLS)
    set(USE_TLS TRUE)
endif()


### Check if fopen support O_CLOEXEC
message(STATUS "Check for working O_CLOEXEC")
try_compile(HAVE_O_CLOEXEC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/TryCompileO_CLOEXEC.cxx
)
if(HAVE_O_CLOEXEC)
    message(STATUS "Check for working O_CLOEXEC -- works")
endif()
    

### std::bind or alternatives
message(STATUS "Check for working std::bind in <functional>")
try_compile(HAVE_STD_BIND
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/TryCompileStdBind.cxx
)
if(${HAVE_STD_BIND})
    message(STATUS "Check for working std::bind in <functional> -- works")
endif()

message(STATUS "Check for working std::tr1::bind in <tr1/functional>")
try_compile(HAVE_STD_TR1_BIND
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/TryCompileStdTr1Bind.cxx
)
if(${HAVE_STD_TR1_BIND})
    message(STATUS "Check for working std::tr1::bind in <tr1/functional> -- works")
endif()

if(NOT ${HAVE_STD_BIND} AND NOT ${HAVE_STD_TR1_BIND})
    message(STATUS "Falling back to boost for 'bind'.")
    find_package(Boost REQUIRED)
    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${Boost_LIBRARIES})
    endif()
endif()


include(FindPkgConfig)

### FLTK
# OpenGL and Forms libraries are not required.
set(FLTK_SKIP_OPENGL TRUE)
set(FLTK_SKIP_FORMS TRUE)
# Try to find the config way first
#find_package(FLTK REQUIRED CONFIG)
find_package(FLTK REQUIRED)
if(FLTK_FOUND)
    include_directories(${FLTK_INCLUDE_DIRS})
    # Set required CXX flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLTK_CXX_FLAGS}")
    # FindFLTK does not properly handle ldflags for static libraries.
    get_filename_component(FLTK_LIB_EXT ${FLTK_BASE_LIBRARY} EXT)
    if(${FLTK_LIB_EXT} STREQUAL ${CMAKE_STATIC_LIBRARY_SUFFIX})
        message(STATUS "Using static FLTK libraries.")
        execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --use-images --ldstaticflags
            OUTPUT_VARIABLE FLTK_LDFLAGS)
        separate_arguments(FLTK_LDFLAGS)
        foreach(_FLAG ${FLTK_LDFLAGS})
            string(REGEX MATCH "^-l.+" _LDFLAG ${_FLAG})
            string(STRIP "${_LDFLAG}" _LDFLAG)
            list(APPEND FLTK_LIBRARIES ${_LDFLAG})
        endforeach()
        list(REMOVE_DUPLICATES FLTK_LIBRARIES)
    endif()
    list(APPEND COMMON_LINK_LIBS ${FLTK_LIBRARIES})
    set(HAVE_FLUID TRUE)
endif()

### OSS
if(WIN32)
    message(STATUS "OSS disabled for windows targets.")
    set(USE_OSS FALSE)
    set(OSS_VERSION "Disabled")
else()
    pkg_check_modules(OSS oss)
    if(OSS_FOUND)
        include_directories(${OSS_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${OSS_LIBRARIES})
        add_definitions(-DUSE_OSS)
        set(USE_OSS TRUE)
    else()
        set(OSS_VERSION "Not found")
    endif()
endif()

### libpng
pkg_check_modules(PNG REQUIRED libpng)
if(PNG_FOUND AND PNG_VERSION VERSION_GREATER "1.2.8")
    include_directories(${PNG_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${PNG_LIBRARIES})
    add_definitions(-DUSE_PNG)
    set(USE_PNG TRUE)
endif()

### libsamplerate
pkg_check_modules(SAMPLERATE REQUIRED samplerate)
if(SAMPLERATE_FOUND AND SAMPLERATE_VERSION VERSION_GREATER 0.1.1)
    include_directories(${SAMPLERATE_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${SAMPLERATE_LIBRARIES})
    add_definitions(-DUSE_SAMPLERATE)
    set(USE_SAMPLERATE TRUE)
endif()

### libsndfile
pkg_check_modules(SNDFILE sndfile)
if(SNDFILE_FOUND AND SNDFILE_VERSION VERSION_GREATER 1.0.10)
    include_directories(${SNDFILE_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${SNDFILE_LIBRARIES})
    add_definitions(-DUSE_SNDFILE)
    set(USE_SNDFILE TRUE)
endif()

### portaudio
pkg_check_modules(PORTAUDIO portaudio-2.0)
if(PORTAUDIO_FOUND AND PORTAUDIO_VERSION VERSION_GREATER 18)
    include_directories(${PORTAUDIO_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${PORTAUDIO_LIBRARIES})
    exec_program(${PKG_CONFIG_EXECUTABLE}
        ARGS --cflags-only-other portaudio-2.0
        OUTPUT_VARIABLE PORTAUDIO_CFLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PORTAUDIO_CFLAGS}")
    add_definitions(-DUSE_PORTAUDIO)
    set(USE_PORTAUDIO TRUE)
endif()

### pulseaudio
if(WIN32)
    message(STATUS "PulseAudio disabled for windows targets.")
    set(USE_PULSEAUDIO FALSE)
    set(PULSEAUDIO_VERSION "Disabled")
else()
    pkg_check_modules(PULSEAUDIO libpulse-simple)
    if(PULSEAUDIO_FOUND AND PULSEAUDIO_VERSION VERSION_GREATER 0.9.7)
        include_directories(${PULSEAUDIO_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${PULSEAUDIO_LIBRARIES})
        add_definitions(-DUSE_PULSEAUDIO)
        set(USE_PULSEAUDIO TRUE)
    endif()
endif()

if(NOT OSS_FOUND AND NOT PORTAUDIO_FOUND AND NOT PULSEAUDIO_FOUND)
    message(WARNING "*** $PACKAGE will be compiled without audio device support ***")
endif()

### hamlib
if(ENABLE_HAMLIB)
    pkg_check_modules(HAMLIB hamlib)
    if(HAMLIB_FOUND AND HAMLIB_VERSION VERSION_GREATER 1.2.4)
        include_directories(${HAMLIB_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${HAMLIB_LIBRARIES})
        add_definitions(-DUSE_HAMLIB)
        # We compile our own copy of hamlib's locator.c if hamlib is not
        # available or if it is version <= 1.2.9, which has a broken
        # azimuth_long_path function
        if(HAMLIB_VERSION VERSION_LESS 1.2.10)
            set(NEED_HAMLIB_LOCATOR TRUE)
        endif()
    elseif(HAMLIB_FOUND)
        message(ERROR "Hamlib found but your version (${HAMLIB_VERSION}) is too old.")
    else()
        message(WARNING "Hamlib support requested but the library could not be found.")
        set(HAMLIB_VERSION "Not found")
        set(NEED_HAMLIB_LOCATOR TRUE)
    endif()
endif()

### X11
if(NOT APPLE AND NOT WIN32)
    pkg_check_modules(X x11)
    if(X_FOUND)
        include_directories(${X_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${X_LIBRARIES})
        add_definitions(-DUSE_X)
    endif()
endif()


### libintl
if(ENABLE_NLS)
    find_package(Libintl)
    if(LIBINTL_FOUND)
        list(APPEND COMMON_LINK_LIBS ${LIBINTL_LIBRARIES})
        find_library(LIBICONV iconv)
        if(LIBICONV)
            message(STATUS "Found Libiconv: ${LIBICONV}")
            list(APPEND COMMON_LINK_LIBS ${LIBICONV})
        else()
            message(STATUS "Couldn't find libiconv, hope we don't need it!")
        endif()
    else()
        mesage(WARNING "Language support requested but libintl was not found.
Disabling language support.")
        set(ENABLE_NLS OFF)
    endif()
endif()

### asciidoc
find_program(ASCIIDOC asciidoc)
find_program(A2X a2x)
if(ASCIIDOC AND A2X)
    message(STATUS "Asciidoc found: ${ASCIIDOC}")
    message(STATUS "     a2x found: ${A2X}")
    add_subdirectory(doc)
else()
    message(STATUS "Both asciidoc and a2x are required to build man pages and html guide.")
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)
add_definitions(-DHAVE_CONFIG_H)


### summary
message("
Configuration summary:

  Version ..................... ${FLDIGI_VERSION}

  Static linking .............. ${ENABLE_STATIC}
  Build Type .................. ${CMAKE_BUILD_TYPE}

  fldigi ...................... ${WANT_FLDIGI}
  flarq ....................... ${WANT_FLARQ}

  i18n ........................ ${ENABLE_NLS}
")

message("
fldigi build options:

  fltk ........................ ${FLTK_VERSION}
  sndfile ..................... ${SNDFILE_VERSION}
  oss ......................... ${OSS_VERSION}
  portaudio ................... ${PORTAUDIO_VERSION}
  pulseaudio .................. ${PULSEAUDIO_VERSION}

  hamlib ...................... ${HAMLIB_VERSION}
")

add_subdirectory(src)
add_subdirectory(data)
if(GETTEXT_FOUND AND ENABLE_NLS)
    add_subdirectory(po)
endif()


if(WIN32 AND (WANT_FLDIGI OR WANT_FLARQ))
    # Collect required runtimes (libraries not linked statically)
    # Must be run at install time since binaries are scanned.
    configure_file(cmake/GetDependencies.cmake.in cmake/GetDependencies.cmake
        @ONLY)
    install(SCRIPT ${CMAKE_BINARY_DIR}/cmake/GetDependencies.cmake)
    include(MakeNsisInstaller)
    include(CPack)
elseif(WIN32)
    message(WARNING "Neither Fldigi or Flarq set to build, an installer is not possible.")
endif()
