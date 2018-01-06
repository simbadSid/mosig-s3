#include "defs.h"



void *Allocation(size_t taille)
{
  void *str;
  if(taille<=0) 
    taille=1; 
  str=(void *)malloc(taille);
  if(!str) { 
    perror ("Problème de mémoire!\n"); 
    exit (1); 
  }
  return(str);
}

void FileScan(char *filename, long int *m, long int *d)
{
   FILE *fd;
   long int  i=0, ch, prev='\n', dim=0, lines=0;
   char *str, sep[]=": \t";
   
   str=(char *)malloc(MAXSTR*sizeof(char ));
 
   if((fd=fopen(filename,"r"))==NULL){
     perror(filename);
     exit(0);
   }
   while((ch=fgetc(fd)) != EOF){
     if(!lines)
        str[i++]=ch;
     
     if(ch=='\n')
        ++lines;
     
     prev=ch;
   }   
   fclose(fd);
   if(prev != '\n')
      ++lines;
   
   i--;
   str[i]='\0';
   str=strtok(str,sep);
   str=strtok((char *)NULL,sep);
   while(str)
   {
      str=strtok((char *)NULL,sep);
      dim++;
   }
   *m=lines;
   *d=dim;
   
   free((char *) str);
}

void FileScanSS(char *filename, long int *m, long int *u, long int *d)
{
   FILE *fd;
   long int  i, j, cls, firstCompo;
   char ch, *token, *str, sep[]=": \t";
   
  token=(char *)Allocation(MAXSTR*sizeof(char )); 
  str=(char *)Allocation(MAXSTR*sizeof(char ));
  if((fd=fopen(filename,"r"))==NULL){
    printf("Erreur de lecture du fichier %s\n",filename);
    exit(0);
  }
  *d= *u= *m =0;
  j=i=0;
  firstCompo=1;
  
  while((token[j++]=fgetc(fd))!=EOF)
  {
    while((ch=fgetc(fd))!='\n'){
      token[j++]=ch;
      if(token[j-1]==' '){
        if(firstCompo){
          firstCompo=0;
          token[j-1]='\0';
          sscanf(token,"%ld",&cls);
          if(cls!=0)
             *m+=1;
          else
             *u+=1;
        }
        j=0;
      }
      if(*d==0)
        str[i++]=ch;

    }
    if(ch=='\n' && *d==0){ 
      i--;
      str[i]='\0';
      str=strtok(str,sep);
      str=strtok((char *)NULL,sep);
      while(str)
      {
         str=strtok((char *)NULL,sep);
         *d+=1;
      }
    }

    j=0;
    firstCompo=1;
  }
   free((char *) token);
   free((char *) str);
}


void ChrgMatrix(char *ficname, DATA TrainingSet) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j;
  FILE       *fd;

  token=(char *)malloc(MAXSTR*sizeof(char));
  

  if((fd=fopen(ficname,"r"))==NULL){
    perror("Erreur d'ouverture du fichier de donnees");
    exit(0);
  }
  
  i=1;
  
 
  while(i<=TrainingSet.m+TrainingSet.u)
  {
    j=0;
    while((token[j++]=fgetc(fd))!='\n');
    j--;
    token[j]='\0';
    str=strtok(token,sep);
    sscanf(str, "%lf",&TrainingSet.y[i]);
    str=strtok((char *)NULL,sep);
    j=1;
    while(str)
    {
      sscanf(str,"%lf",&TrainingSet.X[i][j]);
      str=strtok((char *)NULL,sep);
      j++;
	}
    i++;
  }
  
  fclose(fd);
  free(token);
}

void ChrgMatrixUnSup(char *ficname, long int u, long int d, double **X) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j;
  FILE       *fd;

  token=(char *)malloc(MAXSTR*sizeof(char));
  

  if((fd=fopen(ficname,"r"))==NULL){
    perror("Erreur d'ouverture du fichier de donnees");
    exit(0);
  }
  
  i=1;
  
  while(i<=u)
  {
    j=0;
    while((token[j++]=fgetc(fd))!='\n');
    j--;
    token[j]='\0';
    str=strtok(token,sep);
    j=1;
    while(str)
    {
      sscanf(str,"%lf",&X[i][j]);
      str=strtok((char *)NULL,sep);
      j++;
	}
    i++;
  }
  
  fclose(fd);
  free(token);
}



