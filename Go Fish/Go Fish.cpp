#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <random>
#include <functional>
#include <memory>
#include <chrono>
#include <sstream>
#include <map>
#include <stack>
#include "Utility.h"

bool testing = true;//If true, you will not be prompted for you name to save time while testing.
bool autoGuess = true;//If true, your turns will be replaced with automatic guesses to save time while testing.

const int MIN_PLAYERS = 2;
const int MAX_PLAYERS = 6;
const int LOCAL_PLAYER_NUMBER = 0;
const int NO_PLAYER = -1;

const int DECK_SIZE = 52;
const int CARDS_PER_SUIT = 13;
const int SUITS_PER_DECK = DECK_SIZE / CARDS_PER_SUIT;
const std::string cardDisaplayNames[] = { "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King" };
const std::string suitDisplayNames[] = { "Spades", "Hearts", "Clubs", "Diamonds" };
const std::string DEFAULT_NUM_NAME = "Default Num";
const std::string DEFAULT_SUIT_NAME = "Default Suit";

static int FourOfAKinds[] = { NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER };

struct Card {
public:
    Card() : CardID(DECK_SIZE) {}
    Card(int cardID) : CardID(cardID) {}
    Card(int cardNumber, int suit) : CardID(cardNumber * SUITS_PER_DECK + suit) {}

    /// <summary>
    /// CardID is a number from 0 to 51 representing a playing card.
    /// </summary>
    int CardID;

	/// <summary>
	/// Gets the 0 through 12 card number which is the index for this cards name in cardDisaplayNames.
	/// </summary>
    int CardNumber() const {
        return CardID / SUITS_PER_DECK;
    }

	/// <summary>
	/// Gets the 0 through 3 card suit which is the index for this cards name in suitDisplayNames.
	/// </summary>
    int Suit() const {
        return CardID % SUITS_PER_DECK;
    }

	std::string NumberName() const {
		return NumberName(CardNumber());
	}

	static std::string NumberName(int cardNumber) {
		return cardNumber >= 0 && cardNumber < CARDS_PER_SUIT ? cardDisaplayNames[cardNumber] : DEFAULT_NUM_NAME;
	}

	std::string SuitName() const {
		return SuitName(Suit());
	}

	static std::string SuitName(int suit) {
		return suit >= 0 && suit < SUITS_PER_DECK ? suitDisplayNames[suit] : DEFAULT_SUIT_NAME;
	}

    std::string FullName() const {
        return NumberName() + " of " + SuitName();
    }

	bool operator==(const Card& other) const {
		return CardID == other.CardID;
	}

	bool operator!=(const Card& other) const {
		return !(*this == other);
	}

	bool operator<(const Card& other) const {
		return CardID < other.CardID;
	}

	bool operator>(const Card& other) const {
		return CardID > other.CardID;
	}

	bool operator<=(const Card& other) const {
		return CardID <= other.CardID;
	}

	bool operator>=(const Card& other) const {
		return CardID >= other.CardID;
	}

	Card(const Card& other) : CardID(other.CardID) {}

	static std::string ToString(const Card& card) {
		return card.FullName();
	}
};

enum GuessResultID {
	None,
	FailGoFish,
	Success,
	Success4OfAKind,
	GoFish4OfAKind
};

class Player;

struct Guess {
	Guess () : targetPlayerNumber(-1), currentPlayerNumber(-1), card(Card(DECK_SIZE)), guessResult(GuessResultID::None), numberOfCardsRecieved(-1) {}
	Guess(int TargetPlayerNumber, int CurrentPlayerNumber, int CardID, int GuessResult = GuessResultID::None, int NumberOfCardsRecieved = 1) : 
		targetPlayerNumber(TargetPlayerNumber), currentPlayerNumber(CurrentPlayerNumber), card(CardID, 0),
		guessResult(GuessResult), numberOfCardsRecieved(NumberOfCardsRecieved) {}

	int targetPlayerNumber;
	int currentPlayerNumber;
	Card card;
	int guessResult;
	int numberOfCardsRecieved;

	std::string GuessToString(std::string(*GetPlayerNameFunc)(int)) {
		return GetPlayerNameFunc(currentPlayerNumber) + ": " + GetPlayerNameFunc(targetPlayerNumber) + ", do you have any " + card.NumberName() + "'s?";
	}

	void PrintGuess(std::string(*GetPlayerNameFunc)(int)) {
		std::cout << GuessToString(GetPlayerNameFunc) << std::endl;
	}

