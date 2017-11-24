/* -----------------------------------------------------------------------*
 *			       Algorithme d'Adaboost - cas bi-classes	     		  *
 * Auteur : Massih-Reza AMINI                 			                  *
 * Date   : 30 octobre 2013                                               *
 * -----------------------------------------------------------------------*/

#include "defs.h"



int main(argc, argv)
int argc;
char **argv; 
{
    long int   i, j, T,t;
    double     *h, **W, *Alpha, *H, Erreur, Precision, Rappel, F, PosPred, PosEffect, PosEffPred;
    char       input_filename[200], params_filename[200];
    DATA       TestSet;

    if(argc!=3){
        printf("USAGE : AdaBoost-Test Filename ParamsFile\n");
        exit(0);
    }
    TestSet.u=0;
    FileScan(argv[1],&TestSet.m,&TestSet.d);
    FileScan(argv[2],&T,&i);
    printf("AdaBoost on a test collection containing %ld examples in dimension %ld with %ld weak-classifiers\n",TestSet.m,TestSet.d,T);
    
    h = (double *)  malloc((TestSet.m+1)*sizeof(double ));
    H = (double *)  malloc((TestSet.m+1)*sizeof(double ));
    TestSet.y  = (double *)  malloc((TestSet.m+1)*sizeof(double ));
    TestSet.X  = (double **) malloc((TestSet.m+1)*sizeof(double *));
    if(!TestSet.X){
        printf("AdaBoost-test: Allocation problem\n");
        exit(0);
    }
    TestSet.X[1]=(double *)malloc((size_t)((TestSet.m*TestSet.d+1)*sizeof(double)));
    if(!TestSet.X[1]){
        printf("AdaBoost-test: Allocation problem\n");
        exit(0);
    }
    for(i=2; i<=TestSet.m; i++)
      TestSet.X[i]=TestSet.X[i-1]+TestSet.d;

    Alpha = (double *) malloc((1+T)*sizeof(double ));
    W  = (double **) malloc((1+T)*sizeof(double *));
    if(!W){
        printf("AdaBoost-test: Allocation problem\n");
    }
    W[1]=(double *)malloc((size_t)(((TestSet.d+1)*T)*sizeof(double)));
    if(!W[1]){
        printf("AdaBoost-test: Allocation problem\n");
        exit(0);
    }
    for(i=2; i<=T; i++)
        W[i]=W[i-1]+TestSet.d+1;
        
    // Defined in utilitaire.c
    ChrgMatrix(argv[1], TestSet);
    load_adaboost_params(argv[2],W,Alpha,T,TestSet.d);
    for(i=1; i<=TestSet.m; i++)
      H[i]=0.0;
            
    for(t=1; t<=1; t++)
       for(i=1; i<=TestSet.m; i++){
        /*@$\rhd h_t(\mathbf{x}_i)\leftarrow w^{(t)}_0+\left\langle \boldsymbol w^{(t)},\mathbf{x}_i\right\rangle$@*/
           for(h[i]=W[t][0], j=1; j<=TestSet.d; j++)
             h[i]+=(W[t][j]*TestSet.X[i][j]);

           H[i]+=Alpha[t]*h[i];

        }
    
    for(i=1,PosPred=PosEffect=PosEffPred=Erreur=0.0; i<=TestSet.m; i++){
        if(TestSet.y[i]*H[i]<=0.0)
            Erreur+=1.0;
        if(TestSet.y[i]==1.0){
            PosEffect++;
            if(H[i]>0.0)
                PosEffPred++;
        }
        if(H[i]>0.0)
            PosPred++;
    }
    
    Erreur/=(double)TestSet.m;
    Precision=PosEffPred/PosPred;
    Rappel=PosEffPred/PosEffect;
    F=2.0*Precision*Rappel/(Precision+Rappel);
    
    printf("Precision:%lf Recall:%lf F1-measure:%lf Error=%lf\n",Precision,Rappel,F,Erreur);
    return 1;
}


