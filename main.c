#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define new(size, type) (type*)malloc((size)*sizeof(type))
#define INFINITY INT_MAX

#define false 0
#define true 1
typedef char bool;

struct Node_type;

typedef struct Neighbor_type {
    int distance;
    struct Node_type* target;
    struct Neighbor_type* next;
}Neighbor;

typedef struct Node_type {
    char* name;
    bool final;
    int d;
    Neighbor* head_neighbor;
    struct Node_type* next;
    struct Node_type* last;
}Node;

typedef struct Boundary_type {
    Node* node;
    struct Boundary_type* left;
    struct Boundary_type* right;
} Boundary;

Node* readFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char addr1[15], addr2[15];
    int d = 0;
    Node* head = NULL;
    int i = 0;
    while (fscanf(fp, "%s %s %d", addr1, addr2, &d) == 3) {
        if (head == NULL) {
            head = new(1, Node);
            head->d = INFINITY;
            head->final = false;
            head->last = NULL;
            head->name = new(strlen(addr1), char);
            strcpy(head->name, addr1);

            Node* dest = new(1, Node);
            dest->d = INFINITY;
            dest->final = false;
            dest->last = NULL;
            dest->name = new(strlen(addr2), char);
            strcpy(dest->name, addr2);

            head->next = dest;
            head->head_neighbor = new(1, Neighbor);
            head->head_neighbor->target = dest;
            head->head_neighbor->next = NULL;
            head->head_neighbor->distance = d;
            dest->next = NULL;
            dest->head_neighbor = new(1, Neighbor);
            dest->head_neighbor->target = head;
            dest->head_neighbor->next = NULL;
            dest->head_neighbor->distance = d;
        } else {
            Node* t = head;
            Node* src = NULL;
            Node* dest = NULL;
            while (t) {
                if (src == NULL && strcmp(t->name, addr1) == 0) {
                    src = t;
                }
                if (dest == NULL && strcmp(t->name, addr2) == 0) {
                    dest = t;
                }
                if (src && dest) {
                    break;
                }
                t = t->next;
            }
            if (src == NULL) {
                src = new(1, Node);
                src->d = INFINITY;
                src->final = false;
                src->last = NULL;
                src->name = new(strlen(addr1), char);
                strcpy(src->name, addr1);
                src->next = head;
                head = src;
            }
            if (dest == NULL) {
                dest = new(1, Node);
                dest->final = false;
                dest->d = INFINITY;
                dest->last = NULL;
                dest->name = new(strlen(addr2), char);
                strcpy(dest->name, addr2);
                dest->next = head;
                head = dest;
            }
            Neighbor* n1 = new(1, Neighbor);
            n1->distance = d;
            n1->target = dest;
            n1->next = src->head_neighbor;
            src->head_neighbor = n1;
            Neighbor* n2 = new(1, Neighbor);
            n2->distance = d;
            n2->target = src;
            n2->next = dest->head_neighbor;
            dest->head_neighbor = n2;
        }
        ++i;
    }
    printf("i: %d\n", i);
    fclose(fp);
    return head;
}

void destroyBoundaryList(Boundary* bd) {
    if (bd == NULL) {
        return;
    }
    destroyBoundaryList(bd->left);
    destroyBoundaryList(bd->right);
    free(bd);
}

void shortestPath (Node* graph, char* srcName, char* dstName) {
    Node* t = graph;
    while (t) {
        if (strcmp(t->name, srcName) == 0) {
            break;
        }
        t = t->next;
    }
    if (t == NULL) {
        printf("%s is not found in graph.\n", srcName);
        return;
    }
    Node* src = t;
    src->d = 0;
    Boundary* bd = new(1, Boundary);
    bd->left = NULL;
    bd->node = src;
    bd->right = NULL;
    while (bd) {
        // find minimum boundary and remove it from list
        Boundary* t = bd;
        Boundary* last = NULL;
        if (t->left) {
            last = t;
            t = t->left;
            while (t->left) {
                last = t;
                t = t->left;
            }
        }
        Boundary* min = t;
        if (last) {
            last->left = min->right;
        } else {
            bd = bd->right;
        }
        min->right = NULL;
        // examine destination
        if (strcmp(min->node->name, dstName) == 0) {
            printf("we find it!\n");
            Node* dst = min->node;
            while (dst) {
                printf("%s <- ", dst->name);
                dst = dst->last;
            }
            printf("\n");
            destroyBoundaryList(bd);
            free(min);
            return;
        }
        // set minimum boundary to final and find its neighbor nodes
        min->node->final = true;
        Neighbor* neighbor = min->node->head_neighbor;
        while (neighbor) {
            // ignore final neighbor
            if (neighbor->target->final) {
                neighbor = neighbor->next;
                continue;
            }
            // update neighbor's distance
            if (neighbor->target->d > (neighbor->distance + min->node->d)) {
                if (neighbor->target->d == INFINITY) {
                    // insert into boundary list
                    Boundary* newBd = new(1, Boundary);
                    newBd->node = neighbor->target;
                    newBd->left = NULL;
                    newBd->right = NULL;
                    if (bd == NULL) {
                        bd = newBd;
                    } else {
                        Boundary *t = bd;
                        while (true) {
                            if (newBd->node->d < t->node->d) {
                                if (t->left == NULL) {
                                    t->left = newBd;
                                    break;
                                } else {
                                    t = t->left;
                                }
                            } else {
                                if (t->right == NULL) {
                                    t->right = newBd;
                                    break;
                                } else {
                                    t = t->right;
                                }
                            }
                        }
                    }
                }
                neighbor->target->d = neighbor->distance + min->node->d;
                neighbor->target->last = min->node;
            }
            neighbor = neighbor->next;
        }
        free(min);
    }
    destroyBoundaryList(bd);
    printf("Not found\n");
}

int main() {
    Node* head = readFile("graph.dat");
    Node* t = head;
    while (t) {
        printf("%s -> ", t->name);
        Neighbor* n = t->head_neighbor;
        while (n) {
            Node* dst = n->target;
            printf("%s=%d,", dst->name, n->distance);
            n = n->next;
        }
        printf("\n");
        t = t->next;
    }
    char* src = "1";
    char* dst = "5";
    shortestPath(head, src, dst);
    // free memory
    t = head;
    while (t) {
        free(t->name);
        Neighbor* n = t->head_neighbor;
        while (n) {
            Neighbor* del = n;
            n = n->next;
            free(del);
        }
        Node* del = t;
        t = t->next;
        free(del);
    }
    return 0;
}