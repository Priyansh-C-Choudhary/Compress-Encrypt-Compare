#ifndef HUFFMAN_H
#define HUFFMAN_H
#define _CRT_SECURE_NO_WARNINGS
#define MAX_CODE_LENGTH 260
#include <stdio.h>

const int BIT_WIDTH_CHAR = 8;
const int END_SYM_FLAG = 200;
int gl_total_node_num = 0;
int gl_inputFileName_length = 0;

struct node
{
    char code[MAX_CODE_LENGTH];
    long count;
    int checked;
    char sym;
    struct node* next, *left, *right;
};

struct node* generate_count(FILE*, struct node*);
struct node* generate_tree(struct node*);
void generate_code(struct node* list);
struct node* free_tree(struct node* list);
void generate_outputFileName(FILE*, FILE*, struct node*);

struct node* de_get_sym_list(FILE*);
void de_generate_outputFileName(FILE*, FILE*, struct node*);

struct node* content_char(struct node* list, char ch);
struct node* content_code(struct node* list, const char* ch);
struct node* create_new_node(char ch, int count);
struct node* insert_node(struct node* list, struct node* new_node);
void print_list(struct node* list);
struct node* get_smallest_node(struct node* list);

void huffmancompress(FILE* inputFileName, FILE* outputFileName);
void huffmandecompress(FILE* inputFileName, FILE* outputFileName);

#endif  // HUFFMAN_H