#include "Net.h"

#include <boost/bind/bind.hpp>
#include <iostream>

namespace
{
    static constexpr size_t BUF_SIZE = 1024;
}

namespace asio = boost::asio;

void NetworkPool::run()
{
    while (true)
        m_context.run();
}

std::unique_ptr<UdpSocket> NetworkPool::getSocket(std::uint16_t localPort)
{
    try
    {
        return std::make_unique<UdpSocket>(m_context, localPort);
    }
    catch (...)
    {
        return nullptr;
    }
}

UdpSocket::UdpSocket(asio::io_context &context,
                     std::uint16_t localPort)
    : m_socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), localPort)),
      m_listener(nullptr)
{
    m_socket.set_option(asio::socket_base::broadcast(true));
}

void UdpSocket::send(std::uint16_t remotePort, const std::string &data)
{
    m_socket.async_send_to(
        boost::asio::buffer(data),
        asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), remotePort),
        boost::bind(&UdpSocket::handleSendTo,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void UdpSocket::listen(UdpSocketListener *listener)
{
    m_listener = listener;
    m_buf.resize(BUF_SIZE);
    m_socket.async_receive(boost::asio::buffer(m_buf),
                           boost::bind(&UdpSocket::handleReceive,
                                       this,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}

void UdpSocket::handleSendTo(const boost::system::error_code &error, size_t bytesSent)
{
    if (error || bytesSent == 0)
        std::cerr << "Couldn't send message ("
                  << error.message()
                  << ")."
                  << std::endl;
}

void UdpSocket::handleReceive(const boost::system::error_code &error, size_t bytesReceived)
{
    if (error || bytesReceived == 0)
        std::cerr << "Couldn't receive message("
                  << error.message()
                  << ")."
                  << std::endl;
    else
        m_listener->onData(m_buf.data(), bytesReceived);

    m_socket.async_receive(boost::asio::buffer(m_buf, BUF_SIZE),
                           boost::bind(&UdpSocket::handleReceive,
                                       this,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}
