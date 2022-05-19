#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

/* This small test file will help you determine if you have successfully
 * completed the load_image function in knn.
 * 
 * compile this program as "gcc -Wall -g -std=gnu99 -lm -o test_loadimage test_loadimage.c knn.c"
 * This program prints the image to standard output, so you should redirect it
 * to a file and compare it with the original image.
 */
unsigned char image1[NUM_PIXELS];
unsigned char image2[NUM_PIXELS];

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }

    load_image(argv[1], image1);
    load_image(argv[2], image2);
    double rst = distance(image1, image2);
    printf("%f", rst);
    return rst;
}
