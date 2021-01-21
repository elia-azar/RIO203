LINK_TARGET = \
	temperature

OBJS = \
	temperature.o

REBUILDABLES = $(OBJS) $(LINK_TARGET)

all : $(LINK_TARGET)

clean: 
	rm -f $(REBUILDABLES)


$(LINK_TARGET) : $(OBJS)
	cc -g -o $@ $^ -lpthread


%.o : %.c
	cc -g  -Wall -o $@ -c $<
  
temperature.o : temperature.c