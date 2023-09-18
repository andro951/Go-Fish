#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <random>

template<typename T>
class linkedList;

template<typename T>
struct element {
public:
	element(T Value, linkedList<T>* List, element* PreviousElement = nullptr, element* NextElement = nullptr) : value(Value), list(List), previousElement(PreviousElement), nextElement(NextElement) {}

	T value;
	element<T>* nextElement;
	element<T>* previousElement;
	linkedList<T>* list;

	void InsertValueBeforeMe(const T& value) {
		element<T>* newElement = new element<T>(value, list);
		newElement->LetMeInBefore(this);
	}

	void InsertValueAfterMe(const T& value) {
		element<T>* newElement = new element<T>(value, list);
		newElement->LetMeInAfter(this);
	}

private:
	void BridgeAcrossMe() {
		if (previousElement == nullptr) {
			//I'm the first element
			list->firstElement = nextElement;
			nextElement->previousElement = nullptr;
		}
		else {
			previousElement->nextElement = nextElement;
			nextElement->previousElement = previousElement;
		}
	}

	void LetMeInBefore(element<T>* other) {
		if (other->previousElement == nullptr) {
			//Other is the first element
			previousElement = nullptr;
			nextElement = other;
			list->firstElement = this;
			other->previousElement = this;
		}
		else {
			previousElement = other->previousElement;
			nextElement = other;
			other->previousElement = this;
			previousElement->nextElement = this;
		}
	}

	void LetMeInAfter(element<T>* other) {
		if (other->nextElement == nullptr) {
			//Other is the last element, not allowed to move the last element
			LetMeInBefore(other);
		}
		else {
			previousElement = other;
			nextElement = other->nextElement;
			other->nextElement = this;
			nextElement->previousElement = this;
		}
	}

public:
	void RemoveMe() {
		BridgeAcrossMe();
		delete this;
	}

	void Swap(element<T>& other) {
		element<T>* tempPrevious = previousElement;
		element<T>* tempNext = nextElement;
		previousElement = other.previousElement;
		nextElement = other.nextElement;
		other.previousElement = tempPrevious;
		other.nextElement = tempNext;
	}

	void MoveToBefore(element<T>* other) {
		BridgeAcrossMe();
		LetMeInBefore(other);
	}

	void MoveToAfter(element<T>* other) {
		BridgeAcrossMe();
		LetMeInAfter(other);
	}
};

template<typename T>
class linkedList {
	element<T>* firstElement;
	element<T>* lastElement;//Last element is a dummy element to help with knowing where the end is.  It should always be the last element.
	int count = 0;
	bool sorted = false;

	void Setup(bool sort = true) {
		lastElement = new element<T>(T(), this, nullptr, nullptr);
		firstElement = lastElement;
		if (sort)
			Sort();
	}

public:
	friend struct element<T>;
	linkedList(bool sort = false) {
		Setup(sort);
	}

	linkedList(const T& value, bool sort = false) {
		Setup(sort);
		Add(value);
	}

	template<size_t S>
	linkedList(const T(&arr)[S], bool sort = false) {
		Setup(sort);

		Add(arr);
	}

	int Count() {
		return count;
	}

	template<size_t S>
	void Add(const T(&arr)[S], int start = 0, int endNotIncluded = S) {
		if (endNotIncluded <= start)
			return;

		while (start < endNotIncluded) {
			Add(arr[start++]);
		}
	}

	void Add(const T& value) {
		element<T>* newElement = new element<T>(value, this);
		lastElement->InsertValueBeforeMe(value);
		++count;
	}

	template<typename ElementToStringFunc>
	std::string ToString(ElementToStringFunc elementToStringFunc, std::string label = "", bool reverse = false) const {
		bool first = true;
		std::string result = "";
		if (label != "") {
			result += label;
			if (reverse)
				result += "-Reverse";

			result += ": ";
		}

		result += "{ ";

		if (!reverse) {
			for (element<T>* current = firstElement; current->nextElement != nullptr; current = current->nextElement) {
				if (first) {
					first = false;
				}
				else {
					result += ", ";
				}

				result += elementToStringFunc(current->value);
			}
		}
		else {
			for (element<T>* current = lastElement->previousElement; current != nullptr; current = current->previousElement) {
				if (first) {
					first = false;
				}
				else {
					result += ", ";
				}

				result += elementToStringFunc(current->value);
			}
		}

		result += " }";

		return result;
	}

