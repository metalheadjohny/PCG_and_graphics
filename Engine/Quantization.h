#pragma once
#include "pch.h"


// Expects min < max, does not check or assert for it, it's your responsibility to assure it. In practice min and max will almost never be equal
template <typename InType, typename OutType>
inline OutType QuantizeToType(InType in, InType min, InType max)
{
	static_assert(std::is_floating_point_v<InType>);	// May work for others but as far as I can see, it would generally be a mistake to create it
	static_assert(sizeof(InType) < sizeof(OutType));	// What's the point otherwise? At least for my use cases.

	InType ratio = (in - min) / (max - min);

	return ratio * static_cast<OutType>(~0);
}