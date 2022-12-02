#include "walk3d.h"


class PlayerWalk3D : public Walk3D {
public:
	PlayerWalk3D(const std::string& id, const pybind11::kwargs&);
	void control() override;

	void keyCallback(int key) override;

};


