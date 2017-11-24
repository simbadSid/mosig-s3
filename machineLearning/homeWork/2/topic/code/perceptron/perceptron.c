
#include "defs.h"
void perceptron(X, Y, w, m, d, eta, T)
double **X;
double *Y;
double *w;
long int m;
long int d;
double  eta;
long int T;
{
  long int i, j, t=0;
    double ProdScal;
  // Initialisation of the weight vector
  for(j=0; j<=d; j++)
    w[j]=0.0;
  
  // While the maximum number of iterations is not reached do 
    
  while(t<T)
  {
    i=(rand()%m) + 1;

    for(ProdScal=w[0], j=1; j<=d; j++)
       ProdScal+=w[j]*X[i][j];
    
    // Learning rule of the perceptron for a misclassified example 
    if(Y[i]*ProdScal<= 0.0){
      w[0]+=eta*Y[i];
      for(j=1; j<=d; j++)
         w[j]+=eta*Y[i]*X[i][j];
    }
    t++;
  }
}
