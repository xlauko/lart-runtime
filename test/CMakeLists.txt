cmake_minimum_required( VERSION 3.13 )

add_library( catch_main STATIC catch_main.cpp )

find_package( Catch2 CONFIG REQUIRED )
target_link_libraries( catch_main PRIVATE Catch2::Catch2 )

target_link_libraries( catch_main PRIVATE project_options libcxx )

add_executable( unit shadow.cpp taint.cpp stash.cpp )
target_link_libraries( unit
  PRIVATE 
    project_warnings
    project_options
    catch_main
    lart-runtime
)

include( CTest )
include( ParseAndAddCatchTests )
ParseAndAddCatchTests( unit )