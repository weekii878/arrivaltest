#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Core/Net.h"
#include "Core/Utils.hpp"

#include <iostream>
#include <sstream>
#include <thread>

namespace
{

    static constexpr std::uint16_t LOCAL_PORT = 45679;
    static NetworkPool pool;

    class Dumper : public UdpSocketListener
    {
    public:
        void onData(char *data, size_t len) override
        {
            std::stringstream ss;
            ss << std::string(data, len);

            size_t receivedTs;
            boost::property_tree::ptree root;
            try
            {
                boost::property_tree::read_json(ss, root);
                if (root.get<std::string>("event") != "BroadcastTimestamp")
                    return;
                receivedTs = root.get<size_t>("ts");
            }
            catch (const std::exception &e)
            {
                std::cerr << "Couldn't parse a message."
                          << std::endl
                          << std::endl;
                return;
            }

            size_t currentTs = Utils::msSinceEpoch();
            size_t diff = currentTs - receivedTs;

            std::cout << ++m_seq << ") ReceivedTs: " << receivedTs << std::endl
                      << "CurrentTs: " << currentTs << std::endl
                      << "Diff: " << diff << std::endl
                      << std::endl;
        }

    private:
        size_t m_seq = 0;
    };

    void networkPoolTh()
    {
        Dumper dumper;
        auto receiverSocket = pool.getSocket(LOCAL_PORT);
        receiverSocket->listen(&dumper);
        if (!receiverSocket)
        {
            std::cerr << "Couldn't open the socket." << std::endl;
            std::abort();
        }
        pool.run();
    }

}

int main()
{
    std::cout << "Start client" << std::endl;

    std::thread _networkPoolTh(networkPoolTh);

    _networkPoolTh.join();
}
