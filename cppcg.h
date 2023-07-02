#include <deque>
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

std::string CardToText		(	Card        																);
void		PrintHand		(	Hand        																);
bool		CardAllowed		(	Card        , Deck         													);
int			PutDownCard		(	Hand &      , Deck &		, int      										);
int			DrawCard		(	Hand &      , Deck &       					 								);
void		ShuffleDeck		(	Deck &      																);
Card		TopOfDeck		(	Deck &      																);
Deck		InitDeck		(																				);
void		StapleToDeck	(	Deck &      , Deck &       					 								);
void		PrintStapleFull	(	Deck        																);
void		PrintStaple		(	Deck        																);
void		PlayTurn		(	Hand &      , Deck &		, Deck &   , int &  							);
int			CheckForWinners	(	Hand        																);
int			RunTurn			(	int &       , int			, Hand * & , Deck & , Deck & , int & , bool &	);
void		LocalGame		(																				);
bool		runArg			(	std::string 																);
int			main			(	int         , const char **													);
