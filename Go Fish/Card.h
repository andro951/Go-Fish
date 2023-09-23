#pragma once

#include<string>
#include "ConstantsAndGlobals.h"

struct Card {
public:
	Card() : CardID(DECK_SIZE) {}
	Card(int cardID) : CardID(cardID) {}
	Card(int cardNumber, int suit) : CardID(cardNumber* SUITS_PER_DECK + suit) {}

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