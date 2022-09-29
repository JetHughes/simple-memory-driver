
// the free list should be initialised with values 0 to MAX_BID - 1
static int8_t free_list[MAX_BID];

// use this code to shuffle the storage position of data blocks
void shuffle(int8_t *array, int n) {
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int8_t t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

// call the shuffle function like this: 
shuffle(free_list+1, MAX_BID-1);

// when given the -r option, instead of storing data at
// sequential blocks (1,2,3... MAX_BID-1) data should be
// stored at positions determined by the values in free_list
// at positions (1,2,3... MAX_BID-1) 
