#pragma once

#include <vector>
#include <map>
#include <queue>
#include <memory>
#include "Utility.h"

std::shared_ptr<int[]> BigWheelSieve(int requiredPrimesCount) {
	std::shared_ptr<int[]> primes(new int[requiredPrimesCount]);
	int lastPrimeIndex = requiredPrimesCount - 1;
	int nextPrimeArrayIndex = 0;
	primes[lastPrimeIndex] = 0;
	//std::vector<int> primes = { 2, 3, 5, 7 };
	int startingPrimes[] = {2, 3, 5, 7};
	int startingPrimesCount = SizeOfArray(startingPrimes);
	for (; nextPrimeArrayIndex < startingPrimesCount && nextPrimeArrayIndex < requiredPrimesCount; ++nextPrimeArrayIndex) {
		primes[nextPrimeArrayIndex] = startingPrimes[nextPrimeArrayIndex];
	}

	int potentialPrime = 11;
	int bigWheelIndex = 2;
	int bigWheelSize = 10;
	int primesNextWheelIndex = 2;
	int nextWheelToDrop = 5;
	std::vector<int> bigWheel = { 4, 2, 4, 2, 4, 2, 4, 2, 4, 2 };
	std::map<int, int> primeIndexes = { { 2, 0 }, { 3, 1 }, { 5, 2 }, {7, 3 } };
	std::vector<int> wheelRepititionCircumfrances = { 2, 6, 30, 210 };//Same size as primes
	std::vector<std::vector<int>> stitches = { {}, {}, { 5 }, { 7 } };//Same size as primes
	//std::vector<std::vector<int*>> stitches = { {}, {}, { &bigWheel[1] }, { &bigWheel[2] }};//Same size as primes.  Points to the 2nd of the 2 values that need to be stitched

	int nextCompositeWheelBlockingValue = 25;
	std::vector<int> compositeWheels = { 5, 7 };
	std::vector<int> compositeWheelNextBlockingValues = { 25, 49 };//Same size as compositeWheels
	std::vector<int> compositeWheelPrimeMultiplierIndexes = { 2, 3 };//Same size as compositeWheels
	std::vector<int> compositeWheelToPrimeWheelIndex = { 2, 3 };//Same size as compositeWheels
	std::vector<int> compositeWheelPrimeStartingIndexs = { 2, 3 };//Same size as compositeWheels
	std::vector<bool> compositeWheelWillHaveChildren = { false, false };//Same size as compositeWheels

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> compositeWheelIndexQueue;
	compositeWheelIndexQueue.push({ 25, 0 });
	compositeWheelIndexQueue.push({ 49, 1 });

	while (primes[lastPrimeIndex] == 0) {
		//Check if the number is the next composite that isn't skipped by the big wheel.
		if (potentialPrime == nextCompositeWheelBlockingValue) {
			//Composite value hit.  Replace the next composite blocking value with the next composite value from the queue.

			//The compositeWheel stays the same while the primeMultiplierIndex increases so that compositeWheel is multiplied by 
			// every prime >= it's primeStartingIndex.
			//Denoted as compositeWheel * (startingPrime) meaning the compositeWheel will be multiplied by the set of all primes 
			// starting with startingPrime while the product is <= repetition circumference.
			//Example 13 * (13) is 13 * set of prime numbers between 13 and 173.
			//13 * (13)
			//13 *  13
			//13 *  17
			//13 *  19
			//13 *  23
			//...
			//13 *  173
			std::pair<int, int> queValue = compositeWheelIndexQueue.top();
			compositeWheelIndexQueue.pop();
			int compositeWheelIndex = queValue.second;
			int compositeWheel = compositeWheels[compositeWheelIndex];
			int compositeWheelPrimeStartingIndex = compositeWheelPrimeStartingIndexs[compositeWheelIndex];
			int compositeWheelBasePrimeIndex = compositeWheelToPrimeWheelIndex[compositeWheelIndex];
			int wheelRepetitionCircumfrance = wheelRepititionCircumfrances[compositeWheelBasePrimeIndex];
			int currentPrimeMultiplerIndex = compositeWheelPrimeMultiplierIndexes[compositeWheelIndex];
			stitches[compositeWheelBasePrimeIndex].push_back(potentialPrime);

			//Create a Larger Child composite wheel if a previous child attempt first hit wasn't above the repetition circumference.
			//compositeWheel * startingPrime has a child, compositeWheel * startingPrime * (startingPrime).
			//Example: 13 * 13 has a child, 13 * 13 * (13).
			//Putting them all together:
			//Repetition Circumference: 30030
			//13 * (13)
			//13 *  13 -> 13 * 13 * (13)
			//            13 * 13 *  13 -> 13 * 13 * 13 * (13)
			//                             13 * 13 * 13 *  13 "no child"
			//                             13 * 13 * 13 *  17 "37,349 too high, stop"
			// 
			//            13 * 13 *  17 -> 13 * 13 * 17 * (17) "37,349 too high, stop having children"
			//            13 * 13 *  19
			//            13 * 13 *  23
			//            ...
			//            13 * 13 *  173
			//            13 * 13 *  179 "30,251 too high, stop"
			// 
			//13 *  17 -> 13 * 17 * (17)
			//            13 * 17 *  17
			//            13 * 17 *  19
			//            13 * 17 *  23
			//            ...
			//            13 * 17 *  131
			//            13 * 17 *  137 "30,277 too high, stop"
			//
			//13 *  19 -> 13 * 19 * (19)
			//            13 * 19 *  19
			//            13 * 19 *  23
			//            13 * 19 *  29
			//            ...
			//            13 * 19 *  113
			//            13 * 19 *  127 "31,369 too high, stop"
			//..........................
			//13 *  47 -> 13 * 47 * (47)
			//            13 * 47 *  47
			//            13 * 47 *  53 "32,383 too high, stop"
			// 
			//13 *  53 -> 13 * 53 * (53) "32,383 too high, stop having children"
			//13 *  59
			//13 *  61
			//...
			//13 *  2309
			//13 *  2311 "30,043 too high, stop"
			if (compositeWheelWillHaveChildren[compositeWheelIndex]) {
				int compositeWheelPrimeStartingValue = primes[currentPrimeMultiplerIndex];
				int max = wheelRepetitionCircumfrance / compositeWheelPrimeStartingValue;
				if (potentialPrime <= max) {
					int newCompositeWheelBlockingValue = potentialPrime * compositeWheelPrimeStartingValue;
					if (wheelRepetitionCircumfrance >= newCompositeWheelBlockingValue) {
						int newCompositeWheelIndex = compositeWheels.size();
						compositeWheels.push_back(potentialPrime);
						compositeWheelNextBlockingValues.push_back(newCompositeWheelBlockingValue);
						compositeWheelToPrimeWheelIndex.push_back(compositeWheelBasePrimeIndex);
						compositeWheelPrimeMultiplierIndexes.push_back(currentPrimeMultiplerIndex);
						compositeWheelPrimeStartingIndexs.push_back(currentPrimeMultiplerIndex);
						compositeWheelWillHaveChildren.push_back(true);
						compositeWheelIndexQueue.push({ newCompositeWheelBlockingValue, newCompositeWheelIndex });
					}
					else {
						compositeWheelWillHaveChildren[compositeWheelIndex] = false;
					}
				}
			}

			//Check the next composite hit for the composite wheel that was just hit.
			int nextPrimeMultiplerIndex = ++compositeWheelPrimeMultiplierIndexes[compositeWheelIndex];
			int nextPrimeToMultiply = primes[nextPrimeMultiplerIndex];

			//compositeWheelWillHaveChildren[compositeWheelIndex] = thisCompositeWheelWillHaveChildren;
			int max = wheelRepetitionCircumfrance / compositeWheel;
			if (nextPrimeToMultiply <= max) {
				int nextHitValue = compositeWheel * nextPrimeToMultiply;
				compositeWheelNextBlockingValues[compositeWheelIndex] = nextHitValue;
				compositeWheelIndexQueue.push({ nextHitValue, compositeWheelIndex });
			}


			nextCompositeWheelBlockingValue = compositeWheelNextBlockingValues[compositeWheelIndexQueue.size() > 0 ? compositeWheelIndexQueue.top().second : 0];
		}
		else {
			//Not a composite hit, so the number is prime.
			primeIndexes.emplace(potentialPrime, nextPrimeArrayIndex);
			primes[nextPrimeArrayIndex] = potentialPrime;
			nextPrimeArrayIndex++;

			//Check if the first important blocking value, potentialPrime^2 is in the desired range.
			if (requiredPrimesCount / potentialPrime >= potentialPrime) {//Same as if (potentialPrime * potentialPrime < requiredPrimesCount), but prevents overflow.
				//Prime data
				int square = potentialPrime * potentialPrime;
				int primeIndex = nextPrimeArrayIndex - 1;
				int lastWheelCircumfrance = wheelRepititionCircumfrances[wheelRepititionCircumfrances.size() - 1];
				int wheelRepititionCircumfrance;
				if (lastWheelCircumfrance >= requiredPrimesCount) {
					wheelRepititionCircumfrance = requiredPrimesCount;
				}
				else {
					int max = requiredPrimesCount / lastWheelCircumfrance;
					if (potentialPrime > max) {
						wheelRepititionCircumfrance = requiredPrimesCount;
					}
					else {
						wheelRepititionCircumfrance = lastWheelCircumfrance * potentialPrime;
						if (wheelRepititionCircumfrance > requiredPrimesCount)
							wheelRepititionCircumfrance = requiredPrimesCount;
					}
				}

				//Cap wheelRepititionCircumfrance at requiredPrimesCount to prevent overflowing.
				wheelRepititionCircumfrances.push_back(wheelRepititionCircumfrance);

				//Add one or more composite wheels based off the current prime.
				if (wheelRepititionCircumfrance >= square) {
					//Create one composite wheel for the prime itself with a first hit value of potentialPrime^2.
					int compositeWheelIndex = compositeWheels.size();
					compositeWheels.push_back(potentialPrime);
					compositeWheelNextBlockingValues.push_back(square);
					compositeWheelToPrimeWheelIndex.push_back(primeIndex);
					compositeWheelPrimeMultiplierIndexes.push_back(primeIndex);
					compositeWheelPrimeStartingIndexs.push_back(primeIndex);
					compositeWheelWillHaveChildren.push_back(true);
					compositeWheelIndexQueue.push({ square, compositeWheelIndex });
				}

				//Add the first stitch for the prime value being hit.
				stitches.push_back(std::vector<int>{ potentialPrime });
			}
		}

		//Check if the bigWheel has reached the requiredPrimesCount of it's current size.
		//If it has, it needs to be sized up by dropping the current wheel and working 
		//	on the repetition circumference of the next prime wheel.
		if (++bigWheelIndex == bigWheelSize) {
			//Do stitches for current wheel before sizing up the big wheel.
			int tempNum = 1;
			int tempBigWheelIndex = -1;

			//for (const auto& stitch : stitches[primesNextWheelIndex]) {//TODO: Make stitches pointers/reference to the bigWheel values instead of using tempNum/tempBigWheelIndex
			std::vector<int>& stitchesForCurrentWheel = stitches[primesNextWheelIndex];
			for (int i = 0; i < stitchesForCurrentWheel.size(); ++i) {
				//Stitch by combining the value at the index of the stitch with the value at the previous index.
				while (tempNum < stitchesForCurrentWheel[i]) {
					int numToAdd = bigWheel[++tempBigWheelIndex];
					tempNum += numToAdd;
				}

				int numToAdd2 = bigWheel[++tempBigWheelIndex];
				tempNum += numToAdd2;
				bigWheel.erase(bigWheel.begin() + tempBigWheelIndex);
				--tempBigWheelIndex;
				--bigWheelIndex;
				--bigWheelSize;
			}

			//Duplicate the big wheel x number of times, where x is the next prime wheel being worked on. 2, 6, 30, 210, 2310, 30030, 510510...
			++primesNextWheelIndex;
			nextWheelToDrop = primes[primesNextWheelIndex];
			std::vector<int> bigWheelCopy = bigWheel;
			for (int i = 0; i < nextWheelToDrop - 1; ++i) {
				bigWheel.insert(bigWheel.end(), bigWheelCopy.begin(), bigWheelCopy.end());
			}

			bigWheelSize = bigWheel.size();//TODO: calculate manually.
		}

		//Spin the big wheel to get the next potential prime.
		potentialPrime += bigWheel[bigWheelIndex];

	}

	return primes;
}