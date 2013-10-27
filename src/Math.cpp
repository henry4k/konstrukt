#include "Math.h"

using namespace glm;


float SweptAABB( Box a, Box b, vec3* normalOut )
{
	//printf("a.position.x = %.2f, min = %.2f, max = %.2f\n", a.position.x, a.position.x-a.halfWidth.x, a.position.x+a.halfWidth.x);
	//printf("b.position.x = %.2f, min = %.2f, max = %.2f\n", b.position.x, b.position.x-b.halfWidth.x, b.position.x+b.halfWidth.x);

	/*
	// find the distance between the objects on the near and far sides for both x and y
		 F   NN F
		 [   ][ ]
	01234567890123456
			 <>    <- InvEntry
		 <------>  <- InvExit
	*/

	vec3 farDelta  = abs(b.position-a.position) + (b.halfWidth+a.halfWidth);
	vec3 nearDelta = abs(b.position-a.position) - (b.halfWidth+a.halfWidth);

	//printf("farDelta.x = %.2f\n", farDelta.x);
	//printf("nearDelta.x = %.2f\n", nearDelta.x);


	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)

	const vec3 relativeVelocity = a.velocity - b.velocity;
	//printf("relativeddVelocity.x = %.2f\n", relativeVelocity.x);

	vec3 axisEntryTime;
	vec3 axisExitTime;

	for(int i = 0; i < 3; ++i)
	{
		if(relativeVelocity[i] == 0)
		{
			axisEntryTime[i] = -std::numeric_limits<float>::infinity();
			axisExitTime[i]  = +std::numeric_limits<float>::infinity();
		}
		else
		{
			axisEntryTime[i] = nearDelta[i] / relativeVelocity[i];
			axisExitTime[i]  =  farDelta[i] / relativeVelocity[i];
		}

		//printf("axisEntryTime[%d] = %.2f\n", i, axisEntryTime[i]);
		//printf(" axisExitTime[%d] = %.2f\n", i, axisExitTime[i]);
	}


	// find the earliest/latest times of collision (on each axis)
	float entryTime = max(axisEntryTime.x, max(axisEntryTime.y, axisEntryTime.z));
	float exitTime  = min(axisExitTime.x, min(axisExitTime.y, axisExitTime.z));

	//printf("entryTime = %.2f\n", entryTime);
	//printf(" exitTime = %.2f\n", exitTime);


	// if there was no collision
	if( entryTime > exitTime ||
		(
			axisEntryTime.x < 0 &&
			axisEntryTime.y < 0 &&
			axisEntryTime.z < 0
		) ||
		(
			axisEntryTime.x > 1 &&
			axisEntryTime.y > 1 &&
			axisEntryTime.z > 1
		)
	)
	{
		//printf("No collision!\n");
		*normalOut = vec3(0,0,0);
		return 1;
	}
	else
	{
		// calculate normal of collided surface

		// x axis is greatest
		if(axisEntryTime.x > axisEntryTime.y && axisEntryTime.x > axisEntryTime.z)
		{
			*normalOut = vec3(-sign(farDelta.x),0,0);
		}
		// y axis is greatest
		else if(axisEntryTime.y > axisEntryTime.x && axisEntryTime.y > axisEntryTime.z)
		{
			*normalOut = vec3(0,-sign(farDelta.y),0);
		}
		// z axis is greatest
		else
		{
			*normalOut = vec3(0,0,-sign(farDelta.z));
		}

		//printf("Collision!\n");
		return entryTime;
	}
}
