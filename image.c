/*
 * image.cpp
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include "image.h"

#define NOTHING_IMAGE (image){.pixels = NULL, .header=0}
#define NOTHING_SECRET (secret_key){.secret_r0 = 0, .SV = 0}

/*
 * Preluare rutei catre program
 * Returneaza:
 * false - daca nu s-a putut prelua ruta programului
 * true - daca nu a fost nici-o problema
 */
bool get_route(char route[PATH_MAX]) {
    getcwd(route, PATH_MAX);
    if (getcwd(route, PATH_MAX) == NULL) return false;
    strcat(route, "\\");
    return true;
}

/*
 * https://en.wikipedia.org/wiki/Xorshift
 */
uint32_t xorshift32(uint32_t state[static 1]) {
    uint32_t x = state[0];
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state[0] = x;
    return x;
}

/*
 * Incarcarea imaginii in memoria interna.
 *
 * Parametri:
 * path_to_image - numele fisierului
 *
 * Returneaza o structura image cu valorile corespunzatoare fiecarui camp
 * sau o structura goala in cazul in care nu s-a putut face citirea.
 */
image load_image(char* path_to_image) {
    int32_t k;
    unsigned char byte;
    image tmp_image;
    char route[PATH_MAX];
    FILE *file;

    // Incarcarea rutei catre program in variabila route
    if (get_route(route) == false) {
        printf("Eroare la gasirea rutei programului.\n");
        return NOTHING_IMAGE;
    }
    strcat(route, path_to_image);

    // Deschiderea imaginii
    file = fopen(route,"rb");
    if (file == NULL) {
        printf("Eroare la gasirea imaginii.\n");
        return NOTHING_IMAGE;
    }
    // Salvam fiecare element al header-ului
    fseek(file, 0, SEEK_SET);
    fread(&(tmp_image.header.signature), 1, 2, file);
    fread(&(tmp_image.header.size), 1, 4, file);
    fread(&(tmp_image.header.reserved1), 1, 2, file);
    fread(&(tmp_image.header.reserved2), 1, 2, file);
    fread(&(tmp_image.header.offset), 1, 4, file);
    fread(&(tmp_image.header.bitmapinfo), 1, 4, file);
    fread(&(tmp_image.header.width), 1, 4, file);
    fread(&(tmp_image.header.height), 1, 4, file);
    fread(&(tmp_image.header.no_planes), 1, 2, file);
    fread(&(tmp_image.header.no_bits_pixel), 1, 2, file);
    fread(&(tmp_image.header.compression), 1, 4, file);
    fread(&(tmp_image.header.size_padding), 1, 4, file);
    fread(&(tmp_image.header.x_pixel_meter), 1, 4, file);
    fread(&(tmp_image.header.y_pixel_meter), 1, 4, file);
    fread(&(tmp_image.header.no_colors), 1, 4, file);
    fread(&(tmp_image.header.no_imp_colors), 1, 4, file);


    // Alocam memorie necesara pentru salvarea imaginii in memoria interna
    tmp_image.pixels = (image_colors*) calloc((size_t)tmp_image.header.width * (size_t)tmp_image.header.height, sizeof(image_colors));
    if (tmp_image.pixels == NULL) {
        printf("Eroare la alocarea de memorie necesara pentru imagine.\n");
        return NOTHING_IMAGE;
    }



    // Salvam fiecare valoare a fiecarui pixel in variabila declarata mai sus
    for (k = 0; k < 3*tmp_image.header.width*tmp_image.header.height; k ++) {
        fread(&byte, 1, 1, file);
        switch (k%3) {
            case 0:
                (*(tmp_image.pixels+(k/3))).B = byte;
                break;
            case 1:
                (*(tmp_image.pixels+(k/3))).G = byte;
                break;
            case 2:
                (*(tmp_image.pixels+(k/3))).R = byte;
                break;
            default:
                break;
        }
    }
    // Inchidem fisierul deschis
    fclose(file);
    return tmp_image;
}

/*
 * Salvarea imaginii in memoria externa.
 *
 * Parametri:
 * path_to_save - numele fisierului
 * image - structura care are informatia
 *
 * Returneaza:
 * false - daca nu s-a putut salva imaginea
 * true - daca nu a fost nici-o problema
 */