	std::string ResultToString(std::string(*GetPlayerNameFunc)(int)) {
		std::string result = GetPlayerNameFunc(targetPlayerNumber) + ": ";
		switch (guessResult) {
			case GuessResultID::None:
				result += "None";
				break;
			case GuessResultID::FailGoFish:
			case GuessResultID::GoFish4OfAKind:
				result += "No, Go Fish!";
				break;
			case GuessResultID::Success:
			case GuessResultID::Success4OfAKind:
				result += "Yes, I have " + std::to_string(numberOfCardsRecieved) + " " + card.NumberName() + (numberOfCardsRecieved > 1 ? "s" : "") + ".";
				break;
			default:
				result += "Unknown";
				break;
		}

		result += "\n";

		switch (guessResult) {
			case GuessResultID::None:
				result += "None";
				break;
			case GuessResultID::FailGoFish:
			case GuessResultID::GoFish4OfAKind:
				result += GetPlayerNameFunc(currentPlayerNumber) + ": {Draws a card from the pile}";
				if (guessResult == GuessResultID::GoFish4OfAKind)
					result += "\nLuck of the draw!  Four of a kind! (" + card.NumberName() + ")";

				break;
			case GuessResultID::Success:
				result += GetPlayerNameFunc(currentPlayerNumber) + ": Thank you!";
				break;
			case GuessResultID::Success4OfAKind:
				result += GetPlayerNameFunc(currentPlayerNumber) + "Nice, four of a kind! (" + card.NumberName() + ")";
				break;
			default:
				result += "Unknown";
				break;
		}

		return result;
	}

	void PrintResult(std::string(*GetPlayerNameFunc)(int)) {
		std::cout << ResultToString(GetPlayerNameFunc) << std::endl << std::endl;
	}
};

std::vector<Guess> lastGuesses;

class NPC {
	virtual Guess NextGuess() = 0;
};

class RandomizerAI : NPC {
public:
	RandomizerAI(int PlayerNumber, int PlayerCount) : playerNumber(PlayerNumber), playerCount(PlayerCount) {}
	int playerNumber;
	int playerCount;
	Guess NextGuess() override {
		int randomPlayerNumber = rand() % (playerCount - 1);
		if (randomPlayerNumber >= playerNumber)
			randomPlayerNumber++;

		int randomCardNumber;
		do {
			randomCardNumber = rand() % CARDS_PER_SUIT;
		} while (FourOfAKinds[randomCardNumber] != -1);

		return Guess(randomPlayerNumber, playerNumber, randomCardNumber);
	}
};

class Player {
	Player(const Player& other) = delete;//Delete copy constructor to prevent copying Player objects.
public:
	int playerNumber;
	Player() : playerNumber(-1), name("Default"), hand(Card::ToString, true) {}
	Player(int PlayerNumber) : playerNumber(PlayerNumber), name("Player " + std::to_string(playerNumber)), hand(Card::ToString, true) {}
	Player(int PlayerNumber, std::string FullName) : playerNumber(PlayerNumber), name(FullName), hand(Card::ToString, true) {}
	std::string name;
	linkedList<Card> hand;

	/// <summary>
	/// Move constructor used to move a player from one container to another when using std containers.
	/// </summary>
	Player(Player&& other) noexcept : playerNumber(std::move(other.playerNumber)), name(std::move(other.name)), hand(std::move(other.hand)) {}

	bool operator==(const Player& other) const {
		return playerNumber == other.playerNumber;
	}

	bool operator!=(const Player& other) const {
		return !(*this == other);
	}

	bool operator<(const Player& other) const {
		return playerNumber < other.playerNumber;
	}

	bool operator>(const Player& other) const {
		return playerNumber > other.playerNumber;
	}

	bool operator<=(const Player& other) const {
		return playerNumber <= other.playerNumber;
	}

	bool operator>=(const Player& other) const {
		return playerNumber >= other.playerNumber;
	}

	static std::string ToString(const Player& player) {
		return player.name;
	}
};

linkedList<Player> players(Player::ToString);
std::stack<Card> deck;
element<Player>* currentPlayer;
std::vector<int> Scores;

std::unique_ptr<linkedList<Card>> createDeck() {
	std::unique_ptr <linkedList<Card>> deck = std::make_unique<linkedList<Card>>(Card::ToString);
    for (int i = 0; i < SUITS_PER_DECK; ++i) {
        for (int j = 0; j < CARDS_PER_SUIT; ++j) {
            deck->Emplace(j, i);
        }
    }

    return deck;
};

bool playerDraw(Player& player, std::stack<Card>& deck, int num = 1) {
    for (int i = 0; i < num; i++) {
		Card card = deck.top();
		player.hand.Emplace(card);
		deck.pop();
    }

	player.hand.Sort();

	return deck.size() > 0;
}

