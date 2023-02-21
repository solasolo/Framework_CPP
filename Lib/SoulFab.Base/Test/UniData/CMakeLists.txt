# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (UniData.test "UniData.test.cpp")

# TODO: Add tests and install targets if needed.
# include_directories ("../ModelCalculator/Include")

include_directories (
	"../../Lib/SoulFab.Base/Include"
)

target_link_libraries (UniData.test 
	SoulFab.System
) 