#include "Common.h"
#include "Squirrel.h"
#include "Shader.h"
#include "Effects.h"


Program g_DefaultProgram = 0;

bool InitEffects()
{
    g_DefaultProgram = LoadProgram("Shaders/Default.vert", "Shaders/Default.frag");
    if(!g_DefaultProgram)
        return false;

    SetLight(
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))
    );

    return true;
}

void DestroyEffects()
{
    FreeProgram(g_DefaultProgram);
}

void BindDefaultProgram()
{
    BindProgram(g_DefaultProgram);
}

void SetLight( glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction )
{
    direction = glm::normalize(direction);

    SetUniform(g_DefaultProgram, "LightAmbient", 3, &ambient.x);
    SetUniform(g_DefaultProgram, "LightDiffuse", 3, &diffuse.x);
    SetUniform(g_DefaultProgram, "LightDirection", 3, &direction.x);
}


// --- Squirrel Bindings ---

SQInteger Squirrel_SetLight( HSQUIRRELVM vm )
{
    glm::vec3 ambient;
    sq_getfloat(vm, 2, &ambient.r);
    sq_getfloat(vm, 3, &ambient.g);
    sq_getfloat(vm, 4, &ambient.b);

    glm::vec3 diffuse;
    sq_getfloat(vm, 5, &diffuse.r);
    sq_getfloat(vm, 6, &diffuse.g);
    sq_getfloat(vm, 7, &diffuse.b);

    glm::vec3 direction;
    sq_getfloat(vm,  8, &direction.x);
    sq_getfloat(vm,  9, &direction.y);
    sq_getfloat(vm, 10, &direction.z);

    SetLight(ambient, diffuse, direction);

    return 0;
}
RegisterStaticFunctionInSquirrel(SetLight, 10, ".fffffffff");
