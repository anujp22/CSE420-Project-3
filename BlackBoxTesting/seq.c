#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

struct Node {
    char *name;
    int id;
    struct Node *next;
    struct Node *prev;
};

struct List {
    struct Node *head;
    struct Node *tail;
};

struct Node *create_node(char *name, int id) {
    struct Node *node = malloc(sizeof(struct Node));
    if (node == NULL) {
        fprintf (stderr, "%s: Couldn't create memory for the node; %s\n", "linkedlist", strerror(errno));
        exit(-1);
    }
    node->name = strdup(name);
    node->id = id;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

struct List *create_list() {
    struct List *list = malloc(sizeof(struct List));
    if (list == NULL) {
        fprintf (stderr, "%s: Couldn't create memory for the list; %s\n", "linkedlist", strerror (errno));
        exit(-1);
    }
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void insert_tail(struct Node *node, struct List *list) {
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
}

void print_list(struct List *list) {
    struct Node *ptr = list->head;  
    while (ptr != NULL) {
        if (ptr != list->head) {
        printf("->");
        }
        printf("(%s,%d)", ptr->name, ptr->id);
        ptr = ptr->next;
    }
    printf("\n");
}

void destroy_list(struct List *list) {
    struct Node *ptr = list->head;
    struct Node *tmp;  
    while (ptr != NULL) {
        free(ptr->name);
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    free(list);
}

struct Node *find_by_id(int id, struct List *list) {
    struct Node *ptr = list->head;  
    while (ptr != NULL) {
        if (ptr->id == id) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void remove_by_id(int id, struct List *list) {
    struct Node *ptr = list->head, *nodeToBeRemoved = find_by_id(id, list), *tmpPtr = NULL;
    if (nodeToBeRemoved != NULL) {
        while (ptr != NULL){
            if (ptr == nodeToBeRemoved && ptr != list->head && ptr != list->tail){
                free(ptr->name);
                tmpPtr = ptr;
                ptr->prev->next = nodeToBeRemoved->next;
                nodeToBeRemoved->next->prev = ptr->prev;
                free(tmpPtr);
                return;
            }
            else if (ptr == nodeToBeRemoved && ptr == list->head){
                free(ptr->name);
                tmpPtr = ptr;
                list->head = ptr->next;
                free(tmpPtr);
                return;
            }
            else if (ptr == nodeToBeRemoved && ptr == list->tail){
                free(ptr->name);
                tmpPtr = ptr;
                ptr->prev->next = NULL;
                list->tail = ptr->prev;
                free(tmpPtr);
                return;
            }
            ptr = ptr->next;
        }
    }
}

void insert_sorted(struct Node *node, struct List *list) {
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        struct Node *ptr = list->head, *ptrPrev = NULL;
        while (ptr != NULL){
            if (node->id < ptr->id && ptr == list->head){
                list->head = node;
                node->next = ptr;
                ptr->prev = node;
                return;
            }else if(node->id < ptr->id && ptr == list->tail){
                ptrPrev->next = node;
                node->next = ptr;
                ptr->prev = node;
                return;
            }else if(node->id < ptr->id){
                node->next = ptr;
                ptrPrev->next = node;
                return;
            }
            ptrPrev = ptr;
            ptr = ptr->next;
        }
        insert_tail(node, list);
    }
}

void insert_sorted_only_unique(struct Node *node, struct List *list) {
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        struct Node *ptrr = list->head;
        while (ptrr != NULL){
            if(ptrr->id == node->id){
                return;
            }
            ptrr = ptrr->next;
        }
        struct Node *ptr = list->head, *ptrPrev = NULL;
        while (ptr != NULL){
            if (node->id < ptr->id && ptr == list->head){
                list->head = node;
                node->next = ptr;
                ptr->prev = node;
                return;
            }else if(node->id < ptr->id && ptr == list->tail){
                ptrPrev->next = node;
                node->next = ptr;
                ptr->prev = node;
                return;
            }else if(node->id < ptr->id){
                node->next = ptr;
                ptrPrev->next = node;
                return;
            }
            ptrPrev = ptr;
            ptr = ptr->next;
        }
        insert_tail(node, list);
    }
}

void insert_after_id(int id, struct Node *node, struct List *list) {
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        struct Node *ptr = list->tail, *ptrPrev = NULL;
        while (ptr != NULL){
            if(ptr->id == id && ptr == list->tail){
                ptr->next = node;
                node->prev = ptr;
                list->tail = node;
                return;
            }else if(ptr->id == id){
                node->next = ptrPrev;
                node->prev = ptr;
                ptr->next = node;
                ptrPrev->prev = node;
                return;
            }
            ptrPrev = ptr;
            ptr = ptr->prev;
        }
        insert_tail(node, list);
    }
}

void insertion_sort_by_ID_increasing(struct List *list) {
    struct List *listInOrder = create_list();
    struct Node *ptr = list->head, *tmpPtr = NULL;
    while (ptr != NULL) {
        tmpPtr = create_node((ptr->name), (ptr->id));
        insert_sorted(tmpPtr, listInOrder);
        ptr = ptr->next;
    }
    list->head = NULL;
    list->tail = NULL;
    ptr = listInOrder->head;
    while (ptr != NULL) {
        tmpPtr = create_node((ptr->name), (ptr->id));
        insert_tail(tmpPtr,list);
        ptr = ptr->next;
    }
    destroy_list(listInOrder);
}

int main(int argc, char *argv[])
{
    struct List *list = create_list();
    struct Node *tmp = NULL;

    int K = atoi(argv[1]);
    char *directoryPath = argv[2];
    char *outputFile = argv[3];
    printf("++++++++++++++++\n");
    struct dirent *pDirent;
    DIR *pDir = opendir(directoryPath);
    char *checkIfGoodfileOne = "in";
    char *checkIfGoodfileTwo = ".txt";
    while ((pDirent = readdir(pDir)) != NULL) {
        char *nameOfFile = pDirent->d_name;
        if(strstr(nameOfFile,checkIfGoodfileOne) && strstr(nameOfFile,checkIfGoodfileTwo)){
            FILE *file;
            char line[256];
            //this below code took so long for no reason :(
            size_t pathLength = strlen(directoryPath) + strlen(nameOfFile) + 2; // +2 for '/' and '\0'
            char *tmpp = malloc(pathLength);
            snprintf(tmpp, pathLength, "%s/%s", directoryPath, nameOfFile);
            file = fopen(tmpp, "r");
            int c = 0;
            while (fgets(line, sizeof(line), file)) {
                // printf("%d\n", c);
                tmp = create_node("Node",atoi(line));
                insert_sorted_only_unique(tmp, list);
                c += 1; 
            }
        }
    }
    FILE *fptr;
    fptr = fopen(outputFile, "w");
    for(int i = 0; i < K; i++){
        int highID = list->tail->id;
        printf("%d\n", highID);
        fprintf(fptr,"%d\n", highID);
        remove_by_id(highID,list);
        insertion_sort_by_ID_increasing(list);
        // print_list(list);
    }
    printf("++++++++++++++++\n");
    free(pDirent);
    closedir (pDir);
    destroy_list(list);
    fclose(fptr);
    return 0;
}