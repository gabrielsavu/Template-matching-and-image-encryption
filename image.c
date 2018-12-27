/*
 * image.c
 * Created by Savu Liviu Gabriel on 29.11.2018.
 * Compiled by mingw w64
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
 * Preluarea rutei catre fisierul programului cu ajutorul fucției
 * getcwd din librăria ‘unistd.h’. Ruta este furnizată prin parametrul ‘route’.
 *
 * Returneaza:
 * false - daca nu s-a putut prelua ruta programului
 * true - daca nu a fost nici-o problema
 */
bool get_route(char route[PATH_MAX]) {
    if (getcwd(route, PATH_MAX) == NULL) return false;
    strcat(route, "\\");
    return true;
}

/*
 * Calculul ariei dreptunghiului cu lungimea x respectiv lățimea y.
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
 * Încărcarea imaginii in memoria internă sub structura ‘image’.
 *
 * Parametri:
 * path_to_image - numele fișierului.
 *
 * Returnează o structură ‘image’ cu valorile corespunzătoare fiecărui camp sau
 * o structură ‘image’ goală în cazul în care nu s-a putut face citirea.
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
 * Salvarea imaginii în memoria externă.
 *
 * Parametri:
 * path_to_save - numele fișierului;
 * image - structura care are informația.
 *
 * Returnează:
 * false - dacă nu s-a putut salva imaginea.
 * true - dacă nu a fost nici-o problemă.
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
 * Citirea cheilor secrete din fișier.
 *
 * Parametri:
 * path_to_secret - numele fișierului cu cheiile secrete.
 *
 * Returnează o structură secret_key cu valorile corespunzatoare
 * fiecărui camp sau o structură goală în cazul în care nu s-a putut face citirea.
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
 * Generează cele 2*W*H-1 valori pseudo-random folosind algoritmul xorshift32.
 *
 * Parametri:
 * seed - valoarea cheii secrete;
 * block_size - mărimea width*height a imaginii.
 *
 * Returnează:
 * Pointer catre începutul tabloului de valori pseudo-random sau pointerul NULL
 * dacă nu s-a putut face alocarea de memorie.
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
 * Generează permutarea necesară pentru interschimbarea pixelilor conform algoritmulu Fisher-Yatesi.
 *
 * Paramteri:
 * *r - pointer la începutul tabloului de valori pseudo-aleatoare;
 * block_size - mărimea width*height a imaginii.
 *
 * Returnează:
 * Pointer catre începutul tabloului de permutări sau pointerul NULL
 * dacă nu s-a putut face alocarea de memorie.
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
 * Generează inversa permutării necesară pentru interschimbarea pixelilor.
 *
 * Paramteri:
 * *permutation - permutarea pentru care trebuie să se calculeze inversa;
 * block_size - mărimea width*height a imaginii.
 *
 * Returnează:
 * Pointer catre începutul tabloului de permutări sau pointerul NULL
 * dacă nu s-a putut face alocarea de memorie.
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
 * Se realizează copierea header-ului imaginii originale în cea criptată,
 * se permută fiecare pixel și se criptează conform problemei.
 *
 * Parametri:
 * real_image - structura imaginii care trebuie criptată;
 * *r - pointer la începutul tabloului de valori pseudo-aleatoare;
 * *permutation - pointer la începutul permutării;
 * SV - cheia secretă.
 *
 * Se presupune ca *r si *permutation au mărimile necesare altfel nu se ajunge până la apelul acestei funcții.
 * Returnează imaginea criptată cu valorile corespunzătoare fiecărui camp sau
 * o structură de tip 'image' goală în cazul în care nu s-a putut aloca numărul de pixeli necesari.
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
 * Se realizează copierea header-ului imaginii criptate in cea decriptată,
 * se decriptează conform problemei și se permută pixelii.
 * Parametri:
 * ciphered_image - structura imaginii care trebuie decriptată;
 * *r - pointer la începutul tabloului de valori pseudo-aleatoare;
 * *permutation - pointer la începutul permutarii inverse;
 * SV - cheia secretă.
 *
 * Se presupune că *r si *permutation au mărimile necesare altfel nu se ajunge până la apelul acestei funcții.
 * Returnează imaginea decriptată cu valorile corespunzatoare fiecărui câmp sau
 * o structură de tip 'image' goală în cazul în care nu s-a putut aloca numarul de pixeli necesari.
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
 * Funcția de criptare a imaginii.
 *
 * Parametri:
 * path_to_image - imaginea ce urmează sa fie criptată;
 * path_to_crypt - imaginea criptată;
 * secret_path - fișierul ce conține cele 2 chei secrete.
 *
 * Returnează:
 * false - dacă nu s-a putut crea imaginea criptată.
 * true - dacă nu a fost nici-o problemă în crearea imaginii.
 */
