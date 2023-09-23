#pragma once

#include<string>
#include "Card.h"
#include "ConstantsAndGlobals.h"

enum GuessResultID {
	None,
	FailGoFish,
	Success,
	Success4OfAKind,
	GoFish4OfAKind
};

class Player;

struct Guess {
	Guess() : targetPlayerNumber(-1), currentPlayerNumber(-1), card(Card(DECK_SIZE)), guessResult(GuessResultID::None), numberOfCardsRecieved(-1) {}
	Guess(int TargetPlayerNumber, int CurrentPlayerNumber, int CardID, int GuessResult = GuessResultID::None, int NumberOfCardsRecieved = 1) :
		targetPlayerNumber(TargetPlayerNumber), currentPlayerNumber(CurrentPlayerNumber), card(CardID, 0),
		guessResult(GuessResult), numberOfCardsRecieved(NumberOfCardsRecieved) {
	}

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