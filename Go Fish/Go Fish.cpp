#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <random>
#include <functional>
#include <memory>
#include <chrono>

template<typename T>
class linkedList;

/// <summary>
/// element acts as a container for a value in a linkedList that stores the value and pointers to the surrounding elements and the list.
/// </summary>
template<typename T>
struct element {
	
	#pragma region Constructos/Properties

	T value;
	element<T>* nextElement = nullptr;
	element<T>* previousElement = nullptr;
	linkedList<T>* list;

	template<typename... Args>
	element(linkedList<T>* List, Args&&... args) : list(List), value(std::forward<Args>(args)...) {}

	~element() {
		BridgeAcross();
		list->count--;
	}

	element(const element& other) = delete;

	//element(const element& other) {
	//	nextElement = other.nextElement;
	//	previousElement = other.previousElement;
	//	value = other.value;
	//	list = other.list;
	//}

	#pragma endregion

	#pragma region Getters/Checks

	T& Next() const {
		return nextElement->value;
	}

	T& Prev() const {
		return previousElement->value;
	}

	bool IsFirst() const {
		return previousElement == nullptr;
	}

	bool IsEnd() const {
		return nextElement == nullptr;
	}

	bool IsLast() const {
		return !IsEnd() && nextElement->IsEnd();
	}

	template<typename T>
	static void Inc(element<T>*& el) {
		el = el->nextElement;
	}

	template<typename T>
	static void Dec(element<T>*& el) {
		el = el->previousElement;
	}

	#pragma endregion

	#pragma region Insert/Remove

	/// <summary>
	/// Inserts value just before this element in the list
	/// </summary>
	void InsertNewBeforeMe(const T& value) {
		element<T>* newElement = new element<T>(list, value);
		newElement->InsertMeBeforeOther(this);
	}

	/// <summary>
	/// Emplace value just before this element in the list
	/// </summary>
	template<typename... Args>
	void EmplaceNewBeforeMe(Args&&... args) {
		element<T>* newElement = new element<T>(list, args...);
		newElement->InsertMeBeforeOther(this);
	}

	/// <summary>
	/// Inserts value just after this element in the list
	/// </summary>
	void InsertNewAfterMe(const T& value) {
		element<T>* newElement = new element<T>(list, value);
		newElement->InsertMeAfterOther(this);
	}

	/// <summary>
	/// Emplace value after before this element in the list
	/// </summary>
	template<typename... Args>
	void EmplaceNewAfterMe(Args&&... args) {
		element<T>* newElement = new element<T>(list, args...);
		newElement->InsertMeAfterOther(this);
	}

	void Remove() {
		delete this;
	}

	void MoveMeBeforeOther(element<T>* other) {
		BridgeAcross();
		InsertMeBeforeOther(other);
	}

	void MoveMeAfterOther(element<T>* other) {
		BridgeAcross();
		InsertMeAfterOther(other);
	}

	#pragma endregion

	#pragma region Manage Links

	void Swap(element<T>& other) {
		//Store the surrounding elements of this element
		element<T>* tempPrevious = previousElement;
		element<T>* tempNext = nextElement;

		//Connect this Element to the surrounding elements of other
		previousElement = other.previousElement;
		nextElement = other.nextElement;

		//Connect other to the surrounding elements of this element
		other.previousElement = tempPrevious;
		other.nextElement = tempNext;
	}

	/// <summary>
	/// Connects this element to the surrounding elements, then connects the surrounding elements to this element.
	/// Used when inserting or moving an element.
	/// </summary>
	void InsertMeBeforeOther(element<T>* other) {
		if (other->IsFirst()) {
			//other is the first element

			//Connect other and the list to this element
			previousElement = nullptr;
			nextElement = other;

			//Connect this element to the list and other
			list->firstElement = this;
			other->previousElement = this;
		}
		else {
			//other is not the first element

			//Connect surrounding elements to this element
			previousElement = other->previousElement;
			nextElement = other;

			//Connect this element to surrounding elements
			other->previousElement = this;
			previousElement->nextElement = this;
		}
	}

