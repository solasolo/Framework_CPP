#include "SimpleTCP.hpp"

#include "TelegramBuilder.hpp"

using namespace SoulFab::Config;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

using namespace std;

namespace SoulFab::Link
{
    SimpleTCPClient::SimpleTCPClient(const std::string ip, unsigned int port, SoulFab::Base::String schema_file)
        : MessageTCPClient<short, SimpleFrameCodec>(ip, port)
    {
        auto builder = new BinaryTelegramCodec(schema_file);

        this->Builder.setCodec(builder);
    }

    SimpleTCPServer::SimpleTCPServer(const std::string ip, unsigned int port, SoulFab::Base::String schema_file)
        : MessageTCPServer<short, SimpleFrameCodec>(ip, port)
    {
        auto builder = new BinaryTelegramCodec(schema_file);

        this->Builder.setCodec(builder);
    }
}
