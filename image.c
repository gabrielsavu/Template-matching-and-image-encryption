/*
 * image.cpp
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
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
 * Calculul ariei dreptunghiului cu lungimea x respectiv latimea y
 *
 * Paramteri:
 * x - lungimea(latimea)
 * y - latimea(lungimea)
 */
static inline uint32_t area(uint32_t x, uint32_t y) {
    return x*y;
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
    int32_t i, j, contor = 0;
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
    if(tmp_image.header.width % 4 != 0) tmp_image.padding = 4 - (3 * tmp_image.header.width) % 4;
    else tmp_image.padding = 0;
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
    for (i = 0; i < tmp_image.header.height; i ++) {
        for (j = 0; j < tmp_image.header.width; j ++) {
            fread(&(*(tmp_image.pixels + tmp_image.header.width*(tmp_image.header.height-(i+1)) + j)).B, 1, 1, file);
            fread(&(*(tmp_image.pixels + tmp_image.header.width*(tmp_image.header.height-(i+1)) + j)).G, 1, 1, file);
            fread(&(*(tmp_image.pixels + tmp_image.header.width*(tmp_image.header.height-(i+1)) + j)).R, 1, 1, file);
            contor ++ ;
        }
        fseek(file, tmp_image.padding, SEEK_CUR);
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
    int32_t i, j, zero = 0, contor = 0;
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

    for (i = 0; i < image.header.height; i ++) {
        for (j = 0; j < image.header.width; j ++) {
            fwrite(&((*(image.pixels + image.header.width*(image.header.height-(i+1)) + j)).B), 1, 1, file);
            fwrite(&((*(image.pixels + image.header.width*(image.header.height-(i+1)) + j)).G), 1, 1, file);
            fwrite(&((*(image.pixels + image.header.width*(image.header.height-(i+1)) + j)).R), 1, 1, file);
            contor ++ ;
        }
        fwrite(&zero, 1, (size_t)image.padding, file);
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
uint32_t* generate_permutation(uint32_t const* r, uint32_t block_size) {

    uint32_t *permutation = (uint32_t*) calloc(block_size, sizeof(uint32_t)), i, random, contor = 1;
    if (permutation == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru permutari.");
        return NULL;
    }
    for (i = 0; i < block_size; i ++)
        *(permutation+i) = i;

    for (i = block_size - 1; i > 0; i --) {
        random = (*(r+contor))%(i+1);
        uint32_t temp = *(permutation+i);
        *(permutation+i) = *(permutation+random);
        *(permutation+random) = temp;
        contor ++;
    }

    return permutation;
}

/*
 * Genereaza inversa permutari necesara pentru interschimbarea pixelilor.
 *
 * Paramteri:
 * *permutation - permutarea pentru care trebuie sa se calculeze inversa
 * block_size - marimea w*h a imaginii
 *
 * Returneaza:
 * un pointer catre inceputul tabloului de permutari
 * sau pointerul NULL daca nu s-a putut face alocarea de memorie
 */
uint32_t* reverse_permutation(uint32_t const* permutation, uint32_t block_size) {
    uint32_t i = 0;
    uint32_t *rev_permutation = (uint32_t*)calloc(block_size, sizeof(uint32_t));
    if (rev_permutation == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru inversa permutari.");
        return NULL;
    }
    for(i = 0; i < block_size; i ++) {
        *(rev_permutation + *(permutation+i)) = i;
    }
    return rev_permutation;
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
image crypting_method(image real_image, uint32_t *r, uint32_t const* permutation, uint32_t SV) {
    image ciphered_image;
    uint32_t size = (uint32_t)real_image.header.width * real_image.header.height , i;

    ciphered_image.header = real_image.header;
    ciphered_image.padding = real_image.padding;
    ciphered_image.pixels = (image_colors*) calloc(size, sizeof(image_colors));
    if (ciphered_image.pixels == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru imaginea criptata.");
        return NOTHING_IMAGE;
    }

    // Permutarea pixelilor conform permutari
    for (i = 0; i < size; i ++)
        (*(ciphered_image.pixels + *(permutation+i))) = (*(real_image.pixels + i));


    // Initializarea primului pixel conform C(0)=SV^P'(0)^r(w*h)
    (*(ciphered_image.pixels)).B = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).B^((*(r+size))&0b11111111));
    SV = SV >> 8;
    (*(r+size)) = (*(r+size)) >> 8;
    (*(ciphered_image.pixels)).G = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).G^((*(r+size))&0b11111111));
    SV = SV >> 8;
    (*(r+size)) = (*(r+size)) >> 8;
    (*(ciphered_image.pixels)).R = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).R^((*(r+size))&0b11111111));


    // Generarea restului de pixeli conform C(k)=C(k-1)^P'(k)^r(w*h+k)
    for (i = 1; i < (uint32_t)(real_image.header.width*real_image.header.height); i ++) {
        (*(ciphered_image.pixels + i)).B = (unsigned char)(((*(ciphered_image.pixels + (i-1))).B)^((*(ciphered_image.pixels + i)).B)^((*(r+size+i))&0b11111111));
        (*(r+size+i)) = (*(r+size+i)) >> 8;
        (*(ciphered_image.pixels + i)).G = (unsigned char)(((*(ciphered_image.pixels + (i-1))).G)^((*(ciphered_image.pixels + i)).G)^((*(r+size+i))&0b11111111));
        (*(r+size+i)) = (*(r+size+i)) >> 8;
        (*(ciphered_image.pixels + i)).R = (unsigned char)(((*(ciphered_image.pixels + (i-1))).R)^((*(ciphered_image.pixels + i)).R)^((*(r+size+i))&0b11111111));


    }
    return ciphered_image;
}

