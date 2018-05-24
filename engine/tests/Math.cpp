#include <math.h>
#include <float.h>
#include "../Common.h"
#include "../Math.h"
#include "TestTools.h"


const float Epsilon = FLT_EPSILON;


// Vector:

InlineTest("Vec3Length")
{
    {
        Vec3 v = {{1,0,0}};
        Require(Vec3Length(v) == 1);
    }

    {
        Vec3 v = {{-1,0,0}};
        Require(Vec3Length(v) == 1);
    }

    {
        Vec3 v = {{10,20,30}};
        Require((int)Vec3Length(v) == 37);
    }

    {
        Vec3 v = {{10,20,-30}};
        Require((int)Vec3Length(v) == 37);
    }
}

InlineTest("NormalizeVec3")
{
    {
        Vec3 v = {{1,0,0}};
        Vec3 r = NormalizeVec3(v);
        Vec3 e = {{1,0,0}};
        Require(ArraysAreEqual(r._, e._, 3));
    }

    {
        Vec3 v = {{1,2,3}};
        Vec3 r = NormalizeVec3(v);
        Require(AreNearlyEqual(Vec3Length(r), 1, Epsilon));
    }
}

InlineTest("CrossProductOfVec3")
{
    Vec3 a = {{1,0,0}};
    Vec3 b = {{0,1,0}};
    Vec3 r = CrossProductOfVec3(a, b);
    Vec3 e = {{0,0,1}};
    Require(ArraysAreEqual(r._, e._, 3));
}


// Matrix:

static Mat4 CreateMat4( float id )
{
    Mat4 m;
    REPEAT(4*4,i) { m._[i] = ((float)i + id) * 0.01f; }
    return m;
}

static void LogMat4( const char* name, Mat4 m )
{
    dummyLog("Mat4 %s =", name);
    REPEAT(4,y)
    {
        dummyLog("    %.8gf, %.8gf, %.8gf, %.8gf,",
                MAT4_AT(m,0,y),
                MAT4_AT(m,1,y),
                MAT4_AT(m,2,y),
                MAT4_AT(m,3,y));
    }
}

InlineTest("MulMat4")
{
    const Mat4 m = {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}};
    const Mat4 r = MulMat4(m, Mat4Identity);
    Require(ArraysAreEqual(r._, m._, 4*4));
}

InlineTest("ScaleMat4")
{
    Mat4 m = Mat4Identity;
    MAT4_AT(m,3,0) = 1;
    MAT4_AT(m,3,1) = 2;
    MAT4_AT(m,3,2) = 3;
    const Vec3 v = {{2,2,2}};
    const Mat4 r = ScaleMat4(m, v);

    Require(MAT4_AT(r,3,0) == 1);
    Require(MAT4_AT(r,3,1) == 2);
    Require(MAT4_AT(r,3,2) == 3);

    Require(MAT4_AT(r,0,0) == 2);
    Require(MAT4_AT(r,1,1) == 2);
    Require(MAT4_AT(r,2,2) == 2);
}

InlineTest("RotateMat4ByAngleAndAxis")
{
    Mat4 t = Mat4Identity;
    MAT4_AT(t,3,0) = 1;
    MAT4_AT(t,3,1) = 0;
    MAT4_AT(t,3,2) = 0;
    const Vec3 axis = {{0,1,0}};
    const Mat4 m = RotateMat4ByAngleAndAxis(t, PI/2, axis);
    const Vec3 v = {{10,0,0}};
    const Vec3 r = MulMat4ByVec3(m, v);

    const Vec3 e = {{1,0,10}};
    Require(ArraysAreNearlyEqual(r._, e._, 3, Epsilon*14));
}

InlineTest("TranslateMat4")
{
    Mat4 m = Mat4Identity;
    MAT4_AT(m,3,0) = 1;
    MAT4_AT(m,3,1) = 2;
    MAT4_AT(m,3,2) = 3;

    const Vec3 v = {{10,0,0}};
    const Mat4 r = TranslateMat4(m, v);

    Require(MAT4_AT(r,3,0) == 11);
    Require(MAT4_AT(r,3,1) == 2);
    Require(MAT4_AT(r,3,2) == 3);
}

InlineTest("TranslateScaleRotateMat4")
{
    const Vec3 t = {{10,0,0}};
    const Vec3 s = {{2,2,2}};
    Mat4 m = TranslateMat4(Mat4Identity, t);
         m = ScaleMat4(m, s);
    const Vec3 v = {{1,1,0}};
    const Vec3 r = MulMat4ByVec3(m, v);

    const Vec3 e = {{12,2,0}};
    Require(ArraysAreNearlyEqual(r._, e._, 3, Epsilon*2));
}

