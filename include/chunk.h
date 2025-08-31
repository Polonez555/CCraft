#pragma once
#include <blocks.h>

typedef struct 
{
    Block_t chunk_data[16*16*256];
} Chunk_t;

typedef struct
{
    int vao;
    int vbo;
    int vtx_count;
    int elem_count;
    Chunk_t* bound_chunk;
} ChunkRenderingInstance_t;

void ChunkRenderingInstanceSetUp(ChunkRenderingInstance_t* chk_inst);
void ChunkRenderingInstanceMesh(ChunkRenderingInstance_t* chk_inst);
void ChunkRenderingInstanceRender(ChunkRenderingInstance_t chk_inst);