#include <assert.h>
#include <string.h> // memset

#include "Common.h"
#include "AttachmentTarget.h"


void InitAttachmentTarget( AttachmentTarget* target )
{
    memset(target, 0, sizeof(AttachmentTarget));
}

void DestroyAttachmentTarget( AttachmentTarget* target )
{
    UnsetAttachmentTarget(target);
}

void CopyAttachmentTarget( AttachmentTarget* dst, const AttachmentTarget* src )
{
    dst->filter = src->filter;
    if(src)
    {
        switch(src->type)
        {
            case NO_ATTACHMENT_TARGET:
                UnsetAttachmentTarget(dst);
                break;

            case SOLID_ATTACHMENT_TARGET:
                SetSolidAsAttachmentTarget(dst,
                                           src->data.solid.physicsWorld,
                                           src->data.solid.solidId);
                break;
        }
    }
    else
        UnsetAttachmentTarget(dst);
}

void SetAttachmentTargetFilter( AttachmentTarget* target, int filter )
{
    target->filter = filter;
}

void SetSolidAsAttachmentTarget( AttachmentTarget* target,
                                 PhysicsWorld* physicsWorld,
                                 SolidId solid )
{
    UnsetAttachmentTarget(target);

    target->type = SOLID_ATTACHMENT_TARGET;
    target->data.solid.physicsWorld = physicsWorld;
    target->data.solid.solidId = solid;

    ReferenceSolid(physicsWorld, solid);
}

void UnsetAttachmentTarget( AttachmentTarget* target )
{
    switch(target->type)
    {
        case SOLID_ATTACHMENT_TARGET:
        {
            PhysicsWorld* world = target->data.solid.physicsWorld;
            const SolidId solid = target->data.solid.solidId;
            ReleaseSolid(world, solid);
            break;
        }

        default:
            ;
    }

    target->type = NO_ATTACHMENT_TARGET;
}

Vec3 GetAttachmentTargetPosition( const AttachmentTarget* target )
{
    if(target->type == NO_ATTACHMENT_TARGET ||
       target->filter & FILTER_POSITION)
        return Vec3Zero;

    switch(target->type)
    {
        case SOLID_ATTACHMENT_TARGET:
        {
            PhysicsWorld* world = target->data.solid.physicsWorld;
            const SolidId solid = target->data.solid.solidId;
            return GetSolidMotionState(world, solid)->position;
        }

        default:
            FatalError("Unknown attachment target type %d.", target->type);
            return Vec3Zero;
    }
}

Quat GetAttachmentTargetRotation( const AttachmentTarget* target )
{
    if(target->type == NO_ATTACHMENT_TARGET ||
       target->filter & FILTER_ROTATION)
        return QuatIdentity;

    switch(target->type)
    {
        case SOLID_ATTACHMENT_TARGET:
        {
            PhysicsWorld* world = target->data.solid.physicsWorld;
            const SolidId solid = target->data.solid.solidId;
            return GetSolidMotionState(world, solid)->rotation;
        }

        default:
            FatalError("Unknown attachment target type %d.", target->type);
            return QuatIdentity;
    }
}

Vec3 GetAttachmentTargetLinearVelocity( const AttachmentTarget* target )
{
    if(target->type == NO_ATTACHMENT_TARGET ||
       target->filter & FILTER_LINEAR_VELOCITY)
        return Vec3Zero;

    switch(target->type)
    {
        case SOLID_ATTACHMENT_TARGET:
        {
            PhysicsWorld* world = target->data.solid.physicsWorld;
            const SolidId solid = target->data.solid.solidId;
            return GetSolidMotionState(world, solid)->linearVelocity;
        }

        default:
            FatalError("Unknown attachment target type %d.", target->type);
            return Vec3Zero;
    }
}

Mat4 GetAttachmentTargetTransformation( const AttachmentTarget* target )
{
    const Vec3 position = GetAttachmentTargetPosition(target);
    const Quat rotation = GetAttachmentTargetRotation(target);
    Mat4 r = Mat4FromQuat(rotation);
    REPEAT(3,i) { MAT4_AT(r,3,i) = position._[i]; }
    return r;
}
