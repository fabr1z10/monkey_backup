#include "component.h"


std::type_index Component::getType() {
	return std::type_index(typeid(*this));
}