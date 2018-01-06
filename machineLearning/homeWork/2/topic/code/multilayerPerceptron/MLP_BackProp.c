#include "defs.h"


void PMC_stochastique(TrainSet, NumLayers, NumNeurons, W, params,ficParam)
DATA      TrainSet;     // base d'entrainement 
long int  NumLayers;            // nombre de couches cachées 
long int  *NumNeurons;  // nombre d'unités sur chaque couche
double    ***W;         // paramètres (poids) du modèle
LR_PARAM  params;       // paramètres du programme 
char      *ficParam;    // nom du fichier des paramètres
{
   long int   i, j, q, l,k, Av, Ap,nc, Layer, t=1, U;
   double     *delta[20], *z[20], aj, sommeDW, NEWe, OLDe, SumE;
   FILE       *fd;

   srand(time(NULL));

   for(Layer=0; Layer<=NumLayers+1; Layer++)
      z[Layer]=(double *)malloc((1+NumNeurons[Layer])*sizeof(double ));
   for(Layer=0; Layer<=NumLayers+1; Layer++)
      delta[Layer]=(double *)malloc((1+NumNeurons[Layer])*sizeof(double ));
   
     // Initialisation aléatoire des paramètres du modèle
   for(Layer=0; Layer<=NumLayers; Layer++)
      for(i=0; i<=NumNeurons[Layer]; i++)
	    for(j=1; j<=NumNeurons[Layer+1]; j++)
	       W[Layer][j][i]=2.0*(rand() / (double) RAND_MAX)-1.0;
   
   SumE=NEWe=0.0;
   OLDe=-1.0;
   printf("Training ");
   while((fabs(NEWe-OLDe)>(double )t*params.eps) && (t<=params.T))
   {
     OLDe=NEWe;
	 
	 // Tirage aléatoire de l'indice d'un exemple d'entrainement
	 U=(rand()%TrainSet.m) + 1;
	 // /*@$\rhd~\forall i, z_i=x_i$ @*/pour les cellules de la couche d'entrée
	 for(i=1; i<=NumNeurons[0]; i++)
	   z[0][i]=TrainSet.X[i][U];
  
   	 /* ------------------------------------------------- *
	  *                Phase de propagation 	          *
	  * ------------------------------------------------- */
	 for(Layer=1; Layer<=(NumLayers+1); Layer++)
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
	 
	 k=NumNeurons[NumLayers+1];
	 // /*@$\rhd~\forall i, z_i=h_i$ pour les cellules de la couche de sortie@*/
	 SumE=0.0;
	 for(q=1; q<=k; q++)
	 {
	    SumE+=0.5*pow(z[NumLayers+1][q]-TrainSet.Y[q][U], 2);
	    // /*@$\rhd~\delta_q\leftarrow \mathcal{A}'(a_q)\times(h_q-y_q)$@*/
		delta[NumLayers+1][q]=Aprime(z[NumLayers+1][q])*(z[NumLayers+1][q]-TrainSet.Y[q][U]);
	 }
	 NEWe+=SumE;///(double) t;

	 /* ------------------------------------------------------ *
	  *  Phase de RetroPropagation et de correction des poids  *
	  * ------------------------------------------------------ */
	 for(Layer=NumLayers; Layer>=0; Layer--)
	 {
	    Ap=NumNeurons[Layer+1];
	    nc=NumNeurons[Layer];
	    for(j=1; j<=nc; j++)
	    {
		  // /*@$\rhd~\delta_j\leftarrow \mathcal{A}'(a_j)\sum_{l\in Ap(j)}\delta_l w_{lj}$ (\'equation \ref{eq:PMC:chaine2})@*/
		  for(sommeDW=0.0, l=1; l<=Ap; l++)
		     sommeDW+=delta[Layer+1][l]*W[Layer][l][j];
		  delta[Layer][j]=Aprime(z[Layer][j])*sommeDW;
		  
   	       /* ------------------------------------------------- *
	        *        descente de gradient stochastique          *
	        * ------------------------------------------------- */
		  // /*@$\rhd~\Delta_{lj}\leftarrow -\eta\delta_l z_j$ (\'equation \ref{eq:PMC:MiseAJour}) @*/
		  for(l=1; l<=Ap; l++)
		     W[Layer][l][j]-=(params.eta*delta[Layer+1][l]*z[Layer][j]);
	    }
        for(l=1; l<=Ap; l++)
	       W[Layer][l][0]-=(params.eta*delta[Layer+1][l]);
	 }

     if (!(t%300) && (t%1500))
        printf(".");
     if (!(t%1500))
        printf("%ld",t);

       //printf("Epoque : %ld \n",t);
       //printf("Delta erreur (en-ligne) = %lf\n",fabs(NEWe-OLDe)/(double )t);
     
     
      t++;
   } /* fin while((fabs(NEWe-OLDe)>params.eps) && (t<=params.T)) */
   printf("\n");
	    
   if((fd=fopen(ficParam,"w"))==NULL){
      printf("MLP_BackProp: error file   %s\n",ficParam);
      exit(0);
   }
   
   fprintf(fd,"%ld\n",NumLayers);
   for(Layer=1; Layer<=NumLayers; Layer++)
     fprintf(fd,"%ld ",NumNeurons[Layer]);
   fprintf(fd,"\n");
   for(Layer=0; Layer<=NumLayers; Layer++){
      for(i=0; i<=NumNeurons[Layer]; i++){
	    for(j=1; j<=NumNeurons[Layer+1]; j++)
	      fprintf(fd,"%.24lf ",W[Layer][j][i]);
        fprintf(fd,"\n");
      }
      //fprintf(fd,"\n");
    }
   fclose(fd);


    

   for(Layer=0; Layer<=NumLayers+1; Layer++)
      free((char *) z[Layer]);
   for(Layer=1; Layer<=NumLayers+1; Layer++)
      free((char *)delta[Layer]);

}
