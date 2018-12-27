/*
 * main.c
 * Created by Savu Liviu Gabriel on 29.11.2018.
 * Compiled by mingw w64
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <inttypes.h>
#include "image.h"



int main() {

    unsigned option;
    char *path_to_image, *path_to_crypt, *path_to_decrypt, *secret_key;


    path_to_image = calloc(256, sizeof(char));
    path_to_crypt = calloc(256, sizeof(char));
    path_to_decrypt = calloc(256, sizeof(char));
    secret_key = calloc(256, sizeof(char));

    printf("Options:\n");
    printf("0 - Exit\n");
    printf("1 - Criptare imagine;\n");
    printf("2 - Decriptare imagine;\n");
    printf("3 - chi-square test;\n");
    printf("4 - Template-matching pentru imaginea test.bmp.\n");
    do {
        scanf("%u", &option);
        switch (option) {
            case 0: {
                printf("exit...\n");
                break;
            }
            case 1: {
                printf("Imaginea de criptat:\n");
                scanf("%s" , path_to_image);
                printf("Cu ce nume se va salva imaginea criptata?:\n");
                scanf("%s" , path_to_crypt);
                printf("Fisierul cu cheile secrete:\n");
                scanf("%s" , secret_key);
                if (crypting_image(path_to_image, path_to_crypt, secret_key) == true)
                    printf("Criptarea imaginii a fost un succes.\n");
                else printf("A aparut o eroare la criptarea imaginii.\n");
                break;
            }
            case 2: {
                printf("Imaginea de decriptat:\n");
                scanf("%s" , path_to_image);
                printf("Cu ce nume se va salva imaginea decriptata?:\n");
                scanf("%s" , path_to_decrypt);
                printf("Fisierul cu cheile secrete:\n");
                scanf("%s" , secret_key);
                if (decrypting_image(path_to_image, path_to_decrypt, secret_key) == true)
                    printf("Decriptarea imaginii a fost un succes.\n");
                else printf("A aparut o eroare la criptarea imaginii.\n");
                break;
            }
            case 3: {
                printf("Imaginea pe care se aplica chi-square(imaginea 1):\n");
                scanf("%s" , path_to_image);
                printf("Imaginea pe care se aplica chi-square(imaginea 2):\n");
                scanf("%s" , path_to_crypt);
                chisquare_test(path_to_image);
                chisquare_test(path_to_crypt);
                break;
            }
            case 4: {
                printf("Imaginea:\n");
                char *path_to_grey;
                char *ptr;
                path_to_grey = calloc(256, sizeof(char));
                scanf("%s" , path_to_image);
                strcpy(path_to_grey, path_to_image);
                ptr = strstr(path_to_grey, ".bmp");
                strcpy(ptr, "_grey.bmp");

                printf("Se calculeaza...\n");
                grayscale_image(path_to_image, path_to_grey);
                image img, *template_cifra;
                template_cifra = calloc(10, sizeof(image));
                img = load_image(path_to_grey);

                window fi[10], merge;
                template_cifra[0] = load_image("sabloane\\cifra0.bmp");
                template_matching(img, template_cifra[0], 0.5, &(fi[0]), (image_colors){255, 0, 0});
                free(template_cifra[0].pixels);

                template_cifra[1] = load_image("sabloane\\cifra1.bmp");
                template_matching(img, template_cifra[1], 0.5, &(fi[1]), (image_colors){255, 255, 0});
                free(template_cifra[1].pixels);

                template_cifra[2] = load_image("sabloane\\cifra2.bmp");
                template_matching(img, template_cifra[2], 0.5, &(fi[2]), (image_colors){0, 255, 0});
                free(template_cifra[2].pixels);

                template_cifra[3] = load_image("sabloane\\cifra3.bmp");
                template_matching(img, template_cifra[3], 0.5, &(fi[3]), (image_colors){0, 255, 255});
                free(template_cifra[3].pixels);

                template_cifra[4] = load_image("sabloane\\cifra4.bmp");
                template_matching(img, template_cifra[4], 0.5, &(fi[4]), (image_colors){255, 0, 255});
                free(template_cifra[4].pixels);

                template_cifra[5] = load_image("sabloane\\cifra5.bmp");
                template_matching(img, template_cifra[5], 0.5, &(fi[5]), (image_colors){0, 0, 255});
                free(template_cifra[5].pixels);

                template_cifra[6] = load_image("sabloane\\cifra6.bmp");
                template_matching(img, template_cifra[6], 0.5, &(fi[6]), (image_colors){192, 192, 192});
                free(template_cifra[6].pixels);

                template_cifra[7] = load_image("sabloane\\cifra7.bmp");
                template_matching(img, template_cifra[7], 0.5, &(fi[7]), (image_colors){255, 140, 0});
                free(template_cifra[7].pixels);

                template_cifra[8] = load_image("sabloane\\cifra8.bmp");
                template_matching(img, template_cifra[8], 0.5, &(fi[8]), (image_colors){128, 0, 128});
                free(template_cifra[8].pixels);

                template_cifra[9] = load_image("sabloane\\cifra9.bmp");
                template_matching(img, template_cifra[9], 0.5, &(fi[9]), (image_colors){128, 0, 0});
                free(template_cifra[9].pixels);

                merge = merge_windows(fi, 10);


                draw_windows(img, merge);

                save_image("template_matching.bmp", img);

                free(fi[0].pos);
                free(fi[1].pos);
                free(fi[2].pos);
                free(fi[3].pos);
                free(fi[4].pos);
                free(fi[5].pos);
                free(fi[6].pos);
                free(fi[7].pos);
                free(fi[8].pos);
                free(fi[9].pos);
                free(merge.pos);
                printf("Algoritmul de template_matching s-a aplicat pe imaginea test.bmp: template_matching.bmp\n");
                break;
            }
            default: {
                printf("Comanda necunoscuta.\n");
                break;
            }

        }
    }
    while (option != 0);
    return 0;
}