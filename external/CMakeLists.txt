cmake_minimum_required( VERSION 3.12 )

option( LART_BUILD_LIBCXX           "Build with libc++ library." ON )
option( LART_LIBCXX_WITH_EXCEPTIONS "Build libc++ with exception support." OFF )

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

add_library( libcxx INTERFACE )

target_compile_options( libcxx
    INTERFACE
        "-fsanitize=dataflow;-nostdinc++;-stdlib=libc++"
)

target_include_directories( libcxx INTERFACE ${TEST_LLVM_BINARY_DIR}/include/c++/v1 )
target_link_directories( libcxx INTERFACE ${TEST_LLVM_BINARY_DIR}/lib )
target_link_options( libcxx INTERFACE "-fsanitize=dataflow;-stdlib=libc++;-Wl,-rpath,${TEST_LLVM_BINARY_DIR}/lib" )