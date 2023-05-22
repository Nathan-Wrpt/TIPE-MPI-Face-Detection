// locateonscreen.c contient les fonctions qui permettent de localiser une image sur une autre image

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <opencv2/opencv.h>

#define HAUTEUR 1080
#define LONGUEUR 1920

// Structure d'une image
typedef struct {
    int** red;
    int** green;
    int** blue;
} Image;

//Fonction qui à partir de coordonées x et y renvoie une fenetre de taille taillextaille de l'image en 1920x1080
Image extract(Image pic, int x, int y, int taille);

//Fonction qui calcule la distance_euclidienne entre deux images de taille taillextaille
double distanceeuclidienne(Image image1, Image image2, int taille);

//Fonction qui à partir d'une image de taille 1920*1080, déplacer une fenêtre de taille tfenetre*tfenetre sur toute l'image grâce à extract et qui va s'arrêter si la fonction distance_euclidienne est égale à zéro. La fonction renverra la plus petite distance euclidienne trouvée et la fenêtre pour laquelle c'est le cas.
double detecte_image(Image pic, Image smallpic, int tfenetre)

//Fonction qui lit une image grâce à la librairie opencv
Image read_image(char* nom_image);

//Fonction qui libère l'espace mémoire alloué à une image
void free_image(Image image);