	template<typename ElementToStringFunc>
	void Print(ElementToStringFunc elementToStringFunc, std::string label = "", bool reverse = false) const {
		std::cout << ToString(elementToStringFunc, label, reverse) << std::endl;
	}

	element<T>* operator[](int index) {
		if (index < 0 || index >= count)
			throw std::out_of_range("Index out of range. index: " + std::to_string(index) + ", count: " + std::to_string(count));

		if (index <= count / 2) {
			element<T>* current = firstElement;
			while (--index > -1) {
				current = current->nextElement;
			}

			return current;
		}
		else {
			element<T>* current = lastElement;
			int end = count + 1;
			while (++index < end) {
				current = current->previousElement;
			}

			return current;
		}
	}

	element<T>* FindInsertElementByIndex(int start, const T& value, bool after = true) {
		element<T>* startElement = (*this)[start];
		bool goingUp = after && value >= startElement->value || !after && value > startElement->value;

		return FindInsertElement(startElement, value, goingUp, after);
	}

	/// <summary>
	/// Finds the element where the new value/element would be inserted.
	/// goindUp: true means it will go up the list, false means it will go down the list.
	/// after: Only matters if there are duplicates in a list or if value is equal to an element in the list.
	/// after being true will continue to pass elements of the same value, putting it after the existing ones.
	/// after being false will stop at the first element of the same value, putting it before the existing ones.
	/// </summary>
	element<T>* FindInsertElement(element<T>* start, const T& value, bool goingUp = true, bool after = true) {
		if (goingUp) {
			if (after) {
				while (start->nextElement != nullptr && value >= start->value) {
					start = start->nextElement;
				}
			}
			else {
				while (start->nextElement != nullptr && value > start->value) {
					start = start->nextElement;
				}
			}
		}
		else {
			if (after) {
				while (start->previousElement != nullptr && value < start->previousElement->value) {
					start = start->previousElement;
				}
			}
			else {
				while (start->previousElement != nullptr && value <= start->previousElement->value) {
					start = start->previousElement;
				}
			}
		}

		return start;
	}

	bool Find(const T& value, bool firstInstance = true) {
		element<T>* current = FindInsertElementByIndex(0, value, firstInstance);
		if (firstInstance) {
			bool found = current != nullptr && current->previousElement != nullptr && current->previousElement->value == value;
			if (!found && current->previousElement != nullptr)
				current = current->previousElement;

			return found;
		}
		else {
			return current->value == value;
		}
	}

	void Insert(int index, const T& value, bool after = false) {
		Insert((*this)[index], value, after);
	}

	void Insert(element<T>* current, const T& value, bool after = false) {
		if (!after) {
			current->InsertValueBeforeMe(value);
		}
		else {
			current->InsertValueAfterMe(value);
		}
	}

	void Remove(element<T>* current) {
		current->RemoveMe();
	}

	void RemoveAt(int index) {
		Remove((*this)[index]);
	}

	void Sort() {
		sorted = true;

		element<T>* current = firstElement->nextElement;
		for (; current->nextElement != nullptr;) {
			element<T>* insertElement = FindInsertElement(current, current->value, false, true);
			T& currentValue = current->value;
			T& insertValue = insertElement->value;
			element<T>* copy = current;
			current = current->nextElement;
			if (insertElement == copy)
				continue;//Already in the right place.

			copy->MoveToBefore(insertElement);
		}
	}

	void TryFindPrintResult(const T& value, bool firstInstance = true) {
		bool found = Find(value, firstInstance);
		if (found) {
			std::cout << "Found " << value << std::endl;
		}
		else {
			std::cout << value << " not found." << std::endl;
		}
	}

	template<typename ElementToStringFunc, size_t S>
	void SortAndTryFindPrintResults(const T(&valuesToFind)[S], ElementToStringFunc elementToStringFunc, bool firstInstance = true) {
		std::string name = "Unsorted";
		Print(elementToStringFunc, name);
		Print(elementToStringFunc, name, true);

		bool first = true;
		for (int i = 0; i < count; ++i) {
			if (first) {
				first = false;
			}
			else {
				std::cout << ", ";
			}

			std::cout << i << ": " << (*this)[i]->value;
		}

		std::cout << std::endl;

		Sort();
		name = "Sorted";
		Print(elementToStringFunc, name);
		Print(elementToStringFunc, name, true);
		for (const auto& value : valuesToFind) {
			TryFindPrintResult(value, firstInstance);
		}

		std::cout << std::endl;
	}