/*
 * Se realizeaza copierea header-ului imaginii criptate in cea decriptata,
 * se decripteaza conform problemei si se permuta pixelii
 *
 * Parametri:
 * ciphered_image - structura imaginii care trebuie decriptata
 * *r - pointer la inceputul tabloului de valori pseudo-aleatoare
 * *permutation - pointer la inceputul permutarii inverse
 * SV - cheia secreta
 *
 * Se presupune ca *r si *permutation au marimile necesare altfel nu se
 * ajunge pana la apelul acestei functii.
 *
 * Returneaza imaginea decriptata cu valorile corespunzatoare fiecarui camp
 * sau o structura goala in cazul in care nu s-a putut aloca numarul de pixeli necesari.
 */
image decrypting_method(image ciphered_image, uint32_t * r, uint32_t const* permutation, uint32_t SV) {
    image tmp_image, real_image;
    uint32_t size = (uint32_t)ciphered_image.header.width * ciphered_image.header.height, i;
    tmp_image.header = ciphered_image.header;
    tmp_image.padding = ciphered_image.padding;
    real_image.header = ciphered_image.header;
    real_image.padding = ciphered_image.padding;
    tmp_image.pixels = (image_colors*) calloc(size, sizeof(image_colors));
    real_image.pixels = (image_colors*) calloc(size, sizeof(image_colors));

    if (tmp_image.pixels == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru imaginea decriptata.");
        return NOTHING_IMAGE;
    }
    if (real_image.pixels == NULL) {
        printf("Nu s-a putut aloca memorie necesara pentru imaginea decriptata.");
        return NOTHING_IMAGE;
    }

    (*(tmp_image.pixels)).B = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).B^(*(r+size)));
    SV = SV >> 8;
    (*(tmp_image.pixels)).G = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).G^(*(r+size)));
    SV = SV >> 8;
    (*(tmp_image.pixels)).R = (unsigned char)((SV&0b11111111)^(*(ciphered_image.pixels)).R^(*(r+size)));

    for (i = 1; i < size; i ++) {
        (*(tmp_image.pixels + i)).B = (unsigned char)(((*(ciphered_image.pixels + (i-1))).B)^((*(ciphered_image.pixels + i)).B)^((*(r+size+i))&0b11111111));
        ((*(r+size+i))) = ((*(r+size+i))) >> 8;
        (*(tmp_image.pixels + i)).G = (unsigned char)(((*(ciphered_image.pixels + (i-1))).G)^((*(ciphered_image.pixels + i)).G)^((*(r+size+i))&0b11111111));
        ((*(r+size+i))) = ((*(r+size+i))) >> 8;
        (*(tmp_image.pixels + i)).R = (unsigned char)(((*(ciphered_image.pixels + (i-1))).R)^((*(ciphered_image.pixels + i)).R)^((*(r+size+i))&0b11111111));

    }
    for (i = 0; i < size; i ++)
        (*(real_image.pixels + *(permutation+i))) = (*(tmp_image.pixels + i));

    free(tmp_image.pixels);
    return real_image;
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
    image_ciphered = crypting_method(image, r, permutation, secret.SV);
    save_image(path_to_crypt, image_ciphered);

    //Eliberam memoria
    free(r);
    free(permutation);
    free(image.pixels);
    free(image_ciphered.pixels);
    return true;
}

