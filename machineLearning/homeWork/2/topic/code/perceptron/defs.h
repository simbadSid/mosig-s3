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
  long int K;       // Nombre de groupes (apprentissage non-supervisé)
  double   lambda;  // Facteur d'implication des exemples non-étiquetés (apprentissage semi-supervisé)
  int      display; // Affichage de l'erreur courant
  
} LR_PARAM;

typedef struct IndxVal {
  long int  ind;	//  Indice d'une caractéristique non-nulle d'un vecteur creux      
  double    val;    //  Valeur associée      
} FEATURE;

typedef struct SV {
  FEATURE  *Att;    // Ensemble des caractéristiques non-nulles d'un vecteur creux 
  long int N;       // Nombre de caractéristiques non-nulles d'un vecteur creux
} SPARSEVECT;




void nrerror(char *);
void FileScan(char *, long int *, long int *);
void Normalize(double **, long int, long int);
void ConstructTrainTest(char *, double **, double *, long int, long int, double);
void ChrgMatrix(char *, long int, long int, double **, double *);
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
void FileScanSemiSup(char *, long int *, long int *, long int *, long int *, long int *);
void ChrgMatrixSemiSup(char *, SPARSEVECT *, SPARSEVECT *, long int *, long int, long int);
void Standby();
void aide();

// Procédure du gradient conjugué
void grdcnj(double **, double *, long int, long int, double *, double, int);
// Procédure de la recherche linéaire
void rchln(double *, double, double *, double *, double *, double *, double **, double *, long int, long int);
// Méthode de quasi-Newton
void qsnewton(double **, double *, long int, long int,double *, double);


// Fonction qui estime le vecteur gradient à un point
double *Gradient(double *, double **, double *, long int, long int); 
// Fonction qui calcule la fonction de coût sur une base d'apprentissage
double FoncLoss(double *, double **, double *, long int, long int); 

// Algorithmeq du perceptron
void perceptron(double **, double *, double *, long int, long int, double, long int);
void MarginPerceptron(double **, double *, double *, long int, long int, double, long int, double);
// Algorithme du perceptron
void winnow(double **, double *, double *, long int, long int, double, long int);

// Modèle régression logisitque
void RegressionLogistique(double **, double *, long int, long int, double *, double, int);
// Algorithme d'AdaBoost
long int Boosting(double **, double **, double *, double *, double *, double **, double *, long int, long int, LR_PARAM);
// algorithme des k-moyennes
void Kmoyennes(double **, long int, long int, long int, long int **, double **, long int *, double, long int , int);
// Modèle naive-Bayes semi-supervisé
void CEMss_NaiveBayes(SPARSEVECT *, long int *, SPARSEVECT *, long int, long int, long int, long int, double , double, long int, int, double **, double *);
void NaiveBayesTest(SPARSEVECT *, long int   *, long int, long int, long int, double **, double *);
void ChrgMatrixSparseVectors(char *, SPARSEVECT *, long int *, long int);

void FileSparseVectors(char *, long int *, long int *, long int *, long int *);
void lire_commande_naiveBayes_Tst(char *, char *, int, char **);
void *Allocation(size_t);

void KNN(SPARSEVECT *, long int *, long int, SPARSEVECT *, long int *, long int, long int, char *);

