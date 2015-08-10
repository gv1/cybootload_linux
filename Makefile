SRC=main.c communication_api.c cybtldr_api.c cybtldr_command.c cybtldr_parse.c
OBJ=$(subst .c,.o,$(SRC))
all: cybootload_linux

%.o: %.c
	gcc -g -c $< -o $@

cybootload_linux: $(OBJ)
	gcc -g -o $@ $+

clean:
	-rm *.o cybootload_linux

StringImage.h: "Bootloadable Blinking LED.cyacd"
	perl conv.pl "$<"


gdb: cybootload_linux
	-rm -f gdb.log
	gdb -batch -x run.gdb
