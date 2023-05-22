// kpp.c sert à évaluer la performance de notre adaptation de l'algorithme des k plus proches voisins sur une base de données d'images de visages et de non visages.

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE 100
#define DIMENSIONS 30000

// Structure pour représenter un nœud dans un arbre k-dimensionnel
typedef struct kd_node_t {
    double* point; // le point dans l'espace k-dimensionnel
    int index; // l'indice du point dans la base de données
    struct kd_node_t* left; // l'enfant gauche du nœud
    struct kd_node_t* right; // l'enfant droit du nœud
} kd_node;

// Structure pour représenter un point dans l'espace k-dimensionnel
typedef struct {
    double* values; // les valeurs dans l'espace k-dimensionnel
    int index; // l'indice du point dans la base de données
} point;

//Structure pour représenter une image
typedef struct {
    int red[100][100];
    int green[100][100];
    int blue[100][100];
} Image;

//Structure pour représenter une base de données d'images
typedef struct {
    Image* visages;
    Image* non_visages;
    int taille1;
    int taille2;
} database;

// Structure pour représenter un voisin dans l'arbre KD
typedef struct {
    double distance; // la distance au point initial
    point p; // le point voisin
} neighbor;

//Fonction qui calcule la distance_euclidienne entre deux images de taille 100x100
double distanceeuclidienne(Image image1, Image image2);

//Fonction pour construire un arbre k-dimensionnel à partir d'un ensemble de points
kd_node* build_kd_tree_from_points(point** points, int start, int end, int depth);

// Fonction pour construire un arbre k-dimmensionnel à partir de la base de données
kd_node* build_kd_tree_from_database(database db);

//Fonction pour libérer la mémoire allouée pour un arbre k-dimensionnel
void free_kd_tree(kd_node* node);

//Fonction qui détermine si un nœud de l'arbre représente une image de db.visages ou de db.non_visages
char* get_image_type(kd_node* node, database* db);

//Fonction qui trouve les k nœuds de l'arbre les plus proches du new_point
double distance(point p1, kd_node* p2, int dimensions);

//Fonction qui trouve les k nœuds de l'arbre les plus proches du point
void findNearestNeighbors(kd_node* root, point query, int k, int dimensions, neighbor* neighbors, double* maxDistance);

//Fonction qui évalue si une image est de la classe visages ou non_visages en trouvant les 5 plus proches voisins de l'image dans l'arbre
int classify_point(database db, point new_point, int k, int dimensions);

//Fonction qui transforme une image en point pour pouvoir l'utiliser dans l'algorithme
point* image_to_point(Image image);

//Fonction qui évalue la performance de l'algorithme en calculant le pourcentage d'images correctement classifiées
double evaluate_performance(database db, int k, int dimensions);