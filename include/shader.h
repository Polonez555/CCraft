#pragma once

#include <gli.h>
#include <cglm/mat4.h>

typedef struct 
{
    GLuint shader;
    char uniforms_are_indexed;
    GLuint* indexed_global_uniforms;
} Shader_t;

Shader_t CreateShader(const char* vtxSource, const char* ftxSource);
void UseShader(Shader_t* shader);
void ShaderSetMatrix4(GLuint uniform_location, mat4 matrix);
void SetupGlobalShaderUniforms(const char* uniforms, int count);
void ShaderSetUniform1i(GLuint uniform_location, float value);
GLuint GetGlobalUniform(int index);