bool crypting_image(char *path_to_image, char *path_to_crypt, char *secret_path) {
    image image, image_ciphered;
    secret_key secret = get_secret_key(secret_path);
    if (secret.SV == 0) return false;
    image = load_image(path_to_image);
    uint32_t block_size = (uint32_t)image.header.width*image.header.height;
    uint32_t *r , *permutation;

    if ((r = generate_random_values(secret.secret_r0, 2*block_size)) == NULL) return false;
    if ((permutation = generate_permutation(r, block_size)) == NULL) return false;
    image_ciphered = crypting_method(image, r, permutation, secret.SV);
    if (image_ciphered.pixels == NULL) return false;
    save_image(path_to_crypt, image_ciphered);

    //Eliberam memoria
    free(r);
    free(permutation);
    free(image.pixels);
    free(image_ciphered.pixels);
    return true;
}

/*
 * Funcția de decriptare a imaginii.
 *
 * Parametri:
 * path_to_image - imaginea ce urmează să fie decriptată;
 * path_to_decrypt - imaginea decriptată;
 * secret_path - fișierul ce conține cele 2 chei secrete.
 *
 * Returnează:
 * false - dacă nu s-a putut crea imaginea criptată.
 * true - dacă nu a fost nici-o problemă în crearea imaginii.
 */
bool decrypting_image(char *path_to_image, char *path_to_decrypt, char *secret_path) {
    image image, image_deciphered;
    secret_key secret = get_secret_key(secret_path);
    if (secret.SV == 0) return false;
    image = load_image(path_to_image);
    uint32_t block_size = (uint32_t)image.header.width*image.header.height;
    uint32_t *r , *permutation, *rev_permutation;

    if ((r = generate_random_values(secret.secret_r0, 2*block_size)) == NULL) return false;
    if ((permutation = generate_permutation(r, block_size)) == NULL) return false;
    if ((rev_permutation = reverse_permutation(permutation, block_size)) == NULL) return false;
    // Eliberam memoria de care nu mai avem nevoie pentru a face mai mult loc
    free(permutation);
    image_deciphered = decrypting_method(image, r, rev_permutation, secret.SV);
    if (image_deciphered.pixels == NULL) return false;
    save_image(path_to_decrypt, image_deciphered);
    //Eliberam memoria
    free(r);
    free(rev_permutation);
    free(image.pixels);
    free(image_deciphered.pixels);
    return true;
}

/*
 * Calculul sumei chisquare.
 *
 * Parametri:
 * img - imaginea pe care se face calculul;
 * fm - constanta (width*height)/256;
 * chanel - canalul pe care se face calculul(R,G,B);
 * *expression - expresia de sub suma.
 *
 * Returnează suma de pe canalul 'chanel'.
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
    printf("(%f, %f, %f)\n", sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'R', expression),
           sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'G', expression),
           sigma_chitest(img, 256, (float)(img.header.width*img.header.height)/256, 'B', expression)
    );
    //Eliberam memoria
    free(img.pixels);
}


/*
 * Transformarea imaginii ‘path_to_image’ în imagine grayscale.
 *
 * Paramteri:
 * path_to_image - numele imaginii ce urmează să se transforme;
 * path_to_grey - numele imaginii grayscale.
 */
bool grayscale_image(char* path_to_image, char* path_to_grey) {
    image img;
    unsigned char aux;
    img = load_image(path_to_image);
    if (img.pixels == NULL) return false;
    uint32_t size = (uint32_t)img.header.width * img.header.height, i;
    // Transformam fiecare pixel in grey
    for (i = 0; i < size; i ++) {
        aux = (unsigned char)(0.299*((*(img.pixels+i)).R) + 0.587*((*(img.pixels+i)).G) + 0.114*((*(img.pixels+i)).B));
        ((*(img.pixels+i)).B) = ((*(img.pixels+i)).G) = ((*(img.pixels+i)).R) = aux;
    }
    if (save_image(path_to_grey, img) == false) return false;
    // Eliberam memoria
    free(img.pixels);
    return true;
}

/*
 * Returnează calculul ecuației sigma_fi
 *
 * Parametri:
 * n - numărul de pixeli width*height al șablonului;
 * height - înaltimea imaginii pe care se aplică template_matching;
 * width - lațimea imaginii pe care se aplică template_matching;
 * height - înalțimea imaginii pe care se aplică template_matching;
 * pos - structura x0y a poziției ferestrei în imagine;
 * img - imaginea pe care se aplică template_matching.
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
 * Returnează calculul ecuației sigma_s.
 *
 * Parametri:
 * n - numărul de pixeli width*height al șablonului;
 * template - șablonul pe care se va face calculul.
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
 * Desenează în imginea ‘img’ conturul ferestrelor ‘fi’.
 *
 * Paramteri:
 * img - imaginea pe care se va desena;
 * win - vectorul de ferestre din care se va desena;
 * n - numarul de ferestre din vectorul 'win'.
 */
