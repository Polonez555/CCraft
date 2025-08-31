#include <camera.h>
#include <cglm/affine.h>
#include <gli.h>
#include <matf.h>
#include <cglm/cam.h>

mat4 lookMatrix;

void CalculateCamera(Camera_t* cam, float walkSpeed)
{
    glm_mat4_identity(lookMatrix);
    glm_rotate_y(lookMatrix,DEG_TO_RAD(cam->cameraRotation[1]),lookMatrix);
    glm_rotate_x(lookMatrix,DEG_TO_RAD(cam->cameraRotation[0]),lookMatrix);
    cam->lookVector[0] = 0;
    cam->lookVector[1] = 0;
    cam->lookVector[2] = 1;
    cam->lookVector[3] = 0;
    cam->walkRightVector[0] = walkSpeed;
    cam->walkRightVector[1] = 0;
    cam->walkRightVector[2] = 0;
    cam->walkRightVector[3] = 0;
    cam->walkForwardVector[0] = 0;
    cam->walkForwardVector[1] = 0;
    cam->walkForwardVector[2] = walkSpeed;
    cam->walkForwardVector[3] = 0;
    glm_mat4_mulv(lookMatrix,cam->lookVector,cam->lookVector);
    glm_mat4_mulv(lookMatrix,cam->walkRightVector,cam->walkRightVector);
    glm_mat4_mulv(lookMatrix,cam->walkForwardVector,cam->walkForwardVector);
}

void ApplyCamera(Camera_t* cam)
{
    glm_mat4_identity(cam->viewMatrix);
    glm_look(
        cam->cameraPosition,
        cam->lookVector,
        (vec3){0,1,0},
    cam->viewMatrix);
    ShaderSetMatrix4(GetGlobalUniform(1),cam->viewMatrix);
}