#pragma once

#include <vector>
#include <sstream>

template<typename T>
std::vector<T> strToVec(const std::string& s, char delim=',') {
	std::stringstream s_stream(s); //create string stream from the string
	std::vector<T> out;
	while(s_stream.good()) {
		std::string substr;
		getline(s_stream, substr, delim); //get first string delimited by comma
		std::stringstream ss;
		ss << substr;
		T value;
		ss >> value;
		out.push_back(value);

	}
	return out;

}