bool save_image(char *path_to_save, image image) {
    char route[PATH_MAX];
    int32_t k;
    FILE *file;

    // Incarcarea rutei catre program in variabila route
    if (get_route(route) == false) {
        printf("Eroare la gasirea rutei programului.\n");
        return false;
    }
    strcat(route, path_to_save);

    // Deschiderea imaginii
    file = fopen(route,"wb");
    if (file == NULL) {
        printf("Eroare la scrierea imaginii.\n");
        return false;
    }

    fwrite(&(image.header.signature), 1, 2, file);
    fwrite(&(image.header.size), 1, 4, file);
    fwrite(&(image.header.reserved1), 1, 2, file);
    fwrite(&(image.header.reserved2), 1, 2, file);
    fwrite(&(image.header.offset), 1, 4, file);
    fwrite(&(image.header.bitmapinfo), 1, 4, file);
    fwrite(&(image.header.width), 1, 4, file);
    fwrite(&(image.header.height), 1, 4, file);
    fwrite(&(image.header.no_planes), 1, 2, file);
    fwrite(&(image.header.no_bits_pixel), 1, 2, file);
    fwrite(&(image.header.compression), 1, 4, file);
    fwrite(&(image.header.size_padding), 1, 4, file);
    fwrite(&(image.header.x_pixel_meter), 1, 4, file);
    fwrite(&(image.header.y_pixel_meter), 1, 4, file);
    fwrite(&(image.header.no_colors), 1, 4, file);
    fwrite(&(image.header.no_imp_colors), 1, 4, file);

    for (k = 0; k < image.header.width*image.header.height; k ++) {
        fwrite(&((*(image.pixels+k)).B), 1, 1, file);
        fwrite(&((*(image.pixels+k)).G), 1, 1, file);
        fwrite(&((*(image.pixels+k)).R), 1, 1, file);
    }
    fclose(file);

    return true;
}

/*
 * Citirea cheilor secrete din fisier.
 *
 * Parametri:
 * path_to_secret - numele fisierului cu cheiile secrete
 *
 * Returneaza o structura secret_key cu valorile corespunzatoare fiecarui camp
 * sau o structura goala in cazul in care nu s-a putut face citirea.
 */
secret_key get_secret_key(char *path_to_secret) {
    FILE *file;
    secret_key temp_secret;
    char route[PATH_MAX];

    // Incarcarea rutei catre program in variabila route
    if (get_route(route) == false) {
        printf("Eroare la gasirea rutei programului.\n");
        return NOTHING_SECRET;
    }
    // Concatenarea rutei cu numele fisierului
    strcat(route, path_to_secret);

    file = fopen(route, "r");
    if (file == NULL) {
        printf("Nu s-a putut citi fisierul cu cheiea secreta.");
        return NOTHING_SECRET;
    }
    fscanf(file, "%" SCNu32 " %" SCNu32 "", &(temp_secret.secret_r0), &(temp_secret.SV));
    return temp_secret;
}

/*
 * Genereaza cele 2*W*H-1 valori pseudo-random folosind algoritmul xorshift32.
 *
 * Parametri:
 * seed - valoarea cheii secrete
 * block_size - marimea w*h a imaginii
 *
 * Returneaza:
 * pointer catre inceputul tabloului de valori pseudo-random
 * sau pointerul NULL daca nu s-a putut face alocarea de memorie.
 */
uint32_t* generate_random_values(uint32_t seed, uint32_t block_size) {
    uint32_t *r = (uint32_t*) calloc(block_size, sizeof(uint32_t));
    size_t i;
    if (r == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru valorile pseudo-random.");
        return NULL;
    }
    *r = seed;
    for (i = 1; i < block_size; i ++) {
        *(r + i) = xorshift32(&seed);
    }
    return r;
}

/*
 * Genereaza permutarea necesara pentru interschimbarea pixelilor.
 *
 * Paramteri:
 * *r - pointer la inceputul tabloului de valori pseudo-aleatoare
 * block_size - marimea w*h a imaginii
 *
 * Returneaza:
 * un pointer catre inceputul tabloului de permutari
 * sau pointerul NULL daca nu s-a putut face alocarea de memorie
 */
