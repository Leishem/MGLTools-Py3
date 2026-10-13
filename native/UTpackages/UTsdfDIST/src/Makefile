OBJS = common.o compute.o init.o main.o head.o new_adjust.o octree.o propagate.o
LIBNAME = sdfLib.a

$(LIBNAME) : $(OBJS)
	ar rv $(LIBNAME) $(OBJS)  

$(OBJS): common.h head.h sdfLib.h

common.o: common.cpp
	g++ -Wall -c common.cpp

compute.o: compute.cpp
	g++ -Wall -c compute.cpp

init.o: init.cpp
	g++ -Wall -c init.cpp

main.o: main.cpp
	g++ -Wall -c main.cpp

new_adjust.o: new_adjust.cpp
	g++ -Wall -c new_adjust.cpp

octree.o: octree.cpp
	g++ -Wall -c octree.cpp

head.o: head.cpp
	g++ -Wall -c head.cpp

propagate.o: propagate.cpp
	g++ -Wall -c propagate.cpp

.PHONY: clean

clean:
	rm -f $(OBJS)

