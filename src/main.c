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

ChunkRenderingInstance_t chri[9*9];
Chunk_t chunks[9*9];
Camera_t player_camera;

Block_t GetBlock(int x, int y, int z)
{
    if(y < 0 || y > 255)return 0;

    int chunkX = x/16;
    int chunkY = z/16;
    int blkX = x%16;
    int blkZ = z%16;
    int blkY = y;

    for(int i = 0;i < 9*9;i++)
    {
        Chunk_t chk = chunks[i];
        if(chunkX == chk.X && chunkY == chk.Y)
        {
            return chk.chunk_data[blkX + ((blkZ + (blkY*16))*16)];
        }
    }
    return 0;
}

void LoadAndGenChunk(Chunk_t* chk, int cx, int cy)
{
    for(int i = 0;i < 16*16*256;i++)chk->chunk_data[i] = 0;
    for(int x = 0;x < 16;x++)
        for(int z = 0;z < 16;z++)
        {
            int y = 0;
            chk->chunk_data[x + ((z + (y*16))*16)] = 1+((x+z)%2);
        }

    chk->chunk_data[5+((5 + (5*16))*16)] = 2;
    chk->X = cx;
    chk->Y = cy;
}

void ProcessChunks()
{
    int px = player_camera.cameraPosition[0];
    int pz = player_camera.cameraPosition[2];
    int playerChunkX = px / 16;
    int playerChunkZ = pz / 16;
    
    for(int i = 0;i < 9*9;i++)
    {
        chunks[i].inRectangle = (
            (chunks[i].X > (playerChunkX-4)) &&
            (chunks[i].X < (playerChunkX+4)) &&
            (chunks[i].Y > (playerChunkZ-4)) &&
            (chunks[i].Y < (playerChunkZ+4))
        );
    }

    for(int x = 0;x < 9;x++)
    for(int z = 0;z < 9;z++)
    {
        int cx = (x+playerChunkX)-4;
        int cz = (z+playerChunkZ)-4;
        for(int i = 0;i < 9*9;i++)
        {
            if(chunks[i].X == cx && chunks[i].Y == cz)goto finc;
        }
        for(int i = 0;i < 9*9;i++)
        {
            if(!chunks[i].inRectangle)
            {
                LoadAndGenChunk(&chunks[i],cx,cz);
                ChunkRenderingInstanceMesh(&chri[i]);
                chunks[i].inRectangle = 1;
                goto finc;
            }
        }
        finc:;
    }
}



void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_3D);

    RegisterBlocks();
    GenerateBlockAtlas();
    
    int i = 0;
    for(int x = 0;x < 9;x++)
    for(int z = 0;z < 9;z++)
    {
        ChunkRenderingInstanceSetUp(&chri[i]);
        chri[i].bound_chunk = &chunks[i];
        LoadAndGenChunk(&chunks[i],x,z);
        ChunkRenderingInstanceMesh(&chri[i]);
        i++;
    }

    glm_mat4_identity(projectionMatrix);
    glm_mat4_identity(modelMatrix);

    player_camera.cameraPosition[1] = 20;
}

double last_time = 0;
double last_log = 0;

vec3 playerVelocity = {0,0,0};

void update(double delta)
{
}

char bottom_hit = 0;

void render()
{
    
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UseShader(current_shader);

    double currentTime = glfwGetTime();
    double deltaTime = currentTime - last_time;
    last_time = currentTime;

    if(currentTime - last_log > 5)
    {
        printf("LOG: %f FPS\n",1/deltaTime);
        last_log = currentTime;
    }
    update(deltaTime);
    CalculateCamera(&player_camera,deltaTime*2);

    int state = glfwGetKey(wnd, GLFW_KEY_W);
    player_camera.walkForwardVector[1] = 0;
    player_camera.walkRightVector[1] = 0;
    playerVelocity[0] = 0;
    playerVelocity[2] = 0;
    if (state == GLFW_PRESS)
    {
        glm_vec3_add(playerVelocity,player_camera.walkForwardVector,playerVelocity);
    }
    state = glfwGetKey(wnd, GLFW_KEY_S);
    if (state == GLFW_PRESS)
    {
        glm_vec3_sub(playerVelocity,player_camera.walkForwardVector,playerVelocity);
    }
    state = glfwGetKey(wnd, GLFW_KEY_A);
    if (state == GLFW_PRESS)
    {
        glm_vec3_add(playerVelocity,player_camera.walkRightVector,playerVelocity);
    }
    state = glfwGetKey(wnd, GLFW_KEY_D);
    if (state == GLFW_PRESS)
    {
        glm_vec3_sub(playerVelocity,player_camera.walkRightVector,playerVelocity);
    }
    state = glfwGetKey(wnd, GLFW_KEY_SPACE);
    if (state == GLFW_PRESS && bottom_hit)
    {
        playerVelocity[1] += 0.02f;
    }

    playerVelocity[1] = min(playerVelocity[1]-(deltaTime/20),1);

    vec3 new_pos = {player_camera.cameraPosition[0],player_camera.cameraPosition[1],player_camera.cameraPosition[2]};
    glm_vec3_sub(new_pos,(vec3){0,2,0},new_pos);
    vec3 old_pos = {new_pos[0],new_pos[1],new_pos[2]};
    glm_vec3_add(new_pos,playerVelocity,new_pos);

    if(GetBlock(new_pos[0],old_pos[1],old_pos[2]) != 0)
    {
        new_pos[0] = old_pos[0];
    }
    if(GetBlock(old_pos[0],new_pos[1],old_pos[2]) != 0)
    {
        new_pos[1] = old_pos[1];
        playerVelocity[1] = 0;
        bottom_hit = 1;
    }else{
        bottom_hit = 0;
    }
    if(GetBlock(old_pos[0],old_pos[1],new_pos[2]) != 0)
    {
        new_pos[2] = old_pos[2];
    }

    glm_vec3_add(new_pos,(vec3){0,2,0},new_pos);
    player_camera.cameraPosition[0] = new_pos[0];
    player_camera.cameraPosition[1] = new_pos[1];
    player_camera.cameraPosition[2] = new_pos[2];


    ProcessChunks();
    ShaderSetMatrix4(GetGlobalUniform(0),modelMatrix);
    ShaderSetMatrix4(GetGlobalUniform(2),projectionMatrix);
    ShaderSetUniform1i(GetGlobalUniform(3),0);
    ApplyCamera(&player_camera);
     int i = 0;
    for(int x = 0;x < 9;x++)
    for(int z = 0;z < 9;z++)
    {
        ChunkRenderingInstanceRender(chri[i]);
        i++;
    }
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