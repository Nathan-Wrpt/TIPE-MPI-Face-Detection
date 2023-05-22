#include "kpp.h"

database* db;

double distanceeuclidienne(Image image1, Image image2) {
    double distance = 0.0;
    
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            int redDiff = image1.red[i][j] - image2.red[i][j];
            int greenDiff = image1.green[i][j] - image2.green[i][j];
            int blueDiff = image1.blue[i][j] - image2.blue[i][j];
            
            distance += redDiff * redDiff + greenDiff * greenDiff + blueDiff * blueDiff; 
        }
    }

    distance = sqrt(distance);
    return distance;
}


kd_node* build_kd_tree_from_points(point** points, int start, int end, int depth) {
    if (start >= end) {
        return NULL;
    }
    int axis = depth % DIMENSIONS; // choix de l'axe de division
    int mid = (start + end) / 2; // choix du point médian
    kd_node* node = (kd_node*) malloc(sizeof(kd_node));
    node->point = points[mid]->values;
    node->index = points[mid]->index;
    node->left = build_kd_tree_from_points(points, start, mid, depth+1);
    node->right = build_kd_tree_from_points(points, mid+1, end, depth+1);
    return node;
}

kd_node* build_kd_tree_from_database(database db) {
    point** points = (point**) malloc(sizeof(point*) * (db.taille1 + db.taille2));
    int count = 0;
    for (int i = 0; i < db.taille1; i++) {
        double* values = (double*) malloc(sizeof(double) * DIMENSIONS);
        int index = i;
        int count2 = 0;
        for (int j = 0; j < TAILLE; j++) {
            for (int k = 0; k < TAILLE; k++) {
                values[count2] = db.visages[i].red[j][k];
                count2++;
                values[count2] = db.visages[i].green[j][k];
                count2++;
                values[count2] = db.visages[i].blue[j][k];
                count2++;
            }
        }
        point* p = (point*) malloc(sizeof(point));
        p->values = values;
        p->index = index;
        points[count++] = p;
    }
    for (int i = 0; i < db.taille2; i++) {
        double* values = (double*) malloc(sizeof(double) * DIMENSIONS);
        int index = i + db.taille1;
        int count2 = 0;
        for (int j = 0; j < TAILLE; j++) {
            for (int k = 0; k < TAILLE; k++) {
                values[count2] = db.non_visages[i].red[j][k];
                count2++;
                values[count2] = db.non_visages[i].green[j][k];
                count2++;
                values[count2] = db.non_visages[i].blue[j][k];
                count2++;
            }
        }
        point* p = (point*) malloc(sizeof(point));
        p->values = values;
        p->index = index;
        points[count++] = p;
    }
    kd_node* root = build_kd_tree_from_points(points, 0, db.taille1 + db.taille2, 0);
    return root;
}


void free_kd_tree(kd_node* node) {
    if (node == NULL) {
        return;
    }
    free_kd_tree(node->left);
    free_kd_tree(node->right);
    free(node->point);
    free(node);
}

char* get_image_type(kd_node* node, database* db) {
    if (node->index < db->taille1) {
        return "visages";
    } else {
        return "non_visages";
    }
}

double distance(point p1, kd_node* p2, int dimensions) {
    double sum = 0.0;
    for (int i = 0; i < dimensions; i++) {
        double diff = p1.values[i] - p2->point[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void findNearestNeighbors(kd_node* root, point query, int k, int dimensions, neighbor* neighbors, double* maxDistance) {
    if (root == NULL) {
        return;
    }

    // Calcul de la dimension actuelle
    int currentDimension = 0; // Supposant que 0 est la première dimension
    double dist = distance(query, root, dimensions);

    // Mise à jour des voisins les plus proches si nécessaire
    if (dist < *maxDistance) {
        // Trouver la position pour insérer le nouveau voisin
        int position = k;
         while (position > 0 && neighbors[position - 1].distance > dist) {
            position--;
        }

        // Insérer le nouveau voisin à la position appropriée
        for (int i = k - 1; i > position; i--) {
            neighbors[i] = neighbors[i - 1];
        }

        neighbors[position].distance = dist;
        neighbors[position].p = query;

        // Mise à jour de la distance maximale
        *maxDistance = neighbors[k - 1].distance;
    }

    // Choix du sous-arbre qui contient le point initial
    kd_node* nearSubtree;
    kd_node* farSubtree;
    if (query.values[currentDimension] < root->point[currentDimension]) {
        nearSubtree = root->left;
        farSubtree = root->right;
    } else {
        nearSubtree = root->right;
        farSubtree = root->left;
    }

    // Recherche de manière récursive du sous-arbre qui contient le point initial
    findNearestNeighbors(nearSubtree, query, k, dimensions, neighbors, maxDistance);

    // Vérifie si l'autre partie du sous-arbre peut contenir des points plus proches
    if (fabs(query.values[currentDimension] - root->point[currentDimension]) < *maxDistance) {
        findNearestNeighbors(farSubtree, query, k, dimensions, neighbors, maxDistance);
    }
}

int classify_point(database db, point new_point, int k, int dimensions) {
    kd_node* root = build_kd_tree_from_database(db);
    neighbor* neighbors = malloc(sizeof(neighbor) * k);
    double max_distance = INFINITY;
    findNearestNeighbors(root, new_point, k, dimensions, neighbors, &max_distance);
    int visage_count = 0;
    int non_visage_count = 0;
    for (int i = 0; i < k; i++) {
        if (neighbors[i].p.index < db.taille1) {
            visage_count++;
        } else {
            non_visage_count++;
        }
    }
    free(neighbors);
    free_kd_tree(root);
    if (visage_count > non_visage_count) {
        return 1;
    } else {
        return 0;
    }
}

point* image_to_point(Image image){
    double* values = (double*) malloc(sizeof(double) * DIMENSIONS);
        int count2 = 0;
        for (int j = 0; j < TAILLE; j++) {
            for (int k = 0; k < TAILLE; k++) {
                values[count2++] = image.red[j][k];
                values[count2++] = image.green[j][k];
                values[count2++] = image.blue[j][k];
            }
        }
        point* p = (point*) malloc(sizeof(point));
        p->values = values;
        p->index = -1;
}

double evaluate_performance(database db, int k, int dimensions) {
    int correct_count = 0;
    for (int i = 0; i < db.taille1; i++) {
        point* p = image_to_point(db.visages[i]);
        if (classify_point(db, *p, k, dimensions) == 1) {
            correct_count++;
        }
    }
    for (int i = 0; i < db.taille2; i++) {
        point* p = image_to_point(db.non_visages[i]);
        if (classify_point(db, *p, k, dimensions) == 0) {
            correct_count++;
        }
    }
    return (double) correct_count / (db.taille1 + db.taille2);
}

int main() {
    db = read_database("data.txt");
    printf("Performance: %f\n", evaluate_performance(*db, 5, DIMENSIONS));
    
    return 0;
}