void Normalize(double **X, long int m, long int d)
{
   double max;
   long int i,j;

   for(j=1; j<=d; j++){
      max=X[1][j];
      for(i=2; i<=m; i++)
        if(fabs(X[i][j])>max)
            max=fabs(X[i][j]);
        
      for(i=1; i<=m; i++)
         X[i][j]/=max;
    }
}


void ConstructTrainTest(char *filename, double **X, double *Y, long int m, long int d, double proportion)
{
    long int N, j, i, U, *HasBeen;
    char str[300];
    FILE *fdTrain, *fdTest;
    
    N=(long int)(proportion*(double) m);
    
    srand(time(NULL));
    
    HasBeen=(long int *)malloc(m*sizeof(long int ));
    for(i=1; i<=m; i++)
        HasBeen[i]=0;
    
    sprintf(str,"%s-Train",filename);
    
    if((fdTrain=fopen(str,"w"))==NULL)
    {
        printf("Erreur de creation de %s\n",str);
        exit(0);
    }
    
    i=1;
    while(i<=N){
        U=(rand()%m)+1;
        if(!HasBeen[U])
        {
            fprintf(fdTrain,"%lf ",Y[U]);
            for(j=1; j<=d; j++)
                fprintf(fdTrain,"%lf ",X[U][j]);
            fprintf(fdTrain,"\n");
            HasBeen[U]=1;
            i++;
        }
    }
    fclose(fdTrain);
    sprintf(str,"%s-Test",filename);
    
    if((fdTest=fopen(str,"w"))==NULL)
    {
        printf("Erreur de creation de %s\n",str);
        exit(0);
    }
    
    for(i=1;i<=m;i++)
      if(!HasBeen[i])
      {
         fprintf(fdTest,"%lf ",Y[i]);
         for(j=1; j<=d; j++)
            fprintf(fdTest,"%lf ",X[i][j]);
         fprintf(fdTest,"\n");
      }

    
    fclose(fdTest);
    free((char *) HasBeen);
}

void save_params(char *filename,double *w,long int d)
{
   long int j;
   FILE *fd;
   
  if((fd=fopen(filename,"w"))==NULL){
    perror("Erreur de création du fichier de données");
    exit(0);
  }
   
  for(j=0; j<=d; j++)
    fprintf(fd,"%lf ",w[j]); 
  fclose(fd);   
}

void load_params(char *filename,double *w,long int d)
{
    long int j;
    FILE *fd;
    
    if((fd=fopen(filename,"r"))==NULL){
        perror("Erreur d'ouverture du fichier de données");
        exit(0);
    }
    
    for(j=0; j<=d; j++)
        fscanf(fd,"%lf",&w[j]);
    fclose(fd);   
}

void save_adaboost_params(char *filename,double **w,double *alpha, long int T, long int d)
{
    long int j, t;
    FILE *fd;
    
    if((fd=fopen(filename,"w"))==NULL){
        perror("Erreur de création du fichier de données");
        exit(0);
    }
    
    for(t=1; t<=T; t++){
      fprintf(fd,"%lf ",alpha[t]);
      for(j=0; j<=d; j++)
        fprintf(fd,"%lf ",w[t][j]);
      fprintf(fd,"\n");
    }
    fclose(fd);
}

void load_adaboost_params(char *filename,double **w,double *alpha, long int T, long int d)
{
    long int j, t;
    FILE *fd;
    
    if((fd=fopen(filename,"r"))==NULL){
        perror("Erreur d'ouverture du fichier de données");
        exit(0);
    }
    
    for(t=1; t<=T; t++){
        fscanf(fd,"%lf",&alpha[t]);
        for(j=0; j<=d; j++)
            fscanf(fd,"%lf",&w[t][j]);
    }
    fclose(fd);
}

