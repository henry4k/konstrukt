#ifndef __KONSTRUKT_ATTACHMENT_TARGET__
#define __KONSTRUKT_ATTACHMENT_TARGET__

#include "Math.h"
#include "PhysicsWorld.h"


enum AttachmentTargetType
{
    NO_ATTACHMENT_TARGET,
    SOLID_ATTACHMENT_TARGET
};

enum AttachmentTargetFilterFlags
{
    FILTER_POSITION = (1 << 0),
    FILTER_ROTATION = (1 << 1),
    FILTER_LINEAR_VELOCITY = (1 << 2)
};

struct AttachmentTarget
{
    int filter;
    AttachmentTargetType type;
    union
    {
        struct
        {
            PhysicsWorld* physicsWorld;
            SolidId solidId;
        } solid;
    } data;
};


void InitAttachmentTarget( AttachmentTarget* target );
void DestroyAttachmentTarget( AttachmentTarget* target );

/**
 * @param src
 * `NULL` will unset the destination target.
 */
void CopyAttachmentTarget( AttachmentTarget* dst, const AttachmentTarget* src );

/**
 * @see AttachmentTargetFilterFlags
 */
void SetAttachmentTargetFilter( AttachmentTarget* target, int filter );

void SetSolidAsAttachmentTarget( AttachmentTarget* target,
                                 PhysicsWorld* physicsWorld,
                                 SolidId solid );

void UnsetAttachmentTarget( AttachmentTarget* target );

Vec3 GetAttachmentTargetPosition( const AttachmentTarget* target );
Quat GetAttachmentTargetRotation( const AttachmentTarget* target );
Mat4 GetAttachmentTargetTransformation( const AttachmentTarget* target );
Vec3 GetAttachmentTargetLinearVelocity( const AttachmentTarget* target );

static inline bool AttachmentTargetIsSet( const AttachmentTarget* target )
{
    return target->type != NO_ATTACHMENT_TARGET;
}


#endif