	/// <summary>
	/// Connects this element to the surrounding elements, then connects the surrounding elements to this element.
	/// Used when inserting or moving an element.
	/// </summary>
	void InsertMeAfterOther(element<T>* other) {
		if (other->IsEnd()) {
			//Other is the end element, not allowed to move the end element, so insert before it instead.
			InsertMeBeforeOther(other);
		}
		else {
			//other is not the end element

			//Connect surrounding elements to this element
			previousElement = other;
			nextElement = other->nextElement;

			//Connect this element to surrounding elements
			other->nextElement = this;
			nextElement->previousElement = this;
		}
	}

	/// <summary>
	/// Connects the previous and next elements from the surrounding elements together.
	/// Used when removing or moving an element.
	/// </summary>
	void BridgeAcross() {
		if (IsFirst()) {
			//This element is the first element

			//Connect the list to the next element
			list->firstElement = nextElement;
			nextElement->previousElement = nullptr;
		}
		else {
			//This element is not the first element

			//Connect the previous and next elements together
			previousElement->nextElement = nextElement;
			nextElement->previousElement = previousElement;
		}
	}

	#pragma endregion

	#pragma region Opperators

	bool operator<(const element<T>& other) const {
		return value < other.value;
	}

	bool operator>(const element<T>& other) const {
		return value > other.value;
	}

	bool operator<=(const element<T>& other) const {
		return value <= other.value;
	}

	bool operator>=(const element<T>& other) const {
		return value >= other.value;
	}

	bool operator==(const element<T>& other) const {
		return value == other.value;
	}

	bool operator!=(const element<T>& other) const {
		return value != other.value;
	}

	#pragma endregion

};

/// <summary>
/// linkedList uses elements of "element" to store values.
/// </summary>
template<typename T>
class linkedList {
private:

	#pragma region Properties

	/// <summary>
	/// First element of the list which starts as the same as the end element and is changed when the 
	///		first element is added or when elements are removed/inserted.
	/// </summary>
	element<T>* firstElement;

	/// <summary>
	/// End element is a dummy element to help with knowing where the end is and inserting new elements.<br/>
	/// It should always be the end element.
	/// </summary>
	element<T>* endElement;

	/// <summary>
	/// Number of elements in the list, not including the end element.
	/// </summary>
	int count = 0;

	/// <summary>
	/// If sorted is true, adding elements will automatically sort them.
	/// Otherwise, they will be added to the end of the list.
	/// </summary>
	bool sorted = false;

	typedef std::string(*ToStringFunc)(const T&);
	static std::string ElementToStringFuncDefault(const T& value) { return ""; }
	ToStringFunc elementToStringFunc;

	/// <summary>
	/// Creates the 
	/// </summary>
	void Setup() {
		endElement = new element<T>(this);
		firstElement = endElement;
	}

	linkedList(const linkedList& other) = delete;

	//linkedList(const linkedList& other) {
	//	//Fatal design flaw, each element tracks the list, so you need to update the list pointer for each element.
	//	element<T>* current = First();
	//	while (current != nullptr) {
	//		current->list = this;
	//		element<T>::Inc(current);
	//	}
	//}

	friend struct element<T>;

	#pragma endregion

public:

	#pragma region Constructors

	/// <summary>
	/// Initialize the list with no values except the end element.
	/// </summary>
	/// <param name="sort"></param>
	linkedList(ToStringFunc ElementToStringFunc = ElementToStringFuncDefault, bool sort = false) : elementToStringFunc(ElementToStringFunc), sorted(sort) {
		Setup();
	}

	/// <summary>
	/// Initialize the list with value as the first value.
	/// </summary>
	linkedList(const T& value, ToStringFunc ElementToStringFunc = ElementToStringFuncDefault, bool sort = false) : elementToStringFunc(ElementToStringFunc), sorted(sort) {
		Setup();
		Add(value);
	}

	/// <summary>
	/// Initialize the list, then add all values from the array to the list.
	/// </summary>
	template<size_t S>
	linkedList(const T(&arr)[S], ToStringFunc ElementToStringFunc = ElementToStringFuncDefault, bool sort = false) : elementToStringFunc(ElementToStringFunc), sorted(sort) {
		Setup();
		Add(arr);
	}

	#pragma endregion

	#pragma region Getters

	/// <summary>
	/// Gets the first element in the list.
	/// </summary>
	element<T>* First() const {
		return firstElement;
	}

