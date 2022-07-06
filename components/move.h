#pragma once

#include "../component.h"

#include <pybind11/pybind11.h>

class Move : public Component {
public:
	Move(pybind11::function f);
	void update(double) override;

private:
	pybind11::function m_func;
	float m_time;

};

