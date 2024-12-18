#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
// #include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct List *globalList;  // Global linked list for top K integers
int K; int messageSendCounter; int flagCounter;

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

void insert_sorted_only_unique(struct Node *node, struct List *list, int knum) {
    if (list->head == NULL && list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        struct Node *ptrr = list->head;
        while (ptrr != NULL){
            if(ptrr->id == node->id){
                free(node->name);
                free(node);
                return;
            }
            ptrr = ptrr->next;
        }
        struct Node *ptrrr = list->head;
        int cc = 0; int flag = 0;
        while (ptrrr != NULL){
            if(ptrrr->id < node->id && cc > knum){
                remove_by_id(ptrrr->id, list);
                ptrrr = list->tail;
                flag = 1;
            }
            cc += 1;
            ptrrr = ptrrr->next;
        }
        if(cc < knum){
            flag = 1;
        }
        if (flag != 1){
            struct Node *ptrrrr = list->head;
            while (ptrrrr != NULL){
                if(ptrrrr->id < node->id){
                    remove_by_id(ptrrrr->id, list);
                    ptrrrr = list->tail;
                    flag = 1;
                }
                cc += 1;
                ptrrrr = ptrrrr->next;
            }
        }
        if(flag == 1){
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
        }else{
            free(node->name);
            free(node);
        }
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

void get_top_K_values(struct List *list, int Knum){
    struct List *listInOrder = create_list();
    struct Node *ptr = list->tail, *tmpPtr = NULL;
    int cFlag = 0;
    while (ptr != NULL && cFlag < Knum) {
        tmpPtr = create_node((ptr->name), (ptr->id));
        insert_sorted(tmpPtr, listInOrder);
        ptr = ptr->prev;
        cFlag += 1;
    }
    cFlag = 0;
    list->head = NULL;
    list->tail = NULL;
    ptr = listInOrder->head;
    while (ptr != NULL && cFlag < Knum) {
        tmpPtr = create_node((ptr->name), (ptr->id));
        insert_tail(tmpPtr,list);
        ptr = ptr->next;
        cFlag += 1;
    }
    destroy_list(listInOrder);
}

void process_file(char *fileName, int msgid) {
    struct List *localList = create_list();
    FILE *file = fopen(fileName, "r");
    int lineNum; int c = 0;

    while (fscanf(file, "%d", &lineNum) == 1) {
        struct Node *tmp = create_node("Node", lineNum);
        insert_sorted_only_unique(tmp, localList, K);
        c += 1;
    }
    free(fileName);
    fclose(file);

    struct Node *ptr = localList->head;
    while (ptr != NULL) {
        msgsnd(msgid, &ptr->id, sizeof(ptr->id), 0);
        ptr = ptr->next;
    }
    destroy_list(localList);
}

int main(int argc, char *argv[])
{
    struct Node *tmp = NULL;
    K = atoi(argv[1]);
    char *directoryPath = argv[2];
    char *outputFile = argv[3];
    printf("++++++++++++++++\n");
    struct dirent *pDirent;
    DIR *pDir = opendir(directoryPath);
    char *checkIfGoodfileOne = "in";
    char *checkIfGoodfileTwo = ".txt";

    globalList = create_list(); 
    pid_t child_pid, wpid;
    int status = 0;
    messageSendCounter = 0;
    flagCounter = 0;

    key_t key = ftok("sender.c", 1);
    int msgid = msgget(key, 0644 | IPC_CREAT);
    
    while ((pDirent = readdir(pDir)) != NULL) {
        char *nameOfFile = pDirent->d_name;
        if(strstr(nameOfFile,checkIfGoodfileOne) && strstr(nameOfFile,checkIfGoodfileTwo)){
            size_t pathLength = strlen(directoryPath) + strlen(nameOfFile) + 2;
            char *tmpp = malloc(pathLength);
            snprintf(tmpp, pathLength, "%s/%s", directoryPath, nameOfFile);

            child_pid = fork();
            if (child_pid == 0) {
                process_file(tmpp, msgid);
                exit(0);
            }
            free(tmpp);
            messageSendCounter += K;
        }
    }
    while ((wpid = wait(&status)) > 0);
    int iddd;
    while (msgrcv(msgid, &iddd, sizeof(iddd), 0, 0) != -1) {
        struct Node *tmp = create_node("Node", iddd);
        insert_sorted_only_unique(tmp, globalList, K);
        flagCounter += 1;
        if (flagCounter == messageSendCounter){
            break;
        }
    }
    FILE *fptr;
    fptr = fopen(outputFile, "w");
    int tempKHolder = K; int highCounter = 1;
    int finalArr[tempKHolder];
    tmp = globalList->head;
    while(tmp != NULL){
        finalArr[(tempKHolder-highCounter)] = tmp->id;
        highCounter += 1;
        tmp = tmp->next;
    }
    for(int i = 0; i < K; i++){
        fprintf(fptr,"%d\n", finalArr[i]);
    }
    printf("++++++++++++++++\n");
    // free(pDirent);
    closedir (pDir);
    fclose(fptr);

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}