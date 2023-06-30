#include <iostream>
#include <deque>
#include <random>
#include <algorithm>
#include <list>
#include <chrono>
#include <conio.h>
#include <string>
#include "Server.h"
#include "Client.h"

#pragma region Card_Defines
#define Card_0 0
#define Card_1 1
#define Card_2 2
#define Card_3 3
#define Card_4 4
#define Card_5 5
#define Card_6 6
#define Card_7 7
#define Card_8 8
#define Card_9 9
#define Card_Reverse 10
#define Card_PlusTwo 11
#define Card_Skip 12
#define Card_PlusFour 13
#define Card_ColorChange 14
#define Color_Yellow 0
#define Color_Blue 1
#define Color_Green 2
#define Color_Red 3
#pragma endregion

#pragma region Structs
struct Card {
    int type;
    int color;
};

struct Deck {
    std::deque<Card> cards;
    int cards_left = 0;
};

struct Hand {
    std::string name;
    std::list<Card> cards;
};
#pragma endregion

#pragma region DeckMethods
void ShuffleDeck(Deck& deck) {
    auto rng = std::default_random_engine{};
    rng.seed(unsigned int(std::chrono::system_clock::now().time_since_epoch().count()));
    std::shuffle(std::begin(deck.cards), std::end(deck.cards), rng);
}

Card TopOfDeck(Deck& deck) {
    Card cpy = deck.cards[0];
    Card failure = { -255, -255 };
    if (cpy.type >= 0) {
        deck.cards.pop_front();
        deck.cards_left--;
        return cpy;
    }
    return failure;
}

Deck InitDeck() {
    Deck deck;
    deck.cards_left = 0;
    for (int t = 0; t <= Card_ColorChange; t++) {
        for (int i = 0; i < 4; i++) {
            Card c = { t, i };
            deck.cards.push_front(c);
            deck.cards_left++;
        }
    }
    return deck;
}

void StapleToDeck(Deck& deck, Deck& staple) {
    Deck cpy = staple;
    cpy.cards.pop_front();
    ShuffleDeck(cpy);
    for (int i = 1; i < staple.cards_left; i++) {
        cpy.cards.push_back(TopOfDeck(staple));
    }
}

#pragma endregion

#pragma region PrintMethods
std::string CardToText(Card card) {
    int t = card.type;
    int c = card.color;
    std::string r = "";
    switch (c) {
    case Color_Blue:
        r.append("Blue ");
        break;
    case Color_Green:
        r.append("Green ");
        break;
    case Color_Red:
        r.append("Red ");
        break;
    case Color_Yellow:
        r.append("Yellow ");
        break;
    }

    int s = 8 - int(r.size());
    while (s > 0) {
        s--;
        r.append(" ");
    }

    if (t < 10) {
        r += std::to_string(t);
    }
    else switch (t) {
    case Card_PlusTwo:
        r.append("+2");
        break;
    case Card_Reverse:
        r.append("Rev");
        break;
    case Card_PlusFour:
        r.append("+4");
        break;
    case Card_ColorChange:
        r.append("ColorChange");
        break;
    case Card_Skip:
        r.append("Skip");
        break;
    }

    s = 17 - int(r.size());
    while (s > 0) {
        s--;
        r.append(" ");
    }

    return "|" + r + "|";
}

void PrintHand(Hand hand) {
    std::cout << "#-----------------#\n|" << hand.name << ":\n";
    for (Card c : hand.cards) {
        std::cout << CardToText(c) << "\n";
    }
    std::cout << "#-----------------#\n";
}

void PrintStaple(Deck d) {
    std::cout << "#-----------------#\n|" << "Staple:" << "\n";
    std::cout << CardToText(d.cards.front()) << "\n";
    std::cout << "#-----------------#\n";
}

void PrintStapleFull(Deck d) {
    std::cout << "#-----------------#\n|" << "Staple:" << "\n";
    for (Card c : d.cards) {
        std::cout << CardToText(c) << "\n";
    }
    std::cout << "#-----------------#\n";
}

#pragma endregion

#pragma region CardMethods
bool CardAllowed(Card c, Deck staple) {
    
    if (staple.cards.size() == 0) {
        return true;
    }
    Card stackCard = staple.cards[0];
    if (c.color == stackCard.color) {
        return true;
    }
    if (c.type == stackCard.type && stackCard.type < Card_PlusFour) {
        return true;
    }
    if ((c.type == Card_ColorChange && stackCard.type != Card_ColorChange) || (c.type == Card_PlusFour && stackCard.type != Card_PlusFour)) {
        return true;
    }
    return false;
}

