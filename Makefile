SRC=main.c communication_api.c cybtldr_api.c cybtldr_command.c cybtldr_parse.c
OBJ=$(subst .c,.o,$(SRC))
all: cybootload_linux

%.o: %.c
	gcc -Wall -g -c $< -o $@

cybootload_linux: $(OBJ)
	gcc -Wall -g -o $@ $+

test:
	./cybootload_linux "Bootloadable Blinking LED.cyacd"
test1:
	./cybootload_linux CY8CKIT-049-41XX_GPIO_Example.cyacd
test2:
	./cybootload_linux CY8CKIT-049-41XX_PWM_Example.cyacd

test3:
	./cybootload_linux CY8CKIT-049-41XX_UART_Example.cyacd
clean:
	-rm *.o cybootload_linux

StringImage.h: "Bootloadable Blinking LED.cyacd"
	perl conv.pl "$<"


gdb: cybootload_linux
	-rm -f gdb.log
	gdb -batch -x run.gdb
