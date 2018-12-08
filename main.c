/*
 * main.cpp
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <float.h>
#include <inttypes.h>
#include "image.h"

int main() {
    /*
    crypting_image("peppers.bmp","hah1a.bmp", "secret_key.txt");
    decrypting_image("hah1a.bmp","dasdas.bmp", "secret_key.txt");
    chisquare_test("hah1a.bmp");*/
    grayscale_image("test.bmp", "test_grey.bmp");
    image img, s;
    img = load_image("test_grey.bmp");
    s = load_image("sabloane\\cifra7.bmp");
    window fi;
    fi.width = 11;
    fi.height = 10;
    fi.x = 100;
    fi.y = 150;
    template_matching(img, s, 0.70, &fi);

    printf("%d - %d" , fi.x, fi.y);
    draw_window(img, fi);
    save_image("cacat.bmp", img);


    return 0;
}