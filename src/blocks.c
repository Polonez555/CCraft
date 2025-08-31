#include <blocks.h>
#include <stdlib.h>
#include <external/stb_image.h>
#include <gli.h>

int block_count = 0;
static char** block_names = 0;

static int* block_atlas_data;

void BlockRegister(unsigned short id, unsigned short data, const char* name)
{
    block_count++;
    block_names = (char**)realloc(block_names,block_count*sizeof(char*));
    block_names[block_count-1] = name;
}

void GenerateBlockAtlas()
{
    block_atlas_data = (int*)malloc(block_count*16*16*4);
    unsigned char* running_pointer = (unsigned char*)block_atlas_data;
    char image_path[512];
    for(int i = 0;i < block_count;i++)
    {
        int x, y, n;
        sprintf_s(image_path,512,"assets/textures/blocks/%s.png",block_names[i]);
        printf("Loading %s\n",image_path);
        unsigned char* image_data = stbi_load(image_path,&x,&y,&n,4);
        memcpy(running_pointer,image_data,16*16*4);
        running_pointer += (16*16*4);
        stbi_image_free(image_data);
    }
    GLuint block_atlas_texture;
    glGenTextures(1, &block_atlas_texture);
    glBindTexture(GL_TEXTURE_3D, block_atlas_texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8,16,16, block_count, 0, GL_RGBA, GL_UNSIGNED_BYTE, block_atlas_data);
    glGenerateMipmap(GL_TEXTURE_3D);
    free(block_atlas_data);
    glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, block_atlas_texture);
}