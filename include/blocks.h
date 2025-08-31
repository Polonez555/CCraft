#pragma once

typedef unsigned int Block_t;

void BlockRegister(unsigned short id, unsigned short data, const char* name);
void GenerateBlockAtlas();
void RegisterBlocks();