#include "defs.h"

// Uniform Distribution
void DistUniforme(Dt, m)
double *Dt;
long int m;
{
   int i;
   for (i=1; i<=m; i++)
      Dt[i]=1.0/(double )m;
}

// Sampling with the reject method  
void Echantillonnage(Dt, TrainSet, Echantillon)
double   *Dt;
DATA     TrainSet, Echantillon;
{
   long int i, j, U, Nb;
   double   V, Maximum,  Majorant;
   long int *Ex;
    
    Ex=(long int *)malloc((size_t)((TrainSet.m+1)*sizeof(long int)));
   
    Ex[1]=0;
    Maximum=Dt[1];
    for(i=2; i<=TrainSet.m; i++){
        Ex[i]=0;
    
     if(Dt[i]>Maximum)
        Maximum=Dt[i];
   }
   Majorant=1.1*Maximum;
   i=1;
   while(i<=TrainSet.m)
   {
      U=(rand()%TrainSet.m)+1;
      V=Majorant*(rand()/ (double) RAND_MAX);
      if(Dt[U]>V)
      {
         Ex[U]=1;
  	     for(j=1; j<=TrainSet.d; j++)
	       Echantillon.X[i][j]=TrainSet.X[U][j];
         Echantillon.y[i]=TrainSet.y[U];
	    i++;
	  }
    }
    
    for(Nb=0,i=1; i<=TrainSet.m; i++)
        if(Ex[i]!=0)
            Nb++;
}

double Pseudo_Loss(Dt, h, Y, m, d)
double   *Dt, *h, *Y;
long int m, d;
{
   long int i;
   double Erreur;

   // /*@$\rhd~\epsilon_t\leftarrow \displaystyle{\sum_{i:f_t(\mathbf{x}_i)\neq y_i} D_t(i)}$@*/
   for(Erreur=0.0, i=1; i<=m; i++)
      if(Y[i]*h[i]<=0.0)
        Erreur+=Dt[i];
  
   return(Erreur);
}

void MiseAJourPoids(Dt, alpha, Y, h, d, m, Z1)
double   *Dt, *Y, *h, alpha;
long int d, m;
double   Z1;
{
   long int i;
   double  Z, sign;
 
   // /*@$\rhd~Z_t\leftarrow \displaystyle{\sum_{i=1}^m D_t(i)e^{-\alpha_t y_i f_t(\mathbf{x}_i)}}$@*/
   for(Z=0.0, i=1; i<=m; i++)
   {
       sign=(h[i]*Y[i]>0.0?1.0:-1.0);
       if(sign==-1.0)
           Dt[i]*=exp(-1.0*alpha*sign);
       Z+=Dt[i];
   }

    // /*@$\rhd~\forall i\in\{1,\ldots,m\}, D_{t+1}(i)\leftarrow \frac{D_t(i)e^{-\alpha_t y_i f_t(\mathbf{x}_i)}}{Z_t}$ (Eq. \ref{eq:ch5:Regle})@*/
    for(i=1; i<=m; i++)
      Dt[i]/=Z;
}


long int Boosting(TrainSet, Echantillon, Dt, W, Alpha, input_params)
DATA     TrainSet, Echantillon;
double   *Dt, **W, *Alpha;
LR_PARAM input_params;
{
    long int i, j, t, epoque=0;
    double *h, err=0.0, E, Z;
    LR_PARAM ss_input_params;
    
    srand(time(NULL));

    h = (double *)  malloc((TrainSet.m+1)*sizeof(double ));

    /*@$\rhd~\forall i\in \{1,\ldots,m\}, D_1(i)=\frac{1}{m}$@*/
   
    Echantillon.d=TrainSet.d;
    Echantillon.m=TrainSet.m;

    DistUniforme(Dt, TrainSet.m);
    Echantillonnage(Dt, TrainSet, Echantillon);
    ss_input_params.eps=1e-4;
	ss_input_params.T=5000;
	ss_input_params.display=0;
    ss_input_params.eta=0.1;
    for(t=1; t<=input_params.T && err<0.5; t++)
    {
        RegressionLogistique(W[t],Echantillon,ss_input_params);
        for(i=1; i<=TrainSet.m; i++)
       /*@$\rhd h_t(\mathbf{x}_i)\leftarrow w^{(t)}_0+\left\langle \boldsymbol w^{(t)},\mathbf{x}_i\right\rangle$@*/
        for(h[i]=W[t][0], j=1; j<=TrainSet.d; j++)
            h[i]+=(W[t][j]*TrainSet.X[i][j]);

        err= ((E=Pseudo_Loss(Dt, h, TrainSet.y, TrainSet.m, TrainSet.d))==0.0?1e-5:E);
               
        if(err<0.5){
          Alpha[t]=0.5*log((1.0-err)/err);
          Z=2.0*sqrt(err*(1.0-err));
          MiseAJourPoids(Dt, Alpha[t], TrainSet.y, h, TrainSet.d, TrainSet.m,Z);
          Echantillonnage(Dt, TrainSet, Echantillon);
          printf("Iteration=%ld - Epsilon=%lf Alpha=%lf\n",t,err,Alpha[t]);
        }
    }
    
   if(t>input_params.T)
        t--;
   else
        t-=2;
    
  free((char *) h);
  return(t);
}



