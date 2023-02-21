

add_library(SoulFab.Data SHARED 
	"Code/JSON.cpp"
	"Code/SimpleXML.cpp"
	"Code/UniData.cpp"
	"Code/FieldDefine.cpp"
 )

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Windows/Include"

)

target_link_libraries (SoulFab.Data
	SoulFab.Base
	SoulFab.Windows
	
) 