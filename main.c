#include <stdio.h>

struct Node_type;

typedef struct Neighbor_type {
    struct Node_type* target;
    struct Neighbor_type* next;
}Neighbor;

typedef struct Node_type {
    Neighbor* head_neighbor;
    struct Node_type* next;
}Node;

void readFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char addr1[15], addr2[15];
    int d = 0;
    int r = fscanf(fp, "%s ,%s ,%d", addr1, addr2, &d);
    printf("r: %d\n", r);
    printf("address1: %s,%s,%d\n", addr1, addr2, d);
    fclose(fp);
}


int main() {
    readFile("graph.dat");
    return 0;
}