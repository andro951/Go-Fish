#pragma once

#include <iostream>
#include <string>

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
		while (!start->IsEnd() && (after && value >= start->value || !after && value > start->value)) {
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

	std::stack<T> ToStack() {
		std::stack<T> stack;
		if (count > 0) {
			//Stacks are LIFO, so we need to go backwards through the list so that the first element is on top.
			for (element<T>* current = End(); !current->IsFirst(); element<T>::Dec(current)) {
				stack.push(current->previousElement->value);
			}
		}

		return stack;
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