/* ********** *************************************************************** ******** *
 *             Fichier format index:valeur pour apprentissage semi-supervisé           *
 * ********** *************************************************************** ******** */

 /*
 Lecture du fichier de type 'classe index:valeur' par ligne avec des classe=0 pour des 
 données non-étiquetées
 */

void SparseFileScanSemiSup(char *filename, long int *m, long int *u, long int *K, long int *d, long int *MaxLGN)
{
  char       *token;
  long int   j, cls, dim, first, cmpt;
  FILE       *fd;

  token=(char *)Allocation(MAXSTR*sizeof(char ));
  if((fd=fopen(filename,"r"))==NULL){
    printf("Erreur de lecture du fichier %s\n",filename);
    exit(0);
  }
  *K= *d= *u= *m =*MaxLGN=0;
  j=0;
  first=1;
  
  while((token[j++]=fgetc(fd))!=EOF)
  {
    cmpt=0;
    while((token[j++]=fgetc(fd))!='\n'){
      if(token[j-1]==' '){
        if(first){
          first=0;
          token[j-1]='\0';
          sscanf(token,"%ld",&cls);
          if(cls>0)
          {
             *m+=1;
             if(*K < cls)
               *K=cls;
          }
          else
             *u +=1;
        }
        j=0;
      }
      if(token[j-1]==':'){
        token[j-1]='\0';
        cmpt++;
        sscanf(token,"%ld",&dim);
        if(*d < dim)
	       *d=dim;
      }   
    }
    if(cmpt > *MaxLGN)
      *MaxLGN=cmpt;
   
    j=0;
    first=1;
  }
  
  fclose(fd);
}


void ChrgSparseMatrixSemiSup(char *ficname, SPARSEVECT *LabIndx, SPARSEVECT *UnlabIndx, long int *Des, long int u, long int m) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j, l, ind;
  double     x;
  FILE       *fd;

  token= (char *)Allocation(MAXSTR*sizeof(char));

  if((fd=fopen(ficname,"r"))==NULL){
    printf("Enable to open: %s\n",ficname);
    exit(0);
  }

  for(i=1; i<=m; i++)
  {
    l=0;
    while((token[l++]=fgetc(fd))!='\n');
    l--;
    token[l]='\0';

    str=strtok(token,sep);
    sscanf(str,"%ld",&Des[i]);
    str=strtok((char *)NULL,sep);
    j=1;
    while(str)
    {
      sscanf(str,"%ld",&ind);
      str=strtok((char *)NULL,sep);
      sscanf(str,"%lf",&x);
      str=strtok((char *)NULL,sep);
      LabIndx[i].Att[j].val=x;
      LabIndx[i].Att[j].ind=ind;
      j++;
    }
    LabIndx[i].N=j-1;
  }
  
  for(i=1; i<=u; i++)
  {
    l=0;
    while((token[l++]=fgetc(fd))!='\n');
    l--;
    token[l]='\0';

    str=strtok(token,sep);
    str=strtok((char *)NULL,sep);
    j=1;
    while(str)
    {
      sscanf(str,"%ld",&ind);
      str=strtok((char *)NULL,sep);
      sscanf(str,"%lf",&x);
      str=strtok((char *)NULL,sep);
      UnlabIndx[i].Att[j].val=x;
      UnlabIndx[i].Att[j].ind=ind;
      j++;
    }
    UnlabIndx[i].N=j-1;
  }

  fclose(fd);
  free((char *) token);
}


