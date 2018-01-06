#include "defs.h"



int main(argc, argv)
int argc;
char **argv; 
{
    long int   i, j;
    double     *h, *w, Erreur, Precision, Rappel, F, PosPred, PosEffect, PosEffPred;
    DATA       TestSet;

    if(argc!=3){
        printf("USAGE : LogisticRegression-Test Filename ParamsFile\n");
        exit(0);
    }
    TestSet.u=0;
    FileScan(argv[1],&TestSet.m,&TestSet.d);
    printf("Prediction of the logistic regression model on the test set containing %ld examples in dimension %ld\n",TestSet.m,TestSet.d);
    
    h = (double *)  malloc((TestSet.m+1)*sizeof(double ));
    TestSet.y  = (double *)  malloc((TestSet.m+1)*sizeof(double ));
    TestSet.X  = (double **) malloc((TestSet.m+1)*sizeof(double *));
    if(!TestSet.X){
        printf("Probleme d'allocation de la matrice des données\n");
        exit(0);
    }
    TestSet.X[1]=(double *)malloc((size_t)((TestSet.m*TestSet.d+1)*sizeof(double)));
    if(!TestSet.X[1]){
        printf("Probleme d'allocation de la matrice des données\n");
        exit(0);
    }
    for(i=2; i<=TestSet.m; i++)
      TestSet.X[i]=TestSet.X[i-1]+TestSet.d;

    w  = (double *) malloc((1+TestSet.d)*sizeof(double ));
      
    // Chargement de la matrice des données, procédure définie dans utilitaire.c
    ChrgMatrix(argv[1], TestSet);
    load_params(argv[2],w,TestSet.d);
    for(i=1; i<=TestSet.m; i++)
      /*@$\rhd h_t(\mathbf{x}_i)\leftarrow w^{(t)}_0+\left\langle \boldsymbol w^{(t)},\mathbf{x}_i\right\rangle$@*/
      for(h[i]=w[0], j=1; j<=TestSet.d; j++)
        h[i]+=(w[j]*TestSet.X[i][j]);


    
    for(i=1,PosPred=PosEffect=PosEffPred=Erreur=0.0; i<=TestSet.m; i++){
        if(TestSet.y[i]*h[i]<=0.0)
            Erreur+=1.0;
        if(TestSet.y[i]==1.0){
            PosEffect++;
            if(h[i]>0.0)
                PosEffPred++;
        }
        if(h[i]>0.0)
            PosPred++;
    }
    
    Erreur/=(double)TestSet.m;
    Precision=PosEffPred/PosPred;
    Rappel=PosEffPred/PosEffect;
    F=2.0*Precision*Rappel/(Precision+Rappel);
    
    printf("Precision:%lf Rappel:%lf mesure-F:%lf Erreur=%lf\n",Precision,Rappel,F,Erreur);
    return 1;
}


