all: LogisticRegression-test LogisticRegression-learn
clean :
	rm *.o *~ LogisticRegression-test LogisticRegression-learn

LogisticRegression-learn: utilitaire.o Optimiseurs.o LogisticRegression-learn.o LogisticRegression-learn-main.o
	gcc -g utilitaire.o Optimiseurs.o LogisticRegression-learn.o LogisticRegression-learn-main.o -o LogisticRegression-learn -lm

LogisticRegression-test: utilitaire.o LogisticRegression-test.o
	gcc -g utilitaire.o LogisticRegression-test.o -o LogisticRegression-test -lm

%.o : %.c 
	gcc -g -c $< -o $@
