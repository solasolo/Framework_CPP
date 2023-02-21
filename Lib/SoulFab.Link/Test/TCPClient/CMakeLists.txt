add_executable (TCPClient.test
	"TCPClient.test.cpp"
)

include_directories (
	"../../../SoulFab.Logger/Include"
	"../../../SoulFab.Link/Include"
)

target_link_libraries (TCPClient.test
	SoulFab.Link
	SoulFab.Logger
	SoulFab.Windows
) 