#pragma region Player Input

//From previous projects

std::string join(const std::vector<std::string>& list_strings, const std::string& separator = "\n") {
	std::string result = "";
	bool first = true;
	for (const auto& s : list_strings) {
		if (first) {
			first = false;
		}
		else {
			result += separator;
		}

		result += s;
	}

	return result;
}

bool is_integer(const std::string& str) {
	if (str.empty())
		return false;

	bool found_minus = (str[0] == '-');
	for (int i = (found_minus ? 1 : 0); i < str.length(); i++) {
		if (!isdigit(str[i]))
			return false;
	}

	return true;
}

std::string get_integer_inputs(const std::string& prompt, std::vector<int>& integers, bool display = true) {
	if (display)
		std::cout << prompt << std::endl;

	std::string answer;
	std::getline(std::cin, answer);
	std::vector<std::string> answer_list;
	std::stringstream ss(answer);
	std::string temp;
	while (std::getline(ss, temp, ' ')) {
		answer_list.push_back(temp);
	}

	for (const auto& str : answer_list) {
		std::string stripped_str = str;
		stripped_str.erase(remove(stripped_str.begin(), stripped_str.end(), ','), stripped_str.end());
		if (is_integer(stripped_str))
			integers.push_back(stoi(stripped_str));
	}

	return answer;
}

int get_integer_input(const std::string& prompt, bool display = true) {
	std::vector<int> integers;
	std::string answer = get_integer_inputs(prompt, integers, display);

	if (integers.size() == 1) {
		return integers[0];
	}
	else if (integers.size() > 1) {
		std::cout << "Received multiple numbers." << std::endl;
	}
	else {
		if (answer != "") {
			std::cout << answer << " is not a whole number." << std::endl;
		}
		else {
			return get_integer_input(prompt, false);
		}
	}

	return get_integer_input(prompt);
}

int get_integer_input_in_range(const std::string& prompt, int range_min, int range_max) {
	int value = -1;
	bool getting_input = true;
	while (getting_input) {
		value = get_integer_input(prompt);
		if (value < range_min || value > range_max) {
			std::cout << value << " is out of range." << std::endl;
		}
		else {
			getting_input = false;
		}
	}

	return value;
}

int get_option(const std::vector<std::string>& option_list) {
	// List of all options with number labels
	std::vector<std::string> options;
	for (int i = 0; i < option_list.size(); i++) {
		options.push_back(std::to_string(i + 1) + ". " + option_list[i]);
	}

	std::string prompt = join(options) + "\n";
	return get_integer_input_in_range(prompt, 1, option_list.size()) - 1;
}

template<size_t S>
int get_option(const std::string(&option_list)[S]) {
	// List of all options with number labels
	std::vector<std::string> options;
	for (int i = 0; i < S; i++) {
		options.push_back(std::to_string(i + 1) + ". " + option_list[i]);
	}

	std::string prompt = join(options) + "\n";
	return get_integer_input_in_range(prompt, 1, S) - 1;
}

#pragma endregion

/// <summary>
/// Gets the player number.  Passed to Guess for printing the result of the guess.
/// This helps minimize circular dependencies.
/// </summary>
std::string GetPlayerName(int playerNumber) {
	return players[playerNumber]->value.name;
}

void Setup() {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	std::cout << "Hello!\n";
	std::cout << "Who's ready for an exciting fame of Go Fish?!\n\n";

	std::string playersPrimpt = "How many NPC players would you like to play with? (1 - 5)";
	int numberOfPlayers = !testing ? get_integer_input_in_range(playersPrimpt, MIN_PLAYERS - 1, MAX_PLAYERS - 1) + 1 : 2;
	std::cout << std::endl;
	std::cout << "What is your name?\n";
	std::string player0Name = testing ? "Local Player" : "";
	if (!testing)
		std::cin >> player0Name;

	std::cout << std::endl;

	players.Emplace(0, player0Name);
	Scores = { 0 };
	for (int i = 1; i < numberOfPlayers; i++) {
		players.Emplace(players.Count());
		Scores.push_back(0);
	}
	
	bool first = true;
	for (element<Player>* player = players.First()->nextElement; !player->IsEnd(); element<Player>::Inc(player)) {
		if (first) {
			first = false;
		}
		else {
			if (player->IsLast()) {
				std::cout << " and ";
			}
			else {
				std::cout << ", ";
			}
		}

		std::cout << player->value.name;
	}

	std::cout << " " << (numberOfPlayers > 2 ? "have" : "has") << " joined the game.\n\n";

	currentPlayer = players[std::rand() % players.Count()];
	std::cout << currentPlayer->value.name << " is up first.\n\n";

	std::unique_ptr<linkedList<Card>> deckList = createDeck();
	deckList->Shuffle();
	deck = deckList->ToStack();

	int startingCards = numberOfPlayers > 2 ? 5 : 7;
	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		playerDraw(player->value, deck, startingCards);
	}

	bool printHandsAndDeck = false;
	if (printHandsAndDeck) {
		for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
			player->value.hand.Print(player->value.name + " hand (" + std::to_string(player->value.hand.Count()) + ")");
		}

		std::cout << std::endl;

		std::stack<Card> deckCopy = deck;
		std::cout << "Deck (" << deckCopy.size() << "): ";
		while (!deckCopy.empty()) {
			std::cout << Card::ToString(deckCopy.top()) << " ";
			deckCopy.pop();
		}
	}

	//Fill guesses vector with empty guesses for each player.
	for (int i = 0; i < players.Count(); i++) {
		lastGuesses.emplace_back();
	}
}