	/// <summary>
	/// Gets the last element in the list.  Equivalent of count - 1 if indexing.
	/// </summary>
	element<T>* Last() const {
		return endElement->previousElement;
	}

	/// <summary>
	/// Gets the end element.
	/// The endElelment should only be used for checking if the end of the list has been reached or inserting elements before it.
	/// </summary>
	element<T>* End() const {
		return endElement;
	}

	/// <summary>
	/// Gets the number of elements in the list, not including the end element.
	/// </summary>
	int Count() {
		return count;
	}

	#pragma endregion

	#pragma region Adding/Removing

	/// <summary>
	/// Creates a new element, and adds it to the list.
	/// </summary>
	void Add(const T value) {
		if (sorted) {
			//If the list is already sorted, insert sort the new element.
			InsertSort(value);
		}
		else {
			//If not sorted, add the new element to the end of the list.
			endElement->InsertNewBeforeMe(value);
		}

		++count;
	}

	/// <summary>
	/// Adds all values from the array to the list from [start, end).
	/// </summary>
	/// <param name="end">Stops adding elements when start < end.  The value arr[end] is not added.</param>
	template<size_t S>
	void Add(const T(&arr)[S], int start = 0, int end = S) {
		if (end <= start)
			return;

		//Add each value from [start, end).
		while (start < end) {
			Add(arr[start++]);
		}
	}

	/// <summary>
	/// Creates a new element by directly constructing it's value in place, and adds it to the list.
	/// </summary>
	template<typename... Args>
	void Emplace(Args&&... args) {
		element<T>* newElement = new element<T>(this, args...);
		if (sorted) {
			//If the list is already sorted, insert sort the new element.
			InsertSort(newElement);
		}
		else {
			//If not sorted, add the new element to the end of the list.
			newElement->InsertMeBeforeOther(endElement);
		}

		++count;
	}

	/// <summary>
	/// Inserts a new element into the list at the current element.
	/// </summary>
	void Insert(element<T>* current, const T& value, bool after = false) {
		if (!after) {
			current->InsertNewBeforeMe(value);
		}
		else {
			current->InsertNewAfterMe(value);
		}
	}

	/// <summary>
	/// Inserts the value into the list in sorted order.
	/// </summary>
	void InsertSort(const T& value) {
		element<T>* current = FindInsertElement(value);
		current->InsertNewBeforeMe(value);
	}

	/// <summary>
	/// Creates a new element by directly constructing it's value in place, and inserts it into the list in sorted order.
	/// </summary>
	template<typename... Args>
	void EmplaceSort(Args&&... args) {
		element<T>* newElement = new element<T>(this, args...);
		InsertSort(newElement);
	}

	/// <summary>
	/// Inserts the new element into the list in sorted order.
	/// </summary>
	void InsertSort(element<T>* newElement) {
		element<T>* current = FindInsertElement(newElement->value);
		newElement->InsertMeBeforeOther(current);
	}

	/// <summary>
	/// Removes the current element from the list.
	/// </summary>
	void Remove(element<T>* current) {
		current->Remove();
	}

	/// <summary>
	/// Removes the element at the index.
	/// </summary>
	void RemoveAt(int index) {
		Remove((*this)[index]);
	}

	/// <summary>
	/// Deletes all elements in the list, except the end element.
	/// </summary>
	void Clear() {
		element<T>* current = End();
		while (!current->IsFirst()) {
			current->previousElement->Remove();
		}
	}

	#pragma endregion

	#pragma region Sorting/Finding

	/// <summary>
	/// Finds the element where the new value/element would be inserted.
	/// Should be used when adding new elements to a sorted list.
	/// </summary>
	/// <param name="after">Only used when values in the list are equal to the value being searched.
	/// If true, the resulting element will occur just after an element with the same value.
	/// If false, it will be before the last element of the same value.</param>
	element<T>* FindInsertElement(const T& value, bool after = true) {
		element<T>* startElement = First();
		return FindInsertElement(startElement, value, after);
	}

	/// <summary>
	/// Finds the element where the new value/element would be inserted.
	/// </summary>
	/// <param name="after">- Only matters if there are duplicates in a list or if value is equal to an element in the list.
	/// after being true will continue to pass elements of the same value, putting it after the existing ones.
	/// after being false will stop at the first element of the same value, putting it before the existing ones.</param>
	/// <param name="goingUp">- true means it will go up the list, false means it will go down the list.</param>
	/// <returns></returns>
	element<T>* FindInsertElement(element<T>* start, const T& value, bool after = true) {
		while (!start->IsLast() && (after && value >= start->value || !after && value > start->value)) {
			element<T>::Inc(start);
		}

		return start;
	}

