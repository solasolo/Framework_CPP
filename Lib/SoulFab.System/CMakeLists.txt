add_library(SoulFab.System SHARED 
	"Code/Application.cpp"
	"Code/Thread.cpp"
	"Code/Synchronization.cpp"
	"Code/ConCurrency.cpp"
	"Code/Performance.cpp"
)

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Windows/Include"

)

target_link_libraries (SoulFab.System 
	SoulFab.Base
	SoulFab.Windows
) 
