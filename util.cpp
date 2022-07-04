#include <locale>
#include <codecvt>

std::u32string getString32(const std::string& str) {
	return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str.c_str());
}