#pragma once

#include <vector>
#include <sstream>
#include <glm/glm.hpp>



const float epsilon = 0.0001f;

template <class T>
bool isEqual (T x, T y, T eps = epsilon) {
    return std::abs(x-y) < eps;
}

template <class T>
bool isZero (T x, T eps = epsilon) {
    return isEqual(x, 0.0f, eps);
}


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

std::u32string getString32(const std::string&);

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) ;

// returns 1 if f >= 0, -1 otherwise
inline int sign (float f) {
	return f >= 0.0f ? 1 : -1;
}

// returns 1 if f >= 0, -1 otherwise
inline float signf (float f) {
    return f >= 0.0f ? 1.f : -1.f;
}

float computeOverlap(glm::vec2& p1, glm::vec2& p2);

bool seg2seg(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float &t);