InlineTest("TransposeMat4")
{
    const Mat4 m = {{1,0,0,1,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1}};
    const Mat4 r = TransposeMat4(m);

    const Mat4 e = {{1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     1,0,0,1}};
    Require(ArraysAreNearlyEqual(r._, e._, 4*4, Epsilon));
}

InlineTest("MulMat4ByVec3")
{
    Mat4 m = Mat4Identity;
    MAT4_AT(m,3,0) = 10; // translate X by 10
    const Vec3 v = {{1,1,1}};
    const Vec3 r = MulMat4ByVec3(m, v);

    const Vec3 e = {{11,1,1}};
    Require(ArraysAreEqual(r._, e._, 3));
}

InlineTest("InverseMat4")
{
    // A matrix, multiplied by its inverse should give the identity matrix:
    //const Mat4 m = CreateMat4(1);
    //const Mat4 i = InverseMat4(m);
    //const Mat4 r = MulMat4(i, m);
    //LogMat4("i", i);
    //LogMat4("r", r);

    const Mat4 r = InverseMat4(Mat4Identity);
    Require(ArraysAreNearlyEqual(r._, Mat4Identity._, 4*4, Epsilon));
    // TODO: This is a stupid test. :/
}

InlineTest("PerspectivicProjection")
{
    const Mat4 r = PerspectivicProjection(80, 0.8f, 1, 100);

    const Mat4 e = {{-1.1188537f, 0, 0, 0,
                     0, -0.89508295f, 0, 0,
                     0, 0, 1.020202f, 1,
                     0, 0, -2.0202019f, 0}};
    Require(ArraysAreNearlyEqual(r._, e._, 4*4, Epsilon));
}

InlineTest("OrthographicProjection")
{
    const Mat4 r = OrthographicProjection(1,2,3,4,5,6);

    const Mat4 e = {{ 2,  0,    0, 0,
                      0,  2,    0, 0,
                      0,  0,   -2, 0,
                     -3, -7,  -11, 1}};
    Require(ArraysAreNearlyEqual(r._, e._, 4*4, Epsilon));
}

InlineTest("CreateLookAtMat4")
{
    const Vec3 eye    = {{0,0,0}};
    const Vec3 center = {{1,0,0}};
    const Vec3 up     = {{0,1,0}};
    const Mat4 r = CreateLookAtMat4(eye, center, up);

    const Mat4 e = {{ 0, 0, 1, 0,
                      0, 1, 0, 0,
                     -1, 0, 0, 0,
                      0, 0, 0, 1}};
    Require(ArraysAreNearlyEqual(r._, e._, 4*4, Epsilon));
}


// Quaternion:

InlineTest("QuatFromAngleAndAxis")
{
    const Vec3 axis = {{1,0,0}};
    const Quat r = QuatFromAngleAndAxis(4, axis);

    const Quat e = {{0.90929741f, 0, 0, -0.41614684f}};
    Require(ArraysAreEqual(r._, e._, 4));
}

InlineTest("NormalizeQuat")
{
    const Quat q = {{1,2,3,4}};
    const Quat r = NormalizeQuat(q);

    const Quat e = {{0.18257418f, 0.36514837f, 0.54772258f, 0.73029673f}};
    Require(ArraysAreEqual(r._, e._, 4));
}

InlineTest("QuatConjugate")
{
    const Quat q = {{1,2,3,4}};
    const Quat r = QuatConjugate(q);

    const Quat e = {{-1,-2,-3,4}};
    Require(ArraysAreEqual(r._, e._, 4));
}

InlineTest("InvertQuat")
{
    const Quat q = {{1,2,3,4}};
    const Quat r = InvertQuat(q);

    const Quat e = {{-0.033333335f, -0.06666667f, -0.1f, 0.13333334f}};
    Require(ArraysAreEqual(r._, e._, 4));
}

InlineTest("MulQuat")
{
    const Quat a = {{1,2,3,4}};
    const Quat b = {{4,3,2,1}};
    const Quat r = MulQuat(a, b);

    const Quat e = {{12, 24, 6, -12}};
    Require(ArraysAreEqual(r._, e._, 4));
}

InlineTest("MulQuatByVec3")
{
    const Quat q = {{1,2,3,4}};
    const Vec3 v = {{5,6,7}};
    const Vec3 r = MulQuatByVec3(q, v);

    const Vec3 e = {{-91, 54, 7}};
    Require(ArraysAreEqual(r._, e._, 3));
}

InlineTest("Mat4FromQuat")
{
    const Vec3 axis = {{0,1,0}};
    const Quat q = QuatFromAngleAndAxis(PI/2, axis);
    const Mat4 m = Mat4FromQuat(q);
    const Vec3 v = {{1,0,0}};
    const Vec3 r = MulMat4ByVec3(m,v);

    const Vec3 e = {{0,0,1}};
    Require(ArraysAreNearlyEqual(r._, e._, 3, Epsilon*2));
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    return RunTests();
}
