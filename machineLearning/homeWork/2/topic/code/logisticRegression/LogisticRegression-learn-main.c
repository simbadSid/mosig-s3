#include "defs.h"


int main(int argc, char **argv)
{
  LR_PARAM   input_params;
  long int   i,j;
  double     *w, *h, Erreur, Precision, Rappel, F, PosPred, PosEffect, PosEffPred;
  char input_filename[200], params_filename[200];
  DATA       TrainingSet;

  // Reading of the line command 
  lire_commande(&input_params,input_filename, params_filename,argc, argv);
  // Scan of the training file, procedure defined in utilitaire.c
  FileScan(input_filename,&TrainingSet.m,&TrainingSet.d);
  printf("The training set contains %ld examples in dimension %ld\n",TrainingSet.m,TrainingSet.d);

  TrainingSet.u=0;
  TrainingSet.y  = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
  TrainingSet.X  = (double **) malloc((TrainingSet.m+1)*sizeof(double *));
  if(!TrainingSet.X){
    printf("Memory allocation problem\n");
    exit(0);
  }
  TrainingSet.X[1]=(double *)malloc((size_t)((TrainingSet.m*TrainingSet.d+1)*sizeof(double)));
  if(!TrainingSet.X[1]){
    printf("Memory allocation problem\n");
    exit(0);
  }
  for(i=2; i<=TrainingSet.m; i++)
    TrainingSet.X[i]=TrainingSet.X[i-1]+TrainingSet.d;

  w  = (double *) malloc((TrainingSet.d+1) * sizeof(double ));
  srand(time(NULL));

  // Random initialization of weights
  for(j=0; j<=TrainingSet.d; j++)
     w[j]= 2.0*(rand() / (double) RAND_MAX)-1.0;

  // Loading of the training matrix, defined in utilitaire.c
  ChrgMatrix(input_filename, TrainingSet);

  // Minimization of the Logistic surrogate with the conjugate gradient algorithm
  RegressionLogistique(w, TrainingSet, input_params);
    
  h = (double *)  malloc((TrainingSet.m+1)*sizeof(double ));
  for(i=1; i<=TrainingSet.m; i++)
  /*@$\rhd h_t(\mathbf{x}_i)\leftarrow w^{(t)}_0+\left\langle \boldsymbol w^{(t)},\mathbf{x}_i\right\rangle$@*/
     for(h[i]=w[0], j=1; j<=TrainingSet.d; j++)
        h[i]+=(w[j]*TrainingSet.X[i][j]);
    
    
    
  for(i=1,PosPred=PosEffect=PosEffPred=Erreur=0.0; i<=TrainingSet.m; i++){
    if(TrainingSet.y[i]*h[i]<=0.0)
        Erreur+=1.0;
    if(TrainingSet.y[i]==1.0){
        PosEffect++;
        if(h[i]>0.0)
            PosEffPred++;
    }
    if(h[i]>0.0)
        PosPred++;
  }
    
  Erreur/=(double)TrainingSet.m;
  Precision=PosEffPred/PosPred;
  Rappel=PosEffPred/PosEffect;
  F=2.0*Precision*Rappel/(Precision+Rappel);
    
  printf("Precision:%lf Recall:%lf F1-measure:%lf Error=%lf\n",Precision,Rappel,F,Erreur);
    
    
  free((char *)h);
  // Ecriture des paramètres du poids dans le fichier params_filename, procédure définie dans utilitaire.c
  save_params(params_filename, w,TrainingSet.d);

  return 1;
}



void lire_commande(LR_PARAM *ss_input_params, char *fic_apprentissage, char *fic_params, \
                   int num_args, char **args)
{
  long int i;

  ss_input_params->eps=1e-4;
  ss_input_params->display=0;

  for(i=1; (i<num_args) && (args[i][0] == '-'); i++){
    printf("%s\n",args[i]);
    switch((args[i])[1]){
      case 'e': i++; sscanf(args[i],"%lf",&ss_input_params->eps); break;
      case 'd': i++; sscanf(args[i],"%d",&ss_input_params->display); break;
      case '?': i++; aide();exit(0); break;
      
      default : printf("Unknown option %s\n",args[i]);Standby();aide();exit(0);
    }
  }
  if((i+1)>=num_args){
    printf("\n ---------------------------- \n Number of input parameters insufficient \n ----------------------------\n\n");
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
  printf("\nLogistic Regression with the conjugate gradient technique\n");
  printf("\nAuthor: Massih R. Amini \n");
  printf("Date: 25 october 2013\n\n");
  printf("usage: LogisticRegression_Train [options] training_file parameter_file\n\n");
  printf("Options:\n");
  printf("      -e             -> precision (default, 1e-4)\n");
  printf("      -d             -> display (default, 0 {0 : non; 1 - yes})\n");
  printf("      -?             -> this help\n");
  printf("Arguments:\n");
  printf("     training_file   -> file containing training examples\n");
  printf("     parameter_file  -> file containing the parameters\n\n"); 
}

 


