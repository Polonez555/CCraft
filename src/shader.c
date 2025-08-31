#include <shader.h>
#include <stdio.h>

static const char** global_uniforms;
static int global_uniform_count = 0;

Shader_t CreateShader(const char* vtxSource, const char* ftxSource)
{
    int  success;
    char infoLog[512];

    GLuint vtxShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint ftxShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vtxShader,1,&vtxSource,0);
    glShaderSource(ftxShader,1,&ftxSource,0);

    glCompileShader(vtxShader);
    glGetShaderiv(vtxShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vtxShader,512,0,infoLog);
        printf("Error compiling vertex shader\n%s\n",infoLog);
    }
    glCompileShader(ftxShader);
    glGetShaderiv(ftxShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(ftxShader,512,0,infoLog);
        printf("Error compiling fragment shader\n%s\n",infoLog);
    }

    Shader_t shad;
    shad.uniforms_are_indexed = 0;
    shad.indexed_global_uniforms = (GLuint*)malloc(sizeof(GLuint)*global_uniform_count);
    shad.shader = glCreateProgram();
    glAttachShader(shad.shader, vtxShader);
    glAttachShader(shad.shader, ftxShader);
    glLinkProgram(shad.shader);
    glGetProgramiv(shad.shader,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(shad.shader,512,0,infoLog);
        printf("Error linking shader\n%s\n",infoLog);
    }
    printf("Shader created\n");
    return shad;
}

static Shader_t* currentShader;

void UseShader(Shader_t* shader)
{
    currentShader = shader;
    if(!shader->uniforms_are_indexed)
    {
        for(int i = 0;i < global_uniform_count;i++)
        {
           shader->indexed_global_uniforms[i] = glGetUniformLocation(shader->shader,global_uniforms[i]);
        }
        shader->uniforms_are_indexed = 1;
    }
    glUseProgram(shader->shader);
}

void ShaderSetMatrix4(GLuint uniform_location, mat4 matrix)
{
    glUniformMatrix4fv(uniform_location,1,GL_FALSE,matrix[0]);
}

void ShaderSetUniform1i(GLuint uniform_location, float value)
{
    glUniform1i(uniform_location,value);
}

void SetupGlobalShaderUniforms(const char* uniforms, int count)
{
    global_uniforms = uniforms;
    global_uniform_count = count;
}

GLuint GetGlobalUniform(int index)
{
    return currentShader->indexed_global_uniforms[index];
}