CC = gcc
LC = flex
YC = bison
TARGET = minic

$(TARGET) : minic.tab.c minic.yy.c ast.o
	$(CC) -o $(TARGET) minic.tab.c minic.yy.c ast.o -lfl

minic.yy.c : minic.l
	$(LC) -o minic.yy.c minic.l

minic.tab.c : minic.y
	$(YC) -d minic.y

ast.o : ast.c
	$(CC) -c ast.c
clean :
	rm *.o minic.tab.h minic.tab.c minic.yy.c minic.exe
