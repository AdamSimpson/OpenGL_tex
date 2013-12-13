all: opengl tex

opengl: opengl.c
	gcc -lglfw3 -lGLEW -framework OpenGL -framework Cocoa -framework IOkit opengl.c -o opengl
tex: tex.c
	gcc -lglfw3 -lGLEW -framework OpenGL -framework Cocoa -framework IOkit tex.c -o tex
clean:
	rm -rf *.o
	rm -rf *.dSYM

