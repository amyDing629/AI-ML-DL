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

unsigned char dataset[MAX_SIZE][NUM_PIXELS];
unsigned char labels[MAX_SIZE];
unsigned char image[NUM_PIXELS];

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }
    int i = load_dataset(argv[1], dataset, labels);
    load_image(argv[2], image);
    int K = *argv[3];
    int rst = knn_predict(image, K, dataset, labels, i);
    printf("%d", rst);
    
    return 0;
}