#pragma once

#include<string>

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