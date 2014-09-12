#.rst:
# FindFLTK
# --------
#
# Find the native FLTK includes and library
#
#
#
# By default FindFLTK.cmake will search for all of the FLTK components
# and add them to the FLTK_LIBRARIES variable.
#
# ::
#
#    You can limit the components which get placed in FLTK_LIBRARIES by
#    defining one or more of the following three options:
#
#
# ::
#
#      FLTK_SKIP_OPENGL, set to true to disable searching for opengl and the FLTK GL library
#      FLTK_SKIP_FORMS,  set to true to disable searching for fltk_forms
#      FLTK_SKIP_IMAGES, set to true to disable searching for fltk_images
#
#
# ::
#
#      FLTK_SKIP_FLUID, set to true if the fluid binary need not be present at build time
#      FLTK_CONFIG_DIR, location of ConfigFLTK.cmake. Set if a non-standard location is being used including the fltk build root.
#
#
# The following variables will be defined:
#
# ::
#
#      FLTK_FOUND,       True if all components not skipped were found
#      FLTK_INCLUDE_DIR, Where to find include files
#      FLTK_LIBRARIES,   List of fltk libraries you should link against
#      FLTK_FLUID_EXECUTABLE, Where to find the Fluid tool
#      FLTK_WRAP_UI,     This enables the FLTK_WRAP_UI command
#
#
# The following cache variables are assigned but should not be used.
# See the FLTK_LIBRARIES variable instead.
#
# ::
#
#      FLTK_BASE_LIBRARY,   the full path to fltk.lib
#      FLTK_GL_LIBRARY,     the full path to fltk_gl.lib
#      FLTK_FORMS_LIBRARY,  the full path to fltk_forms.lib
#      FLTK_IMAGES_LIBRARY, the full path to fltk_images.lib

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if(NOT FLTK_SKIP_OPENGL)
  find_package(OpenGL)
endif()

#  Platform dependent libraries required by FLTK
if(WIN32 AND NOT CYGWIN)
  if(BORLAND)
    set(FLTK_PLATFORM_DEPENDENT_LIBS import32)
  else()
    set(FLTK_PLATFORM_DEPENDENT_LIBS wsock32 comctl32)
  endif()
endif()

if(UNIX AND NOT APPLE)
  find_package(X11)
  find_library(FLTK_MATH_LIBRARY m)
  set(FLTK_PLATFORM_DEPENDENT_LIBS ${X11_LIBRARIES} ${FLTK_MATH_LIBRARY})
endif()

if(APPLE)
  set(FLTK_PLATFORM_DEPENDENT_LIBS  "-framework Carbon -framework Cocoa -framework ApplicationServices -lz")
endif()


# If FLTK has been built using CMake we try to find everything directly

# Search only if the location is not already known.
if(NOT FLTK_CONFIG_DIR)
  # Get the system search path as a list.
  file(TO_CMAKE_PATH "$ENV{PATH}" FLTK_CONFIG_DIR_SEARCH2)

  # Construct a set of paths relative to the system search path.
  set(FLTK_CONFIG_DIR_SEARCH "")
  foreach(dir ${FLTK_CONFIG_DIR_SEARCH2})
    list(APPEND FLTK_CONFIG_DIR_SEARCH "${dir}/../lib/fltk")
  endforeach()
  if(FLTK_DEBUG)
    message(STATUS "Raw search directories: ${FLTK_CONFIG_DIR_SEARCH}")
  endif()
  string(REPLACE "//" "/" FLTK_CONFIG_DIR_SEARCH "${FLTK_CONFIG_DIR_SEARCH}")

  #
  # Look for an installation or build tree.
  #
  find_file(FLTK_CONFIG_FILE FLTKConfig.cmake
    # Look for an environment variable FLTK_DIR.
    PATHS
    ENV FLTK_DIR

    # Look in places relative to the system executable search path.
    ${FLTK_CONFIG_DIR_SEARCH}

    # On mulilib systems FLTKConfig.cmake would go in the library path since
    # it contains arch specific data.
    ${CMAKE_SYSTEM_LIBRARY_PATH}

    # Look in standard UNIX install locations.
    /usr/local/lib/fltk
    /usr/lib/fltk
    /usr/local/fltk
    /usr/X11R6/include

    # Read from the CMakeSetup registry entries.  It is likely that
    # FLTK will have been recently built.
    # TODO: Is this really a good idea?  I can already hear the user screaming, "But
    # it worked when I configured the build LAST week!"
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]

    PATH_SUFFIXES
      FLTK
      FLTK-1.0
      FLTK-1.1
      FLTK-1.2
      FLTK-1.3
    # Help the user find it if we cannot.
    DOC "The directory containing FLTKConfig.cmake.  This is either the root of the build tree, PREFIX/lib/fltk, or similar for an installation.")
