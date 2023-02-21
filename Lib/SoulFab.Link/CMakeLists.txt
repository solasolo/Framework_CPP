
add_library(SoulFab.Link SHARED 
	"Code/StreamBuffer.cpp"
	"Code/BaseCodec.cpp"
	"Code/SimpleProtocol.cpp"
	"Code/Socket.cpp"
	"Code/TCPConnection.cpp"
	"Code/TCPServer.cpp"
	"Code/TCPClient.cpp"
	"Code/TelegramBuilder.cpp"
	"Code/TeleGramScheme.cpp"
	"Code/SimpleTCP.cpp"
    "Code/UDPServer.cpp"
    "Code/UDPClient.cpp"
    "Code/BaseChannel.cpp"
)

include_directories (
	"./Include"
	"../SoulFab.Base/Include"
	"../SoulFab.Data/Include"
	"../SoulFab.System/Include"
	"../SoulFab.Windows/Include"
	"../SoulFab.Config/Include"

)

target_link_libraries (SoulFab.Link 
	SoulFab.Base
	SoulFab.Data
	SoulFab.System
	SoulFab.Windows

) 


add_subdirectory ("Test/TCPServer")
add_subdirectory ("Test/TCPClient")