/*
 * Functia de decriptare a imaginii
 *
 * Parametri:
 * path_to_image - imaginea ce urmeaza sa fie decriptata
 * path_to_decrypt - imaginea decriptata
 * secret_path - fisierul ce contine cele 2 chei secrete
 *
 * Returneaza:
 * false - daca nu s-a putut crea imaginea criptata
 * true - daca nu a fost nici-o problema in crearea imaginii
 */
bool decrypting_image(char *path_to_image, char *path_to_decrypt, char *secret_path) {
    image image, image_deciphered;
    secret_key secret = get_secret_key(secret_path);
    image = load_image(path_to_image);
    uint32_t block_size = (uint32_t)image.header.width*image.header.height;
    uint32_t *r , *permutation, *rev_permutation;

    if ((r = generate_random_values(secret.secret_r0, 2*block_size)) == NULL) return false;
    if ((permutation = generate_permutation(r, block_size)) == NULL) return false;
    if ((rev_permutation = reverse_permutation(permutation, block_size)) == NULL) return false;
    // Eliberam memoria de care nu mai avem nevoie pentru a face mai mult loc
    free(permutation);
    image_deciphered = decrypting_method(image, r, rev_permutation, secret.SV);
    save_image(path_to_decrypt, image_deciphered);
    //Eliberam memoria
    free(r);
    free(rev_permutation);
    free(image.pixels);
    free(image_deciphered.pixels);
    return true;
}

/*
 * Calculul sumei chisquare
 *
 * Parametri:
 * img - imaginea pe care se face calculul
 * fm - constanta width*height/256
 * chanel - canalul pe care se face calculul(R,G,B)
 * *expression - expresia de sub suma
 */
float sigma_chitest(image img, int n, float fm, unsigned char chanel, float (*expression)(image, float, uint32_t, unsigned char)) {
    uint32_t i = 0;
    float s = 0;
    for (i = 0; i < n; i ++) {
        s = s + expression(img, fm, i, chanel);
    }
    return s;
}

/*
 * Expresia de sub sigma
 */
float expression(image image, float fm, uint32_t i, unsigned char chanel) {
    int32_t k;
    float fi = 0;
    for (k = 0; k < image.header.height*image.header.width; k ++) {
        switch (chanel) {
            case 'R':
                if ((*(image.pixels + k)).R == i) fi += 1;
                break;
            case 'G':
                if ((*(image.pixels + k)).G == i) fi += 1;
                break;
            case 'B':
                if ((*(image.pixels + k)).B == i) fi += 1;
                break;
            default:
                return 0;
        }
    }
    return (((fi-fm)*(fi-fm))/fm);
}

/*
 * Calculul testului chi-patrat
 *
 * Parametri:
 * path_to_image - numele imaginii
 */
void chisquare_test(char *path_to_image) {
    image img;
    img = load_image(path_to_image);
    printf("(%f, %f, %f)", sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'R', expression),
           sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'G', expression),
           sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'B', expression)
    );
    //Eliberam memoria
    free(img.pixels);
}


/*
 * Transformarea imaginii path_to_image in imagine grayscale
 *
 * Paramteri:
 * path_to_image - locatia catre imaginea ce urmeaza sa se transforme
 * path_to_grey - loactia catre imaginea grayscale
 */
void grayscale_image(char* path_to_image, char* path_to_grey) {
    image img;
    unsigned char aux;
    img = load_image(path_to_image);
    uint32_t size = (uint32_t)img.header.width * img.header.height, i;
    // Transformam fiecare pixel in grey
    for (i = 0; i < size; i ++) {
        aux = (unsigned char)(0.299*((*(img.pixels+i)).R) + 0.587*((*(img.pixels+i)).G) + 0.114*((*(img.pixels+i)).B));
        ((*(img.pixels+i)).B) = ((*(img.pixels+i)).G) = ((*(img.pixels+i)).R) = aux;
    }
    save_image(path_to_grey, img);
    // Eliberam memoria
    free(img.pixels);
}

/*
 * Returneaza calculul ecuatiei sigma_fi
 *
 * Parametri:
 * n - numarul de pixeli latime*lungime al sablonului
 * height - inaltimea imaginii
 * width - latimea imaginii pe care se aplica template_matching
 * height - inaltimea imaginii pe care se aplica template_matching
 * pos - structura x0y a pozitiei ferestrei in imagine
 * img - imaginea pe care se aplica template_matching
 */
