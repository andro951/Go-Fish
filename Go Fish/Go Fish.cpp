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
#include "linkedList.h"
#include "ConstantsAndGlobals.h"
#include "Card.h"
#include "Guess.h"
#include "NPC.h"
#include "Player.h"
#include "PlayerInput.h"

bool testing = true;//If true, you will not be prompted for you name to save time while testing.
bool autoGuess = true;//If true, your turns will be replaced with automatic guesses to save time while testing.

std::vector<Guess> lastGuesses;//The last round of guesses are kept for the local player to see during their turn.
linkedList<Player> players(Player::ToString);
std::stack<Card> deck;
element<Player>* currentPlayer;
std::vector<int> Scores;//Not used until the end of the game when the scores are tallied.

/// <summary>
/// Gets the player number.  Passed to Guess for printing the result of the guess.
/// This helps minimize circular dependencies.
/// </summary>
std::string GetPlayerName(int playerNumber) {
	return players[playerNumber]->value.name;
}

/// <summary>
/// Creates a linked list of cards that are ordered.
/// </summary>
std::unique_ptr<linkedList<Card>> createDeck() {
	std::unique_ptr <linkedList<Card>> deck = std::make_unique<linkedList<Card>>(Card::ToString);
    for (int i = 0; i < SUITS_PER_DECK; ++i) {
        for (int j = 0; j < CARDS_PER_SUIT; ++j) {
            deck->Emplace(j, i);
        }
    }

    return deck;
};

/// <summary>
/// The player draws num card(s) from the deck.
/// </summary
bool playerDraw(Player& player, std::stack<Card>& deck, int num = 1) {
    for (int i = 0; i < num; i++) {
		Card card = deck.top();
		player.hand.Emplace(card);
		deck.pop();
    }

	player.hand.Sort();

	return deck.size() > 0;
}

int GetNumberOfPlayers() {
	std::cout << "Hello!\n";
	std::cout << "Who's ready for an exciting fame of Go Fish?!\n\n";

	std::string playersPrimpt = "How many NPC players would you like to play with? (1 - 5)";
	int numberOfPlayers = !testing ? get_integer_input_in_range(playersPrimpt, MIN_PLAYERS - 1, MAX_PLAYERS - 1) + 1 : 2;
	std::cout << std::endl;

	return numberOfPlayers;
}

std::string GetLocalPlayerName() {
	std::cout << "What is your name?\n";
	std::string player0Name = testing ? "Local Player" : "";
	if (!testing)
		std::cin >> player0Name;

	std::cout << std::endl;

	return player0Name;
}

void PopulatePlayersAndScores(int numberOfPlayers, std::string player0Name) {
	players.Emplace(0, player0Name);
	Scores = { 0 };
	for (int i = 1; i < numberOfPlayers; i++) {
		players.Emplace(players.Count());
		Scores.push_back(0);
	}
}

void PrintPlayers() {
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

	std::cout << " " << (players.Count() > 2 ? "have" : "has") << " joined the game.\n\n";
}

void SelectFirstPlayer() {
	currentPlayer = players[std::rand() % players.Count()];
	std::cout << currentPlayer->value.name << " is up first.\n\n";
}

void CreateAndShuffleDeck() {
	std::unique_ptr<linkedList<Card>> deckList = createDeck();
	deckList->Shuffle();
	deck = deckList->ToStack();
}

void DealOpeningHands() {
	int startingCards = players.Count() > 2 ? 5 : 7;
	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		playerDraw(player->value, deck, startingCards);
	}
}

