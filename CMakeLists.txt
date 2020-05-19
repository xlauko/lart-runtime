cmake_minimum_required( VERSION 3.12 )

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

set( CMAKE_C_COMPILER "clang" )
set( CMAKE_CXX_COMPILER "clang++" )

# if ( NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )
#   message( FATAL_ERROR "LART-Runtime requires clang compiler." )
# endif()

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
option( LART_BUILD_LIBCXX           "Build with libc++ library." ON )
option( LART_LIBCXX_WITH_EXCEPTIONS "Build libc++ with exception support." OFF )

set( LART_LLVM_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm-project )
set( LART_LLVM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm-project-build )

if ( LART_BUILD_LIBCXX )
    include(ExternalProject)

    set( LLVM_ENABLE_PROJECTS "libcxx|libcxxabi" )
    
    ExternalProject_Add( llvm
        GIT_REPOSITORY    https://github.com/llvm/llvm-project.git
        GIT_SHALLOW       1
        CMAKE_ARGS
            -G ${CMAKE_GENERATOR}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DLLVM_USE_SANITIZER=DataFlow
            -DLLVM_ENABLE_LIBCXX=TRUE
            -DLLVM_BUILD_DOCS=OFF
            -DLLVM_INCLUDE_DOCS=OFF
            -DLLVM_ENABLE_DOXYGEN=OFF
            -DLLVM_ENABLE_SPHINX=OFF
            -DLLVM_ENABLE_OCAMLDOC=OFF
            -DLLVM_ENABLE_BINDINGS=OFF
            -DLLVM_ENABLE_ASSERTIONS=ON
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DLIBCXX_ENABLE_EXCEPTIONS=${LART_LIBCXX_WITH_EXCEPTIONS}
            -DLLVM_ENABLE_PROJECTS=${LLVM_ENABLE_PROJECTS}
        SOURCE_DIR        ${LART_LLVM_SOURCE_DIR}
        SOURCE_SUBDIR     llvm
        BINARY_DIR        ${LART_LLVM_BINARY_DIR}
        UPDATE_COMMAND    ""
        LIST_SEPARATOR    |

        BUILD_COMMAND     ${CMAKE_COMMAND} --build . --target cxx cxxabi
        INSTALL_COMMAND   ""  # Disable install step
        STEP_TARGETS      download
    )

    ExternalProject_Add_StepDependencies( llvm configure )
endif()

# LIBRARY
add_library( lart-runtime
  SHARED
    src/choose.cpp
    src/taint.cpp
)

add_dependencies( lart-runtime llvm )
# link against sanitized libc++
add_compile_options( "-fsanitize=dataflow;-nostdinc++;-fno-exceptions" )
add_link_options( "-fsanitize=dataflow;-stdlib=libc++;-Wl,-rpath,${LART_LLVM_BINARY_DIR}/lib" )

target_include_directories( lart-runtime 
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${LART_LLVM_BINARY_DIR}/include/c++/v1
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries( lart-runtime
  PRIVATE
    project_warnings
    project_options
)

target_link_directories( lart-runtime
   PUBLIC
     ${LART_LLVM_BINARY_DIR}/lib
)

# TESTING
if( ENABLE_TESTING )
  enable_testing()
  add_subdirectory( test )
endif()

# INSTALL
include( GNUInstallDirs )