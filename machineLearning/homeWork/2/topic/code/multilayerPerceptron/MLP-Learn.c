#include "defs.h"

int  main(argc, argv)
int  argc;
char **argv; 
{
   DATA         D;
   LR_PARAM     input_params;
   long int     NumLayers, Layer, NumNeurons[20], i;
   double       **W[20];
   char         input_filename[100], params_filename[100];
   
   // Reading of the command line 
   lire_commande(&input_params,input_filename, params_filename,argc, argv);

   FileScanMltCls(input_filename, &D.m, &D.d, &D.K);
   
   printf("Number of examples %ld, dimension: %ld, number of classes: %ld\n",D.m,D.d,D.K);

   Architecture(NumNeurons, &NumLayers);
   NumNeurons[0]=D.d;
   NumNeurons[NumLayers+1]=D.K;

   D.X=(double **)malloc((NumNeurons[0]+1)*sizeof(double *));
   for(i=1; i<=NumNeurons[0]; i++)
	   D.X[i]=(double *)malloc((D.m+1)*sizeof(double ));
   
   D.Y=(double **)malloc((NumNeurons[NumLayers+1]+1)*sizeof(double *));
   for(i=1; i<=NumNeurons[NumLayers+1]; i++)
	   D.Y[i]=(double *)malloc((D.m+1)*sizeof(double ));

   for(Layer=0; Layer<=NumLayers; Layer++){
      W[Layer]=(double **)malloc((1+NumNeurons[Layer+1])*sizeof(double *));
	  for(i=1; i<=NumNeurons[Layer+1]; i++)
		  W[Layer][i]=(double *)malloc((NumNeurons[Layer]+2)*sizeof(double ));
   }
   ChargeFicBaseMltClass(input_filename, &D);
   
   PMC_stochastique(D, NumLayers, NumNeurons, W, input_params,params_filename);
   return 1;
}

void lire_commande(LR_PARAM *mlp_input_params, char *fic_apprentissage, char *fic_params, int num_args, char **args)
{
    long int i;
    
    mlp_input_params->T=10000;
    mlp_input_params->eta=0.1;

    for(i=1; (i<num_args) && (args[i][0] == '-'); i++){
        printf("%s\n",args[i]);
        switch((args[i])[1]){
            case 't': i++; sscanf(args[i],"%ld",&mlp_input_params->T); break;
            case 'a': i++; sscanf(args[i],"%lf",&mlp_input_params->eta); break;
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
    printf("\nMultilayer Perceptron\n");
    printf("\nAuthor: Massih R. Amini \n");
    printf("usage: MLP-learn [options] Training-file Parameter-file\n\n");
    printf("Options:\n");
    printf("      -t             -> Maximum number of iterations (default, 10000)\n");
    printf("      -a             -> Learning rate (default, 0.1)\n");
    printf("      -?             -> This help\n");
    printf("Arguments:\n");
    printf("     Training-file  -> file containing training examples\n");
    printf("     Parameter-file -> file containing parameters\n\n");
}
