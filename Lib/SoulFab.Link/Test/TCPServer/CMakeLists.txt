add_executable (TCPServer.test
	"TCPServer.test.cpp"
)

include_directories (
	"../../../SoulFab.Logger/Include"
	"../../../SoulFab.Link/Include"
)

target_link_libraries (TCPServer.test
	SoulFab.Link
	SoulFab.Logger
	SoulFab.Windows
) 