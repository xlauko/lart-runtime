add_library( catch_main STATIC catch_main.cpp )

find_package( Catch2 CONFIG REQUIRED )
target_link_libraries( catch_main PRIVATE Catch2::Catch2 )

target_link_libraries( catch_main PRIVATE project_options )

add_executable( unit shadow.cpp )
target_link_libraries( unit PRIVATE project_warnings project_options catch_main )

include( CTest )
include( ParseAndAddCatchTests )
ParseAndAddCatchTests( unit )
