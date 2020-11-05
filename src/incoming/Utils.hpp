
#pragma once

#include <sstream>
#include <vector>
#include <algorithm>

namespace utils
{
	inline std::string trim(const std::string &str)
	{
		auto first = str.find_first_not_of(' ');
		if (std::string::npos == first)
			return str;
		auto last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	inline std::string toLower(std::string &value)
	{
		std::transform(value.begin(), value.end(), value.begin(), ::tolower);
		return value;
	}

	inline std::vector<std::string> split(const std::string &source, char delimiter)
	{
		std::vector<std::string> strings;
		std::istringstream iss(source);
		std::string s;

		while (std::getline(iss, s, delimiter))
		{
			strings.push_back(utils::trim(s));
		}

		return strings;
	}
} // namespace utils
