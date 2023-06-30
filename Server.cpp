#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_service& io_service, int port)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        startAccept();
    }

private:
    void startAccept() {
        tcp::socket* socket = new tcp::socket(io_service_);
        acceptor_.async_accept(*socket,
            boost::bind(&Server::handleAccept, this, socket,
                boost::asio::placeholders::error));
    }

    void handleAccept(tcp::socket* socket,
        const boost::system::error_code& error) {
        if (!error) {
            startRead(socket);
        }
        else {
            delete socket;
        }

        startAccept();
    }

    void startRead(tcp::socket* socket) {
        boost::asio::async_read(*socket, boost::asio::buffer(buffer_),
            boost::asio::transfer_at_least(1),
            boost::bind(&Server::handleRead, this, socket,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handleRead(tcp::socket* socket,
        const boost::system::error_code& error,
        std::size_t bytes_transferred) {
        if (!error) {
            std::cout << "Received: " << std::string(buffer_.data(), bytes_transferred) << std::endl;

            // Echo back the received data to the client
            boost::asio::async_write(*socket, boost::asio::buffer(buffer_, bytes_transferred),
                boost::bind(&Server::handleWrite, this, socket,
                    boost::asio::placeholders::error));
        }
        else {
            delete socket;
        }
    }

    void handleWrite(tcp::socket* socket,
        const boost::system::error_code& error) {
        if (!error) {
            startRead(socket);
        }
        else {
            delete socket;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    std::array<char, 1024> buffer_;
};

int RunServer() {
    try {
        boost::asio::io_service io_service;
        Server server(io_service, 12345);
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}