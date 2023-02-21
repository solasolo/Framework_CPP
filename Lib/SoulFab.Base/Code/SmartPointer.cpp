#include "SmartPointer.hpp"

namespace SoulFab::Base
{
	VariantPointer::VariantPointer()
		: ObjectRef(nullptr), Counter(nullptr)
	{
	}

	VariantPointer::~VariantPointer()
	{
		assert(!this->Counter || (*this->Counter) <= 0);
	}
}