all: AdaBoost-test AdaBoost-learn
clean :
	rm *.o *~ AdaBoost-test AdaBoost-learn

AdaBoost-learn: utilitaire.o Optimiseurs.o perceptron.o LogisticRegression-learn.o AdaBoost-learn.o AdaBoost-learn-main.o
	gcc -g utilitaire.o Optimiseurs.o perceptron.o LogisticRegression-learn.o AdaBoost-learn.o AdaBoost-learn-main.o -o AdaBoost-learn -lm

AdaBoost-test: utilitaire.o AdaBoost-test.o
	gcc -g utilitaire.o AdaBoost-test.o -o AdaBoost-test

%.o : %.c 
	gcc -g -c $< -o $@
