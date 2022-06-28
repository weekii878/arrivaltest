#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Core/Net.h"
#include "Core/Utils.hpp"
#include "Core/TasksQueue.h"

#include <iostream>
#include <sstream>
#include <thread>

namespace
{

    constexpr std::uint16_t LOCAL_PORT = 45678;
    constexpr std::uint16_t REMOTE_PORT = 45679;
    constexpr std::uint16_t SENDING_PERIOD_MS = 100;
    static NetworkPool pool;
    static TasksQueue queue;

    std::string getMessage()
    {
        boost::property_tree::ptree root;
        root.put("ts", Utils::msSinceEpoch());
        root.put("event", "BroadcastTimestamp");

        std::stringstream ss;
        write_json(ss, root, false);

        return ss.str();
    }

    void pushTask(UdpSocket *socket)
    {
        queue.push(
            [socket]()
            {
                socket->send(REMOTE_PORT, getMessage());
                pushTask(socket);
            },
            SENDING_PERIOD_MS);
    }

    void tasksQueueTh()
    {
        auto senderSocket = pool.getSocket(LOCAL_PORT);
        if (!senderSocket)
        {
            std::cerr << "Couldn't open the socket." << std::endl;
            std::abort();
        }

        pushTask(senderSocket.get());
        queue.run();
    }

    void networkPoolTh()
    {
        pool.run();
    }

}

int main()
{
    std::cout << "Start server" << std::endl;

    std::thread _tasksQueueTh(tasksQueueTh);
    std::thread _networkPoolTh(networkPoolTh);

    _tasksQueueTh.join();
    _networkPoolTh.join();
}
