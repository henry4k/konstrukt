#include <assert.h>
#include <string.h> // memset
#include <math.h> //
#include <float.h> // FLT_EPSILON
#include "Common.h"
#include "Math.h"


static const float MinPrecision = FLT_EPSILON*2;


const Vec3 Vec3Zero = {{0,0,0}};

const Mat3 Mat3Identity = {{1,0,0,
                            0,1,0,
                            0,0,1}};

const Mat4 Mat4Identity = {{1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1}};

const Quat QuatIdentity = {{0,0,0,1}};


bool AreNearlyEqual( float a, float b, float maxDelta )
{
    return fabs(a-b) <= maxDelta;
}


// Generic array stuff:

bool ArraysAreEqual( const float* a,
                     const float* b,
                     int elementCount )
{
    for(int i = 0; i < elementCount; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

bool ArraysAreNearlyEqual( const float* a,
                           const float* b,
                           int elementCount,
                           float maxDelta )
{
    float delta = 0;
    for(int i = 0; i < elementCount; i++)
    {
        delta += fabs(a[i]-b[i]);
        if(delta >= maxDelta)
            return false;
    }
    return true;
}


// Vectors:

Vec3 Vec3FromVec4( Vec4 v )
{
    const Vec3 r = {{v._[0], v._[1], v._[2]}};
    return r;
}

Vec4 Vec4FromVec3( Vec3 v )
{
    const Vec4 r = {{v._[0], v._[1], v._[2], 1}};
    return r;
}

static float VecInverseLength( const float* v, int n )
{
    float r = 0;
    REPEAT(n,i) { r += v[i] * v[i]; }
    return r;
}

static float VecLength( const float* v, int n )
{
    return sqrtf(VecInverseLength(v, n));
}

float Vec3Length( Vec3 v )
{
    return VecLength(v._, 3);
}

static void NormalizeVec( const float* in, float* out, int n )
{
    const float k = 1 / VecLength(in, n);
    REPEAT(n,i) { out[i] = in[i] * k; }
}

Vec3 NormalizeVec3( Vec3 v )
{
    Vec3 r;
    NormalizeVec(v._, r._, 3);
    return r;
}

static bool IsNormalizedVec( const float* v, int n )
{
    return AreNearlyEqual(VecLength(v, n), 1, MinPrecision);
}

float DotProductOfVec( const float* a, const float* b, int n )
{
    float r = 0;
    REPEAT(n,i) { r += a[i]*b[i]; }
    return r;
}

float DotProductOfVec3( Vec3 a, Vec3 b )
{
    return DotProductOfVec(a._, b._, 3);
}

static void CrossProduct( const float* a, const float* b, float* r )
{
    r[0] = a[1]*b[2] - a[2]*b[1];
    r[1] = a[2]*b[0] - a[0]*b[2];
    r[2] = a[0]*b[1] - a[1]*b[0];
}

Vec3 CrossProductOfVec3( Vec3 a, Vec3 b )
{
    Vec3 r;
    CrossProduct(a._, b._, r._);
    return r;
}


// Matrices:

Mat3 Mat3FromMat4( Mat4 m )
{
    Mat3 r;
    REPEAT(3,y)
    REPEAT(3,x)
        MAT3_AT(r,x,y) = MAT4_AT(m,x,y);
    return r;
}

Mat4 MulMat4( Mat4 a, Mat4 b )
{
    Mat4 r;
    REPEAT(4,x)
    REPEAT(4,y)
    {
        MAT4_AT(r,x,y) = 0;
        REPEAT(4,i)
        {
            //MAT4_AT(r,x,y) += MAT4_AT(a,x,i) * MAT4_AT(b,i,y);
            MAT4_AT(r,x,y) += MAT4_AT(a,i,y) * MAT4_AT(b,x,i);
        }
    }

    return r;
}

Mat4 ScaleMat4( Mat4 m, Vec3 v )
{
    Mat4 s = Mat4Identity;
    REPEAT(3,i) { MAT4_AT(s,i,i) = v._[i]; }
    return MulMat4(m, s);
}

Mat4 RotateMat4ByAngleAndAxis( Mat4 m, float angle, Vec3 axis )
{
    const Mat4 r = Mat4FromQuat(QuatFromAngleAndAxis(angle, axis));
    return MulMat4(m, r);
}

Mat4 TranslateMat4( Mat4 m, Vec3 v )
{
    Mat4 t = Mat4Identity;
    REPEAT(3,i) { MAT4_AT(t,3,i) = v._[i]; }
    return MulMat4(m, t);
}

Mat4 TransposeMat4( Mat4 m )
{
    Mat4 r;
    REPEAT(4,x)
    REPEAT(4,y)
        MAT4_AT(r,x,y) = MAT4_AT(m,y,x);
    return r;
}

Vec3 MulMat4ByVec3( Mat4 m, Vec3 v )
{
    float v4[4] = {v._[0], v._[1], v._[2], 1};
    float r4[4];

    // r(0) = m(0,0)*v(0) + m(1,0)*v(1)
    // r(1) = m(0,1)*v(0) + m(1,1)*v(1)
    //
    // r(y) = m(x,y)*v(x) + ...

    REPEAT(4,y)
    {
        r4[y] = 0;
        REPEAT(4,x) { r4[y] += MAT4_AT(m,x,y) * v4[x]; }
    }

    const Vec3 r = {{r4[0] / r4[3],
                     r4[1] / r4[3],
                     r4[2] / r4[3]}};
    return r;
}

// Kudos to Luke Benstead (Kazade) for this, as I shamelessly copied it from
// him. See https://github.com/Kazade/kazmath/blob/master/kazmath/mat4.c#L70
Mat4 InverseMat4( Mat4 m_ )
{
    const float* m = m_._;
    float tmp[4*4];

    tmp[0] = m[ 5] * m[10] * m[15] -
             m[ 5] * m[11] * m[14] -
             m[ 9] * m[ 6] * m[15] +
             m[ 9] * m[ 7] * m[14] +
             m[13] * m[ 6] * m[11] -
             m[13] * m[ 7] * m[10];

    tmp[4] = -m[ 4] * m[10] * m[15] +
              m[ 4] * m[11] * m[14] +
              m[ 8] * m[ 6] * m[15] -
              m[ 8] * m[ 7] * m[14] -
              m[12] * m[ 6] * m[11] +
              m[12] * m[ 7] * m[10];

    tmp[8] = m[ 4] * m[ 9] * m[15] -
             m[ 4] * m[11] * m[13] -
             m[ 8] * m[ 5] * m[15] +
             m[ 8] * m[ 7] * m[13] +
             m[12] * m[ 5] * m[11] -
             m[12] * m[ 7] * m[ 9];

    tmp[12] = -m[ 4] * m[ 9] * m[14] +
               m[ 4] * m[10] * m[13] +
               m[ 8] * m[ 5] * m[14] -
               m[ 8] * m[ 6] * m[13] -
               m[12] * m[ 5] * m[10] +
               m[12] * m[ 6] * m[ 9];

    tmp[1] = -m[ 1] * m[10] * m[15] +
              m[ 1] * m[11] * m[14] +
              m[ 9] * m[ 2] * m[15] -
              m[ 9] * m[ 3] * m[14] -
              m[13] * m[ 2] * m[11] +
              m[13] * m[ 3] * m[10];

    tmp[5] = m[ 0] * m[10] * m[15] -
             m[ 0] * m[11] * m[14] -
             m[ 8] * m[ 2] * m[15] +
             m[ 8] * m[ 3] * m[14] +
             m[12] * m[ 2] * m[11] -
             m[12] * m[ 3] * m[10];

    tmp[9] = -m[ 0] * m[ 9] * m[15] +
              m[ 0] * m[11] * m[13] +
              m[ 8] * m[ 1] * m[15] -
              m[ 8] * m[ 3] * m[13] -
              m[12] * m[ 1] * m[11] +
              m[12] * m[ 3] * m[ 9];

    tmp[13] = m[ 0] * m[ 9] * m[14] -
              m[ 0] * m[10] * m[13] -
              m[ 8] * m[ 1] * m[14] +
              m[ 8] * m[ 2] * m[13] +
              m[12] * m[ 1] * m[10] -
              m[12] * m[ 2] * m[ 9];

    tmp[2] = m[ 1] * m[6] * m[15] -
             m[ 1] * m[7] * m[14] -
             m[ 5] * m[2] * m[15] +
             m[ 5] * m[3] * m[14] +
             m[13] * m[2] * m[ 7] -
             m[13] * m[3] * m[ 6];

    tmp[6] = -m[ 0] * m[6] * m[15] +
              m[ 0] * m[7] * m[14] +
              m[ 4] * m[2] * m[15] -
              m[ 4] * m[3] * m[14] -
              m[12] * m[2] * m[ 7] +
              m[12] * m[3] * m[ 6];

    tmp[10] = m[ 0] * m[5] * m[15] -
              m[ 0] * m[7] * m[13] -
              m[ 4] * m[1] * m[15] +
              m[ 4] * m[3] * m[13] +
              m[12] * m[1] * m[ 7] -
              m[12] * m[3] * m[ 5];

    tmp[14] = -m[ 0] * m[5] * m[14] +
               m[ 0] * m[6] * m[13] +
               m[ 4] * m[1] * m[14] -
               m[ 4] * m[2] * m[13] -
               m[12] * m[1] * m[ 6] +
               m[12] * m[2] * m[ 5];

    tmp[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[ 7] +
              m[9] * m[3] * m[ 6];

    tmp[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[ 7] -
             m[8] * m[3] * m[ 6];

    tmp[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[ 9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[ 9] -
               m[8] * m[1] * m[ 7] +
               m[8] * m[3] * m[ 5];

    tmp[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[ 9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[ 9] +
              m[8] * m[1] * m[ 6] -
              m[8] * m[2] * m[ 5];

    const float inverseDeterminant = 1 / (m[0] * tmp[0] +
                                          m[1] * tmp[4] +
                                          m[2] * tmp[8] +
                                          m[3] * tmp[12]);

    Mat4 r;
    REPEAT(4*4,i) { r._[i] = tmp[i] * inverseDeterminant; }
    return r;
}

/*
Mat4 InverseMat4( Mat4 m )
{
#define AT MAT4_AT
    float s[6];
    float c[6];
    s[0] = AT(m,0,0)*AT(m,1,1) - AT(m,1,0)*AT(m,0,1);
    s[1] = AT(m,0,0)*AT(m,1,2) - AT(m,1,0)*AT(m,0,2);
    s[2] = AT(m,0,0)*AT(m,1,3) - AT(m,1,0)*AT(m,0,3);
    s[3] = AT(m,0,1)*AT(m,1,2) - AT(m,1,1)*AT(m,0,2);
    s[4] = AT(m,0,1)*AT(m,1,3) - AT(m,1,1)*AT(m,0,3);
    s[5] = AT(m,0,2)*AT(m,1,3) - AT(m,1,2)*AT(m,0,3);

    c[0] = AT(m,2,0)*AT(m,3,1) - AT(m,3,0)*AT(m,2,1);
    c[1] = AT(m,2,0)*AT(m,3,2) - AT(m,3,0)*AT(m,2,2);
    c[2] = AT(m,2,0)*AT(m,3,3) - AT(m,3,0)*AT(m,2,3);
    c[3] = AT(m,2,1)*AT(m,3,2) - AT(m,3,1)*AT(m,2,2);
    c[4] = AT(m,2,1)*AT(m,3,3) - AT(m,3,1)*AT(m,2,3);
    c[5] = AT(m,2,2)*AT(m,3,3) - AT(m,3,2)*AT(m,2,3);

    const float inverseDeterminant = 1 / (s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0]);

    Mat4 r;
    AT(r,0,0) = ( AT(m,1,1) * c[5] - AT(m,1,2) * c[4] + AT(m,1,3) * c[3]) * inverseDeterminant;
    AT(r,0,1) = (-AT(m,0,1) * c[5] + AT(m,0,2) * c[4] - AT(m,0,3) * c[3]) * inverseDeterminant;
    AT(r,0,2) = ( AT(m,3,1) * s[5] - AT(m,3,2) * s[4] + AT(m,3,3) * s[3]) * inverseDeterminant;
    AT(r,0,3) = (-AT(m,2,1) * s[5] + AT(m,2,2) * s[4] - AT(m,2,3) * s[3]) * inverseDeterminant;

    AT(r,1,0) = (-AT(m,1,0) * c[5] + AT(m,1,2) * c[2] - AT(m,1,3) * c[1]) * inverseDeterminant;
    AT(r,1,1) = ( AT(m,0,0) * c[5] - AT(m,0,2) * c[2] + AT(m,0,3) * c[1]) * inverseDeterminant;
    AT(r,1,2) = (-AT(m,3,0) * s[5] + AT(m,3,2) * s[2] - AT(m,3,3) * s[1]) * inverseDeterminant;
    AT(r,1,3) = ( AT(m,2,0) * s[5] - AT(m,2,2) * s[2] + AT(m,2,3) * s[1]) * inverseDeterminant;

    AT(r,2,0) = ( AT(m,1,0) * c[4] - AT(m,1,1) * c[2] + AT(m,1,3) * c[0]) * inverseDeterminant;
    AT(r,2,1) = (-AT(m,0,0) * c[4] + AT(m,0,1) * c[2] - AT(m,0,3) * c[0]) * inverseDeterminant;
    AT(r,2,2) = ( AT(m,3,0) * s[4] - AT(m,3,1) * s[2] + AT(m,3,3) * s[0]) * inverseDeterminant;
    AT(r,2,3) = (-AT(m,2,0) * s[4] + AT(m,2,1) * s[2] - AT(m,2,3) * s[0]) * inverseDeterminant;

    AT(r,3,0) = (-AT(m,1,0) * c[3] + AT(m,1,1) * c[1] - AT(m,1,2) * c[0]) * inverseDeterminant;
    AT(r,3,1) = ( AT(m,0,0) * c[3] - AT(m,0,1) * c[1] + AT(m,0,2) * c[0]) * inverseDeterminant;
    AT(r,3,2) = (-AT(m,3,0) * s[3] + AT(m,3,1) * s[1] - AT(m,3,2) * s[0]) * inverseDeterminant;
    AT(r,3,3) = ( AT(m,2,0) * s[3] - AT(m,2,1) * s[1] + AT(m,2,2) * s[0]) * inverseDeterminant;
    return r;
#undef AT
}
*/

/*
Mat4 InverseMat4( Mat4 m )
{
#define AT MAT4_AT
    const float coef00 = AT(m,2,2) * AT(m,3,3) - AT(m,3,2) * AT(m,2,3);
    const float coef02 = AT(m,1,2) * AT(m,3,3) - AT(m,3,2) * AT(m,1,3);
    const float coef03 = AT(m,1,2) * AT(m,2,3) - AT(m,2,2) * AT(m,1,3);

    const float coef04 = AT(m,2,1) * AT(m,3,3) - AT(m,3,1) * AT(m,2,3);
    const float coef06 = AT(m,1,1) * AT(m,3,3) - AT(m,3,1) * AT(m,1,3);
    const float coef07 = AT(m,1,1) * AT(m,2,3) - AT(m,2,1) * AT(m,1,3);

    const float coef08 = AT(m,2,1) * AT(m,3,2) - AT(m,3,1) * AT(m,2,2);
    const float coef10 = AT(m,1,1) * AT(m,3,2) - AT(m,3,1) * AT(m,1,2);
    const float coef11 = AT(m,1,1) * AT(m,2,2) - AT(m,2,1) * AT(m,1,2);

    const float coef12 = AT(m,2,0) * AT(m,3,3) - AT(m,3,0) * AT(m,2,3);
    const float coef14 = AT(m,1,0) * AT(m,3,3) - AT(m,3,0) * AT(m,1,3);
    const float coef15 = AT(m,1,0) * AT(m,2,3) - AT(m,2,0) * AT(m,1,3);

    const float coef16 = AT(m,2,0) * AT(m,3,2) - AT(m,3,0) * AT(m,2,2);
    const float coef18 = AT(m,1,0) * AT(m,3,2) - AT(m,3,0) * AT(m,1,2);
    const float coef19 = AT(m,1,0) * AT(m,2,2) - AT(m,2,0) * AT(m,1,2);

    const float coef20 = AT(m,2,0) * AT(m,3,1) - AT(m,3,0) * AT(m,2,1);
    const float coef22 = AT(m,1,0) * AT(m,3,1) - AT(m,3,0) * AT(m,1,1);
    const float coef23 = AT(m,1,0) * AT(m,2,1) - AT(m,2,0) * AT(m,1,1);

    const float fac0[] = {coef00, coef00, coef02, coef03};
    const float fac1[] = {coef04, coef04, coef06, coef07};
    const float fac2[] = {coef08, coef08, coef10, coef11};
    const float fac3[] = {coef12, coef12, coef14, coef15};
    const float fac4[] = {coef16, coef16, coef18, coef19};
    const float fac5[] = {coef20, coef20, coef22, coef23};

    const float vec0[] = {AT(m,1,0), AT(m,0,0), AT(m,0,0), AT(m,0,0)};
    const float vec1[] = {AT(m,1,1), AT(m,0,1), AT(m,0,1), AT(m,0,1)};
    const float vec2[] = {AT(m,1,2), AT(m,0,2), AT(m,0,2), AT(m,0,2)};
    const float vec3[] = {AT(m,1,3), AT(m,0,3), AT(m,0,3), AT(m,0,3)};

    float inv0[4];
    float inv1[4];
    float inv2[4];
    float inv3[4];
    REPEAT(4,i) { inv0[i] = vec1[i]*fac0[i] - vec2[i]*fac1[i] + vec3[i]*fac2[i]; }
    REPEAT(4,i) { inv1[i] = vec0[i]*fac0[i] - vec2[i]*fac3[i] + vec3[i]*fac4[i]; }
    REPEAT(4,i) { inv2[i] = vec0[i]*fac1[i] - vec1[i]*fac3[i] + vec3[i]*fac5[i]; }
    REPEAT(4,i) { inv3[i] = vec0[i]*fac2[i] - vec1[i]*fac4[i] + vec2[i]*fac5[i]; }

    const Mat4 inverse = {{ inv0[0], -inv0[1],  inv0[2], -inv0[3],
                           -inv1[0],  inv1[1], -inv1[2],  inv1[3],
                            inv2[0], -inv2[1],  inv2[2], -inv2[3],
                           -inv3[0],  inv3[1], -inv3[2],  inv3[3]}};
    const float row0[] = {AT(inverse,0,0),
                          AT(inverse,1,0),
                          AT(inverse,2,0),
                          AT(inverse,3,0)};
    float dot0[4];
    REPEAT(4,i) { dot0[i] = m._[0] * row0[i]; }

    const float dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);

    const float inverseDeterminant = 1 / dot1;

    Mat4 r;
    REPEAT(4*4,i) { r._[i] = inverse._[i] * inverseDeterminant; }
    return r;
#undef AT
}
*/

static float cotan( float v )
{
    return 1 / tanf(v);
}

Mat4 PerspectivicProjection( float fieldOfView,
                             float aspect,
                             float near,
                             float far )
{
    const float f = cotan(fieldOfView/2);
    Mat4 r;
    memset(&r, 0, sizeof(r));
    MAT4_AT(r,0,0) = f / aspect;
    MAT4_AT(r,1,1) = f;
    MAT4_AT(r,2,2) =    (far+near) / (far-near);
    MAT4_AT(r,3,2) = -(2*far*near) / (far-near);
    MAT4_AT(r,2,3) = 1;
    return r;
}

Mat4 OrthographicProjection( float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far )
{
    Mat4 r;
    memset(&r, 0, sizeof(r));
    MAT4_AT(r,0,0) =  2 / (right-left  );
    MAT4_AT(r,1,1) =  2 / (  top-bottom);
    MAT4_AT(r,2,2) = -2 / (  far-near  );
    MAT4_AT(r,3,0) = -(right+left  ) / (right-left  );
    MAT4_AT(r,3,1) = -(  top+bottom) / (  top-bottom);
    MAT4_AT(r,3,2) = -(  far+near  ) / (  far-near  );
    MAT4_AT(r,3,3) = 1;
    return r;
}

Mat4 CreateLookAtMat4( Vec3 eye, Vec3 center, Vec3 up )
{
    Vec3 f;
    REPEAT(3,i) { f._[i] = center._[i] - eye._[i]; }
    f = NormalizeVec3(f);
    const Vec3 s = NormalizeVec3(CrossProductOfVec3(up, f));
    const Vec3 u = CrossProductOfVec3(f, s);

    Mat4 m = Mat4Identity;
    MAT4_AT(m,0,0) = s._[0];
    MAT4_AT(m,1,0) = s._[1];
    MAT4_AT(m,2,0) = s._[2];
    MAT4_AT(m,3,0) = -DotProductOfVec3(s, eye);

    MAT4_AT(m,0,1) = u._[0];
    MAT4_AT(m,1,1) = u._[1];
    MAT4_AT(m,2,1) = u._[2];
    MAT4_AT(m,3,1) = -DotProductOfVec3(u, eye);

    MAT4_AT(m,0,2) = f._[0];
    MAT4_AT(m,1,2) = f._[1];
    MAT4_AT(m,2,2) = f._[2];
    MAT4_AT(m,3,2) = -DotProductOfVec3(f, eye);
    return m;
}

Mat4 ClipTranslationOfMat4( Mat4 m )
{
    MAT4_AT(m,3,0) = 0;
    MAT4_AT(m,3,1) = 0;
    MAT4_AT(m,3,2) = 0;

    MAT4_AT(m,0,3) = 0;
    MAT4_AT(m,1,3) = 0;
    MAT4_AT(m,2,3) = 0;

    MAT4_AT(m,3,3) = 1;
    return m;
}


// Quaternion:

Quat QuatFromAngleAndAxis( float angle, Vec3 axis )
{
    assert(IsNormalizedVec(axis._, 3));
    Quat r;
    const float s = sinf(angle*0.5f);
    REPEAT(3,i) { r._[i] = axis._[i] * s; }
    r._[3] = cosf(angle*0.5f);
    return r;
}

Quat NormalizeQuat( Quat q )
{
    Quat r;
    NormalizeVec(q._, r._, 4);
    return r;
}

Quat QuatConjugate( Quat q )
{
    Quat r;
    REPEAT(3,i) { r._[i] = -q._[i]; }
    r._[3] = q._[3];
    return r;
}

Quat InvertQuat( Quat q )
{
    Quat r = QuatConjugate(q);
    const float dotProduct = DotProductOfVec(q._, q._, 4);
    REPEAT(4,i) { r._[i] /= dotProduct; }
    return r;
}

Quat MulQuat( Quat a, Quat b )
{
    Quat r;
    CrossProduct(a._, b._, r._);

    REPEAT(3,i) { r._[i] += a._[i] * b._[3]; } // r.xyz += a.xyz * b.w
    REPEAT(3,i) { r._[i] += b._[i] * a._[3]; } // r.xyz += b.xyz * a.w

    const float dotProduct = DotProductOfVec(a._, b._, 3);
    r._[3] = a._[3] * b._[3] - dotProduct; // r.w = a.w * b.w - dp

    return r;
}

Vec3 MulQuatByVec3( Quat q, Vec3 v )
{
/*
 * Method by Fabian 'ryg' Giessen (of Farbrausch)
 *   t = 2 * cross(q.xyz, v)
 *   v' = v + q.w * t + cross(q.xyz, t)
 */
    float t[3];
    CrossProduct(q._, v._, t);
    REPEAT(3,i) { t[i] *= 2; }

    float u[3];
    CrossProduct(q._, t, u);

    Vec3 r;
    REPEAT(3,i) { r._[i] = v._[i] + q._[3] * t[i] + u[i]; }
    return r;
}

Mat4 Mat4FromQuat( Quat q )
{
    const float x = q._[0];
    const float y = q._[1];
    const float z = q._[2];
    const float w = q._[3];

    const float xx = x*x;
    const float xy = x*y;
    const float xz = x*z;
    const float xw = x*w;

    const float yy = y*y;
    const float yz = y*z;
    const float yw = y*w;

    const float zz = z*z;
    const float zw = z*w;

    Mat4 r;
    MAT4_AT(r,0,0) = 1 - 2 * (yy + zz);
    MAT4_AT(r,1,0) = 2 * (xy + zw);
    MAT4_AT(r,2,0) = 2 * (xz - yw);
    MAT4_AT(r,3,0) = 0;

    MAT4_AT(r,0,1) = 2 * (xy - zw);
    MAT4_AT(r,1,1) = 1 - 2 * (xx + zz);
    MAT4_AT(r,2,1) = 2 * (yz + xw);
    MAT4_AT(r,3,1) = 0;

    MAT4_AT(r,0,2) = 2 * (xz + yw);
    MAT4_AT(r,1,2) = 2 * (yz - xw);
    MAT4_AT(r,2,2) = 1 - 2 * (xx + yy);
    MAT4_AT(r,3,2) = 0;

    MAT4_AT(r,0,3) = 0;
    MAT4_AT(r,1,3) = 0;
    MAT4_AT(r,2,3) = 0;
    MAT4_AT(r,3,3) = 1;
    return r;
}
