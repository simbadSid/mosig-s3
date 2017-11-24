#include "defs.h"

void lire_commande(LR_PARAM *boost_input_params, char *fic_apprentissage, char *fic_params, \
                   int num_args, char **args)
{
    long int i;
    
    boost_input_params->T=100;
    
    for(i=1; (i<num_args) && (args[i][0] == '-'); i++){
        printf("%s\n",args[i]);
        switch((args[i])[1]){
            case 't': i++; sscanf(args[i],"%ld",&boost_input_params->T); break;
            case '?': i++; aide();exit(0); break;
                
            default : printf("Unknown option %s\n",args[i]);Standby();aide();exit(0);
        }
    }
    if((i+1)>=num_args){
        printf("\n ---------------------------- \n Insufficient number of parameters \n ----------------------------\n\n");
        Standby();
        aide();
        exit(0);
    }
    printf("%s %s\n",args[i],args[i+1]);
    strcpy(fic_apprentissage, args[i]);
    strcpy(fic_params, args[i+1]);
}

void Standby(){
    printf("\nHelp ... \n");
    (void)getc(stdin);
}

void aide(){
    printf("\nAdaBoost for binary classification\n");
    printf("\nAuthor: Massih R. Amini \n");
    printf("Date: 30 octobre 2013\n\n");
    printf("usage: AdaBoost-learn [options] Training-file Parameter-file\n\n");
    printf("Options:\n");
    printf("      -t             -> Maximum number of iterations (default, 100)\n");
    printf("      -?             -> This help\n");
    printf("Arguments:\n");
    printf("     Training-file  -> file containing training examples\n");
    printf("     Parameter-file -> file containing parameters\n\n");
}


int main(argc, argv)
int argc;
char **argv; 
{
    LR_PARAM input_params;
    long int   i, t;
    double     *Dt, *h, **W, *Alpha, *H;
    char input_filename[200], params_filename[200];
    DATA       TrainingSet, Echantillon;

    // Commande line
    lire_commande(&input_params,input_filename, params_filename,argc, argv);
    // File scan defined in utilitaire.c
    TrainingSet.u=Echantillon.u=0;
    FileScan(input_filename,&TrainingSet.m,&TrainingSet.d);
    printf("Training file contains %ld examples in dimension %ld\n",TrainingSet.m,TrainingSet.d);
    
    h = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
    H = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
    Dt = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
    TrainingSet.y  = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
    TrainingSet.X  = (double **) malloc((TrainingSet.m+1)*sizeof(double *));
    if(!TrainingSet.X){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    TrainingSet.X[1]=(double *)malloc((size_t)((TrainingSet.m*TrainingSet.d+1)*sizeof(double)));
    if(!TrainingSet.X[1]){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    for(i=2; i<=TrainingSet.m; i++)
      TrainingSet.X[i]=TrainingSet.X[i-1]+TrainingSet.d;
    Echantillon.y  = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
    Echantillon.X  = (double **) malloc((TrainingSet.m+1)*sizeof(double *));
    if(!Echantillon.X){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    Echantillon.X[1]=(double *)malloc((size_t)((TrainingSet.m*TrainingSet.d+1)*sizeof(double)));
    if(!Echantillon.X[1]){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    for(i=2; i<=TrainingSet.m; i++)
        Echantillon.X[i]=Echantillon.X[i-1]+TrainingSet.d;
        

    Alpha = (double *) malloc((1+input_params.T)*sizeof(double ));
    W  = (double **) malloc((1+input_params.T)*sizeof(double *));
    if(!W){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    W[1]=(double *)malloc((size_t)(((TrainingSet.d+1)*input_params.T)*sizeof(double)));
    if(!W[1]){
        printf("AdaBoost-learn: Memory allocation problem\n");
        exit(0);
    }
    for(i=2; i<=input_params.T; i++)
        W[i]=W[i-1]+TrainingSet.d+1;
        
    // File loading defined in utilitaire.c
    ChrgMatrix(input_filename, TrainingSet);

    t=Boosting(TrainingSet, Echantillon, Dt,W, Alpha,input_params);
        
    save_adaboost_params(params_filename,W,Alpha,t,TrainingSet.d);
    return 1;
}


