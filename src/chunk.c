#include <chunk.h>
#include <meshing.h>
#include <gli.h>
#include <stdio.h>

float vertex_temp[16 * 256 * 16 * 3 * 4 * 6];

void ChunkRenderingInstanceSetUp(ChunkRenderingInstance_t* chk_inst)
{
    glGenVertexArrays(1,&chk_inst->vao);
    glGenBuffers(1,&chk_inst->vbo);
}

void ChunkRenderingInstanceMesh(ChunkRenderingInstance_t* chk_inst)
{
    int vtx_count = 0;
    mesh_chunk(chk_inst->bound_chunk,vertex_temp,&vtx_count,1);
    printf("%d verices\n",vtx_count);
    chk_inst->vtx_count = vtx_count/3;
    chk_inst->elem_count = vtx_count/(3*4);

    glBindVertexArray(chk_inst->vao);
    glBindBuffer(GL_ARRAY_BUFFER,chk_inst->vbo);
    glBufferData(GL_ARRAY_BUFFER,vtx_count*sizeof(float),vertex_temp,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),0);
    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,6*sizeof(float),3*sizeof(float));
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,6*sizeof(float),4*sizeof(float));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void ChunkRenderingInstanceRender(ChunkRenderingInstance_t chk_inst)
{
    glBindVertexArray(chk_inst.vao);
    glDrawArrays(GL_QUADS,0,chk_inst.vtx_count);
}