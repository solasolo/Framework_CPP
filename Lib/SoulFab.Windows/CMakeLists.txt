
add_library(SoulFab.Windows SHARED 
	"Code/SysException.cpp"
	"Code/BaseSocket.cpp"
	"Code/MemoryMap.cpp"
	"Code/OS.cpp"
 )

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	
)

target_link_libraries (SoulFab.Windows 
	SoulFab.Base
		
	Ws2_32.lib
) 

add_definitions(-D _HAS_STD_BYTE=0 )