int PutDownCard(Hand& player, Deck& staple, int index) {

    std::vector<Card> hand_vec;
    std::copy(player.cards.begin(), player.cards.end(), std::back_inserter(hand_vec));

if (CardAllowed(hand_vec[index], staple)) {
    staple.cards.push_front(hand_vec[index]);

    player.cards.clear();

    for (int i = 0; i < index; i++) {
        player.cards.push_front(hand_vec[i]);
    }
    for (int i = index + 1; i < hand_vec.size(); i++) {
        player.cards.push_front(hand_vec[i]);
    }
    return 1;
}
std::cout << "Not allowed!\n";
return 0;
}

int DrawCard(Hand& player, Deck& deck) {
    Card c = TopOfDeck(deck);
    if (c.type >= -1) {
        player.cards.push_front(c);
        return 1;
    }
    return 0;
}

#pragma endregion

#pragma region Logic
void PlayTurn(Hand& player, Deck& deck, Deck& staple, int& plusCards) {
    std::string action;
    int param[2];

    int success = 0;

    if (player.name[0] != 'P') {
        std::cout << "PLAYER DEAD!";
    }

    while (success == 0) {
        for (int i = 0; i < 40; i++) { std::cout << "\n"; }
        std::cout << "Cards left in deck: " << deck.cards_left << "\n";
        PrintStaple(staple);
        PrintHand(player);

        action = "";
        param[0] = 0;
        param[1] = 0;

        std::cin >> action >> param[0] >> param[1];

        if (action == "draw") {
            do {
                while ((success = DrawCard(player, deck)) != 1) {
                    StapleToDeck(deck, staple);
                }
                success = 0;
                plusCards--;
            } while (plusCards > 0);
        }
        else if (action == "put") {
            success = PutDownCard(player, staple, param[0]);
            if (success == 1) {
                if (staple.cards.front().type >= Card_PlusFour) {
                    staple.cards.front().color = param[1];
                }
                else if (staple.cards.front().type == Card_PlusTwo) {
                    plusCards++;
                }
                else while (plusCards > 0) {
                    std::cout << "Forcedraw\n";
                    while ((success = DrawCard(player, deck)) != 1) {
                        StapleToDeck(deck, staple);
                    }
                    success = 1;
                    plusCards--;
                }
            }
            else {
                std::cout << "Cannot put card somwhoe!\n";
            }
        }
        else {
            std::cout << "Nonono!";
            success = 1;
        }
    }
}

int CheckForWinners(Hand p) {
    return p.cards.size() == 0 ? 1 : 0;
}
#pragma endregion

void LocalGame() {
    Deck deck = InitDeck();
    Deck staple;

    ShuffleDeck(deck);
    ShuffleDeck(deck);

    Hand BugFixer;
    BugFixer.name = "Secret Service";
    DrawCard(BugFixer, deck);
    PutDownCard(BugFixer, staple, 0);

    const int numPlayers = 4;
    Hand players[numPlayers];
    for (int i = 0; i < numPlayers; i++) {
        players[i].name = "Player " + std::to_string(i);
    }

    for (int i = 0; i < 6; i++) {
        for (Hand& player : players)
            DrawCard(player, deck);
    }

    bool reverse = false;
    int plusCards = 0;
    int isOver = 0;
    int i = 0;
    while (isOver == 0) {
        i %= numPlayers;
        if (i < 0) {
            i += 4;
        }
        PlayTurn(players[i], deck, staple, plusCards);
        if (staple.cards.begin()->type == Card_Reverse) {
            std::cout << "REVERSE!!!!\n";
            reverse = !reverse;
        }
        else if (staple.cards.front().type == Card_Skip) {
            i += reverse ? -1 : 1;
        }
        else if (staple.cards.front().type == Card_PlusTwo) {
            plusCards += 2;
        }
        else if (staple.cards.front().type == Card_PlusFour) {
            plusCards += 4;
        }
        for (Hand p : players) {
            isOver += CheckForWinners(p);
            if (isOver > 0) std::cout << p.name << " won the game!";
        }
        if (isOver > 0) break;
        i += reverse ? -1 : 1;
    }
}

bool runArg(std::string arg) {
    if (arg == "--server" || arg == "-s")
    {
        std::cout << "Starting server!\n";
        RunServer();
    }
    else if (arg == "--client" || arg == "-c")
    {
        std::cout << "Starting client!\n";
        RunClient();
    }
    else if (arg == "--local" || arg == "-l") {
        LocalGame();
    }
    else if (arg == "--help" || arg == "-h" || arg == "-?") {
        std::cout << "USAGE:\n\tcppcg.exe [OPTION]\n\nVALID OPTIONS:\n\t-h or --help\n\t-c or --client\n\t-s or --server\n\t-l or --local\n";
    }
    else {
        std::cout << "UNKNOWN COMMAND\n";
    }
    return false;
}


int main(int argc, const char* argv[])
{
    if (argc == 1) {
        runArg("-h");
    }
    else {
        for (int i = 1; i < argc; ++i) {
            std::string argument(argv[i]);
            if (runArg(argument)) break;
        }
    }
    return 1;
}