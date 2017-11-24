#include "defs.h"


int main(int argc, char **argv)
{
  LR_PARAM   input_params;
  long int   i,j, m, d;
  double     *w, *Y, **X, precision,  *h, Erreur, Precision, Rappel, F, PosPred, PosEffect, PosEffPred;
  char input_filename[200], params_filename[200];

    srand(time(NULL));
  // Reading the parameters line
  lire_commande(&input_params,input_filename, params_filename,argc, argv);
  // Scan of the training file 
  // definition in utilitaire.c
  FileScan(input_filename,&m,&d);
  printf("Training set containing %ld examples in dimension %ld\n",m,d);

  Y  = (double *)  malloc((m+1)*sizeof(double ));
  X  = (double **) malloc((m+1)*sizeof(double *));
  if(!X){
    printf("Memory allocation problem (1)\n");
    exit(0);
  }
  X[1]=(double *)malloc((size_t)((m*d+1)*sizeof(double)));
  if(!X[1]){
    printf("Memory allocation problem (2)\n");
    exit(0);
  }
  for(i=2; i<=m; i++)
    X[i]=X[i-1]+d;

  w  = (double *) malloc((d+1) * sizeof(double ));

  // Loading of the data matrix procedure defined in utilitaire.c
  ChrgMatrix(input_filename, m, d, X, Y);

  // Perceptron algorithm
  perceptron(X, Y, w, m, d, input_params.eta, input_params.T);
  h = (double *)  malloc((m+1)*sizeof(double ));
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
    
  printf("Precision:%lf Recall:%lf F1-measure:%lf Error=%lf\n",Precision,Rappel,F,Erreur);
    
    
  free((char *)h);

  save_params(params_filename, w,d);

  return 1;
}



void lire_commande(LR_PARAM *input_params, char *fic_apprentissage, char *fic_params, int num_args, char **args)
{
  long int i;

  input_params->T=5000;
  input_params->eta=0.1;

  for(i=1; (i<num_args) && (args[i][0] == '-'); i++){
    printf("%s\n",args[i]);
    switch((args[i])[1]){
      case 't': i++; sscanf(args[i],"%ld",&input_params->T); break;
      case 'a': i++; sscanf(args[i],"%lf",&input_params->eta); break;
      case '?': i++; aide();exit(0); break;
      
      default : printf("Unknown option %s\n",args[i]);Standby();aide();exit(0);
    }
  }
  if((i+1)>=num_args){
    printf("\n ---------------------------- \n Insuffisant number of parameters\n ----------------------------\n\n");
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
  printf("\nPerceptron algorithm\n");
  printf("\nAuthor: Massih R. Amini \n");
  printf("Date: 25 october 2013\n\n");
  printf("usage: perceptron_train [options] training_file parameters_file\n\n");
  printf("Options:\n");
  printf("      -t             -> Maximum number of iterations  (default, 5000)\n");
  printf("      -a             -> Learning rate (default, 0.1)\n");
  printf("      -?             -> This help\n");
  printf("Arguments:\n");
  printf("     training_file   -> File containing training examples\n");
  printf("     parameter_file  -> File containing model parameters\n\n"); 
}

 


