all: triangle tex

triangle: triangle.c
	gcc -lglfw3 -lGLEW -framework OpenGL -framework Cocoa -framework IOkit triangle.c -o triangle
tex: tex.c
	gcc -lglfw3 -lGLEW -framework OpenGL -framework Cocoa -framework IOkit tex.c -o tex
clean:
	rm -rf *.o
	rm -rf *.dSYM

