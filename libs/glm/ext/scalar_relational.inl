#include "../common.hpp"

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool equal(genType const& x, genType const& y, genType const& epsilon)
	{
		return abs(x - y) <= epsilon;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR bool notEqual(genType const& x, genType const& y, genType const& epsilon)
	{
		return abs(x - y) > epsilon;
	}
}//namespace glm
