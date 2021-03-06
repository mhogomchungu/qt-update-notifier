cmake_minimum_required( VERSION 3.0 )

project( tasks )

add_definitions( -Wextra -Wall -pedantic )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF)

INCLUDE(CMakeDependentOption)

find_package( Qt5Core REQUIRED )

QT5_WRAP_CPP( MOC task.hpp example.h )
QT5_WRAP_CPP( MOC_LIBRARY task.hpp )

include_directories( ${Qt5Core_INCLUDE_DIRS} )

add_library( tasks STATIC ${MOC_LIBRARY} )

if( WIN32 )
	set_target_properties( tasks PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic " )
else()
	set_target_properties( tasks PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic " )
endif()
