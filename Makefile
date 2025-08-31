ifeq ($(TEST),)
	LIBS := "-lglfw -lcglm -lGLEW -lGL -lm"
endif

bnr:
	gcc -g -c src/fio.c -o fio.o -Iinclude $(ADDINCS)
	gcc -g -c src/main.c -o main.o -Iinclude $(ADDINCS)
	gcc -g -c src/shader.c -o shader.o -Iinclude $(ADDINCS)
	gcc -g -c src/meshing.c -o meshing.o -Iinclude $(ADDINCS)
	gcc -g -c src/chunk.c -o chunk.o -Iinclude $(ADDINCS)
	gcc -g -c src/blocks.c -o blocks.o -Iinclude $(ADDINCS)
	gcc -g -c src/gamedata.c -o gamedata.o -Iinclude $(ADDINCS)
	gcc -g -c src/camera.c -o camera.o -Iinclude $(ADDINCS)
	gcc -g main.o shader.o fio.o meshing.o chunk.o blocks.o gamedata.o camera.o $(LIBS) -o ccraft
	ccraft.exe