	/// <summary>
	/// Finds the element where the new value would be inserted by going backwards through the list.
	/// </summary>
	/// <param name="after">- Only matters if there are duplicates in a list or if value is equal to an element in the list.
	/// after being true will continue to pass elements of the same value, putting it after the existing ones.
	/// after being false will stop at the first element of the same value, putting it before the existing ones.</param>
	element<T>* FindInsertElementReverse(element<T>* start, const T& value, bool after = true) {
		while (!start->IsFirst() && (after && value < start->Prev() || !after && value <= start->Prev())) {
			element<T>::Dec(start);
		}

		return start;
	}

	/// <summary>
	/// Gets the value by index.  Should only be used when no elements are already available to use to move through the list.
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	element<T>* operator[](int index) {
		if (index < 0 || index >= count)
			throw std::out_of_range("Index out of range. index: " + std::to_string(index) + ", count: " + std::to_string(count));

		if (index <= count / 2) {
			element<T>* current = First();
			while (index-- > 0) {
				element<T>::Inc(current);
			}

			return current;
		}
		else {
			element<T>* current = End();
			while (index++ < count) {
				element<T>::Dec(current);
			}

			return current;
		}
	}

	/// <summary>
	/// Checks if the list contains the value.
	/// </summary>
	bool Contains(const T& value) {
		element<T>* current = FindInsertElement(value, false);
		return current->value == value;
	}

	/// <summary>
	/// Sorts the list using insertion sort.
	/// </summary>
	void Sort() {
		//Once sorted, it will stay sorted.
		if (sorted)
			return;

		sorted = true;

		element<T>* current = First();
		element<T>::Inc(current);
		while (!current->IsEnd()) {
			element<T>* insertElement = FindInsertElementReverse(current, current->value);
			element<T>* copy = current;
			element<T>::Inc(current);
			if (insertElement == copy)
				continue;//Already in the right place.

			copy->MoveMeBeforeOther(insertElement);
		}
	}

	void Shuffle(int passes = 5) {

	}

	#pragma endregion

	#pragma region ToString

