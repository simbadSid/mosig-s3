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

// Définition de constantes
#define PI     3.14159265358979
#define ALPHA  1.0e-4
#define MINETA 1.0e-4
#define MINTOL 1e-7
#define MAXEXP 400.0
#define GTOL      1e-3
#define MAXSTR    1e8



// Définition de la structure des paramètres pour la regression logistique
typedef struct Learning_param {
  double   eps;     // Précision
  double   eta;     // Pas d'apprentissage
  long int T;       // Nombre d'itérations maximal
  long int p;       // Nombre de seuils - decision stumps 
  long int K;       // Nombre de classes (ou de groupes dans le cas non-supervisé) 
  double   lambda;  // Facteur d'implication des exemples non-étiquetés (apprentissage semi-supervisé)
  int      display; // Affichage de l'erreur courant
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


void nrerror(char *);
void FileScan(char *, long int *, long int *);
void FileScanSS(char *, long int *, long int *, long int *);
void Normalize(double **, long int, long int);
void ConstructTrainTest(char *, double **, double *, long int, long int, double);
void ChrgMatrix(char *, DATA);
void ChrgMatrixUnSup(char *, long int, long int, double **);
void lire_commande(LR_PARAM *, char *, char *, int , char **);
void lire_commande_kmoyennes(LR_PARAM *, char *, char *, int, char **);
void save_params(char *,double *,long int);
void load_params(char *,double *,long int);
void save_adaboost_params(char *,double **,double *, long int, long int);
void load_adaboost_params(char *,double **,double *, long int, long int);
void Normalize(double **, long int, long int);
void ConstructTrainTest(char *, double **, double *, long int, long int, double);
void lire_commande_CEMss_naiveBayes(LR_PARAM *, char *, char *, int, char **);
void SparseFileScanSemiSup(char *, long int *, long int *, long int *, long int *, long int *);
void ChrgSparseMatrixSemiSup(char *, SPARSEVECT *, SPARSEVECT *, long int *, long int, long int);
void ChrgSparseMatrix(char *, SPARSEVECT *, long int *, long int);
void ChrgSparseMatrixRB(char *, SPARSEVECT *, double *, double *, long int *, long int);
double *ssGradientofLogisticSurrogate(double *, DATA);
double ssLogisticSurrogate(double *, DATA);
void SemiSupKNN(double *, DATA, LR_PARAM);
void Standby();
void aide();

// Procédure du gradient conjugué
void grdcnj(double (* )(double *, DATA), double* (* )(double *, DATA),DATA, double *,  double, int);
// Procédure de la recherche linéaire
void rchln(double (* )(double *, DATA), double* (* )(double *,DATA),double *, double, double *, double *, double *, double *, DATA);
// Méthode de quasi-Newton
void qsnewton(double (* )(double *, DATA), double* (* )(double *, DATA),DATA,double *, double);



// Algorithmeq du perceptron
void perceptron(double *, DATA, LR_PARAM);
void adaline(DATA, double *, double, long int);

void MarginPerceptron(double **, double *, double *, long int, long int, double, long int, double);

// Modèle régression logisitque
void RegressionLogistique(double *, DATA, LR_PARAM);
// Algorithme d'AdaBoost
long int Boosting(DATA, DATA, double *, double **, double *, LR_PARAM);
// algorithme des k-moyennes
void Kmoyennes(double **, long int, long int, long int, long int **, double **, long int *, double, long int , int);
// Modèle naive-Bayes semi-supervisé
void CEMss_NaiveBayes(SPARSEVECT *, long int *, SPARSEVECT *, long int, long int, long int, long int, double , double, long int, int, double **, double *);
void NaiveBayesTest(SPARSEVECT *, long int   *, long int, long int, long int, double **, double *);

void FileSparseVectors(char *, long int *, long int *, long int *, long int *);
void lire_commande_naiveBayes_Tst(char *, char *, int, char **);
void *Allocation(size_t);
double **AlloueMatrice(long int, long int);
void LibereMatrice(double **, long int, long int);

void KNN(SPARSEVECT *, long int *, long int, SPARSEVECT *, long int *, long int, long int, char *);
void AutoApprentissageRL(double *, DATA, LR_PARAM);


void lire_commande_RankBoost(LR_PARAM *, char *, char *, int, char **);
void RankBoost(SPARSEVECT *, long int *, long int, long int, LR_PARAM, double *, long int *, double *, double *, double *, double *,  double *, char *);
void WeakLearner(SPARSEVECT *, long int *, long int, long int, double *,  long int, double *, double *, long int *, double *, double *,double *);
