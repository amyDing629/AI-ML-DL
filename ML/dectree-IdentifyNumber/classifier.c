#include "dectree.h"

// Makefile included in starter:
//    To compile:               make
//    To decompress dataset:    make datasets
//
// Running decision tree generation / validation:
//     gcc -Wall -std=c99 -lm -o classifier classifier.c dectree.c
//    ./classifier datasets/training_data.bin datasets/testing_data.bin

/*****************************************************************************/
/* Do not add anything outside the main function here. Any core logic other  */
/* than what is described below should go in `dectree.c`. You've been warned!*/
/*****************************************************************************/

/**
 * main() takes in 2 command line arguments:
 *    - training_data: A binary file containing training image / label data
 *    - testing_data: A binary file containing testing image / label data
 *
 * You need to do the following:
 *    - Parse the command line arguments, call `load_dataset()` appropriately.
 *    - Call `make_dec_tree()` to build the decision tree with training data
 *    - For each test image, call `dec_tree_classify()` and compare the real 
 *        label with the predicted label
 *    - Print out (only) one integer to stdout representing the number of 
 *        test images that were correctly classified.
 *    - Free all the data dynamically allocated and exit.
 * 
 */
int main(int argc, char *argv[]) {
  if (argc != 3) {
        fprintf(stderr, "Usage: %s training test\n", argv[0]);
        exit(1);
    }
  int total_correct = 0;
  
  Dataset* training = load_dataset(argv[1]);
  Dataset* testing = load_dataset(argv[2]);
  DTNode* node = build_dec_tree(training);
  int num = testing->num_items;
  for (int i=0; i<num; i++){
    if (testing->labels[i] == dec_tree_classify(node, &(testing->images[i]))){
      total_correct++;
    }
  }
  free_dec_tree(node);
  free_dataset(training);
  free_dataset(testing);
  //Print out answer
  printf("%d\n", total_correct);
  return 0;
  

}