	/// <summary>
	/// Converts the list to a string.
	/// </summary>
	/// <param name="label">- label is printed before the string if included.</param>
	/// <param name="reverse">- For testing to make sure the list can be traversed backwards.</param>
	std::string ToString(std::string label = "", bool reverse = false) const {
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
			for (element<T>* current = endElement->previousElement; current != nullptr; current = current->previousElement) {
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

	static std::string ToString(const linkedList<T>& list) {
		return list.ToString();
	}

	/// <summary>
	/// Prints the list to the console.
	/// </summary>
	void Print(std::string label = "", bool reverse = false) const {
		std::cout << ToString(label, reverse) << std::endl;
	}

	#pragma endregion

	#pragma region Operators

	bool operator==(const linkedList& other) const {
		if (count != other.count)
			return false;

		element<T>* current = firstElement;
		element<T>* otherCurrent = other.firstElement;
		for (; current->nextElement != nullptr && otherCurrent->nextElement != nullptr;
			current = current->nextElement, otherCurrent = otherCurrent->nextElement) {
			if (current->value != otherCurrent->value)
				return false;
		}

		return true;
	}

	bool operator!=(const linkedList& other) const {
		return !(*this == other);
	}

	bool operator<(const linkedList& other) const {
		element<T>* current = firstElement;
		element<T>* otherCurrent = other.firstElement;
		for (; current != nullptr; current = current->nextElement, otherCurrent = otherCurrent->nextElement) {
			if (otherCurrent == nullptr)
				return false;

			if (current->value < otherCurrent->value)
				return true;

			else if (current->value > otherCurrent->value)
				return false;
		}

		return otherCurrent != nullptr;
	}

	bool operator>(const linkedList& other) const {
		element<T>* current = firstElement;
		element<T>* otherCurrent = other.firstElement;
		for (; current != nullptr; current = current->nextElement, otherCurrent = otherCurrent->nextElement) {
			if (otherCurrent == nullptr)
				return true;

			if (current->value > otherCurrent->value)
				return true;

			else if (current->value < otherCurrent->value)
				return false;
		}

		return false;
	}

	bool operator<=(const linkedList& other) const {
		return !(*this > other);
	}

	bool operator>=(const linkedList& other) const {
		return !(*this < other);
	}

	linkedList& operator=(const linkedList& other) {
		//Fatal design flaw, each element tracks the list, so you need to update the list pointer for each element.
		element<T>* current = First();
		while (current != nullptr) {
			current->list = this;
			element<T>::Inc(current);
		}
	}

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	linkedList(linkedList&& other) noexcept
		: firstElement(other.firstElement), endElement(other.endElement), count(other.count), sorted(other.sorted), elementToStringFunc(other.elementToStringFunc) {
		other.firstElement = nullptr;
		other.endElement = nullptr;
		other.count = 0;
		other.sorted = false;
		other.elementToStringFunc = nullptr;
	}

	#pragma endregion
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

    /// <summary>
    /// CardID is a number from 0 to 51 representing a playing card.
    /// </summary>
    int CardID;

	/// <summary>
	/// Gets the 0 through 12 card number which is the index for this cards name in cardDisaplayNames.
	/// </summary>
    int CardNumber() const {
        return CardID % CARDS_PER_SUIT;
    }

	/// <summary>
	/// Gets the 0 through 3 card suit which is the index for this cards name in suitDisplayNames.
	/// </summary>
    int Suit() const {
        return CardID % SUITS_PER_DECK;
    }

    /// <summary>
    /// Get the cards full display name including number and suit.
    /// </summary>
    std::string Name() const {
        int cardNumber = CardNumber();
        int suit = Suit();

		//Use default name if the card number or suit is out of range.
        std::string numberName = cardNumber >= 0 && cardNumber < CARDS_PER_SUIT ? cardDisaplayNames[cardNumber] : DEFAULT_NUM_NAME;
        std::string suitName = suit >= 0 && suit < SUITS_PER_DECK ? suitDisplayNames[suit] : DEFAULT_SUIT_NAME;
        return numberName + " of " + suitName;
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
		return card.Name();
	}
};

class Player {
	Player(const Player& other) = delete;//Delete copy constructor to prevent copying Player objects.
public:
	int playerNumber;
	Player() : playerNumber(-1), name("Default"), hand(Card::ToString, true) {}
	Player(int PlayerNumber) : playerNumber(PlayerNumber), name("Player " + std::to_string(playerNumber)), hand(Card::ToString, true) {}
	Player(int PlayerNumber, std::string Name) : playerNumber(PlayerNumber), name(Name), hand(Card::ToString, true) {}
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

std::unique_ptr<linkedList<Card>> createDeck() {
	std::unique_ptr <linkedList<Card>> deck = std::make_unique<linkedList<Card>>(Card::ToString);
    for (int i = 0; i < SUITS_PER_DECK; ++i) {
        for (int j = 0; j < CARDS_PER_SUIT; ++j) {
            deck->Emplace(j, i);
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
void shuffleDeck(linkedList<Card>& deck) { //thanks to chatgpt for helping with this function, I had to research the Fisher-Yates algorithm used here
    std::random_device rand;
    std::mt19937 rng(rand());

    //shuffle(deck.begin(), deck.end(), rng);
}

bool playerDraw(Player& player, linkedList<Card>& deck, int num = 1) {
	//linkedList<Card>& hand = hands[player.playerNumber]->value;
    for (int i = 0; i < num; i++) {
		element<Card>* card = deck.First();
		player.hand.Emplace(card->value);
		card->Remove();
    }

	int temp = deck.Count();
	return deck.Count() > 0;
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
	std::unique_ptr<linkedList<Card>> deck = createDeck();
	shuffleDeck(*deck);

	int numberOfPlayers = 4;
	for (int i = 0; i < numberOfPlayers; i++) {
		players.Emplace(players.Count());
	}

	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		playerDraw(player->value, *deck, 5);
	}

	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		player->value.hand.Print(player->value.name + " hand (" + std::to_string(player->value.hand.Count()) + ")");
	}

	std::cout << std::endl;

	deck->Print("Deck");

	return 0;
};