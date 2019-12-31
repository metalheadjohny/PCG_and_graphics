#pragma once
#include <vector>
#include <list>
#include "Math.h"
#include "Hull.h"

class Steering
{
public:

	//trivial but I don't even need it rn...
	static SVec3 seek(SVec3 pos)
	{

	}



	template <typename NavAgent>
	static SVec3 separate(NavAgent& me, const std::list<NavAgent*>& theBois)
	{
		SVec3 result = SVec3::Zero;

		if (theBois.empty())
			return result;

		for (const NavAgent* boi : theBois)
		{
			SVec3 separator = -static_cast<SVec3>(boi->getPosition() - me.getPosition());
			float distance = separator.Length();
			
			
			if (distance < 0.00001)	//avoid self... could be handled otherwise but cba
				continue;

			separator /= distance;
			//float intensityAdjustment = distance;//Math::smoothstep(2.f, 0.f, distance);
			//separator *= intensityAdjustment;
			result += separator;	//me->r
		}

		//result /= theBois.size();
		result.Normalize();

		return result;		// * agent.maxForce... not sure wth that is though, and I need another class not
	}



	template <typename NavAgent>
	static SVec3 stick(NavAgent* me, std::vector<NavAgent*>& theBois)
	{
		if (theBois.empty())
			return SVec3::Zero;

		SVec3 centerOfMass = me->getPosition();

		for (NavAgent* boi : theBois)
		{
			centerOfMass += boi->getPosition();
		}

		centerOfMass /= (theBois.size() + 1);

		SVec3 direction = centerOfMass - me->getPosition();
		//direction *= me->maxSpeed / direction.length();	//normalized and scaled to max speed

		SVec3 force = direction;
		//SVec3 force = direction - me.velocity;
		//force *= (me.maxForce / me.maxSpeed);
		return force;
	}



	template <typename NavAgent>
	static SVec3 alignFacing()
	{

	}



	template <typename NavAgent>
	static SVec3 flock()
	{
		//stick, separate and align
	}
};