endif()

# Check if FLTK was built using CMake
if(EXISTS ${FLTK_CONFIG_FILE})
  include(${FLTK_CONFIG_FILE})

  # Fluid
  if(FLUID_COMMAND)
    set(FLTK_FLUID_EXECUTABLE ${FLUID_COMMAND} CACHE FILEPATH "Fluid executable")
  else()
    find_program(FLTK_FLUID_EXECUTABLE fluid PATHS
      ${FLTK_EXECUTABLE_DIRS}
      ${FLTK_EXECUTABLE_DIRS}/RelWithDebInfo
      ${FLTK_EXECUTABLE_DIRS}/Debug
      ${FLTK_EXECUTABLE_DIRS}/Release
      NO_SYSTEM_PATH)
  endif()

  # This makes no sense. FLTK_DIR is the library location, not the header.
  #set(FLTK_INCLUDE_DIR ${FLTK_DIR})
  #link_directories(${FLTK_LIBRARY_DIRS})

  set(FLTK_BASE_LIBRARY fltk)
  set(FLTK_GL_LIBRARY fltk_gl)
  set(FLTK_FORMS_LIBRARY fltk_forms)
  set(FLTK_IMAGES_LIBRARY fltk_images)

  # Add the extra libraries
  load_cache(${FLTK_CONFIG_FILE}
    READ_WITH_PREFIX
    FL FLTK_USE_SYSTEM_JPEG
    FL FLTK_USE_SYSTEM_PNG
    FL FLTK_USE_SYSTEM_ZLIB
  )

  set(FLTK_IMAGES_LIBS "")
  if(FLFLTK_USE_SYSTEM_JPEG)
    set(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_jpeg)
  endif()
  if(FLFLTK_USE_SYSTEM_PNG)
    set(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_png)
  endif()
  if(FLFLTK_USE_SYSTEM_ZLIB)
    set(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_zlib)
  endif()
  set(FLTK_IMAGES_LIBS "${FLTK_IMAGES_LIBS}" CACHE INTERNAL
    "Extra libraries for fltk_images library.")
