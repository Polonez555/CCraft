#include <stdlib.h>
#include <stdio.h>
#include <gli.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <fio.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>
#include <cglm/cam.h>
#include <cglm/affine.h>
#include <math.h>
#include <matf.h>
#include <chunk.h>
#include <camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

int WindowW = 0;
int WindowH = 0;

Shader_t* current_shader;

mat4 projectionMatrix;
mat4 modelMatrix;

GLFWwindow* wnd;

char cursor_captured = 0;

ChunkRenderingInstance_t chri;
Chunk_t test_chunk;
Camera_t player_camera;

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_3D);

    RegisterBlocks();
    GenerateBlockAtlas();

    ChunkRenderingInstanceSetUp(&chri);
    for(int i = 0;i < 16*16*256;i++)test_chunk.chunk_data[i] = 0;
    for(int x = 0;x < 16;x++)
        for(int z = 0;z < 16;z++)
        {
            int y = 0;
            test_chunk.chunk_data[x + ((z + (y*16))*16)] = ((z+x)%2)+1;
        }
    test_chunk.chunk_data[5+((5 + (5*16))*16)] = 2;
    chri.bound_chunk = &test_chunk;
    ChunkRenderingInstanceMesh(&chri);

    glm_mat4_identity(projectionMatrix);
    glm_mat4_identity(modelMatrix);
}

void render()
{
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UseShader(current_shader);

    CalculateCamera(&player_camera,0.001f);

    int state = glfwGetKey(wnd, GLFW_KEY_W);
    if (state == GLFW_PRESS)
    {
        glm_vec3_add(player_camera.cameraPosition,player_camera.walkForwardVector,player_camera.cameraPosition);
    }
    state = glfwGetKey(wnd, GLFW_KEY_S);
    if (state == GLFW_PRESS)
    {
        glm_vec3_sub(player_camera.cameraPosition,player_camera.walkForwardVector,player_camera.cameraPosition);
    }
    state = glfwGetKey(wnd, GLFW_KEY_A);
    if (state == GLFW_PRESS)
    {
        glm_vec3_add(player_camera.cameraPosition,player_camera.walkRightVector,player_camera.cameraPosition);
    }
    state = glfwGetKey(wnd, GLFW_KEY_D);
    if (state == GLFW_PRESS)
    {
        glm_vec3_sub(player_camera.cameraPosition,player_camera.walkRightVector,player_camera.cameraPosition);
    }

    ShaderSetMatrix4(GetGlobalUniform(0),modelMatrix);
    ShaderSetMatrix4(GetGlobalUniform(2),projectionMatrix);
    ShaderSetUniform1i(GetGlobalUniform(3),0);
    ApplyCamera(&player_camera);
    ChunkRenderingInstanceRender(chri);
}

void GrabCursor(GLFWwindow* wnd)
{
    double hw = WindowW/2;
    double hh = WindowH/2;
    glfwSetInputMode(wnd,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    cursor_captured = 1;
    glfwSetCursorPos(wnd,hw,hh);
}

void ReleaseCursor(GLFWwindow* wnd)
{
    glfwSetInputMode(wnd,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    cursor_captured = 0;
}

void cursorPosCallback(GLFWwindow* wnd, double xpos, double ypos)
{
    double hw = WindowW/2;
    double hh = WindowH/2;

    double difx = xpos-hw;
    double dify = ypos-hh;

    if(cursor_captured){
        if((difx != 0) || (dify != 0))
        {
            glfwSetCursorPos(wnd,hw,hh);
            player_camera.cameraRotation[0] = min(89,max(-89,(player_camera.cameraRotation[0] + ((dify*4)/(hh)))));
            player_camera.cameraRotation[1] = fmod((player_camera.cameraRotation[1] - ((difx*4)/(hw))),360);
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        if(!cursor_captured)GrabCursor(window);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        if(cursor_captured)
        {
            ReleaseCursor(window);
        }
    }
}

void windowResizeCallback(GLFWwindow* wnd, int w, int h)
{
    WindowW = w;
    WindowH = h;
    glViewport(0,0,w,h);
    glm_mat4_identity(projectionMatrix);
    glm_perspective(DEG_TO_RAD(70.0f),((float)w)/((float)h),.1f,1000.0f,projectionMatrix);
    render();
}

void main()
{
    glfwInit();

    wnd = glfwCreateWindow(640,480,"CCraft",0,0);
    glfwMakeContextCurrent(wnd);
    glewInit();
    glfwSetFramebufferSizeCallback(wnd,windowResizeCallback);

    const char* global_uniform_definition[] = {
        "ModelMatrix",
        "ViewMatrix",
        "ProjectionMatrix",
        "blockAtlas"
    };

    SetupGlobalShaderUniforms(global_uniform_definition,sizeof(global_uniform_definition)/sizeof(const char*));

    const char* vtxSource = FileToTextZE("assets/shaders/default.vsh.glsl");
    const char* ftxSource = FileToTextZE("assets/shaders/default.fsh.glsl");

    Shader_t shader = CreateShader(vtxSource,ftxSource);
    
    free(vtxSource);
    free(ftxSource);

    current_shader = &shader;

    init();
    windowResizeCallback(wnd,640,480);
    glfwSetKeyCallback(wnd, keyCallback);
    glfwSetCursorPosCallback(wnd,cursorPosCallback);
    glfwSetMouseButtonCallback(wnd, mouseButtonCallback);

    while(!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        render();
        
        glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}