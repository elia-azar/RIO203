LINK_TARGET = \
	temperature lamp washing-machine

OBJS = \
	temperature.o lamp.o washing-machine.o

REBUILDABLES = $(OBJS) $(LINK_TARGET)

all : $(LINK_TARGET)

clean: 
	rm -f $(REBUILDABLES)


$(LINK_TARGET) : $(OBJS)
	cc -g -o $@ $^ -lpthread


%.o : %.c
	cc -g  -Wall -o $@ -c $<
  
temperature.o : temperature.c
lamp.o : lamp.c
washing-machine.o : washing-machine.c