void PrintLocalPlayersHand() {
	currentPlayer->value.hand.Print("Your hand");
	std::cout << std::endl;
}

void PrintFourOfAKinds() {
	bool atLeastOneFourOfAKind = false;
	for (int i = 0; i < CARDS_PER_SUIT; i++) {
		if (FourOfAKinds[i] != NO_PLAYER) {
			atLeastOneFourOfAKind = true;
			break;
		}
	}

	if (!atLeastOneFourOfAKind) {
		std::cout << "No four of a kinds have been turned in.\n\n";
		return;
	}

	std::cout << "Four of a kinds:\n";
	std::vector<std::vector<int>> fourOfAKinds;
	for (int i = 0; i < players.Count(); i++) {
		fourOfAKinds.emplace_back();
	}

	for (int i = 0; i < CARDS_PER_SUIT; i++) {
		int playerNumber = FourOfAKinds[i];
		if (playerNumber != NO_PLAYER) {
			fourOfAKinds[playerNumber].push_back(i);
		}
	}

	for (int i = 0; i < fourOfAKinds.size(); i++) {
		const std::vector<int>& playerFourOfAKind = fourOfAKinds[i];
		int size = playerFourOfAKind.size();
		if (size < 1)
			continue;

		std::cout << players[i]->value.name << ": ";
		bool first = true;
		for (int j = 0; j < size; j++) {
			if (first) {
				first = false;
			}
			else {
				if (j < size - 1) {
					std::cout << ", ";
				}
				else {
					std::cout << " and ";
				}
			}

			std::cout << Card::NumberName(playerFourOfAKind[j]);
		}
	}

	std::cout << "\n\n";
}

void PrintLastRoundOfGuesses() {
	std::cout << "Last round of guesses:\n";
	for (int i = 0; i < lastGuesses.size(); i++) {
		const Guess& guess = lastGuesses[i];
		if (guess.targetPlayerNumber == NO_PLAYER)
			continue;

		std::cout << GetPlayerName(guess.currentPlayerNumber) << " asked " << GetPlayerName(guess.targetPlayerNumber) << " for " << guess.card.NumberName() << "'s who had " << guess.numberOfCardsRecieved << ".\n";
	}

	std::cout << std::endl;
}

Guess GetNPCGuess() {
	RandomizerAI randomizer(currentPlayer->value.playerNumber, players.Count());//Move this to be part of the player class.

	return randomizer.NextGuess();
}

Guess GetPlayerGuess() {
	int targetPlayerNumber = 1;
	if (players.Count() > 2) {
		std::string prompt = "What player would you like to guess? (2 - " + std::to_string(players.Count()) + ")";
		int targetPlayerNumber = get_integer_input_in_range(prompt, 2, players.Count()) - 1;
		std::cout << std::endl;
	}

	std::string cardPrompt = "What card would you like to guess?";
	std::cout << cardPrompt << std::endl;
	int cardNumber = get_option(cardDisaplayNames);
	std::cout << std::endl;

	return Guess(targetPlayerNumber, currentPlayer->value.playerNumber, cardNumber);
}

void Quit() {
	std::cout << "Thanks for playing!\n";
	std::exit(0);
}

Guess PlayerOptions() {
	if (autoGuess)
		return GetNPCGuess();

	std::vector<std::string> playerOptions = { "Guess", "Check My Hand", "View Four of a kinds", "Check last round of guesses", "Quit"};
	void (*playerOptionsFunctions[])() = { PrintLocalPlayersHand, PrintFourOfAKinds, PrintLastRoundOfGuesses, Quit };
	int selectedOption = get_option(playerOptions);

	if (selectedOption == 0) {
		return GetPlayerGuess();
	}
	else {
		playerOptionsFunctions[selectedOption - 1]();

		return PlayerOptions();
	}
}

