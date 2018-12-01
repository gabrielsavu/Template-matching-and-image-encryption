/*
 * main.cpp
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "image.h"

int main() {

    crypting_image("peppers.bmp","hah1a.bmp", "secret_key.txt");
    return 0;
}