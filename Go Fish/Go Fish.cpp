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
		if (&other == this)
			return;

		if (IsFirst()) {
			list->firstElement = &other;
		}
		else {
			previousElement->nextElement = &other;
		}

		if (other.IsFirst()) {
			list->firstElement = this;
		}
		else {
			other.previousElement->nextElement = this;
		}

		//Store the surrounding elements of this element
		element<T>* tempPrevious = previousElement;
		element<T>* tempNext = nextElement;

		//Connect this Element to the surrounding elements of other
		previousElement = other.previousElement;
		nextElement = other.nextElement;

		//Connect other to the surrounding elements of this element
		other.previousElement = tempPrevious;
		other.nextElement = tempNext;

		//Connect the surrounding elements of this element to this element
		
		nextElement->previousElement = this;

		//Connect the surrounding elements of other to other
		
		other.nextElement->previousElement = &other;
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

	friend struct element<T>;

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

	void Shuffle(int passes = 10) {
		if (count < 2)
			return;

		element<T>* current = First();
		for (int i = 0; i < passes * count; i++) {
			element<T>::Inc(current);
			if (current->IsEnd()) {
				current = First();
				continue;
			}

			int random = rand() % count;
			element<T>* randomElement = (*this)[random];
			if (current->previousElement == randomElement) {
				i--;
				continue;
			}

			current->previousElement->Swap(*randomElement);
		}
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
		targetPlayerNumber(TargetPlayerNumber), currentPlayerNumber(CurrentPlayerNumber), card(CardID),
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
		std::cout << GuessToString(GetPlayerNameFunc) << std::endl << std::endl;
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
					result += "Luck of the draw!  Four of a kind! (" + card.NumberName() + ")";

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

class NPC {
	virtual Guess NextGuess() = 0;
};

class Observer : NPC {
	//const std::string names[] = { "Sheldon", "Leonard", "Hermione", "Dexter", "Neo" };
public:
	Observer(int PlayerNumber, int PlayerCount) : playerNumber(PlayerNumber), playerCount(PlayerCount) {}
	int playerNumber;
	int playerCount;
	Guess NextGuess() override {
		int randomPlayerNumber = rand() % (playerCount - 1);
		if (randomPlayerNumber >= playerNumber)
			randomPlayerNumber++;

		int randomCardNumber;
		do {
			randomCardNumber = rand() % CARDS_PER_SUIT;
		}
		while (FourOfAKinds[randomCardNumber] != -1);

		return Guess(randomPlayerNumber, playerNumber, randomCardNumber);
	}
};

class Randomizer : NPC {
public:
	Guess NextGuess() override {
		return Guess(0, 0, 0);
	}
};

class Trickster : NPC {
public:
	Guess NextGuess() override {
		return Guess(0, 0, 0);
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
std::unique_ptr<linkedList<Card>> deck;
element<Player>* currentPlayer;
std::vector<Guess> lastGuesses;
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

bool playerDraw(Player& player, linkedList<Card>& deck, int num = 1) {
    for (int i = 0; i < num; i++) {
		element<Card>* card = deck.First();
		player.hand.Emplace(card->value);
		player.hand.Print("player hand");
		card->Remove();
    }

	int temp = deck.Count();
	return deck.Count() > 0;
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
	int numberOfPlayers = get_integer_input_in_range(playersPrimpt, MIN_PLAYERS - 1, MAX_PLAYERS - 1) + 1;
	std::cout << std::endl;
	std::cout << "What is your name?\n";
	std::string player0Name;
	std::cin >> player0Name;
	std::cout << std::endl;

	players.Emplace(0, player0Name);
	for (int i = 1; i < numberOfPlayers; i++) {
		players.Emplace(players.Count());
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

	deck = createDeck();
	deck->Shuffle();

	int startingCards = numberOfPlayers > 2 ? 5 : 7;
	for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
		playerDraw(player->value, *deck, startingCards);
	}

	bool printHandsAndDeck = false;
	if (printHandsAndDeck) {
		for (element<Player>* player = players.First(); !player->IsEnd(); element<Player>::Inc(player)) {
			player->value.hand.Print(player->value.name + " hand (" + std::to_string(player->value.hand.Count()) + ")");
		}

		std::cout << std::endl;

		deck->Print("Deck");
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

	std::cout << std::endl;
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

Guess GetPlayerGuess() {
	std::string prompt = "What player would you like to guess? (2 - " + std::to_string(players.Count()) + ")";
	int targetPlayerNumber = get_integer_input_in_range(prompt, 2, players.Count()) - 1;
	std::cout << std::endl;

	prompt = "What card would you like to guess? (1 - " + std::to_string(CARDS_PER_SUIT) + ")";
	int cardNumber = get_integer_input_in_range(prompt, 1, CARDS_PER_SUIT) - 1;
	std::cout << std::endl;

	return Guess(targetPlayerNumber, currentPlayer->value.playerNumber, cardNumber);
}

void Quit() {
	std::cout << "Thanks for playing!\n";
	std::exit(0);
}

Guess PlayerOptions() {
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

Guess GetNPCGuess() {
	Observer observer(currentPlayer->value.playerNumber, players.Count());//Move this to be part of the player class.

	return observer.NextGuess();
}

void UpdateGuessResult(Guess& guess) {
	//Check if the guess is correct.
	int currentPlayerNumber = guess.currentPlayerNumber;
	Card lowestCardOfNumber(guess.card.CardNumber(), 0);
	element<Card>* card = players[guess.targetPlayerNumber]->value.hand.FindInsertElement(lowestCardOfNumber, false);
	int guessedCardNumber = guess.card.CardNumber();
	if (card->value.CardNumber() == guessedCardNumber) {
		guess.guessResult = GuessResultID::Success;
		int transfered = 0;
		element<Card>::Inc(card);
		while (card->previousElement->value.CardNumber() == guessedCardNumber) {
			transfered++;
			int cardID = card->value.CardID;
			players[currentPlayerNumber]->value.hand.Emplace(cardID);
			card->previousElement->Remove();
			element<Card>::Inc(card);
			if (card->IsEnd())
				break;
		}

		guess.numberOfCardsRecieved = transfered;
	}
	else {
		guess.guessResult = GuessResultID::FailGoFish;
		playerDraw(currentPlayer->value, *deck);
	}

	//Check if the player has four of a kind.
	element<Card>* playersCard = currentPlayer->value.hand.FindInsertElement(lowestCardOfNumber, false);
	if (playersCard->value.CardNumber() == guessedCardNumber) {
		//Count the number of cards with the guessed card number.
		int count = 0;
		element<Card>::Inc(playersCard);
		while (playersCard->previousElement->value.CardNumber() == guessedCardNumber) {
			count++;
			element<Card>::Inc(playersCard);
			if (playersCard->IsEnd())
				break;
		}

		//4 of a kind, update the four of a kind array and remove the cards from the players hand.
		if (count == CARDS_PER_SUIT) {
			guess.guessResult = guess.guessResult == GuessResultID::Success ? GuessResultID::Success4OfAKind : GuessResultID::GoFish4OfAKind;
			FourOfAKinds[guessedCardNumber] = currentPlayerNumber;
			while (!card->IsFirst() && card->previousElement->value.CardNumber() == guessedCardNumber) {
				card->previousElement->Remove();
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

	if (deck->Count() > 0)
		CurrentPlayerTurn();
}

void EndGame() {
	std::cout << "Game Over!\n";
	std::cout << "Final Scores:\n";

	for (const int& playerScoreNumber : FourOfAKinds) {
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