#include "dectree.h"

/**
 * Load the binary file, filename into a Dataset and return a pointer to 
 * the Dataset. The binary file format is as follows:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - NUM_PIXELS bytes : Image 1 data (WIDTHxWIDTH)
 *          ...
 *     -   1 byte  : Image N label
 *     - NUM_PIXELS bytes : Image N data (WIDTHxWIDTH)
 *
 * You can set the `sx` and `sy` values for all the images to WIDTH. 
 * Use the NUM_PIXELS and WIDTH constants defined in dectree.h
 */
Dataset *load_dataset(const char *filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp){
        perror("Failed to open file");
        exit(1);
    }
    int i = 0;
    int rst = fread(&i, sizeof(int), 1, fp);
    if (rst != 1){
        fprintf(stderr, "Failed to read number of images/labels from %s.\n", filename);
        exit(1);
    }
    Dataset* dt = malloc(sizeof(Dataset));
    dt->images = malloc(sizeof(Image)*i);
    dt->labels = malloc(sizeof(char)*i);
    dt->num_items = i;
    
    for (int j=0; j<i; j++){
        rst = fread(dt->labels + j, sizeof(char), 1, fp);
        if (rst != 1){
            fprintf(stderr, "Failed to read labels from %s.\n", filename);
            exit(1);
        }
        dt->images[j].sx = WIDTH;
        dt->images[j].sy = WIDTH;
        dt->images[j].data = malloc(sizeof(char) * NUM_PIXELS);
        int ret = fread(dt->images[j].data, sizeof(char), NUM_PIXELS, fp);   
        if (ret != NUM_PIXELS){
            fprintf(stderr, "Failed to read images from %s.\n", filename);
            exit(1);
        }   
    }
    if (fclose(fp)){
        perror("Failed to close file %s");
        exit(1);
    }
    return dt;
}

/**
 * Compute and return the Gini impurity of M images at a given pixel
 * The M images to analyze are identified by the indices array. The M
 * elements of the indices array are indices into data.
 * This is the objective function that you will use to identify the best 
 * pixel on which to split the dataset when building the decision tree.
 *
 * Note that the gini_impurity implemented here can evaluate to NAN 
 * (Not A Number) and will return that value. Your implementation of the 
 * decision trees should ensure that a pixel whose gini_impurity evaluates 
 * to NAN is not used to split the data.  (see find_best_split)
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double gini_impurity(Dataset *data, int M, int *indices, int pixel) {
    int a_freq[10] = {0}, a_count = 0;
    int b_freq[10] = {0}, b_count = 0;

    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];

        // The pixels are always either 0 or 255, but using < 128 for generality.
        if (data->images[img_idx].data[pixel] < 128) {
            a_freq[data->labels[img_idx]]++;
            a_count++;
        } else {
            b_freq[data->labels[img_idx]]++;
            b_count++;
        }
    }

    double a_gini = 0, b_gini = 0;
    for (int i = 0; i < 10; i++) {
        double a_i = ((double)a_freq[i]) / ((double)a_count);
        double b_i = ((double)b_freq[i]) / ((double)b_count);
        a_gini += a_i * (1 - a_i);
        b_gini += b_i * (1 - b_i);
    }

    // Weighted average of gini impurity of children
    return (a_gini * a_count + b_gini * b_count) / M;
}

/**
 * Given a subset of M images and the array of their corresponding indices, 
 * find and use the last two parameters (label and freq) to store the most
 * frequent label in the set and its frequency.
 *
 * - The most frequent label (between 0 and 9) will be stored in `*label`
 * - The frequency of this label within the subset will be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int *indices, int *label, int *freq) {
    // TODO: Set the correct values and return
    int l[10];

    for (int i = 0; i < 10; i++){
        l[i] = 0;
    }
    // l[i] is frequency of i
    for (int i = 0; i < M; i++){
        int lb = data->labels[indices[i]];
        l[lb] += 1;
    }
    // find label with maximum frequency
    for (int i = 0; i < 10; i++){
        if (l[i] > *freq){
            *label = i;
            *freq = l[i];
        }
    }
    
}

/**
 * Given a subset of M images as defined by their indices, find and return
 * the best pixel to split the data. The best pixel is the one which
 * has the minimum Gini impurity as computed by `gini_impurity()` and 
 * is not NAN. (See handout for more information)
 * 
 * The return value will be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int *indices) {
    // TODO: Return the correct pixel
    int pix = -1;
    double gi;
    for (int i = 0; i < NUM_PIXELS; i++){
        double tem_gi = gini_impurity(data, M, indices, i);
        //if tem_gi is nan, go to next loop directly
        if (isnan(tem_gi)){
            continue;
        }
        if (pix == -1){
            pix = i;
            gi = tem_gi;
        }else{
            if (tem_gi < gi){
                pix = i;
                gi = tem_gi;
            }
        }
        
    }
    return pix;
}

/**
 * Create the Decision tree. In each recursive call, we consider the subset of the
 * dataset that correspond to the new node. To represent the subset, we pass 
 * an array of indices of these images in the subset of the dataset, along with 
 * its length M. Be careful to allocate this indices array for any recursive 
 * calls made, and free it when you no longer need the array. In this function,
 * you need to:
 *
 *    - Compute ratio of most frequent image in indices, do not split if the
 *      ratio is greater than THRESHOLD_RATIO
 *    - Find the best pixel to split on using `find_best_split`
 *    - Split the data based on whether pixel is less than 128, allocate 
 *      arrays of indices of training images and populate them with the 
 *      subset of indices from M that correspond to which side of the split
 *      they are on
 *    - Allocate a new node, set the correct values and return
 *       - If it is a leaf node set `classification`, and both children = NULL.
 *       - Otherwise, set `pixel` and `left`/`right` nodes 
 *         (using build_subtree recursively). 
 */
