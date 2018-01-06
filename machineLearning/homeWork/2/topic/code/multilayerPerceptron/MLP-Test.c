#include "defs.h"

int  main(argc, argv)
int  argc;
char **argv; 
{
   DATA         D;
   long int     NumLayers, Layer, NumNeurons[20], i,j;
   double       **W[20];
   FILE         *fd;
    if(argc!=4){
        printf("USAGE : MLP-Test Filename ParamsFile PredictionFile\n");
        exit(0);
    }

   FileScanMltCls(argv[1], &D.m, &D.d, &D.K);
   printf("Number of examples %ld, dimension: %ld, number of classes: %ld\n",D.m,D.d,D.K);

   if((fd=fopen(argv[2],"r"))==NULL){
      printf("MLP-Test: error file  %s\n",argv[2]);
      exit(0);
   }
   fscanf(fd,"%ld ",&NumLayers);
   for(Layer=1; Layer<=NumLayers; Layer++)
     fscanf(fd,"%ld ",&NumNeurons[Layer]);

   NumNeurons[0]=D.d;
   NumNeurons[NumLayers+1]=D.K;
   for(Layer=0; Layer<=NumLayers; Layer++){
      W[Layer]=(double **)malloc((1+NumNeurons[Layer+1])*sizeof(double *));
	  for(i=1; i<=NumNeurons[Layer+1]; i++)
		  W[Layer][i]=(double *)malloc((NumNeurons[Layer]+2)*sizeof(double ));
   }

   for(Layer=0; Layer<=NumLayers; Layer++)
      for(i=0; i<=NumNeurons[Layer]; i++)
	    for(j=1; j<=NumNeurons[Layer+1]; j++)
	      fscanf(fd,"%lf",&W[Layer][j][i]);

   fclose(fd);

      
   D.X=(double **)malloc((NumNeurons[0]+1)*sizeof(double *));
   for(i=1; i<=NumNeurons[0]; i++)
	   D.X[i]=(double *)malloc((D.m+1)*sizeof(double ));

   D.Y=(double **)malloc((NumNeurons[NumLayers+1]+1)*sizeof(double *));
   for(i=1; i<=NumNeurons[NumLayers+1]; i++)
	   D.Y[i]=(double *)malloc((D.m+1)*sizeof(double ));

   ChargeFicBaseMltClass(argv[1], &D);
   Propagation(D, NumLayers, NumNeurons, W, argv[3]);
   return 1;
}

