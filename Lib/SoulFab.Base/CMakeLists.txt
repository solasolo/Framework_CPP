

add_library(SoulFab.Base SHARED
	"Code/BaseEvent.cpp"
	"Code/Exception.cpp"
	"Code/DateTime.cpp"
	"Code/StringHelper.cpp"
	"Code/SmartPointer.cpp"
	"Code/String.cpp"
    "Code/IPAddress.cpp"
 )
	
include_directories (
	"./Include"
)

add_subdirectory ("Test/UniData")
