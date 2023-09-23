#pragma once

#include<string>
#include <vector>
#include <iostream>
#include <sstream>

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

template<size_t S>
int get_option(const std::string(&option_list)[S]) {
	// List of all options with number labels
	std::vector<std::string> options;
	for (int i = 0; i < S; i++) {
		options.push_back(std::to_string(i + 1) + ". " + option_list[i]);
	}

	std::string prompt = join(options) + "\n";
	return get_integer_input_in_range(prompt, 1, S) - 1;
}

#pragma endregion