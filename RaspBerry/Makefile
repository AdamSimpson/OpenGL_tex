all: triangle tex multi_tex

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

triangle: triangle.c
	gcc $(INCLUDES) $(LDFLAGS) triangle.c -o triangle
tex: tex.c
	gcc $(INCLUDES) $(LDFLAGS) tex.c -o tex
multi_tex: multi_tex.c
	gcc $(INCLUDES) $(LDFLAGS) multi_tex.c -o multi_tex
clean:
	rm -rf *.o

