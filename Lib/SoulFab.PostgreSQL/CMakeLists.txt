add_library(SoulFab.PostgreSQL SHARED 
	"Code/PgDbAccess.cpp"
	"Code/PgWrap.cpp"
	"Code/PgScheme.cpp"
 )

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Config/Include"
	"../SoulFab.Data/Include"
	"../SoulFab.Db/Include"
	$ENV{PG_HOME}/include
)

target_link_libraries (SoulFab.PostgreSQL 
	SoulFab.Base
	SoulFab.Data	
	SoulFab.Db
	SoulFab.Config
	$ENV{PG_HOME}/lib/libpq.lib
) 