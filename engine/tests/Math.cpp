#include <engine/Math.h>
#include "TestTools.h"

using namespace glm;


InlineTest("Identical boxes overlap.", dummySignalSandbox)
{
    Box box;
    box.position = vec3(0,0,0);
    box.halfWidth = vec3(1,1,1);
    box.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(box, box) == true);

    vec3 penetration(9,9,9);
    Require(TestAabbOverlap(box, box, &penetration) == true);
    dummyLog("penetration(%f|%f|%f)",
        penetration.x,
        penetration.y,
        penetration.z);
    Require(penetration == vec3(2,0,0));
}

InlineTest("Identical point-like boxes overlap.", dummySignalSandbox)
{
    Box box;
    box.position = vec3(0,0,0);
    box.halfWidth = vec3(0,0,0);
    box.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(box, box) == true);

    vec3 penetration(9,9,9);
    Require(TestAabbOverlap(box, box, &penetration) == true);
    dummyLog("penetration(%f|%f|%f)",
        penetration.x,
        penetration.y,
        penetration.z);
    Require(penetration == vec3(0,0,0));
}

InlineTest("Separated boxes don't overlap.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0,0,0);

    Box b;
    b.position = vec3(2.1,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(a,b) == false);

    vec3 penetration(9,9,9);
    Require(TestAabbOverlap(a, b, &penetration) == false);
    Require(penetration == vec3(9,9,9));
}

InlineTest("Overlapping boxes overlap.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0,0,0);

    Box b;
    b.position = vec3(1.5,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(a,b) == true);

    vec3 penetration(9,9,9);
    Require(TestAabbOverlap(a, b, &penetration) == true);
    dummyLog("penetration(%f|%f|%f)",
        penetration.x,
        penetration.y,
        penetration.z);
    Require(penetration == vec3(-0.5f,0,0));
}

InlineTest("Resting separated boxes don't collide.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0,0,0);

    Box b;
    b.position = vec3(2.1,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(a,b) == false);

    vec3 collisionNormal(9,9,9);
    const float timeTillCollision = SweptAabb(a, b, &collisionNormal, 1.0f);
    Require(timeTillCollision == 1.0f);
}

InlineTest("Moving separated boxes can collide.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0.2,0,0);

    Box b;
    b.position = vec3(2.1,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    Require(TestAabbOverlap(a,b) == false);

    vec3 collisionNormal(9,9,9);
    const float timeTillCollision = SweptAabb(a, b, &collisionNormal, 1.0f);

    dummyLog("timeTillCollision = %f", timeTillCollision);
    Require(timeTillCollision == 0.5f);

    dummyLog("collisionNormal(%f|%f|%f)",
        collisionNormal.x,
        collisionNormal.y,
        collisionNormal.z);
    Require(collisionNormal == vec3(-1, 0, 0));
}

InlineTest("Resting overlapping boxes collide.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0,0,0);

    Box b;
    b.position = vec3(1.5,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    vec3 collisionNormal(9,9,9);
    const float timeTillCollision = SweptAabb(a, b, &collisionNormal, 1.0f);

    dummyLog("timeTillCollision = %f", timeTillCollision);
    Require(timeTillCollision == 0.0f);

    dummyLog("collisionNormal(%f|%f|%f)",
        collisionNormal.x,
        collisionNormal.y,
        collisionNormal.z);
    Require(collisionNormal == vec3(-1, 0, 0));
}

InlineTest("Moving overlapping boxes collide.", dummySignalSandbox)
{
    Box a;
    a.position = vec3(0,0,0);
    a.halfWidth = vec3(1,1,1);
    a.velocity = vec3(0.2,0,0);

    Box b;
    b.position = vec3(1.5,0,0);
    b.halfWidth = vec3(1,1,1);
    b.velocity = vec3(0,0,0);

    vec3 collisionNormal(9,9,9);
    const float timeTillCollision = SweptAabb(a, b, &collisionNormal, 1.0f);

    dummyLog("timeTillCollision = %f", timeTillCollision);
    Require(timeTillCollision == 0.0f);

    dummyLog("collisionNormal(%f|%f|%f)",
        collisionNormal.x,
        collisionNormal.y,
        collisionNormal.z);
    Require(collisionNormal == vec3(-1, 0, 0));
}

InlineTest("A ray that aims for an AABB, will intersect it.", dummySignalSandbox)
{
    Aabb aabb;
    aabb.position = vec3(0,0,0);
    aabb.halfWidth = vec3(1,1,1);

    Ray ray(
        vec3(-5,0,0),
        vec3(1,0,0));

    float distanceTillIntersection = 99;
    Require(RayTestAabb(ray, aabb, &distanceTillIntersection) == true);
    dummyLog("distanceTillIntersection = %f", distanceTillIntersection);
    Require(distanceTillIntersection == 4.0f);
}

InlineTest("A ray that starts inside an AABB, will intersect it.", dummySignalSandbox)
{
    Aabb aabb;
    aabb.position = vec3(0,0,0);
    aabb.halfWidth = vec3(1,1,1);

    Ray ray(
        vec3(0.5,0,0),
        vec3(1,0,0));

    float distanceTillIntersection = 99;
    Require(RayTestAabb(ray, aabb, &distanceTillIntersection) == true);
    dummyLog("distanceTillIntersection = %f", distanceTillIntersection);
    Require(distanceTillIntersection == -1.5f);
}

InlineTest("A ray that doesn't aims for an AABB, will not intersect it.", dummySignalSandbox)
{
    Aabb aabb;
    aabb.position = vec3(0,0,0);
    aabb.halfWidth = vec3(1,1,1);

    Ray ray(
        vec3(-5,0,0),
        vec3(1,1,0));

    float distanceTillIntersection = 99;
    Require(RayTestAabb(ray, aabb, &distanceTillIntersection) == false);
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
