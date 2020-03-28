#-------------------------------------------------------------------------------------
#  This is a CMake configuration file for building the langtool program
#
#  To use it you need CMake which can be downloaded from http://www.cmake.org/
#-------------------------------------------------------------------------------------

#--------------------------------------------------------------------------------------
#  This file is part of LenMus project.
#  Copyright (c) 2002-2019 Cecilio Salmeron
#
#  LenMus is free software; you can redistribute it and/or modify it under the
#  terms of the GNU General Public License as published by the Free Software Foundation,
#  either version 3 of the License, or (at your option) any later version.
#
#  Lomse is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with Lomse; if not, see <http://www.gnu.org/licenses/>.
#  
#  For any comment, suggestion or feature request, please contact the manager of
#  the project at s.cecilio@gmail.com
#
#-------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.10)

# Project name. 
project(langtool)

# check that compiler supports namespace sdt
include(TestForSTDNamespace)
if(CMAKE_NO_STD_NAMESPACE)
  message(FATAL_ERROR "The compiler doesn't support namespace std.")
endif()

# force to use c++11
if (CMAKE_VERSION VERSION_LESS "3.1")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else ()
    set(CMAKE_CXX_STANDARD 11)				#require c+11 or greater
    set(CMAKE_CXX_STANDARD_REQUIRED ON) 	#prevent fallback to any previous standard
endif ()

#check that the compiler supports c++11 and std::regex
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 4.9
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        message(FATAL_ERROR "GCC version must be at least 4.9!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.4
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message(FATAL_ERROR "Clang version must be at least 3.4!")
    endif()
else()
    message(WARNING "** Warning **: You are using an untested compiler! Lomse has only been tested with GCC and Clang.")
endif()

# set directories 
set( LANGTOOL_DIR  ${CMAKE_CURRENT_SOURCE_DIR} )
set( OUTDIR ${LANGTOOL_DIR}/z_bin )
set( EXECUTABLE_OUTPUT_PATH ${OUTDIR} )
set( PACKAGES_DIR  ${LANGTOOL_DIR}/packages )


include_directories(
    ${LANGTOOL_DIR}/src
    ${PACKAGES_DIR}/wxXml2/include
)

#set platform
if(WIN32)
    set( LANGTOOL_PLATFORM_WIN32 "1")
    set( LANGTOOL_PLATFORM_UNIX "0")
    
elseif(UNIX)
    set( LANGTOOL_PLATFORM_WIN32 "0")
    set( LANGTOOL_PLATFORM_UNIX "1")
endif()

#set compiler
if(MSVC)
    set( LANGTOOL_COMPILER_MSVC "1")
    set( LANGTOOL_COMPILER_GCC "0")
    
elseif(CMAKE_COMPILER_IS_GNUCC)
    set( LANGTOOL_COMPILER_MSVC "0")
    set( LANGTOOL_COMPILER_GCC "1")
endif()



#Check wxWidgets
#http://docs.wxwidgets.org/2.8/wx_librarieslist.html
SET( wxWidgets_USE_LIBS  aui base gl net richtext xml xrc core adv html) 
FIND_PACKAGE(wxWidgets REQUIRED)

IF(wxWidgets_FOUND)
    #include_directories( ${wxWidgets_INCLUDE_DIRS} )
    INCLUDE(${wxWidgets_USE_FILE})
    IF(UNIX)
        set(wxWidgets_CXX_FLAGS "`wx-config --cflags -D__WXGTK__  -D_UNICODE`")     
    ENDIF(UNIX)
ENDIF(wxWidgets_FOUND)


#Check for libxml2 and libiconv
FIND_PACKAGE(LibXml2)
message(STATUS "LIBXML2_LIBRARIES => " ${LIBXML2_LIBRARIES})

# "Print all warnings", macros for GCC & __UNIX__
if(UNIX)
    add_definitions( -Wall -DGCC -D__UNIX__ )
endif(UNIX)


message(STATUS "wxWidgets => " ${wxWidgets_LIBRARIES} )


#define a header file to pass CMake settings to source code
configure_file(
    "${LANGTOOL_DIR}/langtool_config.h.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/langtool_config.h"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

link_directories(
    ${LIBRARY_OUTPUT_PATH} 
)
        

# source files to compile
#--------------------------------------------------------------
set( SRC_DIR     ${LANGTOOL_DIR}/src )
set( WXXML2_DIR  ${PACKAGES_DIR}/wxXml2 )

message(STATUS "SRC_DIR => " ${SRC_DIR})
message(STATUS "WXXML2_DIR => " ${WXXML2_DIR})


set(LANGTOOL_FILES
    ${SRC_DIR}/command.cpp
    ${SRC_DIR}/command.h
    ${SRC_DIR}/dlg_compile_book.cpp
    ${SRC_DIR}/dlg_compile_book.h
    ${SRC_DIR}/ebook_processor.cpp
    ${SRC_DIR}/ebook_processor.h
    ${SRC_DIR}/help_processor.cpp
    ${SRC_DIR}/help_processor.h
    ${SRC_DIR}/installer.cpp
    ${SRC_DIR}/installer.h
    ${SRC_DIR}/langtool.cpp
#    ${SRC_DIR}/langtool.rc
    ${SRC_DIR}/main_frame.cpp
    ${SRC_DIR}/main_frame.h
    ${SRC_DIR}/paths.cpp
    ${SRC_DIR}/paths.h
)

set(WXXML2_FILES
    ${WXXML2_DIR}/src/dtd.cpp
    ${WXXML2_DIR}/include/wx/dtd.h
    ${WXXML2_DIR}/src/xml2.cpp
    ${WXXML2_DIR}/include/wx/xml2.h
)


set(ALL_SOURCES 
    ${LANGTOOL_FILES} ${WXXML2_FILES}
)


# Adds folders for Visual Studio and other IDEs
source_group( "langtool"        FILES ${LANGTOOL_FILES} )
source_group( "wxXml2"          FILES ${WXXML2_FILES} )



# langtool program generation
#-------------------------------------------------------------

set(LANGTOOL  langtool)

add_executable( ${LANGTOOL} ${ALL_SOURCES} )

# link to the necessary libraries
target_link_libraries ( ${LANGTOOL}
    ${wxWidgets_LIBRARIES}
    ${LIBXML2_LIBRARIES}
 )

message(STATUS "wxWidgets libraries => " ${wxWidgets_LIBRARIES})
message(STATUS "LibXml2 libraries => " ${LIBXML2_LIBRARIES})


# once built, place executable at langtool/z_bin
set_target_properties(${LANGTOOL}  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${EXECUTABLE_OUTPUT_PATH} )

