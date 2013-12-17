all: triangle tex multi_tex

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

top_dir = $(shell pwd)

triangle: triangles/triangle.c
	mkdir -p bin
	gcc $(INCLUDES) $(LDFLAGS) triangles/triangle.c -o $(top_dir)/bin/triangle
tex: textures/tex.c
	mkdir -p bin
	gcc $(INCLUDES) $(LDFLAGS) textures/tex.c -o $(top_dir)/bin/tex
multi_tex: textures/multi_tex.c
	mkdir -p bin
	gcc $(INCLUDES) $(LDFLAGS) egl_utils.c textures/multi_tex.c -o $(top_dir)/bin/multi_tex
clean:
	rm -rf *.o
	rm -rf bin