uint32_t* generate_permutation(uint32_t* r, uint32_t block_size) {
    uint32_t *permutation = (uint32_t*) calloc(block_size, sizeof(uint32_t)), i, random;
    if (permutation == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru permutari.");
        return NULL;
    }
    for (i = 0; i < block_size; i ++)
        *(permutation+i) = i;

    for (i = block_size; i > 0; i --) {
        random = *(r+i)%i;
        uint32_t temp = *(permutation+i);
        *(permutation+i) = *(permutation+random);
        *(permutation+random) = temp;
    }
    return permutation;
}

/*
 * Se realizeaza copierea header-ului imaginii originale in cea criptata,
 * se permuta fiecare pixel si se cripteaza conform problemei.
 *
 * Parametri:
 * real_image - structura imaginii care trebuie criptata
 * *r - pointer la inceputul tabloului de valori pseudo-aleatoare
 * *permutation - pointer la inceputul permutarii
 * SV - cheia secreta
 *
 * Se presupune ca *r si *permutation au marimile necesare altfel nu se
 * ajunge pana la apelul acestei functii.
 *
 * Returneaza imaginea criptata cu valorile corespunzatoare fiecarui camp
 * sau o structura goala in cazul in care nu s-a putut aloca numarul de pixeli necesari.
 */
image crypting_method(image real_image, uint32_t *r, uint32_t *permutation, uint32_t SV) {
    image ciphered_image;
    uint32_t size = (uint32_t)real_image.header.width * real_image.header.height , i;

    ciphered_image.header = real_image.header;
    ciphered_image.pixels = (image_colors*) calloc(size, sizeof(image_colors));
    if (ciphered_image.pixels == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru imaginea criptata.");
        return NOTHING_IMAGE;
    }
    // Permutarea pixelilor conform permutari
    for (i = 0; i < size; i ++)
        (*(ciphered_image.pixels + *(permutation+i))) = (*(real_image.pixels + i));

    // Initializarea primului pixel conform C(0)=SV^P'(0)^r(w*h)
    (*(ciphered_image.pixels)).R = (unsigned char)((SV)^(*(ciphered_image.pixels)).R^(*(r+size)));
    (*(ciphered_image.pixels)).G = (unsigned char)((SV)^(*(ciphered_image.pixels)).G^(*(r+size)));
    (*(ciphered_image.pixels)).B = (unsigned char)((SV)^(*(ciphered_image.pixels)).B^(*(r+size)));

    // Generarea restului de pixeli conform C(k)=C(k-1)^P'(k)^r(w*h+k)
    for (i = 1; i < (uint32_t)(real_image.header.width*real_image.header.height); i ++) {
        (*(ciphered_image.pixels + i)).R = (unsigned char)(((*(ciphered_image.pixels + (i-1))).R)^((*(ciphered_image.pixels)).R)^(*(r+size+i)));
        (*(ciphered_image.pixels + i)).G = (unsigned char)(((*(ciphered_image.pixels + (i-1))).G)^((*(ciphered_image.pixels)).G)^(*(r+size+i)));
        (*(ciphered_image.pixels + i)).B = (unsigned char)(((*(ciphered_image.pixels + (i-1))).B)^((*(ciphered_image.pixels)).B)^(*(r+size+i)));
    }
    return ciphered_image;
}

/*
 * Functia de criptare a imaginii
 *
 * Parametri:
 * path_to_image - imaginea ce urmeaza sa fie criptata
 * path_to_crypt - imaginea criptata
 * secret_path - fisierul ce contine cele 2 chei secrete
 *
 * Returneaza:
 * false - daca nu s-a putut crea imaginea criptata
 * true - daca nu a fost nici-o problema in crearea imaginii
 */
bool crypting_image(char *path_to_image, char *path_to_crypt, char *secret_path) {
    image image, image_ciphered;
    secret_key secret = get_secret_key(secret_path);
    image = load_image(path_to_image);
    uint32_t block_size = (uint32_t)image.header.width*image.header.height;
    uint32_t *r , *permutation;

    if ((r = generate_random_values(secret.secret_r0, 2*block_size)) == NULL) return false;
    if ((permutation = generate_permutation(r, block_size)) == NULL) return false;
    image_ciphered = crypting_method(image, permutation, r, secret.SV);
    save_image(path_to_crypt, image_ciphered);
    return true;
}
