# CMakeLists.txt - Nighthawk 4.x
# By Jason Nunn, 12OCT20
#
# This file was developed with the help of Troy50's examples:
# https://github.com/ttroy50/cmake-examples
#
# Makefile generation cmdlines:
#
# mkdir <somewhere>/build
# cd <somewhere>/build
# cmake <Nighthawk source code filepath (pointing to this file)>
# ccmake .   (Optional. To edit settings)
# make
#
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Developmental/Alpha testing install path: "."
#
#set(CMAKE_INSTALL_PREFIX "." CACHE PATH "Install path" FORCE)

# Beta/Stable install path
#
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path" FORCE)

# Version control for project normally goes into this function, but I
# find it too restrictive, so retained the version.h file.
#
project(nighthawk)

# Make compiling verbose (so the compile flags can be seen). When
# project becomes stable, this can be commented out.
#
set(CMAKE_VERBOSE_MAKEFILE ON)

# Make building ned optional. Switch it on using "ccmake ."
# Eric: If this is giving you the shits, set this to "ON". The downer, it
# makes everything compile twice (can't workout how to stop that at the moment).
#
option(BUILD_NED "Build Nighthawk Floor Editor (ned)" OFF)

# As above. Once stable, this cam be adjusted accordingly.
#
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
#set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
#set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

# Setup game data
#
set(DATA_PATH   "${CMAKE_INSTALL_PREFIX}/share/nighthawk" CACHE PATH "Data path" FORCE)

# Setup scores file
#
# Check this directory path exists, as this will only be discovered during
# runtime of the game (so better check it now).
#
set(SCORES_PATH "/var/tmp" CACHE PATH "Scores directory" FORCE)
execute_process(COMMAND ls ${SCORES_DIR} RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET)
if (result)
	message(FATAL_ERROR "Scores directory '${SCORES_DIR}' does not exist (choose another location or create it manually).")
endif ()

# Game has fairly relaxed languages standard.
#
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Search for pkg-config (this is used to find packages)
#
include (FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
	message (FATAL_ERROR "pkg-config not found.")
endif ()

# Specify pthread and m glibc libraries
# Nb/ These libraries don't need specifying on Windose
#
if (NOT WIN32)
	# Require maths library
	list(APPEND LIBS m)
	# For CHECK_FUNCTION_EXISTS:
	list(APPEND CMAKE_REQUIRED_LIBRARIES m)

	# Require pthreads
	list(APPEND LIBS pthread)
	list(APPEND CMAKE_REQUIRED_LIBRARIES pthread)

	link_libraries (${LIBS})
endif ()

# Check for libpng
#
# To test the pkg-config database in a shell, type:
# pkg-config libpng --libs
#
pkg_check_modules (LIBPNG libpng)
if (NOT LIBPNG_FOUND)
	message(FATAL_ERROR "PNG devel library not installed.")
else ()
	include_directories (${LIBPNG_INCLUDE_DIRS})
	link_directories (${LIBPNG_LIBRARY_DIRS})
	link_libraries (${LIBPNG_LIBRARIES})
endif ()

# Check for OpenGL and GLU
#
pkg_check_modules (OPENGL glu)
if (NOT OPENGL_FOUND)
	message(FATAL_ERROR "OpenGL/GLU devel library not installed.")
else ()
	include_directories (${OPENGL_INCLUDE_DIRS})
	link_directories (${OPENGL_LIBRARY_DIRS})
	link_libraries (${OPENGL_LIBRARIES})
endif ()

# Check for FreeGLUT
#
# This library is a bit of a funny one. On my 2 year old Linux Mint 19.2 dist,
# the package comes with a pkg-config definition and the code below passes.
# On my newly installed Ubuntu 20.04 dist, the freeglut package doesn't have
# one, and the code below fails. As a work around, I've specified the "-lglut"
# option. However, if this fails on your system, and you want a clean fix,
# download freeglut the source code https://sourceforge.net/projects/freeglut
# This should guarantee a clean compile.
#
pkg_check_modules (GLUT glut)
if (NOT GLUT_FOUND)
	message(WARNING "GLUT devel library not found in pkg-config. Adding -lglut.")
	list(APPEND GLUT_LIBS glut)
	list(APPEND CMAKE_REQUIRED_LIBRARIES glut)
	link_libraries (${GLUT_LIBS})
else ()
	include_directories (${GLUT_INCLUDE_DIRS})
	link_directories (${GLUT_LIBRARY_DIRS})
	link_libraries (${GLUT_LIBRARIES})
endif ()

# Check for OggVorbis
#
pkg_check_modules (VORBIS vorbis)
if (NOT VORBIS_FOUND)
	message(FATAL_ERROR "OggVorbis devel library not installed.")
else ()
	include_directories (${VORBIS_INCLUDE_DIRS})
	link_directories (${VORBIS_LIBRARY_DIRS})
	link_libraries (${VORBIS_LIBRARIES})
endif ()

# Check for Vorbisfile
#
pkg_check_modules (VORBISFILE vorbisfile)
if (NOT VORBISFILE_FOUND)
	message(FATAL_ERROR "Vorbisfile devel library not installed.")
else ()
	include_directories (${VORBISFILE_INCLUDE_DIRS})
	link_directories (${VORBISFILE_LIBRARY_DIRS})
	link_libraries (${VORBISFILE_LIBRARIES})
endif ()

# Check for OpenAL
#
pkg_check_modules (OPENAL openal)
if (NOT OPENAL_FOUND)
	message(FATAL_ERROR "OpenAL devel library not installed.")
else ()
	include_directories (${OPENAL_INCLUDE_DIRS})
	link_directories (${OPENAL_LIBRARY_DIRS})
	link_libraries (${OPENAL_LIBRARIES})
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})

