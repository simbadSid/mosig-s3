
#include "defs.h"
void perceptron(w, TrainSet, params)
double *w;
DATA   TrainSet;
LR_PARAM params;
{
  long int i, j, t=0;
  double ProdScal;
  // Initialisation du vecteur poids /*@$\rhd~\boldsymbol w^{(0)}\leftarrow 0$@*/
  for(j=0; j<=TrainSet.d; j++)
    w[j]=0.0;
  
  /* S'il y a moins d'exemples mal classés que le nombre toléré ou 
     qu'on a atteint le nombre maximum d'itérations on sort de la 
     boucle */
  while(t<params.T)
  {
    // Choisir aléatoirement un exemple /*@$\rhd~(\mathbf{x}_t, y_t)$@*/
    i=(rand()%TrainSet.m) + 1;

    // /*@$\rhd~h(\mathbf{x}_t)\leftarrow w_0^{(t)}+\left\langle\bar{\boldsymbol w}^{(t)},\mathbf{x}_t\right\rangle $@*/
    for(ProdScal=w[0], j=1; j<=TrainSet.d; j++)
       ProdScal+=w[j]*TrainSet.X[i][j];
    
    // Si l'exemple tiré est mal classé /*@$\rhd~y_t\times h(\mathbf{x}_t)\leq 0 $@*/
    if(TrainSet.y[i]*ProdScal<= 0.0){
      // Règle de mise à jour du Perceptron pour un exemple mal classé /*@$\rhd$~Eq. \eqref{ch4:eq:regle2MiseAJourPerceptron}@*/
      w[0]+=params.eta*TrainSet.y[i];
      for(j=1; j<=TrainSet.d; j++)
         w[j]+=params.eta*TrainSet.y[i]*TrainSet.X[i][j];
    }
    t++;
  }
}
