cmake_minimum_required( VERSION 3.17 )

project( liblart-runtime LANGUAGES CXX )
include( cmake/project_settings.cmake )

# Link lart runtime to set the c++ standard / compile-time options requested
add_library( project_options INTERFACE )
target_compile_features( project_options INTERFACE cxx_std_20 )

# Link lart runtime to use the warnings specified in compiler_warnings.cmake
add_library( project_warnings INTERFACE )

# standard compiler warnings
include( cmake/compiler_warnings.cmake )
set_project_warnings( project_warnings )

# sanitizer options if supported by compiler
include( cmake/sanitizers.cmake )
enable_sanitizers( project_options )

if ( NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )
  message( FATAL_ERROR "LART-Runtime requires clang compiler." )
endif()

# clang time profiling
# if ( CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )
  option( ENABLE_BUILD_WITH_TIME_TRACE
    "Enable -ftime-trace to generate time tracing .json files on clang" OFF
  )
  if ( ENABLE_BUILD_WITH_TIME_TRACE )
    add_compile_definitions( project_options INTERFACE -ftime-trace )
  endif()
# endif()

# allow for static analysis options
include( cmake/static_analyzers.cmake )

option( BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF )
option( ENABLE_TESTING "Enable Test Builds" ON )

option( ENABLE_PCH "Enable Precompiled Headers" OFF )
if ( ENABLE_PCH )
  # This sets a global PCH parameter, each project will build its own PCH, which
  # is a good idea if any #define's change
  # 
  # consider breaking this out per project as necessary 
  target_precompile_headers( project_options INTERFACE <utility> )
endif()

# setup external libraries
set( LART_LLVM_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project )
set( LART_LLVM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project-build )
add_subdirectory( external )

# link against sanitized libc++
add_compile_options( "-fsanitize=dataflow;-nostdinc++;-fno-exceptions" )
add_link_options( "-fsanitize=dataflow;-stdlib=libc++;-Wl,-rpath,${LART_LLVM_BINARY_DIR}/lib" )

# LIBRARY
add_library( lart-runtime
  SHARED
    src/choose.cpp
    src/shadow.cpp
    src/stash.cpp
    src/taint.cpp
)

target_include_directories( lart-runtime 
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${LART_LLVM_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${LART_LLVM_BINARY_DIR}/include/c++/v1>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries( lart-runtime
  INTERFACE
    libcxx
  PRIVATE
    project_warnings
    project_options
)

add_dependencies( lart-runtime libcxx )

# TESTING
if( ENABLE_TESTING )
  enable_testing()
  add_subdirectory( test )
endif()

# INSTALL
include( GNUInstallDirs )