void PrintHandsAndDeck() {
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

void SetupLastGuesses() {
	//Fill guesses vector with empty guesses for each player.
	for (int i = 0; i < players.Count(); i++) {
		lastGuesses.emplace_back();
	}
}

void Setup() {
	//Seed the random number generator with the current time.
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	int numberOfPlayers = GetNumberOfPlayers();
	std::string player0Name = GetLocalPlayerName();
	PopulatePlayersAndScores(numberOfPlayers, player0Name);
	PrintPlayers();
	SelectFirstPlayer();
	CreateAndShuffleDeck();

	bool printHandsAndDeck = false;
	if (printHandsAndDeck)
		PrintHandsAndDeck();//For testing

	SetupLastGuesses();
}

void PrintLocalPlayersHand() {
	currentPlayer->value.hand.Print("Your hand");
	std::cout << std::endl;
}

void PrintFourOfAKinds() {
	bool atLeastOneFourOfAKind = false;
	//Check if any for of a kinds have been turned in.
	for (int i = 0; i < CARDS_PER_SUIT; i++) {
		if (FourOfAKinds[i] != NO_PLAYER) {
			atLeastOneFourOfAKind = true;
			break;
		}
	}

	//If none turned in, return
	if (!atLeastOneFourOfAKind) {
		std::cout << "No four of a kinds have been turned in.\n\n";
		return;
	}

	
	std::cout << "Four of a kinds:\n";

	//Create a multi-dimensional vector to hold the four of a kinds for each player so that they can be printed per player.

	//Create the empty vectors
	std::vector<std::vector<int>> fourOfAKinds;
	for (int i = 0; i < players.Count(); i++) {
		fourOfAKinds.emplace_back();
	}

	//Fill the vectors
	for (int i = 0; i < CARDS_PER_SUIT; i++) {
		int playerNumber = FourOfAKinds[i];
		if (playerNumber != NO_PLAYER)
			fourOfAKinds[playerNumber].push_back(i);
	}

	//Print the vectors
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
	//Use RandomizerAI to get a guess for another player and card number that hasn't been turned in as a four of a kind.
	RandomizerAI randomizer(currentPlayer->value.playerNumber, players.Count());

	return randomizer.NextGuess();
}

Guess GetPlayerGuess() {
	//Prompt the local player for another player and card number.
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
	//autoGuess is meant for testing so each turn doesn't have to be manually played while testing.
	if (autoGuess)
		return GetNPCGuess();

	//Prompt the local player for what they would like to do.
	std::vector<std::string> playerOptions = { "Guess", "Check My Hand", "View Four of a kinds", "Check last round of guesses", "Quit"};
	void (*playerOptionsFunctions[])() = { PrintLocalPlayersHand, PrintFourOfAKinds, PrintLastRoundOfGuesses, Quit };
	int selectedOption = get_option(playerOptions);

	//Call the function for the selected option.
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

	//lowestCardOfNumber is used because it minimizes the amount of searching through a players hand because the result will always be the first card of the number.
	//Example: If the guessed card number is 3, then the lowest card number will be 8 because 3's are 8, 9, 10, and 11, 
	//	so the insert index will land at the index of the first 3 in the hand if any.  After that, increment to the right/up until no more 3's are found.
	// 
	//Do you have any 3's?
	//Other players hand { 0, 3, 7, 8, 10, 11, 12, 51 }
	//                   { A, A, 2, 3,  3,  3,  4,  K }
	Card lowestCardOfNumber(guess.card.CardNumber(), 0);//lowestCardOfNumber = 12
	element<Card>* card = players[guess.targetPlayerNumber]->value.hand.FindInsertElement(lowestCardOfNumber, false);//card = 8 (3 of Spades)
	int guessedCardNumber = guess.card.CardNumber();//guessedCardNumber = 3
	if (!card->IsEnd() && card->value.CardNumber() == guessedCardNumber) {//not the end of the list and card number (3) == guessedCardNumber (3)
		guess.guessResult = GuessResultID::Success;
		int transfered = 0;
		element<Card>::Inc(card);//Because the cards will be removed, the card is incremented and the previous card is removed.  card = 10 (3 of Clubs)
		while (card->Prev().CardNumber() == guessedCardNumber) {//previous card number 8 (3 of Spades) == guessedCardNumber (3)
			transfered++;
			int cardID = card->Prev().CardID;
			//"Give" the card to the current player.
			players[currentPlayerNumber]->value.hand.Emplace(cardID);//Create the card in asking players hand.
			card->previousElement->Remove();//Remove the card from player being asked's hand.

			//In the linkedList, there is a dummy element (the end) that helps with iterating through the list.
			//If the current card (which is still leading the one being looked at by 1) is the end, the card being looked at is the last in the hand, so stop.
			if (card->IsEnd())
				break;

			element<Card>::Inc(card);
		}

		guess.numberOfCardsRecieved = transfered;
	}
	else {
		//The other player doesn't have any 3's (or whatever the guessed card number was)
		guess.guessResult = GuessResultID::FailGoFish;
		playerDraw(currentPlayer->value, deck);
	}

	//Check if the player has four of a kind.
	element<Card>* playersCard = currentPlayer->value.hand.FindInsertElement(lowestCardOfNumber, false);
	if (playersCard->value.CardNumber() == guessedCardNumber) {
		//Count the number of cards with the guessed card number.
		int count = 0;
		element<Card>::Inc(playersCard);
		//Like before, start at the next element and look at the previous card to see if it matches the number.
		//Not required to do it this way this time because cards aren't being removed, but it was easy to copy/paste something that was already working.
		while (playersCard->Prev().CardNumber() == guessedCardNumber) {
			count++;
			if (playersCard->IsEnd())
				break;

			element<Card>::Inc(playersCard);
		}

		//4 of a kind, update the four of a kind array and remove the cards from the players hand.
		if (count == SUITS_PER_DECK) {
			//Remove all of the cards of the 4 of a kind number from the players hand.
			element<Card>::Dec(playersCard);
			guess.guessResult = guess.guessResult == GuessResultID::Success ? GuessResultID::Success4OfAKind : GuessResultID::GoFish4OfAKind;
			FourOfAKinds[guessedCardNumber] = currentPlayerNumber;
			while (!playersCard->IsFirst() && playersCard->Prev().CardNumber() == guessedCardNumber) {
				//Since the hand is sorted and the cards are all being removed, the playersCard doesn't need to be incremented as removing the previous 
				playersCard->previousElement->Remove();
			}
		}
	}

	//If the player guessed wrong, it is the next players turn.
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

	//Count the number of 4 of a kinds each player has.
	for (const int& playerScoreNumber : FourOfAKinds) {
		if (playerScoreNumber == NO_PLAYER)
			continue;

		Scores[playerScoreNumber]++;
	}

	//Use a vector for winners in case of a tie.
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

	//Print the scores.
	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		std::cout << player->value.name << ": " << Scores[player->value.playerNumber] << std::endl;
	}

	//Print the winner(s).
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