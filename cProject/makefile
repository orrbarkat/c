CC = gcc
CPP = g++
# put your object files here
OBJS = main.o SPImageProc.o SPPoint.o SPConfig.o SPLogger.o SPBPriorityQueue.o SPList.o SPListElement.o SPExtractor.o KDTree.o KDArray.o
# The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
#/usr/local/Cellar/opencv3/3.1.0_3/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
#/usr/local/Cellar/opencv3/3.1.0_3/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_flann -lopencv_cudaarithm

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPList.h SPListElement.h SPExtractor.h KDTree.h KDArray.h SPBPriorityQueue.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
# a rule for building a simple c souorce file
# use gcc -MM SPPoint.c to see the dependencies

SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPBPriorityQueue.o: SPBPriorityQueue.c SPList.h SPListElement.h SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPExtractor.o: SPExtractor.c SPExtractor.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDArray.o: KDArray.c SPLogger.h SPPoint.h SPConfig.h KDArray.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPKDTree.o: KDTree.c KDArray.h SPBPriorityQueue.h SPLogger.h SPConfig.h SPPoint.h KDTree.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)