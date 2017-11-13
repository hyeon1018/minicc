CC = gcc
LC = flex
YC = bison
TARGET = minic

$(TARGET) : minic.tab.c minic.yy.c
	$(CC) -o $(TARGET) minic.tab.c minic.yy.c

minic.yy.c : minic.l
	$(LC) -o minic.yy.c minic.l

minic.tab.c : minic.y
	$(YC) -d minic.y
