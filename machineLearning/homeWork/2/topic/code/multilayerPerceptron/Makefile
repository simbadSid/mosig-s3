GCC = gcc
CFLAGS = -Wall -g
CPPFLAGS = -Wall -g
LIB = -lm

all: MLP-Learn MLP-Test
clean :
	rm *.o *~ MLP-Learn MLP-Test
	
MLP-Learn: MLP_BackProp.o utilitaire.o MLP-Learn.o
	$(GCC) $(CPPFLAGS) MLP_BackProp.o utilitaire.o MLP-Learn.o -o $@ $(LIB) 

MLP-Test: Propagation.o MLP-Test.o
	$(GCC) $(CPPFLAGS) Propagation.o utilitaire.o MLP-Test.o -o $@ $(LIB) 

%.o : %.c
	$(GCC) $(CPPFLAGS) -c $< -o $@


