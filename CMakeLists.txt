project(fldigi)
cmake_minimum_required(VERSION 2.8)

# Set default built type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Setup options with defaults.
option(WANT_FLDIGI "Build Fldigi." ON)
option(WANT_FLARQ "Build Flarq." ON)
option(USE_NLS "Use Native Language Support" ON)
option(ENABLE_HAMLIB "Enable hamlib for rig control." ON)
option(ENABLE_MAC_UNIVERSAL "Build a universal binary on Mac OS X" OFF)
option(ENABLE_STATIC "Use static linking for some libraries." OFF)

set(CMAKE_CXX_FLAGS "-Wall -ffast-math -finline-functions")

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/irrxml
    ${CMAKE_SOURCE_DIR}/src/libtiniconv
    ${CMAKE_SOURCE_DIR}/src/fileselector
    ${CMAKE_SOURCE_DIR}/src/xmlrpcpp
    ${CMAKE_BINARY_DIR}
)

# Setup standard install locations.
include(GNUInstallDirs)
add_definitions(-DPKGDATADIR="${CMAKE_INSTALL_FULL_DATADIR}")

# major and minor must be integers; patch may
# contain other characters or be empty
set(FLDIGI_VERSION_MAJOR 3)
set(FLDIGI_VERSION_MINOR 22)
set(FLDIGI_VERSION_PATCH .0CN)
set(FLARQ_VERSION_MAJOR 4)
set(FLARQ_VERSION_MINOR 3)
set(FLARQ_VERSION_PATCH .3)

set(FLDIGI_VERSION ${FLDIGI_VERSION_MAJOR}.${FLDIGI_VERSION_MINOR}${FLDIGI_VERSION_PATCH})

#AM_MAINTAINER_MODE
set(COPYRIGHT "Copyright (C) 2007, 2008, 2009 Stelios Bounanos, M0GLD (m0gld AT enotty DOT net)")

set(FLDIGI_AUTHORS "Dave Freese, Stelios Bounanos, Leigh Klotz, Remi Chateauneu, and others")
set(FLARQ_AUTHORS "Dave Freese")
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
set(FLARQ_DOCS "http://www.w1hkj.com/FlarqHelpFiles/flarq.html")

exec_program(date OUTPUT_VARIABLE BUILD_DATE)
cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)

# Checks for programs.
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


# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_C_INLINE
#AC_TYPE_INT16_T
#AC_TYPE_INT32_T
#AC_TYPE_INT64_T
#AC_TYPE_INT8_T
#AC_C_RESTRICT
#AC_TYPE_SIZE_T
#AC_HEADER_TIME
#AC_STRUCT_TM
#AC_TYPE_UINT16_T
#AC_TYPE_UINT32_T
#AC_TYPE_UINT64_T
#AC_TYPE_UINT8_T
#AC_C_VOLATILE

# Checks for library functions.
include(CheckFunctionExists)
check_function_exists(error_at_line HAVE_ERROR_AT_LINE)
set(CHECK_FUNCS "clock_gettime getaddrinfo gethostbyname hstrerror gmtime_r localtime_r memmove memset mkdir select setenv snprintf socket socketpair strcasecmp strcasestr strchr strdup strerror strlcpy strncasecmp strrchr strstr strtol uname unsetenv vsnprintf")
separate_arguments(CHECK_FUNCS)
foreach(FUNC ${CHECK_FUNCS})
    string(TOUPPER ${FUNC} UPPER_${FUNC})
    check_function_exists(${FUNC} HAVE_${UPPER_${FUNC}})
endforeach()

# Check for O_CLOEXEC
#AC_FCNTL_FLAGS

#AC_PRESERVE_HELP_ORDER

### Internationalization
find_package(Gettext)
if(${GETTEXT_FOUND} AND USE_NLS)
    set(HAVE_GETTEXT TRUE)
    add_definitions(-DLOCALEDIR="${CMAKE_INSTALL_FULL_LOCALEDIR}")
endif()

###### OS support
### OSX
# Set ac_cv_mac_universal to yes/no
# Set DARWIN Makefile conditional
# Substitute MAC_UNIVERSAL_CFLAGS and MAC_UNIVERSAL_LDFLAGS in Makefile
#AC_FLDIGI_MACOSX
### win32
# Set WIN32 Makefile conditional
# Set HAVE_WINDRES Makefile conditional
# Substitute WINDRES in Makefile
#AC_FLDIGI_WIN32

### Non-POSIX compatibility (i.e. mingw32)
# Sets various Makefile conditionals; see m4/np-compat.m4
#AC_FLDIGI_NP_COMPAT


### static flag
# Set ac_cv_static to yes/no
# Substitute RTLIB in Makefile
#AC_FLDIGI_STATIC

### benchmark mode
# Set ac_cv_benchmark to yes/no
# Define BENCHMARK_MODE in config.h
# Set ENABLE_BENCHMARK Makefile conditional
option(ENABLE_BENCHMARK "Enable benchmarking?" OFF)

### TLS flag
# Set ac_cv_tls to yes/no
# Define USE_TLS in config.h
#AC_FLDIGI_TLS

