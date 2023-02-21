add_library(SoulFab.Logger SHARED 
	"Code/ThreadLogger.cpp"
	"Code/BaseLogger.cpp"
	"Code/FileLogger.cpp"	
	"Code/ConsoleLogger.cpp"

)

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.System/Include"
	"../SoulFab.Windows/Include"
)

target_link_libraries (SoulFab.Logger 
	SoulFab.Base
	SoulFab.System
) 