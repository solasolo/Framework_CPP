

add_library(SoulFab.Config SHARED 
	"Code/BaseConfig.cpp"
	"Code/XMLConfig.cpp"
	"Code/JSONConfig.cpp"
)

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Data/Include"
	"../SoulFab.Windows/Include"

)

target_link_libraries (SoulFab.Config 
	SoulFab.Base
	SoulFab.Data
	SoulFab.Windows
	
) 