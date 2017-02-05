#ifndef __KONSTRUKT_MATH__
#define __KONSTRUKT_MATH__


static const double PI = 3.14159265358979323846;
static const double TAU = PI*2.0;

enum Mat4FilterFlags
{
    COPY_ROTATION    = (1 << 0),
    COPY_TRANSLATION = (1 << 1)
};


typedef struct
{
    float _[2];
} Vec2;

typedef struct
{
    float _[3];
} Vec3;

typedef struct
{
    float _[4];
} Vec4;

typedef struct
{
    float _[3*3];
} Mat3;

typedef struct
{
    float _[4*4];
} Mat4;

typedef struct
{
    float _[4];
} Quat;


extern const Vec3 Vec3Zero;
extern const Mat3 Mat3Identity;
extern const Mat4 Mat4Identity;
extern const Quat QuatIdentity;


bool AreNearlyEqual( float a, float b, float maxDelta );

bool ArraysAreEqual( const float* a,
                     const float* b,
                     int elementCount );
bool ArraysAreNearlyEqual( const float* a,
                           const float* b,
                           int elementCount,
                           float maxDelta );


// Vec3

Vec3 Vec3FromVec4( Vec4 v );
float Vec3Length( Vec3 v );
Vec3 NormalizeVec3( Vec3 v );
float DotProductOfVec3( Vec3 a, Vec3 b );
Vec3 CrossProductOfVec3( Vec3 a, Vec3 b );


// Vec4

Vec4 Vec4FromVec3( Vec3 v );


// Mat3

Mat3 Mat3FromMat4( Mat4 m );


// Mat4

// Note:
// Be aware that matrices are stored COLUMN MAJOR here.
// This 3x3 matrix
// 11 12 13
// 21 22 23
// 31 32 33
// is stored like this:
// 11, 21, 31, 12, 22, ...

Mat4 MulMat4( Mat4 a, Mat4 b );
Mat4 ScaleMat4( Mat4 m, Vec3 v );
Mat4 RotateMat4ByAngleAndAxis( Mat4 m, float angle, Vec3 axis );
Mat4 TranslateMat4( Mat4 m, Vec3 v );
Mat4 TransposeMat4( Mat4 m );
Vec3 MulMat4ByVec3( Mat4 m, Vec3 v );
Mat4 InverseMat4( Mat4 m );
Mat4 PerspectivicProjection( float fieldOfView,
                             float aspect,
                             float near,
                             float far );
Mat4 OrthographicProjection( float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far );
Mat4 CreateLookAtMat4( Vec3 eye, Vec3 center, Vec3 up );
Mat4 ClipTranslationOfMat4( Mat4 m );
Mat4 FilterMat4( Mat4 m, int flags );


// Quat

Quat QuatFromAngleAndAxis( float angle, Vec3 axis );
Quat NormalizeQuat( Quat q );
Quat QuatConjugate( Quat q );
Quat InvertQuat( Quat q );
Quat MulQuat( Quat a, Quat b );
Vec3 MulQuatByVec3( Quat q, Vec3 v );
Mat4 Mat4FromQuat( Quat q );


#define MAT4_AT(M,X,Y) ((M)._[(X)*4+(Y)])
#define MAT3_AT(M,X,Y) ((M)._[(X)*3+(Y)])


#endif
