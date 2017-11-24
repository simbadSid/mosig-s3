#include "defs.h"

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


void ChrgMatrix(char *ficname, long int m, long int d, double **X, double *Y) 
{
  char       sep[]=": \t", *token, *str;
  long int   i, j, k,  cls, ind, N;
  double     x, max;
  FILE       *fd;

  token=(char *)malloc(MAXSTR*sizeof(char));
  

  if((fd=fopen(ficname,"r"))==NULL){
    perror("Open file error - FileScan");
    exit(0);
  }
  
  i=1;
  
  while(i<=m)
  {
    j=0;
    while((token[j++]=fgetc(fd))!='\n');
    j--;
    token[j]='\0';
    str=strtok(token,sep);
    sscanf(str, "%lf",&Y[i]);
    str=strtok((char *)NULL,sep);
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
        printf("Error of creation of %s - ConstructTrainTest\n",str);
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
        printf("Error of creation of %s - ConstructTrainTest\n",str);
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
    printf("Error of creation of %s - save_params\n",filename);
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
       printf("Open file error %s - save_params\n",filename);
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
       printf("Error of creation of %s - save_adaboost_params\n",filename);
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
        printf("Open file error %s - load_adaboost_params\n",filename);
        exit(0);
    }
    
    for(t=1; t<=T; t++){
        fscanf(fd,"%lf",&alpha[t]);
        for(j=0; j<=d; j++)
            fscanf(fd,"%lf",&w[t][j]);
    }
    fclose(fd);
}



