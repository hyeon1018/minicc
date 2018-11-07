CC = gcc
LC = flex
YC = bison

ifeq ($(OS), Windows_NT)
	TARGET = minic.exe
else
	TARGET = minic
endif

$(TARGET) : minic.tab.c minic.yy.c ast.o ucodegen.o
	$(CC) -o $(TARGET) minic.tab.c minic.yy.c ast.o ucodegen.o -lfl

minic.yy.c : minic.l
	$(LC) -o minic.yy.c minic.l

minic.tab.c : minic.y
	$(YC) -d minic.y

ast.o : ast.c
	$(CC) -c ast.c

ucodegen.o : ucodegen.c
	$(CC) -c ucodegen.c

clean :
	@rm -f *.o minic.tab.h minic.tab.c minic.yy.c minic.exe minic
