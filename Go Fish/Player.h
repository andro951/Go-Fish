#pragma once

#include<string>
#include "ConstantsAndGlobals.h"
#include "linkedList.h"
#include "Card.h"

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