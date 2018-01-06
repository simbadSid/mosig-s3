#include "defs.h"

void Propagation(D, NumLayers, NumNeurons, W, PredictsFile)
DATA      D;             // Data 
long int  NumLayers;     // nombre de couches cachées 
long int  *NumNeurons;   // nombre d'unités sur chaque couche
double    ***W;          // paramètres (poids) du modèle
char      *PredictsFile; // nom du fichier contenant les nouvelles représentations des exemples
{
   long int   i, j,   Av, nc, Layer, U,indMax;
   double     *z[20], aj,accuracy=0.0,Max;
   FILE       *fd;

   for(Layer=0; Layer<=NumLayers+1; Layer++)
      z[Layer]=(double *)malloc((1+NumNeurons[Layer])*sizeof(double ));
     
   if((fd=fopen(PredictsFile,"w"))==NULL){
      printf("Propagation: error file  %s\n",PredictsFile);
      exit(0);
   }


   for(U=1; U<=D.m; U++){
     for(i=1; i<=NumNeurons[0]; i++)
	      z[0][i]=D.X[i][U];
     Max=-Emax;
	 for(Layer=1; Layer<=NumLayers+1; Layer++)
	 {
	    Av=NumNeurons[Layer-1];
	    nc=NumNeurons[Layer];
	    for(j=1; j<=nc; j++)
	    {
	       // /*@$\rhd~a_j\leftarrow \sum_{i\in Av(j)} w_{ji}z_i$@*/
	       for(aj=W[Layer-1][j][0], i=1; i<=Av; i++)
		      aj+=W[Layer-1][j][i]*z[Layer-1][i];
		    
		   // /*@ $\rhd~z_j\leftarrow \mathcal{T}(a_j)$ @*/  
	       z[Layer][j]=A(aj);

	    }
	 }
	 
     for(j=1; j<=NumNeurons[NumLayers+1]; j++){
     	 fprintf(fd, "%.12lf ",z[Layer-1][j]);
         if(z[Layer-1][j]>Max){
           Max=z[Layer-1][j];
           indMax=j;
         }
     }	 
	 fprintf(fd,"\n");
	 if(D.Y[indMax][U]==1.0)
	   accuracy++;

   }
   fclose(fd);

    printf("Error: %lf\n",1.0-accuracy/(double )D.m);
   
   for(Layer=0; Layer<=NumLayers+1; Layer++)
      free((char *) z[Layer]);
}
