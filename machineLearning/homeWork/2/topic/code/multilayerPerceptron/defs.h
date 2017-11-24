/*  *******************************************************************************  //       
//                                                                                   //
//                                     defs.c                                        //
//                                                                                   //
//   Bibliothèques, macros, définitions des procédures/fonctions                     //
//                                                                                   //
//   Author: Massih R. Amini                                                         //
//   Date: 25/10/2013                                                                //
//                                                                                   //
//  ******************************************************************************** */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
/*#include <tr1/unordered_map>
#include <tr1/functional>
#include <vector>

using namespace std::tr1;
using namespace std;
*/// Définition de macros
double maxarg1, maxarg2, minarg1, minarg2;
#define DMAX(a,b) (maxarg1=(a),maxarg2=(b), (maxarg1)>(maxarg2) ? (maxarg1) : (maxarg2))
#define DMIN(a,b) (minarg1=(a),minarg2=(b), (minarg1)<(minarg2) ? (minarg1) : (minarg2))
#define Pui2(x) ((x)*(x))

// Définition de constantes
#define PI     3.14159265358979
#define ALPHA  1.0e-4
#define MINETA 1.0e-4
#define MINTOL 1e-7
#define MAXEXP 400.0
#define GTOL   1e-3
#define MAXSTR 1e8
#define Emax   10e+10


// The structure of the learning parameters 
typedef struct Learning_param {
  double   eps;     // Precision
  double   eta;     // Learning rate 
  long int T;       // Maximum number of iterations
  long int p;       // Number of thresholds - decision stumps 
  long int K;       // Number of classes (or groups in the unsupervised case) 
  double   lambda;  // Implication factor of unlabeled examples in the objective (semi-supervised learning)
  int      display; // Display of current error
} LR_PARAM;


typedef struct IndxVal {
  long int  ind;	//  Indice d'une caractéristique non-nulle d'un vecteur creux      
  double    val;    //  Valeur associée      
} FEATURE;

typedef struct SV {
  FEATURE  *Att;    // Ensemble des caractéristiques non-nulles d0'un vecteur creux 
  long int N;       // Nombre de caractéristiques non-nulles d'un vecteur creux
} SPARSEVECT;

typedef struct Donnees {
  double   **X;   // Matrice des données
  double   *y;    // Vecteur contenant les étiquettes de classe
  double   **Y;   // Matrice contenant les vecteurs indicateurs de classe
  long int d;     // Dimension de l'espace d'entrée
  long int K;     // Nombre de classes   
  long int m;     // Nombre d'exemples étiquetés de la base
  long int u;     // Nombre d'exemples non-étiquetés de la base
} DATA;


double A(double);
double Aprime(double);
void adaline(DATA, double *, double, long int);
void aide();
void *Allocation(size_t);
double **AlloueMatrice(long int, long int);
void Architecture(long int *, long int *);
void AutoApprentissageRL(double *, DATA, LR_PARAM);
long int Boosting(DATA, DATA, double *, double **, double *, LR_PARAM);
void CEMss_NaiveBayes(SPARSEVECT *, long int *, SPARSEVECT *, long int, long int, long int, long int, double , double, long int, int, double **, double *);
void ChargeFicBaseMltClass(char *, DATA *);
void ChrgMatrix(char *, DATA);
void ChrgMatrixUnSup(char *, long int, long int, double **);
void ChrgSparseMatrix(char *, SPARSEVECT *, long int *, long int);
void ChrgSparseMatrixRB(char *, SPARSEVECT *, double *, double *, long int *, long int);
void ChrgSparseMatrixSemiSup(char *, SPARSEVECT *, SPARSEVECT *, long int *, long int, long int);
void ConstructTrainTest(char *, double **, double *, long int, long int, double);
void FileScan(char *, long int *, long int *);
void FileScanMltCls(char *, long int *, long int *, long int *);
void FileScanSS(char *, long int *, long int *, long int *);
void FileSparseVectors(char *, long int *, long int *, long int *, long int *);
void grdcnj(double (* )(double *, DATA), double* (* )(double *, DATA),DATA, double *,  double, int);
void Kmoyennes(double **, long int, long int, long int, long int **, double **, long int *, double, long int , int);
void KNN(SPARSEVECT *, long int *, long int, SPARSEVECT *, long int *, long int, long int, char *);
void LibereMatrice(double **, long int, long int);
void lire_commande(LR_PARAM *, char *, char *, int , char **);
void lire_commande_CEMss_naiveBayes(LR_PARAM *, char *, char *, int, char **);
void lire_commande_kmoyennes(LR_PARAM *, char *, char *, int, char **);
void lire_commande_RankBoost(LR_PARAM *, char *, char *, int, char **);
void lire_commande_naiveBayes_Tst(char *, char *, int, char **);
void load_adaboost_params(char *,double **,double *, long int, long int);
void load_params(char *,double *,long int);
void MarginPerceptron(double **, double *, double *, long int, long int, double, long int, double);
void NaiveBayesTest(SPARSEVECT *, long int   *, long int, long int, long int, double **, double *);
void Normalize(double **, long int, long int);
void nrerror(char *);
void perceptron(double *, DATA, LR_PARAM);
void PMC_stochastique(DATA, long int, long int *, double ***, LR_PARAM, char *);
void Propagation(DATA, long int, long int *, double ***, char *);
void qsnewton(double (* )(double *, DATA), double* (* )(double *, DATA),DATA,double *, double);
void RankBoost(SPARSEVECT *, long int *, long int, long int, LR_PARAM, double *, long int *, double *, double *, double *, double *,  double *, char *);
void RegressionLogistique(double *, DATA, LR_PARAM);
void rchln(double (* )(double *, DATA), double* (* )(double *,DATA),double *, double, double *, double *, double *, double *, DATA);
void save_params(char *,double *,long int);
void save_adaboost_params(char *,double **,double *, long int, long int);
void SemiSupKNN(double *, DATA, LR_PARAM);
void SparseFileScanSemiSup(char *, long int *, long int *, long int *, long int *, long int *);
double *ssGradientofLogisticSurrogate(double *, DATA);
double ssLogisticSurrogate(double *, DATA);
void Standby();
void WeakLearner(SPARSEVECT *, long int *, long int, long int, double *,  long int, double *, double *, long int *, double *, double *,double *);
