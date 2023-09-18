/*
Author - Isaac Richards
Date - 04SEP23
Description - Template functions for searching and sorting arrays.
*/

#pragma once

#include <iostream>

template<typename T, size_t S>
int SizeOfArray(const T(&arr)[S]) {
	return (int)S;
}

/// <summary>
/// Finds the index that the value should be inserted at in the array using a binary search.
/// Requires the type T to have the <, > and == operators defined.
/// </summary>
/// <param name="after">Determines what do to if there are multiple of the same value in the array.  After will find the next index</param>
template<typename T, size_t S>
int FindInsertIndex(const T(&arr)[S], int start, int end, const T& value, bool after = true) {
	//Default
	if (start == end) {
		if (after && value >= arr[start] || !after && value > arr[start]) {
			return start + 1;
		}
		else {
			return start;
		}
	}

	//Check the middle value and half split the remaining array until start == end.
	int mid = (start + end) / 2;
	if (after && value >= arr[mid] || !after && value > arr[mid]) {
		return FindInsertIndex(arr, mid + 1, end, value, after);
	}
	else {
		//Since mid is always rounded down, must prevent mid -1 from being less than start.
		int newEnd = mid - 1;
		if (newEnd < start)
			newEnd = start;

		return FindInsertIndex(arr, start, newEnd, value, after);
	}
}

/// <summary>
/// Finds the index of the first occurrence of the value in the array
/// Requires the type T to have the <, > and == operators defined.
/// </summary>
/// <param name="foundIndex">: Index of the found value.  foundIndex will always be withing the range [start, end - 1].</param>
/// <param name="start">: 0 by default</param>
/// <param name="end">: -1 by default.  If end is -1, it will use the full range of the array.</param>
/// <returns>If the value was found in the array.</returns>
template<typename T, size_t S>
bool Find(const T(&arr)[S], const T& value, int& foundIndex, bool firstInstance = true, int start = 0, int end = -1) {
	if (end == -1)
		end = SizeOfArray(arr) - 1;

	foundIndex = FindInsertIndex(arr, start, end, value, !firstInstance);

	if (firstInstance) {
		bool found = arr[foundIndex] == value;
		//When looking for the first instance, if FindInsertIndex() is successful, the result will be the same as the first value index.
		//Otherwise, it will find the index where the value should be inserted which could be the index after the last value.
		if (!found && foundIndex > 0)
			foundIndex--;

		return found;
	}
	else {
		//foundIndex being 0 means value was the lowest value in the array, so it was not found.
		if (foundIndex == 0)
			return false;

		//FindInsertIndex finds the point in the array the value should be inserted.
		//If the value is equal, the new value would be put to the right of the current (same) value.
		//Since we aren't inserting a value, and just want to know if the array contains the value we're searching for, 
		//	we want to look at the previous value.
		foundIndex--;
		return arr[foundIndex] == value;
	}
}

/// <summary>
/// Moves every value in the array one index to the right, replacing the value then moves the value that was at the end to the start.
/// </summary>
template<typename T, size_t S>
void ShiftRightAndInsert(T(&arr)[S], int start, int end) {
	//Create a copy of the end value.
	T leftValue = arr[end];
	for (int i = end; i > start; --i) {
		arr[i] = arr[i - 1];
	}

	arr[start] = leftValue;
}

/// <summary>
/// Sorts the array from smallest to largest.
/// Requires the type T to have the <, > and == operators defined.
/// </summary>
template<typename T, size_t S>
void Sort(T(&arr)[S]) {
	int size = SizeOfArray(arr);
	if (size < 2)
		return;

	//Start at 1 because the first value has nothing to compare to.
	for (int i = 1; i < size; ++i) {
		T& value = arr[i];
		int insertIndex = FindInsertIndex(arr, 0, i - 1, value);
		if (insertIndex == i)
			continue;

		ShiftRightAndInsert(arr, insertIndex, i);
	}
}

/// <summary>
/// Prints the array to the console.
/// </summary>
/// <typeparam name="S">If a name is provided, it will be before the array, name: array.</typeparam>
template<typename T, size_t S>
void PrintArray(const T(&arr)[S], std::string& name = "") {
	if (name != "")
		std::cout << name << ": ";

	if (SizeOfArray(arr) < 1) {
		std::cout << "<Empty>" << std::endl;
		return;
	}

	bool first = true;
	for (const auto& value : arr) {
		if (first) {
			first = false;
		}
		else {
			std::cout << ", ";
		}

		std::cout << value;
	}

	std::cout << std::endl;
}

template<typename T, size_t S>
void TryFindPrintResult(const T(&arr)[S], const T& value, bool firstInstance = true, int start = 0, int end = -1) {
	if (end == -1)
		end = SizeOfArray(arr) - 1;

	int foundIndex = -1;
	bool found = Find(arr, value, foundIndex, firstInstance, start, end);
	if (found) {
		std::cout << "Found " << value << " at index: " << foundIndex << " arr[foundIndex] = " << arr[foundIndex] << std::endl;
	}
	else {
		std::cout << value << " not found.  Index: " << foundIndex << " arr[foundIndex] = " << arr[foundIndex] << std::endl;
	}
}

template<typename T, size_t S1, size_t S2>
void SortAndTryFindPrintResults(T(&arr)[S1], const T(&valuesToFind)[S2], bool firstInstance = true) {
	std::string name = "Unsorted";
	PrintArray(arr, name);

	Sort(arr);
	name = "Sorted";
	PrintArray(arr, name);
	for (const auto& value : valuesToFind) {
		TryFindPrintResult(arr, value, firstInstance);
	}

	std::cout << std::endl;
}

template<typename T, size_t S1>
void PrintArray(T(&arr)[S1], std::string name = "") {
	if (name != "")
		std::cout << name << ": ";

	bool first = true;
	int arrayCount = SizeOfArray(arr);
	for (int i = 0; i < arrayCount; ++i) {
		if (first) {
			first = false;
		}
		else {
			std::cout << ", ";
		}

		std::cout << arr[i];
	}

	std::cout << std::endl;
}

template<typename T>
void PrintArray(const std::shared_ptr<T[]> arr, int size, std::string name = "") {
	if (name != "")
		std::cout << name << ": ";

	bool first = true;
	for (int i = 0; i < size; ++i) {
		if (first) {
			first = false;
		}
		else {
			std::cout << ", ";
		}

		std::cout << arr[i];
	}

	std::cout << std::endl;
}