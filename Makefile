include /home/jake/repos/eresi/config.h

SRC 	= inject.c
CC 	= gcc
RM	= rm -f
VULN	= hello.c

OBJ64 	= $(SRC:.c=.64.o)
CFLAGS64  =$(shell eresi-config64 libelfsh --cflags)
LDFLAGS64 =$(shell eresi-config64 libelfsh --libs)
NAME 	= inject

all: $(OBJ64)
	$(CC) $(OBJ64) -o $(NAME) $(LDFLAGS64)
	$(CC) $(VULN) -o hijackme

fclean: clean
	$(RM) $(NAME) hijackme* 

clean:
	$(RM) $(OBJ64) *~

%.64.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g 