double sigma_fi(uint32_t n, uint32_t height, uint32_t width, x0y pos, image img) {

    uint32_t i, j;
    double calc = 0, fm = 0;
    for (i = 0; i < height; i ++) {
        for (j = 0; j < width; j ++) {
            fm = fm + ((*(img.pixels + img.header.width*(pos.x+i) + pos.y + j)).R);
        }
    }
    fm = fm/n;
    for (i = 0; i < height; i ++) {
        for (j = 0; j < width; j ++) {
            calc = calc + pow(((*(img.pixels + img.header.width*(pos.x+i) + pos.y + j)).R - fm), 2);
        }
    }
    calc = calc/(n-1);
    calc = sqrt(calc);
    return calc;
}

/*
 * Returneaza calculul ecuatiei sigma_s
 *
 * Parametri:
 * n - numarul de pixeli latime*lungime al sablonului
 * template - sablonul pe care se va face calculul
 */
double sigma_s(uint32_t n, image template) {

    uint32_t i, k;
    double calc = 0, sm = 0;
    for (k = 0; k < template.header.height*template.header.width; k ++) {
        sm = sm + ((*(template.pixels + k)).R);
    }
    sm = sm/n;
    for (i = 0; i < template.header.height*template.header.width; i ++) {
        calc = calc + pow(((*(template.pixels + i)).R - sm), 2);
    }
    calc = calc/(n-1);
    calc = sqrt(calc);
    return calc;
}

/*
 * Desenarea in imginea img a conturii ferestrelor fi.
 *
 * Paramteri:
 * img - imaginea
 * fi - ferestrele
 * index - index-ul ferestrei
 */
void draw_window(image img, window fi, uint32_t index) {
    uint32_t i, j;
    for (i = 0; i < fi.height; i ++) {
        for (j = 0; j < fi.width; j ++) {
            if ((*(fi.pos + index)).x+i == (*(fi.pos + index)).x || ((*(fi.pos + index)).x + fi.height - 1) == (*(fi.pos + index)).x+i || (*(fi.pos + index)).y + j == (*(fi.pos + index)).y || (*(fi.pos + index)).y+j == ((*(fi.pos + index)).y + fi.width - 1)) {
                (*(img.pixels + img.header.width*((*(fi.pos + index)).x+i) + (*(fi.pos + index)).y + j)).R = (*(fi.pos + index)).colors.R;
                (*(img.pixels + img.header.width*((*(fi.pos + index)).x+i) + (*(fi.pos + index)).y + j)).G = (*(fi.pos + index)).colors.G;
                (*(img.pixels + img.header.width*((*(fi.pos + index)).x+i) + (*(fi.pos + index)).y + j)).B = (*(fi.pos + index)).colors.B;
            }
        }
    }
}

void template_matching(image img, image template, float ps, window *fi, image_colors colors) {
    uint32_t i, j, k, l, n, contor = 1;
    double calc = 0, fm = 0, sm = 0, sig_s, sig_fi;
    (*fi).pos = (x0y*) calloc(1, sizeof(x0y));
    (*fi).height = (uint32_t)template.header.height;
    (*fi).width = (uint32_t)template.header.width;


    n = (uint32_t)template.header.height*(uint32_t)template.header.width;
    for (i = 0; i < template.header.height; i ++) {
        for (j = 0; j < template.header.width; j ++) {
            fm = fm + ((*(img.pixels + img.header.width*((*((*fi).pos+(contor-1))).y+i) + (*((*fi).pos+(contor-1))).x + j)).R);
        }
    }
    for (i = 0; i < template.header.height*template.header.width; i ++) {
        sm = sm + ((*(template.pixels + i)).R);
    }
    sm = sm/n;
    fm = fm/n;

    for (k = 0; k < img.header.height; k ++) {
        for (l = 0; l < img.header.width; l++) {
            if ((l + template.header.width) < img.header.width && (k + template.header.height) < img.header.height) {
                (*((*fi).pos+(contor-1))).x = k;
                (*((*fi).pos+(contor-1))).y = l;
                (*((*fi).pos+(contor-1))).colors = colors;
                sig_fi = sigma_fi(n, (uint32_t)template.header.height, (uint32_t)template.header.width, (*((*fi).pos+(contor-1))), img);
                sig_s = sigma_s(n, template);
                for (i = 0; i < template.header.height; i++) {
                    for (j = 0; j < template.header.width; j++) {
                        calc = calc + (((*(img.pixels + (img.header.width * ((*((*fi).pos+(contor-1))).x + i) + (*((*fi).pos+(contor-1))).y + j))).R - fm)*((*(template.pixels + (i*template.header.width) + j)).R - sm));

                    }
                }
                calc = calc / (sig_fi*sig_s);
                calc = calc / n;
                (*((*fi).pos+(contor-1))).ps = calc;
                if (calc >= ps) {
                    contor ++;
                    (*fi).pos = (x0y*) realloc((*fi).pos ,contor*sizeof(x0y));
                }
            }
        }
    }
    (*fi).matches = contor - 1;
}

