/*  ***********************************************************************************  //
//                              Optimiseurs.c                                            //                                                                                       //
// rchln : line search                                                                   // 
// qsnewton : quasi-Newton method, Broyden-Fletcher-Goldfarb-Shanno (BFGS) variant       //
// grdcnj: Minimization of a cost function following the conjugate gradient technique    //
//   Author: Massih R. Amini                                                             //
//   Date: 25/10/2013                                                                    //
//  																					 //
//  ************************************************************************************ */

#include "defs.h"

void rchln(FncCout,Grd,wold, Lold, g, p, w, L, Obs)
double (*FncCout)(double *, DATA);
              // Convex cost function to minimize 
double* (*Grd)(double *, DATA);
			  // Gradient of the cost function
double *wold; // Current weight vector 
double Lold;  // Current value of the cost function
double *g;    // Gradient 
double *p;    // Descente Direction 
double *w;    // New weight vector, 
double *L;    // New value of the cost function
/* 
  Information allowing to estimate the cost function at a given point, 
  The estimation is done by  FncCout(w,X,Y,m,d) 
 */
DATA Obs; // Structure containing information on the training set
{
  long int j;
  double   a, b, delta, L2, coeff1, coeff2, pente, max; 
  double   eta, eta2, etamin, etatmp;

  // Sloop at the actual point 
  for(pente=0.0, j=0; j<=Obs.d; j++)
    pente+=p[j]*g[j];
 
  // Definition of the minimal tolerated value of /*@$\eta$@*/
  // DMAX() returns the maximum of two numbers (definied dans def.h)
  max=0.0;
  for(j=0;j<=Obs.d;j++)
    if(fabs(p[j])>max*DMAX(fabs(wold[j]),1.0))
      max=fabs(p[j])/DMAX(fabs(wold[j]),1.0);
  etamin=MINETA/max;

  // Update of the weight vector for the largest value of eta from which the search begins
  eta=1.0;
  for(j=0;j<=Obs.d;j++)
    w[j]=wold[j]+eta*p[j];

  *L=FncCout(w,Obs);
  
  // Loop until Armijo condition is not satisfait  /*@$(Eq.~\ref{eq:ch4:GoldsteinArmijo})$@*/
  while(*L > (Lold+ALPHA*eta*pente))
  {
    if(eta < etamin)
    {
      for(j=0; j<=Obs.d; j++)
	    w[j]=wold[j];
       // If the learning rate becomes too small the search is terminated
      return;
    }
    else
    {
      if(eta==1.0)
        // Minimiser of the interpolation polynom  of 2nd degree /*@$(Eq.~\ref{eq:ch4:MinimInterpol2})$@*/
        etatmp = -pente/(2.0*(*L-Lold-pente));
      else
      {
        coeff1 = *L-Lold-eta*pente;
        coeff2 = L2-Lold-eta2*pente;
        // Computation of coefficients of the interpolation polynom of degree 3 /*@$(Eq.~\ref{eq:ch4:CoeffInterpol3})$@*/
        a=(coeff1/(eta*eta)-coeff2/(eta2*eta2))/(eta-eta2);
        b=(-eta2*coeff1/(eta*eta)+eta*coeff2/(eta2*eta2))/(eta-eta2);
        if (a != 0.0)
        {
           delta=(b*b)-3.0*a*pente;
           if(delta >= 0.0)
             // Minimiser of the interpolation polynom  of 2nd degree /*@$(Eq.~\ref{eq:ch4:MinimInterpol3})$@*/
             etatmp=(-b+sqrt(delta))/(3.0*a);
           else
             {printf("rchln: interpolation problem");exit(0);}
        }
        else
          etatmp = -pente/(2.0*b);
		
        // /*@$\eta\leq\frac{1}{2}\eta_{p_1}$@*/
        if(etatmp > 0.5*eta)
          etatmp=0.5*eta;
       }
    }
    eta2=eta;
    L2 = *L;
    // /*@$\eta\geq\frac{1}{10}\eta_{p_1}$@*/ - Too small learning rates are avoided
    // DMAX() returns the maximum of two numbers (definied dans def.h)

    eta=DMAX(etatmp,0.1*eta);
  
    for(j=0;j<=Obs.d;j++)
      w[j]=wold[j]+eta*p[j];

    *L=FncCout(w,Obs);
  }
}


void qsnewton(FncCout,Grd,Obs,w,epsilon)
double (*FncCout)(double *, DATA);
                // Convex cost function to minimize 
