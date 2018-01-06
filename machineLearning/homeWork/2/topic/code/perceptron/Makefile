all: perceptron-test perceptron-learn
clean :
	rm *.o *~ perceptron-test perceptron-learn

perceptron-learn: utilitaire.o perceptron.o perceptron-learn-main.o
	gcc -g utilitaire.o perceptron.o perceptron-learn-main.o -o perceptron-learn

perceptron-test: utilitaire.o perceptron-test.o
	gcc -g utilitaire.o perceptron-test.o -o perceptron-test

%.o : %.c 
	gcc -g -c $< -o $@
