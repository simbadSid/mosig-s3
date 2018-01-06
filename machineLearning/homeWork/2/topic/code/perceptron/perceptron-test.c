#include "defs.h"



int main(argc, argv)
int argc;
char **argv; 
{
    long int   i, j, m, d, T,t;
    double     *Y, **X, *h, *w, Erreur, Precision, Rappel, F, PosPred, PosEffect, PosEffPred;
    char       input_filename[200], params_filename[200];

    if(argc!=3){
        printf("USAGE : Perceptron-Test Filename ParamsFile\n");
        exit(0);
    }
    FileScan(argv[1],&m,&d);
    printf("Perceptron sur une base de test contenant %ld exemples en dimension %ld\n",m,d);
    
    h = (double *)  malloc((m+1)*sizeof(double ));
    Y  = (double *)  malloc((m+1)*sizeof(double ));
    X  = (double **) malloc((m+1)*sizeof(double *));
    if(!X){
        printf("Probleme d'allocation de la matrice des données\n");
        exit(0);
    }
    X[1]=(double *)malloc((size_t)((m*d+1)*sizeof(double)));
    if(!X[1]){
        printf("Probleme d'allocation de la matrice des données\n");
        exit(0);
    }
    for(i=2; i<=m; i++)
      X[i]=X[i-1]+d;

    w  = (double *) malloc((1+d)*sizeof(double));
      
    // Database loading deined in utilitaire.c
    ChrgMatrix(argv[1], m, d, X, Y);
    load_params(argv[2],w,d);
    for(i=1; i<=m; i++)
      /*@$\rhd h_t(\mathbf{x}_i)\leftarrow w^{(t)}_0+\left\langle \boldsymbol w^{(t)},\mathbf{x}_i\right\rangle$@*/
      for(h[i]=w[0], j=1; j<=d; j++)
        h[i]+=(w[j]*X[i][j]);


    
    for(i=1,PosPred=PosEffect=PosEffPred=Erreur=0.0; i<=m; i++){
        if(Y[i]*h[i]<=0.0)
            Erreur+=1.0;
        if(Y[i]==1.0){
            PosEffect++;
            if(h[i]>0.0)
                PosEffPred++;
        }
        if(h[i]>0.0)
            PosPred++;
    }
    
    Erreur/=(double)m;
    Precision=PosEffPred/PosPred;
    Rappel=PosEffPred/PosEffect;
    F=2.0*Precision*Rappel/(Precision+Rappel);
    
    printf("Precision:%lf Recall:%lf F1-measure:%lf Errorr=%lf\n",Precision,Rappel,F,Erreur);
    return 1;
}