double* (*Grd)(double *, DATA);
				// Gradient
DATA    Obs;
double  *w;     // Weight vector
double epsilon; // Precision
{
   long int  i,Epoque=1,j;
   double vTg,invgTBg,gTBg,NewLoss,OldLoss; 
   double *wnew,*oldg,**B,*g,*Bg,*p,*u,*v;

   B=malloc((Obs.d+1)*sizeof(double *)); 
   if(!B){
     printf("qsnewton: Memory allocation problem\n");
     exit(0);
   }
   B[0]=(double *)malloc((size_t)(((Obs.d+1)*(Obs.d+1))*sizeof(double)));
   if(!B[0]){
     printf("qsnewton: Memory allocation problem\n");
     exit(0);
   }
  
   for(i=1; i<=Obs.d; i++)
    B[i]=B[i-1]+Obs.d+1;

   oldg= (double *) malloc((Obs.d+1) * sizeof(double ));
   g=(double *) malloc((Obs.d+1) * sizeof(double ));
   v=(double *) malloc((Obs.d+1) * sizeof(double ));
   Bg=(double *) malloc((Obs.d+1) * sizeof(double ));
   wnew=(double *) malloc((Obs.d+1) * sizeof(double ));
   u=(double *) malloc((Obs.d+1) * sizeof(double ));
   p=(double *) malloc((Obs.d+1) * sizeof(double ));


   // Computation of /*@$\mathcal{L}(\boldsymbol w^{(0)})$ and $\nabla \mathcal{L}(\boldsymbol w^{(0)})$@*/
   NewLoss=FncCout(w, Obs);
   g  = Grd(w, Obs);

   // Initialisation /*@$\mathbf{B}_0\leftarrow \mathbf{Id}_d, \mathbf{p}_0\leftarrow -\nabla \mathcal{L}(\boldsymbol w^{(0)})$@*/
  for (i=0;i<=Obs.d;i++) {
    for (j=0;j<=Obs.d;j++) 
      B[i][j]=0.0; 
    B[i][i]=1.0;
    p[i] = -g[i];
  } 
  OldLoss = NewLoss + 2*epsilon;

  while(fabs(OldLoss-NewLoss) > epsilon*(fabs(OldLoss))){
     OldLoss = NewLoss;

     // Computation of new weights /*@$\boldsymbol w^{(t+1)}\leftarrow \boldsymbol w^{(t)}+\eta_t \mathbf{p}_t$@*/
     rchln(FncCout, Grd,w, OldLoss, g, p, wnew, &NewLoss, Obs); // Recherche linéaire /*@(Algorithme \ref{algo:chap4:RechercheLineaire})@*/

    // Computation of /*@$\mathbf{v}_{t+1}= \boldsymbol w^{(t+1)}-\boldsymbol w^{(t)}$@*/
     for (j=0;j<=Obs.d;j++) {
       v[j]=wnew[j]-w[j]; 
       w[j]=wnew[j];
       oldg[j]=g[j]; 
     }
     
    // Computation of /*@$\nabla \mathcal{L}(\boldsymbol w^{(t+1)}$@*/
     g  = Grd(w, Obs);

    // Computation of /*@$\mathbf{g}_{t+1}=\nabla \mathcal{L}(\boldsymbol w^{(t+1)})-\nabla \mathcal{L}(\boldsymbol w^{(t)})$@*/
     for(j=0;j<=Obs.d;j++) 
       oldg[j]=g[j]-oldg[j];

      
     // Computation of /*@$\mathbf{B}_t \mathbf{g}_{t+1}$@*/
     for(j=0;j<=Obs.d;j++) {
       Bg[j]=0.0;
       for (i=0;i<=Obs.d;i++) 
         Bg[j] += B[j][i]*oldg[i];
     }

     // Computation of /*@$\mathbf{v}_{t+1}^T \mathbf{g}_{t+1}$, et de $\mathbf{g}_{t+1}^T\mathbf{B}_t \mathbf{g}_{t+1}$@*/
    for(vTg=gTBg=0.0,j=0;j<=Obs.d;j++) {
      vTg += v[j]*oldg[j]; 
      gTBg += oldg[j]*Bg[j]; 
    }
    vTg=1.0/vTg;
    invgTBg=1.0/gTBg;
   
   
    // /*@$\mathbf{u}_{t+1}=\displaystyle{\frac{\mathbf{v}_{t+1}}{\mathbf{v}_{t+1}^T\mathbf{g}_{t+1}}-\frac{\mathbf{B}_t\mathbf{g}_{t+1}}{\mathbf{g}_{t+1}^T\mathbf{B}_t\mathbf{g}_{t+1}}}$ @*/
    for (j=0;j<=Obs.d;j++) 
      u[j]=vTg*v[j]-invgTBg*Bg[j];
    
    // Update of the inverse of the Hessian, /*@$\mathbf{B}_{t+1}$@*/
    // Broyden-Fletcher-Goldfarb-Shanno formula  /*@ (Eq. \ref{eq:BFGS})@*/  
    for (j=0;j<=Obs.d;j++) 
      for (i=j;i<=Obs.d;i++){
         B[j][i] += vTg*v[j]*v[i] -invgTBg*Bg[j]*Bg[i]+gTBg*oldg[j]*oldg[i];  
         B[i][j]=B[j][i];
      }
    

    // New descente direction /*@$\mathbf{p}_{t+1}=-\mathbf{B}_{t+1}\nabla \mathcal{L}(\boldsymbol w^{(t+1)})$@*/
    for(j=0; j<=Obs.d; j++){
      p[j]=0.0;
      for (i=0;i<=Obs.d;i++) 
         p[j] -= B[j][i]*g[i];
    }
    if(!(Epoque%5))
       printf("Iteration:%ld Loss:%lf\n",Epoque,NewLoss);
  
    Epoque++;
 
  }
   free((char *) oldg);
   free((char *) g);
   free((char *) v);
   free((char *) Bg);
   free((char *) wnew);
   free((char *) u);
   free((char *) p);
   free((char *) B[0]);
   free((char *) B);
}