### std::bind or alternatives
# Set ac_cv_std_bind or ac_cv_std_tr1_bind to yes/no
# Define HAVE_STD_BIND or HAVE_STD_TR1_BIND in config.h
# Substitute BOOST_CPPFLAGS and BOOST_LDFLAGS in Makefile
# if falling back to Boost
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


set(FLDIGI_LINK_LIBS "")
include(FindPkgConfig)

### FLTK
# Substitute FLTK_CFLAGS and FLTK_LIBS in Makefile
# Set FLUID variable
# Set HAVE_FLUID Makefile conditional
find_package(FLTK REQUIRED)
if(${FLTK_FOUND})
    include_directories(${FLTK_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${FLTK_LIBRARIES})
    exec_program(${FLTK_CONFIG_SCRIPT} ARGS --cxxflags
        OUTPUT_VARIABLE FLTK_CXXFLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLTK_CXXFLAGS}")
    set(HAVE_FLUID TRUE)
    exec_program(${FLTK_CONFIG_SCRIPT} ARGS --version
        OUTPUT_VARIABLE FLTK_VERSION)
endif()

### OSS
# Set ac_cv_oss to yes/no
# Define USE_OSS in config.h
pkg_check_modules(OSS oss)
if(${OSS_FOUND})
    include_directories(${OSS_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${OSS_LIBRARIES})
    add_definitions(-DUSE_OSS)
    set(USE_OSS TRUE)
endif()

### libpng
# Required if $ac_cv_want_fldigi is "yes"
# Set ac_cv_png to yes/no    (not used)
# Define USE_PNG in config.h (as above)
# Substitute PNG_CFLAGS and PNG_LIBS in Makefile
pkg_check_modules(PNG REQUIRED libpng)
if(PNG_FOUND AND PNG_VERSION VERSION_GREATER "1.2.8")
    include_directories(${PNG_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${PNG_LIBRARIES})
    add_definitions(-DUSE_PNG)
    set(USE_PNG TRUE)
endif()

### libsamplerate
# Required if $ac_cv_want_fldigi is "yes"
# Set ac_cv_samplerate to yes/no    (not used)
# Define USE_SAMPLERATE in config.h (as above)
# Substitute SAMPLERATE_CFLAGS and SAMPLERATE_LIBS in Makefile
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
pkg_check_modules(PULSEAUDIO libpulse-simple)
if(PULSEAUDIO_FOUND AND PULSEAUDIO_VERSION VERSION_GREATER 0.9.7)
    include_directories(${PULSEAUDIO_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${PULSEAUDIO_LIBRARIES})
    add_definitions(-DUSE_PULSEAUDIO)
    set(USE_PULSEAUDIO TRUE)
endif()

if(NOT OSS_FOUND AND NOT PORTAUDIO_FOUND AND NOT PULSEAUDIO_FOUND)
    message(WARNING "*** $PACKAGE will be compiled without audio device support ***")
endif()

### hamlib
# Set ac_cv_hamlib to yes/no
# Define USE_HAMLIB in config.h
# Substitute HAMLIB_CFLAGS and HAMLIB_LIBS in Makefile
# Set ENABLE_HAMLIB Makefile conditional
pkg_check_modules(HAMLIB hamlib)
if(HAMLIB_FOUND AND HAMLIB_VERSION VERSION_GREATER 1.2.4)
    include_directories(${HAMLIB_INCLUDE_DIRS})
    list(APPEND FLDIGI_LINK_LIBS ${HAMLIB_LIBRARIES})
    add_definitions(-DUSE_HAMLIB)
endif()

# We compile our own copy of hamlib's locator.c if hamlib is not available
# or if it is version <= 1.2.9, which has a broken azimuth_long_path function
#AX_COMPARE_VERSION([$pkg_hamlib_version], [le], [1.2.9], [need_locator_c=yes], [need_locator_c=no])
#AM_CONDITIONAL([NEED_HAMLIB_LOCATOR], [test "$need_locator_c" = "yes"])

### X11
# Set ac_cv_x to yes/no
# Define USE_X in config.h
# Substitute X_CFLAGS and X_LIBS in Makefile
if(NOT APPLE AND NOT WIN32)
    pkg_check_modules(X x11)
    if(X_FOUND)
        include_directories(${X_INCLUDE_DIRS})
        list(APPEND FLDIGI_LINK_LIBS ${X_LIBRARIES})
        add_definitions(-DUSE_X)
    endif()
endif()


### libintl
# Substitute INTL_CFLAGS in Makefile
# Substitute INTL_LIBS in Makefile
# (default to LIBINTL)
#AC_FLDIGI_INTL

### asciidoc
# substitute ASCIIDOC and A2X in doc/Makefile
# set HAVE_ASCIIDOC Makefile conditional
#AC_FLDIGI_DOCS

### build info
# Define various build variables in config.h
#AC_FLDIGI_BUILD_INFO

### silent build rules
#AC_FLDIGI_BUILD_RULES

### output
#AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in doc/Makefile])
#AC_OUTPUT

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

  fldigi ...................... ${BUILD_FLDIGI}
  flarq ....................... ${BUILD_FLARQ}

  i18n ........................ ${USE_NLS}
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
#add_subdirectory(po)
