#ifndef __KONSTRUKT_LIGHT_WORLD__
#define __KONSTRUKT_LIGHT_WORLD__

#include "Math.h" // Vec3
#include "Shader.h" // UniformType


struct Solid;
struct ShaderVariableSet;


enum LightType
{
    /**
     * Illuminates the whole scene.
     *
     * Examples for this kind are directional or hemisphere lights.
     */
    GLOBAL_LIGHT,

    /**
     * Illuminates only a small part of the scene.
     *
     * These light source have a position and a maximum range.
     */
    POINT_LIGHT
};


/**
 * Stores light sources, which illuminate a scene.
 *
 * @see Light
 */
struct LightWorld;

/**
 * A light source.
 *
 * The engine only needs the properties, which are used to estimate the
 * effect a light source has on some model.
 *
 * @see LightType
 */
struct Light;


LightWorld* CreateLightWorld( const char* lightCountUniformName,
                              const char* lightPositionName );

void ReferenceLightWorld( LightWorld* world );
void ReleaseLightWorld( LightWorld* world );

void SetMaxActiveLightCount( LightWorld* world, int max );
void UpdateLights( LightWorld* world );
ShaderVariableSet* GetLightWorldShaderVariableSet( const LightWorld* world );
void GenerateLightShaderVariables( const LightWorld* world,
                                   ShaderVariableSet* variableSet,
                                   Vec3 objectPosition,
                                   float objectRadius );
void SetLightUniforms( LightWorld* world,
                       ShaderProgram* program,
                       Vec3 objectPosition,
                       float objectRadius );


Light* CreateLight( LightWorld* world, LightType type );

void ReferenceLight( Light* light );
void ReleaseLight( Light* light );

void SetLightAttachmentTarget( Light* light, Solid* target, int flags );
void SetLightTransformation( Light* light, Mat4 transformation );
void SetLightValue( Light* light, float value );
void SetLightRange( Light* light, float range );
void SetLightUniform( Light* light,
                      const char* name,
                      UniformType type,
                      const UniformValue* value );
void UnsetLightUniform( Light* light, const char* name );
ShaderVariableSet* GetLightShaderVariableSet( const Light* light );

#endif