/*
 * Functia comparator pentru qsort.
 */
int cmp_function(const void *a, const void *b) {
    x0y c = *(x0y*)a;
    x0y d = *(x0y*)b;

    if (c.ps > d.ps) return -1;
    else return 1;
}

/*
 * Eliminarea non-maximelor
 *
 *
 */
window merge_windows(window *win, uint32_t n) {
    window return_window;
    uint32_t i, j, k, contor = 0;
    float sp;
    return_window.pos = (x0y*) calloc(1, sizeof(x0y));

    return_window.height = win[0].height;
    return_window.width = win[0].width;

    return_window.matches = 0;
    for (i = 0; i < n; i ++) {
        for (j = 0; j < win[i].matches; j ++) {
            *(return_window.pos + contor) = *((*(win + i)).pos + j);
            contor ++;
            return_window.pos = (x0y*) realloc(return_window.pos, (contor+1)*sizeof(x0y));
        }
        return_window.matches += win[i].matches;
    }
    qsort((void*)return_window.pos, return_window.matches, sizeof(x0y), cmp_function);
    for (i = 0; i < return_window.matches - 1; i ++) {
        for (j = i + 1; j < return_window.matches; j ++) {
            sp = 0;
            //Cadranul 4
            if (return_window.pos[i].x <= return_window.pos[j].x && (return_window.pos[i].x + return_window.height) >= return_window.pos[j].x && return_window.pos[i].y <= return_window.pos[j].y && (return_window.pos[i].y + return_window.width) >= return_window.pos[j].y) {
                sp = (float)(area(return_window.pos[i].y + return_window.width - return_window.pos[j].y, return_window.pos[i].x + return_window.height - return_window.pos[j].x))/(area(return_window.height, return_window.width)+area(return_window.height, return_window.width)-area(return_window.pos[i].y + return_window.width - return_window.pos[j].y, return_window.pos[i].x + return_window.height - return_window.pos[j].x));
            }
            //Cadranul 1
            else if (return_window.pos[i].x >= return_window.pos[j].x && (return_window.pos[j].x + return_window.height) >= return_window.pos[i].x && return_window.pos[i].y <= return_window.pos[j].y && (return_window.pos[i].y + return_window.width) >= return_window.pos[j].y) {
                sp = (float)(area(return_window.pos[i].y + return_window.width - return_window.pos[j].y, return_window.pos[j].x + return_window.height - return_window.pos[i].x))/(area(return_window.height, return_window.width)+area(return_window.height, return_window.width)-area(return_window.pos[i].y + return_window.width - return_window.pos[j].y, return_window.pos[j].x + return_window.height - return_window.pos[i].x));
            }
            //Cadranul 2
            else if (return_window.pos[i].x >= return_window.pos[j].x && (return_window.pos[j].x + return_window.height) >= return_window.pos[i].x && return_window.pos[i].y >= return_window.pos[j].y && (return_window.pos[j].y + return_window.width) >= return_window.pos[i].y) {
                sp = (float)(area(return_window.pos[j].y + return_window.width - return_window.pos[i].y, return_window.pos[j].x + return_window.height - return_window.pos[i].x))/(area(return_window.height, return_window.width)+area(return_window.height, return_window.width)-area(return_window.pos[j].y + return_window.width - return_window.pos[i].y, return_window.pos[j].x + return_window.height - return_window.pos[i].x));
            }
            //Cadranul 3
            else if (return_window.pos[i].x <= return_window.pos[j].x && (return_window.pos[i].x + return_window.height) >= return_window.pos[j].x && return_window.pos[i].y >= return_window.pos[j].y && (return_window.pos[j].y + return_window.width) >= return_window.pos[i].y) {
                sp = (float)(area(return_window.pos[j].y + return_window.width - return_window.pos[i].y, return_window.pos[i].x + return_window.height - return_window.pos[j].x))/(area(return_window.height, return_window.width)+area(return_window.height, return_window.width)-area(return_window.pos[j].y + return_window.width - return_window.pos[i].y, return_window.pos[i].x + return_window.height - return_window.pos[j].x));
            }
            if (sp > 0.2) {
                for (k = j; k < return_window.matches - 1; k ++) {
                    return_window.pos[k] = return_window.pos[k+1];
                }
                return_window.matches --;
                return_window.pos = (x0y*) realloc(return_window.pos, return_window.matches*sizeof(x0y));
            }
        }
    }
    return return_window;
}