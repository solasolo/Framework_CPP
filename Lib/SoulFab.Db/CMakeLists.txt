

add_library(SoulFab.Db SHARED 
	"Code/BaseDbAccess.cpp"
	"Code/SQLBuilder.cpp"
	"Code/DBScheme.cpp"
 )

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Data/Include"
	"../SoulFab.Config/Include"
)

target_link_libraries (SoulFab.Db
	SoulFab.Base
	SoulFab.Data
	SoulFab.Config

) 