/*
 * main.cpp
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <inttypes.h>
#include "image.h"



int main() {

    crypting_image("peppers.bmp", "enc_peppers.bmp", "secret_key.txt");
    decrypting_image("enc_peppers.bmp", "dec_peppers.bmp", "secret_key.txt");
    chisquare_test("enc_peppers.bmp");
    grayscale_image("test.bmp", "test_grey.bmp");
    image img, template_cifra[10];
    img = load_image("test_grey.bmp");
    window fi[10], merge;
    uint32_t i;

    template_cifra[0] = load_image("sabloane\\cifra0.bmp");
    template_matching(img, template_cifra[0], 0.5, &(fi[0]), (image_colors){255, 0, 0});
    template_cifra[1] = load_image("sabloane\\cifra1.bmp");
    template_matching(img, template_cifra[1], 0.5, &(fi[1]), (image_colors){255, 255, 0});
    template_cifra[2] = load_image("sabloane\\cifra2.bmp");
    template_matching(img, template_cifra[2], 0.5, &(fi[2]), (image_colors){0, 255, 0});
    template_cifra[3] = load_image("sabloane\\cifra3.bmp");
    template_matching(img, template_cifra[3], 0.5, &(fi[3]), (image_colors){0, 255, 255});
    template_cifra[4] = load_image("sabloane\\cifra4.bmp");
    template_matching(img, template_cifra[4], 0.5, &(fi[4]), (image_colors){255, 0, 255});
    template_cifra[5] = load_image("sabloane\\cifra5.bmp");
    template_matching(img, template_cifra[5], 0.5, &(fi[5]), (image_colors){0, 0, 255});
    template_cifra[6] = load_image("sabloane\\cifra6.bmp");
    template_matching(img, template_cifra[6], 0.5, &(fi[6]), (image_colors){192, 192, 192});
    template_cifra[7] = load_image("sabloane\\cifra7.bmp");
    template_matching(img, template_cifra[7], 0.5, &(fi[7]), (image_colors){255, 140, 0});
    template_cifra[8] = load_image("sabloane\\cifra8.bmp");
    template_matching(img, template_cifra[8], 0.5, &(fi[8]), (image_colors){128, 0, 128});
    template_cifra[9] = load_image("sabloane\\cifra9.bmp");
    template_matching(img, template_cifra[9], 0.5, &(fi[9]), (image_colors){128, 0, 0});

    merge = merge_windows(fi, 10);

    for (i = 0; i < merge.matches; i ++)
        draw_window(img, merge, i);

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
    free(template_cifra[0].pixels);
    free(template_cifra[1].pixels);
    free(template_cifra[2].pixels);
    free(template_cifra[3].pixels);
    free(template_cifra[4].pixels);
    free(template_cifra[5].pixels);
    free(template_cifra[6].pixels);
    free(template_cifra[7].pixels);
    free(template_cifra[8].pixels);
    free(template_cifra[9].pixels);
    return 0;
}