	void Clear() {
		element<T>* current = lastElement;
		while (current->previousElement != nullptr) {
			current->previousElement->RemoveMe();
		}
	}
};

const int DECK_SIZE = 52;
const int CARDS_PER_SUIT = 13;
const int SUITS_PER_DECK = DECK_SIZE / CARDS_PER_SUIT;
const std::string cardDisaplayNames[] = { "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King" };
const std::string suitDisplayNames[] = { "Spades", "Hearts", "Clubs", "Diamonds" };
const std::string DEFAULT_NUM_NAME = "Default Num";
const std::string DEFAULT_SUIT_NAME = "Default Suit";

struct Card {
public:
    Card() : CardID(DECK_SIZE) {}
    Card(int cardID) : CardID(cardID) {}
    Card(int cardNumber, int suit) : CardID(suit* CARDS_PER_SUIT + cardNumber) {}
    int CardID;

    int CardNumber() const {
        return CardID % CARDS_PER_SUIT;
    }

    int Suit() const {
        return CardID % SUITS_PER_DECK;
    }

    std::string Name() const {
        int cardNumber = CardNumber();
        int suit = Suit();
        std::string numberName = cardNumber >= 0 && cardNumber < CARDS_PER_SUIT ? cardDisaplayNames[cardNumber] : DEFAULT_NUM_NAME;
        std::string suitName = suit >= 0 && suit < SUITS_PER_DECK ? suitDisplayNames[suit] : DEFAULT_SUIT_NAME;
        return numberName + " of " + suitName;
    }
};

std::vector<Card> createDeck() {
    std::vector<Card> deck;
    for (int i = 0; i < SUITS_PER_DECK; ++i) {
        for (int j = 0; j < CARDS_PER_SUIT; ++j) {
            deck.push_back(Card(j, i));
        }
    }

    return deck;
};


/*
	using _Diff         = _Iter_diff_t<_RanIt>;
	auto _UTarget       = _UFirst;
	_Diff _Target_index = 1;
	for (; ++_UTarget != _ULast; ++_Target_index) { // randomly place an element from [_First, _Target] at _Target
		_Diff _Off = _RngFunc(static_cast<_Diff>(_Target_index + 1));
		_STL_ASSERT(0 <= _Off && _Off <= _Target_index, "random value out of range");
		if (_Off != _Target_index) { // avoid self-move-assignment
			_STD iter_swap(_UTarget, _UFirst + _Off);
		}
	}
*/
void shuffleDeck(std::vector<Card>& deck) { //thanks to chatgpt for helping with this function, I had to research the Fisher-Yates algorithm used here
    std::random_device rand;
    std::mt19937 rng(rand());
    //shuffle(deck.begin(), deck.end(), rng);
}

struct Player { //create our player structure
    std::string name; //name of the player
    std::vector<Card> hand; //players hand of cards
    int points = 0; //number of points initialized at zero
} cpu1, cpu2, cpu3, cpu4; //computer players

std::vector<Player> players = { cpu1, cpu2, cpu3, cpu4 };

void playerDraw(Player& player, std::vector<Card>& deck) {
    for (int i = 0; i < 5; i++) {
        player.hand.push_back(deck.back()); //adds the last card in the deck to the hand
        deck.pop_back(); //removes said card
    }
}

template<typename T, typename ElementToStringFunc>
void PrintVector(const std::vector<T>& vec, ElementToStringFunc elementToStringFunc, std::string label = "") {
    if (label != "")
        std::cout << label << ": ";

    if (vec.size() < 1) {
        std::cout << "<Empty>" << std::endl;
        return;
    }

    bool first = true;
    for (const T& value : vec) {
        if (first) {
            first = false;
        }
        else {
            std::cout << ", ";
        }

        std::cout << elementToStringFunc(value);
    }

    std::cout << std::endl;
}

int main() {
    std::vector<Card> deck = createDeck();
    shuffleDeck(deck);

    //Draw cards
    for (Player& player : players) {
        playerDraw(player, deck);
    }

    //Print hands
	auto toString = [](const Card& card) { return card.Name(); };
    for (const Player& player : players) {
		PrintVector(player.hand, toString, player.name);//no matching function call to PrintVector
    }

	PrintVector(deck, toString, "Deck");

    return 0;
}