add_executable(nighthawk
	src/version.h
	src/defs.h
	src/linked_data.c
	src/linked_data_externs.h
	src/misc.c
	src/misc_externs.h
	src/scores.c
	src/scores_externs.h
	src/sprite_loader.c
	src/sprite_loader_externs.h
	src/opengl.c
	src/opengl_externs.h
	src/xpm_loader.c
	src/xpm_loader_externs.h
	src/png_loader.c
	src/png_loader_externs.h
	src/openal.c
	src/openal_externs.h
	src/tpower_bay.h
	src/power_bay.cc
	src/tdoor.h
	src/door.cc
	src/tfloor.h
	src/floor.cc
	src/tentity.h
	src/entity.cc
	src/tlaser.h
	src/laser.cc
	src/tdroid.h
	src/droid.cc
	src/tdroid_1xx.h
	src/tdroid_2xx.h
	src/tdroid_3xx.h
	src/tdroid_4xx.h
	src/tdroid_5xx.h
	src/tdroid_6xx.h
	src/tdroid_7xx.h
	src/tdroid_8xx.h
	src/tdroid_9xx.h
	src/droid_xxx.cc
	src/tparadroid.h
	src/paradroid.cc
	src/tship.h
	src/ship.cc
	src/intro_externs.h
	src/intro.cc
	src/game_externs.h
	src/game.cc
	src/main.cc
)

if (BUILD_NED)
add_executable(nighthawk_ned
	src/version.h
	src/defs.h
	src/linked_data.c
	src/linked_data_externs.h
	src/misc.c
	src/misc_externs.h
	src/scores.c
	src/scores_externs.h
	src/sprite_loader.c
	src/sprite_loader_externs.h
	src/opengl.c
	src/opengl_externs.h
	src/xpm_loader.c
	src/xpm_loader_externs.h
	src/png_loader.c
	src/png_loader_externs.h
	src/openal.c
	src/openal_externs.h
	src/tpower_bay.h
	src/power_bay.cc
	src/tdoor.h
	src/door.cc
	src/tfloor.h
	src/floor.cc
	src/tedit_floor.h
	src/ned_externs.h
	src/edit_floor.cc
	src/ned.cc
)
endif()

# Installing
#
if (BUILD_NED)
	install (TARGETS nighthawk nighthawk_ned DESTINATION bin)
else ()
	install (TARGETS nighthawk DESTINATION bin)
endif ()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/ DESTINATION ${DATA_PATH})

# Install nighthawk in games section of man page tree (iaw "man man")
#
install (FILES man/nighthawk.6 DESTINATION man/man6)