else()

  # if FLTK was not built using CMake
  # Find fluid executable.
  find_program(FLTK_FLUID_EXECUTABLE fluid
    PATHS
    ${FLTK_CONFIG_DIR}
    ${FLTK_CONFIG_DIR}/../fluid
  )

  # Use location of fluid to help find everything else.
  set(FLTK_INCLUDE_SEARCH_PATH "")
  set(FLTK_LIBRARY_SEARCH_PATH "")
  if(FLTK_FLUID_EXECUTABLE)
    set(FLTK_WRAP_UI 1)
    get_filename_component(FLTK_BIN_DIR "${FLTK_FLUID_EXECUTABLE}" PATH)
    set(FLTK_INCLUDE_SEARCH_PATH ${FLTK_INCLUDE_SEARCH_PATH}
      ${FLTK_BIN_DIR}/../include ${FLTK_BIN_DIR}/..)
    set(FLTK_LIBRARY_SEARCH_PATH ${FLTK_LIBRARY_SEARCH_PATH}
      ${FLTK_BIN_DIR}/../lib)
  endif()

  #
  # Try to find FLTK include dir, version, and libraries using fltk-config
  #
  if(UNIX)
    find_program(FLTK_CONFIG_SCRIPT fltk-config 
      PATHS
      ${FLTK_BIN_DIR}
      ${FLTK_BIN_DIR}/..
    )
    if(FLTK_CONFIG_SCRIPT)
    # Use fltk-config to generate a list of possible include directories
      if(NOT FLTK_INCLUDE_DIR)
        execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --includedir
          OUTPUT_VARIABLE FLTK_INCLUDE_DIR)
      endif()
    # Find FLTK version.
      execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --version
        OUTPUT_VARIABLE FLTK_VERSION)
      string(STRIP "${FLTK_VERSION}" FLTK_VERSION)

    #
    # Try to find FLTK library
    #
      execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --libs
        OUTPUT_VARIABLE _FLTK_POSSIBLE_LIBS)
      if(_FLTK_POSSIBLE_LIBS)
        get_filename_component(_FLTK_POSSIBLE_LIBRARY_DIR
          ${_FLTK_POSSIBLE_LIBS} PATH)
      endif()
    endif()
  endif()

  set(FLTK_LIBRARY_SEARCH_PATH ${FLTK_LIBRARY_SEARCH_PATH}
    /usr/local/fltk/lib
    /usr/X11R6/lib
    ${FLTK_ROOT_DIR}/lib
    )

  find_library(FLTK_BASE_LIBRARY NAMES fltk fltkd
    HINTS ${_FLTK_POSSIBLE_LIBRARY_DIR}
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  find_library(FLTK_GL_LIBRARY NAMES fltkgl fltkgld fltk_gl
    HINTS ${_FLTK_POSSIBLE_LIBRARY_DIR}
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  find_library(FLTK_FORMS_LIBRARY NAMES fltkforms fltkformsd fltk_forms
    HINTS ${_FLTK_POSSIBLE_LIBRARY_DIR}
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  find_library(FLTK_IMAGES_LIBRARY NAMES fltkimages fltkimagesd fltk_images
    HINTS ${_FLTK_POSSIBLE_LIBRARY_DIR}
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})

  # Find the extra libraries needed for the fltk_images library.
  if(UNIX AND FLTK_CONFIG_SCRIPT)
      execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --use-images --ldflags
        OUTPUT_VARIABLE FLTK_IMAGES_LDFLAGS)
      set(FLTK_LIBS_EXTRACT_REGEX ".*-lfltk_images (.*) -lfltk.*")
      if("${FLTK_IMAGES_LDFLAGS}" MATCHES "${FLTK_LIBS_EXTRACT_REGEX}")
        string(REGEX REPLACE " +" ";" FLTK_IMAGES_LIBS "${CMAKE_MATCH_1}")
        # The EXEC_PROGRAM will not be inherited into subdirectories from
        # the file that originally included this module.  Save the answer.
        set(FLTK_IMAGES_LIBS "${FLTK_IMAGES_LIBS}" CACHE INTERNAL
          "Extra libraries for fltk_images library.")
      endif()
  endif()

endif()

# Append all of the required libraries together (by default, everything)
set(FLTK_LIBRARIES ${FLTK_BASE_LIBRARY})
if(NOT FLTK_SKIP_IMAGES)
  list(APPEND FLTK_LIBRARIES ${FLTK_IMAGES_LIBRARY})
endif()
if(NOT FLTK_SKIP_FORMS)
  list(APPEND FLTK_LIBRARIES ${FLTK_FORMS_LIBRARY})
endif()
if(NOT FLTK_SKIP_OPENGL)
  list(APPEND FLTK_LIBRARIES ${FLTK_GL_LIBRARY} ${OPENGL_gl_LIBRARY})
  set(FLTK_INCLUDE_DIRS ${FLTK_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
  list(REMOVE_DUPLICATES FLTK_INCLUDE_DIR)
endif()

include(FindPackageHandleStandardArgs)
if(FLTK_SKIP_FLUID)
  find_package_handle_standard_args(FLTK 
    REQUIRED_VARS FLTK_LIBRARIES FLTK_INCLUDE_DIR
    VERSION_VAR FLTK_VERSION
  )
else()
  find_package_handle_standard_args(FLTK
    REQUIRED_VARS FLTK_LIBRARIES FLTK_INCLUDE_DIR FLTK_FLUID_EXECUTABLE
    VERSION_VAR FLTK_VERSION
  )
endif()

if(FLTK_FOUND)
  if(APPLE)
    set(FLTK_LIBRARIES ${FLTK_PLATFORM_DEPENDENT_LIBS} ${FLTK_LIBRARIES})
  else()
    set(FLTK_LIBRARIES ${FLTK_LIBRARIES} ${FLTK_PLATFORM_DEPENDENT_LIBS})
  endif()

  # The following deprecated settings are for compatibility with CMake 1.4
  set(HAS_FLTK ${FLTK_FOUND})
  set(FLTK_INCLUDE_DIR ${FLTK_INCLUDE_DIRS})
  set(FLTK_INCLUDE_PATH ${FLTK_INCLUDE_DIR})
  set(FLTK_FLUID_EXE ${FLTK_FLUID_EXECUTABLE})
  set(FLTK_LIBRARY ${FLTK_LIBRARIES})
endif()
