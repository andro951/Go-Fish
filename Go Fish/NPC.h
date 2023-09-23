#pragma once

#include<string>
#include "ConstantsAndGlobals.h"
#include "Guess.h"

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