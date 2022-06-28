#pragma once

#include <boost/asio.hpp>

#include <memory>
#include <vector>

class UdpSocket;

class UdpSocketListener
{
public:
    virtual void onData(char *data, size_t len) = 0;
};

class NetworkPool
{
private:
    NetworkPool(const NetworkPool &) = delete;
    NetworkPool(NetworkPool &&) = delete;
    NetworkPool &operator=(const NetworkPool &) = delete;
    NetworkPool &operator=(NetworkPool &&) = delete;

public:
    NetworkPool() = default;

    void run();

    std::unique_ptr<UdpSocket> getSocket(std::uint16_t localPort);

private:
    boost::asio::io_context m_context;
};

class UdpSocket
{
private:
    UdpSocket(const UdpSocket &) = delete;
    UdpSocket(UdpSocket &&) = delete;
    UdpSocket &operator=(const UdpSocket &) = delete;
    UdpSocket &operator=(UdpSocket &&) = delete;

public:
    explicit UdpSocket(boost::asio::io_context &context,
                       std::uint16_t localPort);

    void send(std::uint16_t remotePort, const std::string &data);
    void listen(UdpSocketListener *listener);
    void handleSendTo(const boost::system::error_code &error, size_t bytesSent);
    void handleReceive(const boost::system::error_code &error, size_t bytesReceived);

private:
    boost::asio::ip::udp::socket m_socket;

    UdpSocketListener *m_listener;
    std::vector<char> m_buf;

    NetworkPool *m_pool;
};
