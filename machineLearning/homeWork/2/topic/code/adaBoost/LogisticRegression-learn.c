#include "defs.h"

// Fonction logistique /*@ $\mathbf{x}\mapsto\frac{1}{1+e^{-\mathbf{x}}}$ @*/
double Logistic(double x)
{
   return (1.0/(1.0+exp(-x)));
}

// Calcul du vecteur gradient /*@$\rhd~\nabla \CnxEmpLoss(\boldsymbol w)\leftarrow \displaystyle{\frac{1}{m}\sum_{i=1}^m y_i\left(\frac{1}{1+e^{-y_i h_{\boldsymbol w}(\mathbf{x}_i)}}-1\right) \times \mathbf{x}_i}$ (Eq. \ref{eq:GradLogistique}) @*/
double *GradientLogisticSurrogateLoss(double *w, DATA TrainingSet) 
{
  double   ps, *g; 
  long int i, j;
  
  g=(double *)malloc((TrainingSet.d+1)*sizeof (double));
  for(j=0; j<=TrainingSet.d; j++)
    g[j]=0.0;

  for(i=1; i<=TrainingSet.m; i++){
 	 /*@$\rhd~h\leftarrow w_0+\left\langle \bar{\wgt},\obs_i\right\rangle$@*/ 
    for(ps=w[0],j=1; j<=TrainingSet.d; j++) 
       ps+=w[j]*TrainingSet.X[i][j];
     g[0]+=(Logistic(TrainingSet.y[i]*ps)-1.0)*TrainingSet.y[i];
     for(j=1; j<=TrainingSet.d; j++)
       g[j]+=(Logistic(TrainingSet.y[i]*ps)-1.0)*TrainingSet.y[i]*TrainingSet.X[i][j];
   }

  for(j=0; j<=TrainingSet.d; j++)
	g[j]/=(double ) TrainingSet.m;

  return(g);
}
// Calcul de la fonction de cout logistique /*@$\rhd~\CnxEmpLoss(\boldsymbol w)\leftarrow \displaystyle{\frac{1}{m}\sum_{i=1}^m \ln(1+e^{-y_i h_{\boldsymbol w}(\mathbf{x}_i)})}$ (Eq. \ref{eq:CoutLogistique}) @*/
double LogisticSurrogateLoss(double *w, DATA TrainingSet) 
{
  double   S=0.0, ps;
  long int i, j;

  for(i=1; i<=TrainingSet.m; i++){
 	 /*@$\rhd~h\leftarrow w_0+\left\langle \bar{\wgt},\obs_i\right\rangle$@*/ 
     for(ps=w[0],j=1; j<=TrainingSet.d; j++) 
       ps+=w[j]*TrainingSet.X[i][j];
     S+= log(1.0+exp(-TrainingSet.y[i]*ps));	
  }
  S/=(double ) TrainingSet.m;
 
  return (S);
}

void RegressionLogistique(double *w, DATA TrainingSet, LR_PARAM params)
{
   //Apprentissage des paramètres du modèle avec la méthode du gradient conjugué
   
    grdcnj(LogisticSurrogateLoss, GradientLogisticSurrogateLoss, TrainingSet, w, params.eps, params.display);

}