void UpdateGuessResult(Guess& guess) {
	//Check if the guess is correct.
	int currentPlayerNumber = guess.currentPlayerNumber;
	Card lowestCardOfNumber(guess.card.CardNumber(), 0);
	element<Card>* card = players[guess.targetPlayerNumber]->value.hand.FindInsertElement(lowestCardOfNumber, false);
	int guessedCardNumber = guess.card.CardNumber();
	if (!card->IsEnd() && card->value.CardNumber() == guessedCardNumber) {
		guess.guessResult = GuessResultID::Success;
		int transfered = 0;
		element<Card>::Inc(card);
		while (card->Prev().CardNumber() == guessedCardNumber) {
			transfered++;
			int cardID = card->Prev().CardID;
			players[currentPlayerNumber]->value.hand.Emplace(cardID);
			card->previousElement->Remove();
			if (card->IsEnd())
				break;

			element<Card>::Inc(card);
		}

		guess.numberOfCardsRecieved = transfered;
	}
	else {
		guess.guessResult = GuessResultID::FailGoFish;
		playerDraw(currentPlayer->value, deck);
	}

	//Check if the player has four of a kind.
	element<Card>* playersCard = currentPlayer->value.hand.FindInsertElement(lowestCardOfNumber, false);
	if (playersCard->value.CardNumber() == guessedCardNumber) {
		//Count the number of cards with the guessed card number.
		int count = 0;
		element<Card>::Inc(playersCard);
		while (playersCard->Prev().CardNumber() == guessedCardNumber) {
			count++;
			if (playersCard->IsEnd())
				break;

			element<Card>::Inc(playersCard);
		}

		//4 of a kind, update the four of a kind array and remove the cards from the players hand.
		if (count == SUITS_PER_DECK) {
			element<Card>::Dec(playersCard);
			guess.guessResult = guess.guessResult == GuessResultID::Success ? GuessResultID::Success4OfAKind : GuessResultID::GoFish4OfAKind;
			FourOfAKinds[guessedCardNumber] = currentPlayerNumber;
			while (!playersCard->IsFirst() && playersCard->Prev().CardNumber() == guessedCardNumber) {
				playersCard->previousElement->Remove();
			}
		}
	}

	if (guess.guessResult == GuessResultID::FailGoFish) {
		element<Player>::Inc(currentPlayer);
		if (currentPlayer->IsEnd())
			currentPlayer = players.First();
	}
}

void CurrentPlayerTurn() {
	int currentPlayerNumber = currentPlayer->value.playerNumber;
	Guess guess = currentPlayer->value.playerNumber == LOCAL_PLAYER_NUMBER ? PlayerOptions() : GetNPCGuess();

	guess.PrintGuess(GetPlayerName);

	UpdateGuessResult(guess);

	guess.PrintResult(GetPlayerName);

	lastGuesses[currentPlayerNumber] = guess;

	if (deck.size() > 0)
		CurrentPlayerTurn();
}

void EndGame() {
	std::cout << "Game Over!\n";
	std::cout << "Final Scores:\n";

	for (const int& playerScoreNumber : FourOfAKinds) {
		if (playerScoreNumber == NO_PLAYER)
			continue;

		Scores[playerScoreNumber]++;
	}

	int highestScore = Scores[0];
	std::vector<int> winners = { 0 };
	for (int i = 1; i < Scores.size(); i++) {
		if (Scores[i] > highestScore) {
			highestScore = Scores[i];
			winners.clear();
			winners.push_back(i);
		}
		else if (Scores[i] == highestScore) {
			winners.push_back(i);
		}
	}

	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		std::cout << player->value.name << ": " << Scores[player->value.playerNumber] << std::endl;
	}

	std::cout << std::endl;
	if (winners.size() == 1) {
		std::cout << "Congratulations " << players[winners[0]]->value.name << ", you are the winner!\n";
	}
	else {
		std::cout << "We have a draw! The winners are: ";
		bool first = true;
		int winnersEndIndex = winners.size() - 1;
		for (int i = 0; i <= winnersEndIndex; i++) {
			if (first) {
				first = false;
			}
			else {
				if (i == winnersEndIndex) {
					std::cout << " and ";
				}
				else {
					std::cout << ", ";
				}
			}

			std::cout << players[winners[i]]->value.name;
		}
	}
	
	std::cout << "Thanks for playing!\n";
}

void GoFish() {
	Setup();

	CurrentPlayerTurn();

	EndGame();
}

int main() {
	GoFish();

	return 0;
};