void draw_windows(image img, window win) {
    uint32_t k, i, j;
    for (k = 0; k < win.matches; k++) {
        for (i = 0; i < win.height; i++) {
            for (j = 0; j < win.width; j++) {
                // Desenam numai conturul ferestrei
                if ((*(win.pos + k)).x + i == (*(win.pos + k)).x ||
                    ((*(win.pos + k)).x + win.height - 1) == (*(win.pos + k)).x + i ||
                    (*(win.pos + k)).y + j == (*(win.pos + k)).y ||
                    (*(win.pos + k)).y + j == ((*(win.pos + k)).y + win.width - 1)) {
                    (*(img.pixels + img.header.width * ((*(win.pos + k)).x + i) + (*(win.pos + k)).y +
                       j)).R = (*(win.pos + k)).colors.R;
                    (*(img.pixels + img.header.width * ((*(win.pos + k)).x + i) + (*(win.pos + k)).y +
                       j)).G = (*(win.pos + k)).colors.G;
                    (*(img.pixels + img.header.width * ((*(win.pos + k)).x + i) + (*(win.pos + k)).y +
                       j)).B = (*(win.pos + k)).colors.B;
                }
            }
        }
    }
}

/*
 * Paramteri:
 * img - imaginea pe care urmeaza se se aplice algoritmul
 * template - sablonul
 * ps - pragul minim de detectie
 * fi - ferestrele furnizate de functie in urma algoritmului
 * colors - culoarea ferestrelor
 */
void template_matching(image img, image template, float ps, window *win, image_colors colors) {
    uint32_t i, j, k, l, n, contor = 1;
    double calc = 0, fm = 0, sm = 0, sig_s, sig_fi;
    (*win).pos = (x0y*) calloc(1, sizeof(x0y));
    (*win).height = (uint32_t)template.header.height;
    (*win).width = (uint32_t)template.header.width;


    n = (uint32_t)template.header.height*(uint32_t)template.header.width;
    for (i = 0; i < template.header.height; i ++) {
        for (j = 0; j < template.header.width; j ++) {
            fm = fm + ((*(img.pixels + img.header.width*((*((*win).pos+(contor-1))).y+i) + (*((*win).pos+(contor-1))).x + j)).R);
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
                (*((*win).pos+(contor-1))).x = k;
                (*((*win).pos+(contor-1))).y = l;
                (*((*win).pos+(contor-1))).colors = colors;
                sig_fi = sigma_fi(n, (uint32_t)template.header.height, (uint32_t)template.header.width, (*((*win).pos+(contor-1))), img);
                sig_s = sigma_s(n, template);
                for (i = 0; i < template.header.height; i++) {
                    for (j = 0; j < template.header.width; j++) {
                        calc = calc + (((*(img.pixels + (img.header.width * ((*((*win).pos+(contor-1))).x + i) + (*((*win).pos+(contor-1))).y + j))).R - fm)*((*(template.pixels + (i*template.header.width) + j)).R - sm));

                    }
                }
                calc = calc / (sig_fi*sig_s);
                calc = calc / n;
                (*((*win).pos+(contor-1))).ps = calc;
                if (calc >= ps) {
                    contor ++;
                    (*win).pos = (x0y*) realloc((*win).pos ,contor*sizeof(x0y));
                }
            }
        }
    }
    (*win).matches = contor - 1;
}

/*
 * Funcția comparator pentru qsort.
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
 * Paramteri:
 * win - vectorul de ferestre pentru fiecare sablon
 * n - numarul de sabloane
 */
window merge_windows(window *win, uint32_t n) {
    window return_window;
    uint32_t i, j, k, contor = 0;
    float sp;
    return_window.pos = (x0y*) calloc(1, sizeof(x0y));

    return_window.height = (*win).height;
    return_window.width = (*win).width;

    return_window.matches = 0;
    // Unirea tuturor ferestrelor in vectorul return_window
    for (i = 0; i < n; i ++) {
        for (j = 0; j < (*(win + i)).matches; j ++) {
            *(return_window.pos + contor) = *((*(win + i)).pos + j);
            contor ++;
            return_window.pos = (x0y*) realloc(return_window.pos, (contor+1)*sizeof(x0y));
        }
        return_window.matches += win[i].matches;
    }
    // Sortarea vectorului return_window descrescator.
    qsort((void*)return_window.pos, return_window.matches, sizeof(x0y), cmp_function);

    // Eliminarea non-maximelor
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
                // Eliminarea elemntului
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