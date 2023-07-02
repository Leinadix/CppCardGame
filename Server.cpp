#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "cppcg.h"

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
            const char* t = "lkjsdflkjsdflkjsdflkj";
            // Echo back the received data to the client
            boost::asio::async_write(*socket, boost::asio::buffer(t, strlen(t)),
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
        Server server(io_service, 32441);
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

void test() {
    Deck deck = InitDeck();
    Deck staple;

    ShuffleDeck(deck);
    ShuffleDeck(deck);

    Hand BugFixer;
    BugFixer.name = "Secret Service";
    DrawCard(BugFixer, deck);
    PutDownCard(BugFixer, staple, 0);

    const int numPlayers = 4;
    Hand* players = new Hand[numPlayers];
    for (int i = 0; i < numPlayers; i++) {
        players[i].name = "Player " + std::to_string(i);
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < numPlayers; j++) {
            DrawCard(players[j], deck);
        }
    }

    bool reverse = false;
    int plusCards = 0;
    int isOver = 0;
    int i = 0;
    while (RunTurn(i, numPlayers, players, deck, staple, plusCards, reverse) > 0) {
    }
}