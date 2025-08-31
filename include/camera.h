#pragma once
#include <cglm/vec3.h>
#include <cglm/mat4.h>

typedef struct
{
    vec3 cameraRotation;
    vec3 cameraPosition;
    vec4 lookVector;
    vec4 walkRightVector;
    vec4 walkForwardVector;
    mat4 viewMatrix;
} Camera_t;

void CalculateCamera(Camera_t* cam, float walkSpeed);
void ApplyCamera(Camera_t* cam);