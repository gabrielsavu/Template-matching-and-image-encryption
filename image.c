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
            fread(&(*(tmp_image.pixels + contor)).B, 1, 1, file);
            fread(&(*(tmp_image.pixels + contor)).G, 1, 1, file);
            fread(&(*(tmp_image.pixels + contor)).R, 1, 1, file);
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
            fwrite(&((*(image.pixels+contor)).B), 1, 1, file);
            fwrite(&((*(image.pixels+contor)).G), 1, 1, file);
            fwrite(&((*(image.pixels+contor)).R), 1, 1, file);
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
image crypting_method(image real_image, uint32_t const* r, uint32_t const* permutation, uint32_t SV) {
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
    (*(ciphered_image.pixels)).R = (unsigned char)((SV)^(*(ciphered_image.pixels)).R^(*(r+size)));
    (*(ciphered_image.pixels)).G = (unsigned char)((SV)^(*(ciphered_image.pixels)).G^(*(r+size)));
    (*(ciphered_image.pixels)).B = (unsigned char)((SV)^(*(ciphered_image.pixels)).B^(*(r+size)));

    // Generarea restului de pixeli conform C(k)=C(k-1)^P'(k)^r(w*h+k)
    for (i = 1; i < (uint32_t)(real_image.header.width*real_image.header.height); i ++) {
        (*(ciphered_image.pixels + i)).R = (unsigned char)(((*(ciphered_image.pixels + (i-1))).R)^((*(ciphered_image.pixels + i)).R)^(*(r+size+i)));
        (*(ciphered_image.pixels + i)).G = (unsigned char)(((*(ciphered_image.pixels + (i-1))).G)^((*(ciphered_image.pixels + i)).G)^(*(r+size+i)));
        (*(ciphered_image.pixels + i)).B = (unsigned char)(((*(ciphered_image.pixels + (i-1))).B)^((*(ciphered_image.pixels + i)).B)^(*(r+size+i)));
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
image decrypting_method(image ciphered_image, uint32_t const* r, uint32_t const* permutation, uint32_t SV) {
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

    (*(tmp_image.pixels)).R = (unsigned char)((SV)^(*(ciphered_image.pixels)).R^(*(r+size)));
    (*(tmp_image.pixels)).G = (unsigned char)((SV)^(*(ciphered_image.pixels)).G^(*(r+size)));
    (*(tmp_image.pixels)).B = (unsigned char)((SV)^(*(ciphered_image.pixels)).B^(*(r+size)));

    for (i = 1; i < size; i ++) {
        (*(tmp_image.pixels + i)).R = (unsigned char)(((*(ciphered_image.pixels + (i-1))).R)^((*(ciphered_image.pixels + i)).R)^(*(r+size+i)));
        (*(tmp_image.pixels + i)).G = (unsigned char)(((*(ciphered_image.pixels + (i-1))).G)^((*(ciphered_image.pixels + i)).G)^(*(r+size+i)));
        (*(tmp_image.pixels + i)).B = (unsigned char)(((*(ciphered_image.pixels + (i-1))).B)^((*(ciphered_image.pixels + i)).B)^(*(r+size+i)));
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

float suma_chitest(image img, int n, float fm, unsigned char chanel, float (*expresie)(image, float, uint32_t, unsigned char)) {
    uint32_t i = 0;
    float s = 0;
    for (i = 0; i < n; i ++) {
        s = s + expresie(img, fm, i, chanel);
    }
    return s;
}


float expresie(image image, float fm, uint32_t i, unsigned char chanel) {
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
    printf("(%f, %f, %f)", suma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'R', expresie),
           suma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'G', expresie),
           suma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'B', expresie)
    );
    //Eliberam memoria
    free(img.pixels);
}







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


void template_matching(image img, image template, float ps, window *fi) {
    uint32_t i, j, k, l, n, contor = 1;
    double calc = 0, fm = 0, sm = 0, sig_s, sig_fi;
    fi = (window*) calloc(contor, sizeof(window));
    (*fi).height = (uint32_t)template.header.height;
    (*fi).width = (uint32_t)template.header.width;
    n = ((*(fi+(contor-1))).height)*((*(fi+(contor-1))).width);
    for (i = 0; i < (*(fi+(contor-1))).height; i ++) {
        for (j = 0; j < (*(fi+(contor-1))).width; j ++) {
            fm = fm + ((*(img.pixels + img.header.width*((*(fi+(contor-1))).y+i) + (*(fi+(contor-1))).x + j)).R);
        }
    }
    for (i = 0; i < template.header.height*template.header.width; i ++) {
        sm = sm + ((*(template.pixels + i)).R);
    }
    sm = sm/n;
    fm = fm/n;

    for (k = 0; k < img.header.height; k ++) {
        for (l = 0; l < img.header.width; l++) {
            if ((l + (*(fi+(contor-1))).width) < img.header.width && (k + (*(fi+(contor-1))).height) < img.header.height) {
                (*(fi+(contor-1))).x = k;
                (*(fi+(contor-1))).y = l;
                sig_fi = sigma_fi(n, *(fi+(contor-1)), img);
                sig_s = sigma_s(n, template);
                for (i = 0; i < (*(fi+(contor-1))).height; i++) {
                    for (j = 0; j < (*(fi+(contor-1))).width; j++) {
                        calc = calc + (((*(img.pixels + (img.header.width * ((*(fi+(contor-1))).x + i) + (*(fi+(contor-1))).y + j))).R - fm)*((*(template.pixels + (i*(*(fi+(contor-1))).width) + j)).R - sm));

                    }
                }
                calc = calc / (sig_fi*sig_s);
                calc = calc / n;
                (*(fi+(contor-1))).ps = calc;
                if (calc >= ps) {
                    contor ++;
                    fi = realloc(fi, contor*sizeof(window));
                }
            }
        }
    }
}

double sigma_fi(uint32_t n, window fi, image img) {

    uint32_t i, j;
    double calc = 0, fm = 0;
    for (i = 0; i < fi.height; i ++) {
        for (j = 0; j < fi.width; j ++) {
            fm = fm + ((*(img.pixels + img.header.width*(fi.x+i) + fi.y + j)).R);
        }
    }
    fm = fm/n;
    for (i = 0; i < fi.height; i ++) {
        for (j = 0; j < fi.width; j ++) {
            calc = calc + pow(((*(img.pixels + img.header.width*(fi.x+i) + fi.y + j)).R - fm), 2);
        }
    }
    calc = calc/(n-1);
    calc = sqrt(calc);
    return calc;
}

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


void draw_window(image img, window fi) {
    uint32_t i, j;
    for (i = 0; i < fi.height; i ++) {
        for (j = 0; j < fi.width; j ++) {
            if ((fi.x+i == fi.x || (fi.x + fi.height - 1) == fi.x+i) || fi.y + j == fi.y || fi.y+j == (fi.y + fi.width - 1)) {
                (*(img.pixels + img.header.width*(fi.x+i) + fi.y + j)).R = 255;
                (*(img.pixels + img.header.width*(fi.x+i) + fi.y + j)).G = 0;
                (*(img.pixels + img.header.width*(fi.x+i) + fi.y + j)).B = 0;
            }
        }
    }
}
