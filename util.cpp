#include <locale>
#include <codecvt>
#include "util.h"

std::u32string getString32(const std::string& str) {
	return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str.c_str());
}

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) {
	return std::acos(glm::dot(v1, v2));
}

float computeOverlap(glm::vec2& p1, glm::vec2& p2) {
	if (p2.x >= p1.x && p2.x <= p1.y) {
		if (p2.y > p1.y) {
			return -(p1.y - p2.x);
		}
	}
	else if (p1.x >= p2.x && p1.x <= p2.y) {
		if (p1.y > p2.y) {
			return (p2.y - p1.x);
		}
	}
	float a = p2.y - p1.x;
	float b = p1.y - p2.x;
	if (abs(a) < abs(b))
		return a;
	return -b;
}

float cross2d (glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

bool seg2seg(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float &t) {
    // eq for segment 1 is
    // Ax + t(Bx - Ax) = Cx + u(Dx - Cx)
    // Ay + t(By - Ay) = Cy + u(Dy - Cy)
    // which is
    // t(Bx - Ax) - u(Dx - Cx) = (Cx - Ax)
    // t(By - Ay) - u(Dy - Cy) = (Cy - Ay)
    // solution is
    //      | Cx - Ax    Dx - Cx |
    //      | Cy - Ay    Dy - Cy |
    // t = ------------------------
    //      | Bx - Ax    Dx - Cx |
    //      | By - Ay    Dy - Cy |
    glm::vec2 AB = B - A;
    glm::vec2 DC = C - D;
    float den = cross2d(AB, DC);
    if (isZero(den)) {
        return false;
    }
    glm::vec2 AC = C - A;
    t = cross2d(AC, DC) / den;
    if (t >= 0.0f && t <= 1.0f) {
        float u = cross2d(AB, AC) / den;
        return (u >= 0.0f && u <= 1.0f);
    }
    return false;
}

// Solve using Cramer's rule
int solve2x2(float a11, float a12, float b1, float a21, float a22, float b2, float& x, float& y) {
    auto det = (a11 * a22 - a12 * a21);
    if (det == 0.f) {
        return 1;
    }
    auto dx = b1 * a22 - a12 * b2;
    auto dy = a11 * b2 - b1 * a21;
    auto idet = 1.0f / det;
    x = dx * idet;
    y = dy * idet;
    return 0;
}