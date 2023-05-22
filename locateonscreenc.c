#include "locateonscreen.h"

typedef struct {
    int** red;
    int** green;
    int** blue;
} Image;

//Fonction qui à partir de coordonées x et y renvoie une fenetre de taille taillextaille de l'image en 1920x1080
Image extract(Image pic, int x, int y, int taille){
    Image fenetre;
    fenetre.red = malloc(taille*sizeof(int*));
    fenetre.green = malloc(taille*sizeof(int*));
    fenetre.blue = malloc(taille*sizeof(int*));
    int i = 0;
    while(i<taille){
        int j = 0;
        while(j<taille){
            fenetre.red[i][j] = pic.red[x+i][y+j];
            fenetre.green[i][j] = pic.green[x+i][y+j];
            fenetre.blue[i][j] = pic.blue[x+i][y+j];
            j++;
        }
        i++;
    }
    return fenetre;
}

//Fonction qui calcule la distance_euclidienne entre deux images de taille taillextaille
double distanceeuclidienne(Image image1, Image image2, int taille) {
    double distance = 0.0;
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            int redDiff = image1.red[i][j] - image2.red[i][j];
            int greenDiff = image1.green[i][j] - image2.green[i][j];
            int blueDiff = image1.blue[i][j] - image2.blue[i][j];
            
            distance += redDiff * redDiff + greenDiff * greenDiff + blueDiff * blueDiff; 
        }
    }

    distance = sqrt(distance);
    return distance;
}


//Fonction qui à partir d'une image de taille 1920*1080, déplacer une fenêtre de taille tfenetre*tfenetre sur toute l'image grâce à extract et qui va s'arrêter si la fonction distance_euclidienne est égale à zéro. La fonction renverra la plus petite distance euclidienne trouvée et la fenêtre pour laquelle c'est le cas.
double detecte_image(Image pic, Image smallpic, int tfenetre){
    int i = 0;
    double min = INFINITY;
    while(i<LONGUEUR-tfenetre){
        int j = 0;
        while(j<HAUTEUR-tfenetre){
            Image fenetre = extract(pic, i, j, tfenetre);
            double curdist = distanceeuclidienne(fenetre, smallpic, tfenetre);
            if(curdist == 0.0){
                return 0.0;
            }else{
                if(curdist<min){
                    min = curdist;
                }
            }
            j++;
        }
        i++;
    }
    return min;
}

Image read_image(char* nom_image){
    IplImage* originalImage = cvLoadImage(nom_image, CV_LOAD_IMAGE_COLOR);
    if (!originalImage) {
        printf("Could not open or find the image.\n");
        return -1;
    }
    Image image;
    for (int i = 0; i < originalImage->height; i++) {
        for (int j = 0; j < originalImage->width; j++) {
            CvScalar pixel = cvGet2D(originalImage, i, j);
            image.red[i][j] = (int)pixel.val[2];    // R
            image.green[i][j] = (int)pixel.val[1];  // G
            image.blue[i][j] = (int)pixel.val[0];   // B
        }
    }
    return image;
}

void free_image(Image image){
    free(image.red);
    free(image.green);
    free(image.blue);
}

int main() {
    Image pic = read_image("test_image.jpg");
    Image smallpic = read_image("test_small_image.jpg");
    int tfenetre = 100;
    double min_distance = detecte_image(pic, smallpic, tfenetre);
    printf("Minimum distance: %f\n", min_distance);
    return 0;
}