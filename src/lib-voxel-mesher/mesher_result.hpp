#pragma once

#include <vector>
#include "fwd.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vertex.hpp"
#include "quad.hpp"

namespace tc
{
	struct mesher_result
	{
		std::vector<vertex> vertices;
		std::vector<quad> quads;
	};
}
