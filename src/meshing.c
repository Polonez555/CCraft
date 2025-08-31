#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <chunk.h>
#include <meshing.h>
#include <blocks.h>

#define CHUNK_X 16
#define CHUNK_Y 256
#define CHUNK_Z 16

typedef struct {
    float x, y, z;
    int block_id; // For texture lookup
} Vertex;

extern int block_count;

void push_vtx(float* vertices, int* vertex_count, float x, float y, float z, int block_id, float u, float v) {
    vertices[(*vertex_count)++] = x;
    vertices[(*vertex_count)++] = y;
    vertices[(*vertex_count)++] = z;
    vertices[(*vertex_count)++] = ((float)(block_id-1)) / ((float)block_count);
    vertices[(*vertex_count)++] = u;
    vertices[(*vertex_count)++] = v;
}

void push_quad(float* vertices, int* vertex_count, 
               float x1, float y1, float z1, 
               float x2, float y2, float z2, 
               int axis, bool flip, int block_id) 
{
    float du = 0.0f, dv = 0.0f;

    if(axis == 0) {       // X axis: UV spans Y-Z plane
        du = y2 - y1;
        dv = z2 - z1;
        if(!flip) {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x1, y2, z1, block_id, du, 0.0f);
            push_vtx(vertices, vertex_count, x1, y2, z2, block_id, du, dv);
            push_vtx(vertices, vertex_count, x1, y1, z2, block_id, 0.0f, dv);
        } else {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x1, y1, z2, block_id, 0.0f, dv);
            push_vtx(vertices, vertex_count, x1, y2, z2, block_id, du, dv);
            push_vtx(vertices, vertex_count, x1, y2, z1, block_id, du, 0.0f);
        }
    } 
    else if(axis == 1) {  // Y axis: UV spans X-Z plane
        du = x2 - x1;
        dv = z2 - z1;
        if(!flip) {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x2, y1, z1, block_id, du, 0.0f);
            push_vtx(vertices, vertex_count, x2, y1, z2, block_id, du, dv);
            push_vtx(vertices, vertex_count, x1, y1, z2, block_id, 0.0f, dv);
        } else {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x1, y1, z2, block_id, 0.0f, dv);
            push_vtx(vertices, vertex_count, x2, y1, z2, block_id, du, dv);
            push_vtx(vertices, vertex_count, x2, y1, z1, block_id, du, 0.0f);
        }
    } 
    else if(axis == 2) {  // Z axis: UV spans X-Y plane
        du = x2 - x1;
        dv = y2 - y1;
        if(!flip) {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x2, y1, z1, block_id, du, 0.0f);
            push_vtx(vertices, vertex_count, x2, y2, z1, block_id, du, dv);
            push_vtx(vertices, vertex_count, x1, y2, z1, block_id, 0.0f, dv);
        } else {
            push_vtx(vertices, vertex_count, x1, y1, z1, block_id, 0.0f, 0.0f);
            push_vtx(vertices, vertex_count, x1, y2, z1, block_id, 0.0f, dv);
            push_vtx(vertices, vertex_count, x2, y2, z1, block_id, du, dv);
            push_vtx(vertices, vertex_count, x2, y1, z1, block_id, du, 0.0f);
        }
    }
}


int get_block(Chunk_t* chunk, int x, int y, int z) {
    if(x >= CHUNK_X || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z || z < 0)
        return 0;
    return chunk->chunk_data[x + (z + (y * CHUNK_Z)) * CHUNK_X];
}

void mesh_chunk(Chunk_t* chunk, float* vtx_ptr, int* vtx_count, int lod) {
    int mask[CHUNK_X * CHUNK_Y]; // mask stores block IDs or 0
    int dims[3] = {CHUNK_X, CHUNK_Y, CHUNK_Z};

    for(int d = 0; d < 3; d++) {
        int u = (d + 1) % 3;
        int v = (d + 2) % 3;

        int x[3] = {0, 0, 0};
        int q[3] = {0, 0, 0};
        q[d] = 1;

        for(x[d] = -1; x[d] < dims[d]; ) {
            int n = 0;
            for(x[v] = 0; x[v] < dims[v]; x[v]++) {
                for(x[u] = 0; x[u] < dims[u]; x[u]++) {
                    int a = (x[d] >= 0) ? get_block(chunk, x[0], x[1], x[2]) : 0;
                    int b = (x[d] < dims[d]-1) ? get_block(chunk, x[0]+q[0], x[1]+q[1], x[2]+q[2]) : 0;

                    if(a && b && a == b) {
                        mask[n++] = 0; // no face if same block type
                    } else if(a) {
                        mask[n++] = a; // positive face
                    } else if(b) {
                        mask[n++] = -b; // negative face
                    } else {
                        mask[n++] = 0;
                    }
                }
            }

            x[d]++;
            if(n == 0) continue;

            // Build quads from mask
            n = 0;
            for(int j = 0; j < dims[v]; j++) {
                for(int i = 0; i < dims[u]; ) {
                    int c = mask[n];
                    if(c != 0) {
                        int block_id = abs(c);
                        int w, h;

                        for(w = 1; i + w < dims[u] && mask[n + w] == c; w++);
                        bool done = false;
                        for(h = 1; j + h < dims[v]; h++) {
                            for(int k = 0; k < w; k++) {
                                if(mask[n + k + h * dims[u]] != c) {
                                    done = true; break;
                                }
                            }
                            if(done) break;
                        }

                        for(int l = 0; l < h; l++) {
                            for(int k = 0; k < w; k++) {
                                mask[n + k + l * dims[u]] = 0;
                            }
                        }

                        x[u] = i; x[v] = j;
                        int du[3] = {0, 0, 0};
                        int dv[3] = {0, 0, 0};
                        du[u] = w; dv[v] = h;

                        push_quad(vtx_ptr, vtx_count,
                                  x[0], x[1], x[2],
                                  x[0] + du[0] + dv[0],
                                  x[1] + du[1] + dv[1],
                                  x[2] + du[2] + dv[2],
                                  d, c < 0, block_id);

                        i += w;
                        n += w;
                    } else {
                        i++; n++;
                    }
                }
            }
        }
    }
}
