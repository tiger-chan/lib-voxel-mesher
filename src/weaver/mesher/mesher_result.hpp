#ifndef WEAVER_MESHER_MESHER_RESULT_HPP
#define WEAVER_MESHER_MESHER_RESULT_HPP

#include <vector>
#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/fwd.hpp"
#include "../core/vector2.hpp"
#include "../core/vector3.hpp"
#include "../core/vertex.hpp"
#include "../core/quad.hpp"

namespace tc
{
	struct WEAVER_API mesher_result
	{
		std::vector<vertex> vertices;
		std::vector<quad> quads;
	};
}

#endif // WEAVER_MESHER_MESHER_RESULT_HPP