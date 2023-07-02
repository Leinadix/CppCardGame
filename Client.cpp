#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

void ReadWriteLoop(tcp::socket& socket) {
    while (true) {
        std::string message;
        std::getline(std::cin, message);

        boost::asio::write(socket, boost::asio::buffer(message));

        std::array<char, 1024> buffer;
        boost::system::error_code error;
        size_t bytes_received = socket.read_some(boost::asio::buffer(buffer), error);
        if (error == boost::asio::error::eof) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        }
        else if (error) {
            throw boost::system::system_error(error);
            break;
        }
        else {
            std::cout << "Received: " << std::string(buffer.data(), bytes_received) << std::endl;
        }
    }
}

int RunClient() {
    try {

        std::string input;

        std::cout << "IP: \n";
        std::getline(std::cin, input);


        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(input, "32441");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        ReadWriteLoop(socket);

        socket.close();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}