DTNode *build_subtree(Dataset *data, int M, int *indices) {
    // DTNode *node = build_subtree_helper(data, M, indices, 0);
    // return node;
    // TODO: Construct and return the tree
    int label = 0;
    int freq = 0;
    get_most_frequent(data, M, indices, &label, &freq);
    DTNode* node = malloc(sizeof(DTNode));
    //leaf situation
    if ((double)freq / M > THRESHOLD_RATIO){  
        node->classification = label; 
        node->left = NULL;
        node->right = NULL;
        }
    //not leaf
    else{
        int pix = find_best_split(data, M, indices);
        int *smaller = malloc(sizeof(int)*M);
        int sIndex = 0;
        int *larger = malloc(sizeof(int)*M);
        int lIndex = 0;
        for (int i=0; i<M; i++){
            if (data->images[indices[i]].data[pix]<128){
                smaller[sIndex] = indices[i];
                sIndex += 1;
            }
            else{
                larger[lIndex] = indices[i];
                lIndex += 1;
            }
        }
        node->pixel = pix;
        node->left = build_subtree(data, sIndex, smaller);
        node->right = build_subtree(data, lIndex, larger);
        node->classification = -1;
        free(smaller);
        free(larger);
    }

    return node;
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `build_subtree()` with the correct parameters.
 */
DTNode *build_dec_tree(Dataset *data) {
    // TODO: Set up `indices` array, call `build_subtree` and return the tree.
    // HINT: Make sure you free any data that is not needed anymore
    // indices are 0~(M-1), all the indices
    int* indices = malloc(sizeof(int)*(data->num_items));
    for (int i=0; i<(data->num_items); i++){
        indices[i] = i;
    }
    DTNode* node = build_subtree(data, data->num_items, indices);
    free(indices);

    return node;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
    // TODO: Return the correct label
    if (root->left == NULL && root->right == NULL){
        return root->classification;
    }else{
        int pix = root->pixel;
        if (img->data[pix] < 128){
            return dec_tree_classify(root->left, img);
        }else{
            return dec_tree_classify(root->right, img);
        }
    }
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
    // TODO: Free the decision tree
    if (node->left==NULL && node->right==NULL){
        free(node);
    }else{
        free_dec_tree(node->left);
        free_dec_tree(node->right);
        free(node);
    }
}

/**
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
    // TODO: Free dataset (Same as A1)
    for (int i=0; i<data->num_items; i++){
        free(data->images[i].data);
    }
    free(data->images);
    free(data->labels);
    free(data);
}
