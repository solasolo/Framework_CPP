
add_library(SoulFab.Linux SHARED 
	"Code/MemoryMap.cpp"
	"Code/BaseSocket.cpp"

)

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	
)

target_link_libraries (SoulFab.Linux 
	SoulFab.Base
		
) 