void FileSparseVectors(char *filename, long int *n, long int *K, long int *d, long int *MaxLGN)
{
  char       *token;
  long int   j, cls, dim, first,cmpt;
  FILE       *fd;


  token= (char *)Allocation(MAXSTR*sizeof(char));

  if((fd=fopen(filename,"r"))==NULL){
    printf("Erreur de lecture du fichier %s\n",filename);
    exit(0);
  }
  
  *K= *d= *n=*MaxLGN=0;
  j=0;
  first=1;
  while((token[j++]=fgetc(fd))!=EOF)
  {
    cmpt=0;
    while((token[j++]=fgetc(fd))!='\n'){
      if(token[j-1]==' '){
        if(first){
          first=0;
          token[j-1]='\0';
          sscanf(token,"%ld",&cls);
          *n+=1;
           if(*K < cls)
             *K=cls;
        }
        j=0;
      }
      if(token[j-1]==':'){
        token[j-1]='\0';
        cmpt++;
        
        sscanf(token,"%ld",&dim);
        if(*d < dim)
	       *d=dim;
      }   
    }
    if(cmpt > *MaxLGN)
      *MaxLGN=cmpt;
    j=0;
    first=1;
  }
  

  fclose(fd);
}


void ChrgSparseMatrix(char *ficname, SPARSEVECT *Data, long int *Des, long int n) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j, ind, l;
  double     x;
  FILE       *fd;

  token= (char *)Allocation(MAXSTR*sizeof(char));

  if((fd=fopen(ficname,"r"))==NULL){
    printf("Enable to open: %s\n",ficname);
    exit(0);
  }

  for(i=1; i<=n; i++)
  {
    l=0;
    j=1;
    while((token[l++]=fgetc(fd))!='\n');
    l--;
    token[l]='\0';

    str=strtok(token,sep);
    sscanf(str,"%ld",&Des[i]);
    str=strtok((char *)NULL,sep);
   
    while(str)
    {
      sscanf(str,"%ld",&ind);
      str=strtok((char *)NULL,sep);
      sscanf(str,"%lf",&x);
      str=strtok((char *)NULL,sep);
      Data[i].Att[j].ind=ind;
      Data[i].Att[j].val=x;
      j++;
    }
    Data[i].N=j-1;

  }

  fclose(fd);
  free((char *) token);
}

void ChrgSparseMatrixRB(char *ficname, SPARSEVECT *Data, double *tabMax, double *tabMin, long int *Des, long int n) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j, ind, l;
  double     x;
  FILE       *fd;

  token= (char *)Allocation(MAXSTR*sizeof(char));

  if((fd=fopen(ficname,"r"))==NULL){
    printf("Enable to open: %s\n",ficname);
    exit(0);
  }

  for(i=1; i<=n; i++)
  {
    l=0;
    j=1;
    while((token[l++]=fgetc(fd))!='\n');
    l--;
    token[l]='\0';

    str=strtok(token,sep);
    sscanf(str,"%ld",&Des[i]);
    str=strtok((char *)NULL,sep);
   
    while(str)
    {
      sscanf(str,"%ld",&ind);
      str=strtok((char *)NULL,sep);
      sscanf(str,"%lf",&x);
      str=strtok((char *)NULL,sep);
      Data[i].Att[j].ind=ind;
      Data[i].Att[j].val=x;
      if(tabMax[ind]<=x)
	    tabMax[ind]=x;
      if(tabMin[ind]>=x)
	    tabMin[ind]=x;
 
      j++;
    }
    Data[i].N=j-1;

  }

  fclose(fd);
  free((char *) token);
}

double **AlloueMatrice(nbLig, nbCol)
long int nbLig;
long int nbCol;
{
  long int i;
  double   **m;
  m=(double **) Allocation((nbLig+1)*sizeof(double *));
  if(!m){
    printf("Probleme d'allocation de la matrice des données\n");
    exit(0);
  }
  m[1]=(double *) Allocation((size_t)((nbLig*nbCol+1)*sizeof(double )));
  if(!m[1]){
    printf("Probleme d'allocation de la matrice des données\n");
    exit(0);
  }

  for(i=2; i<=nbLig; i++)
    m[i]=m[i-1]+nbCol;

  return m;
}

void LibereMatrice(m, nbLig, nbCol)
double **m;
long int nbLig;
long int nbCol;
{
  free((char *) m[1]);
  free((char *) m);
}

