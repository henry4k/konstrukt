#include "Common.h"
#include "Squirrel.h"
#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;

/* TODO
Box GetSweptBroadphaseBox( Box b )
{
    Box broadphasebox;
	memset(&broadphasebox, 0, sizeof(broadphasebox));

	for(int i = 0; i < 3; ++i)
	{
		if(b.velocity[i] > 0)
		{
			broadphasebox.position[i] = b.position[i];
			broadphasebox.size[i]     = b.size[i] + b.velocity[i];
		}
		else
		{
			broadphasebox.position[i] = b.position[i] + b.velocity[i];
			broadphasebox.size[i]     = b.size[i] - b.velocity[i];
		}
	}

    broadphasebox.x = b.vx > 0 ? b.x : b.x + b.vx;
    broadphasebox.w = b.vx > 0 ? b.vx + b.w : b.w - b.vx;

    return broadphasebox;
}

bool AABBCheck(Box b1, Box b2)
{
    return !(b1.x + b1.w < b2.x || b1.x > b2.x + b2.w || b1.y + b1.h < b2.y || b1.y > b2.y + b2.h);
}
*/

float SweptAABB( Box a, Box b, vec3* normalOut, float timeFrame )
{
	Log("a.position.x = %.2f, min = %.2f, max = %.2f", a.position.x, a.position.x-a.halfWidth.x, a.position.x+a.halfWidth.x);
	Log("b.position.x = %.2f, min = %.2f, max = %.2f", b.position.x, b.position.x-b.halfWidth.x, b.position.x+b.halfWidth.x);

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

	Log("farDelta.x = %.2f", farDelta.x);
	Log("nearDelta.x = %.2f", nearDelta.x);


	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)

	const vec3 relativeVelocity = a.velocity - b.velocity;
	Log("relativeddVelocity.x = %.2f", relativeVelocity.x);

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

		Log("axisEntryTime[%d] = %.2f", i, axisEntryTime[i]);
		Log(" axisExitTime[%d] = %.2f", i, axisExitTime[i]);
	}


	// find the earliest/latest times of collision (on each axis)
	float entryTime = max(axisEntryTime.x, max(axisEntryTime.y, axisEntryTime.z));
	float exitTime  = min(axisExitTime.x, min(axisExitTime.y, axisExitTime.z));

	Log("entryTime = %.2f", entryTime);
	Log(" exitTime = %.2f", exitTime);


	// if there was no collision
	if( entryTime > exitTime ||
		(
			axisEntryTime.x < 0 &&
			axisEntryTime.y < 0 &&
			axisEntryTime.z < 0
		) ||
		(
			axisEntryTime.x > timeFrame ||
			axisEntryTime.y > timeFrame ||
			axisEntryTime.z > timeFrame
		)
	)
	{
		Log("No collision!");
		*normalOut = vec3(0,0,0);
		return timeFrame;
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

		Log("Collision!");
		return entryTime;
	}
}

void TestSweptAABB()
{
	Box a;
	a.position  = vec3(0,0,0);
	a.halfWidth = vec3(0.5,0.5,0.5);
	a.velocity  = vec3(1,0.1,0);

	Box b;
	b.position  = vec3(2,0.5,0);
	b.halfWidth = vec3(0.5,0.5,0.5);
	b.velocity  = vec3(0,0,0);

	// simulation
	const float TIME_STEP = 0.1f;
	const float BOUNCE = 1.0f;
	const float SLIDE  = 1.0f;

	for(float time = 0; time < 3; time += TIME_STEP)
	{
		Log("===== Simulating %.2f-%.2f =====", time, time+TIME_STEP);

		vec3 normal;
		float collisionTime = SweptAABB(a,b,&normal,TIME_STEP);
		float remainingTime = TIME_STEP-collisionTime;
		Log("collisionTime = %.2f remainingTime = %.2f", collisionTime, remainingTime);

		vec3 velocityChange(0,0,0);

		for(int i = 0; i < 3; ++i)
			if(abs(normal[i]) > 0.0001f)
				velocityChange[i] += BOUNCE * -a.velocity[i]*2;

		//velocityChange += SLIDE * proj(a.velocity, normal);

		Log("velocityChange = %.2f|%.2f|%.2f", velocityChange.x, velocityChange.y, velocityChange.z);

		a.velocity += velocityChange;

		a.position += a.velocity * collisionTime;
	}
}



// --- Squirrel Bindings ---

SQInteger Squirrel_CreateMatrix4( HSQUIRRELVM vm )
{
	const mat4 matrix;
	PushUserDataToSquirrel(vm, &matrix, sizeof(matrix), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(CreateMatrix4, 1, ".");

SQInteger Squirrel_AddMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	mat4* b;
	sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

	const mat4 r = *a + *b;
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(AddMatrix4, 3, ".uu");

SQInteger Squirrel_SubMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	mat4* b;
	sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

	const mat4 r = *a - *b;
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(SubMatrix4, 3, ".uu");

SQInteger Squirrel_MulMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	mat4* b;
	sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

	const mat4 r = *a * *b;
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(MulMatrix4, 3, ".uu");

SQInteger Squirrel_DivMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	mat4* b;
	sq_getuserdata(vm, 3, (SQUserPointer*)&b, NULL);

	const mat4 r = *a / *b;
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(DivMatrix4, 3, ".uu");

SQInteger Squirrel_TranslateMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	float x, y, z;
	sq_getfloat(vm, 3, &x);
	sq_getfloat(vm, 4, &x);
	sq_getfloat(vm, 5, &x);

	const vec3 t(x,y,z);

	mat4 r = translate(*a, t);
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(TranslateMatrix4, 5, ".ufff");

SQInteger Squirrel_ScaleMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	float x, y, z;
	sq_getfloat(vm, 3, &x);
	sq_getfloat(vm, 4, &x);
	sq_getfloat(vm, 5, &x);

	const vec3 s(x,y,z);

	mat4 r = scale(*a, s);
	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(ScaleMatrix4, 5, ".ufff");

SQInteger Squirrel_RotateMatrix4( HSQUIRRELVM vm )
{
	mat4* a;
	sq_getuserdata(vm, 2, (SQUserPointer*)&a, NULL);

	float x, y, z;
	sq_getfloat(vm, 3, &x);
	sq_getfloat(vm, 4, &x);
	sq_getfloat(vm, 5, &x);

	mat4 r = *a;
	r = rotate(r, x, vec3(1,0,0));
	r = rotate(r, y, vec3(0,1,0));
	r = rotate(r, z, vec3(0,0,1));

	PushUserDataToSquirrel(vm, &r, sizeof(r), NULL);
	return 1;
}
RegisterStaticFunctionInSquirrel(RotateMatrix4, 5, ".ufff");

