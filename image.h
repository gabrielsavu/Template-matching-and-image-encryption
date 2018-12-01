/*
 * image.h
 * Created by Savu Liviu Gabriel on 29.11.2018.
 */

#ifndef PP_PROIECT_IMAGE_H
#define PP_PROIECT_IMAGE_H
#include <stdint.h>
#include <stdbool.h>

/*
 * http://www.fastgraph.com/help/bmp_header_format.html
 * Headerul unui fisier BMP
 */
typedef struct {
    uint16_t signature;         // Semnatura, trbeuie sa fie hex:0x4D42 | decimal:19778
    uint32_t size;              // Marimea fisierului in bytes
    uint16_t reserved1;         // Valoare folosita de software, trebuie sa fie 0
    uint16_t reserved2;         // Valoare folosita de software, trebuie sa fie 0
    uint32_t offset;            // -
    uint32_t bitmapinfo;        // Marimea BITMAPINFOHEADER, trebuie sa fie hex:0x28 | decimal:40
    int32_t  width;             // Latimea imaginii
    int32_t  height;            // Inaltimea imaginii
    uint16_t no_planes;         // Numarul de "planuri" de culoare, trebuie sa fie 1
    uint16_t no_bits_pixel;     // Numarul de biti pe pixel, trebuie sa fie (1, 4, 8 sau 24)
    uint32_t compression;       // Tipul de compresie (0=none, 1=RLE-8, 2=RLE-4)
    uint32_t size_padding;      // Marimea fisierului in bytes (cu padding)
    int32_t  x_pixel_meter;     // Rezolutie orizontala in pixeli pe metru
    int32_t  y_pixel_meter;     // Rezolutie verticala in pixeli pe metru
    uint32_t no_colors;         // Numarul de culori din imagine
    uint32_t no_imp_colors;     // Numarul de culori importante din imagine
} image_header;

typedef struct {
    unsigned char R, G, B;
} image_colors;

typedef struct {
    image_colors *pixels;       // Imaginea in forma liniarizata
    image_header header;        // Header-ul imaginii
} image;

typedef struct {
    uint32_t secret_r0, SV;
} secret_key;

image load_image(char *);

bool save_image(char *, image);

bool crypting_method(char *path_to_image, char *path_to_crypt, char *secret_path);

#endif //PP_PROIECT_IMAGE_H