void grdcnj(FncCout, Grd, Obs, w, epsilon, disp)
double (*FncCout)(double *, DATA);
                // Convex cost function to minimize
double* (*Grd)(double *, DATA);
				// Gradient
DATA    Obs;    // Data structure  
double  *w;     // weight vector
double  epsilon; // Precision
int      disp;    // Display or not of the intermediate values of the cost function
{
  long int   j, Epoque=0;
  double     *wold, OldLoss, NewLoss, *g, *p, *h, dgg, ngg, beta;

  wold = (double *) malloc((Obs.d+1) * sizeof(double ));
  p    = (double *) malloc((Obs.d+1) * sizeof(double )); 
  g    = (double *) malloc((Obs.d+1) * sizeof(double )); 
  h    = (double *) malloc((Obs.d+1) * sizeof(double ));

  for(j=0; j<=Obs.d; j++)
     wold[j]= 2.0*(rand() / (double) RAND_MAX)-1.0;

  NewLoss = FncCout(wold, Obs);
  OldLoss = NewLoss + 2*epsilon;
  g  = Grd(wold, Obs);

 for(j=0; j<=Obs.d; j++)
    p[j] = -g[j];  // /*@ $\mathbf{p}_0=-\nabla \mathcal L(\boldsymbol w^{(0)})$ (Eq. \ref{DirDes})@*/
    
    
  while(fabs(OldLoss-NewLoss) > (fabs(OldLoss)*epsilon)) 
  {
    OldLoss = NewLoss;
    
    rchln(FncCout, Grd, wold, OldLoss, g, p, w, &NewLoss, Obs); // Line search /*@(Algorithme \ref{algo:chap4:RechercheLineaire})@*/
    
    h  = Grd(w, Obs); // New gradient vector /*@ $\nabla \mathcal{L}(\boldsymbol w^{(t+1)})$ (Eq. \ref{eq:MiseAJour})@*/
 
    for(dgg=0.0, ngg=0.0, j=0; j<=Obs.d; j++){
       dgg+=g[j]*g[j];
       ngg+=h[j]*h[j];   
   //    ngg+=h[j]*(h[j]-g[j]); // Ribière-Polak formula  /*@(Eq. \ref{CoeffBeta3})@*/
     }
    
    beta=ngg/dgg; // Fletcher-Reeves formula /*@(Eq. \ref{CoeffBeta4})@*/
    for(j=0; j<=Obs.d; j++){
       wold[j]=w[j];
       g[j]=h[j];
       p[j]=-g[j]+beta*p[j]; // Update of the descente direction /*@(Eq. \ref{DirDes})@*/
    }
    if(!(Epoque%5) && disp)
      printf("Iteration:%ld Loss:%lf\n",Epoque,NewLoss);
    
    Epoque++;
  }
  
  free((char *) wold);
  free((char *) p);
  free((char *) g);
  free((char *) h);
}
