#define _CRT_SECURE_NO_WARNINGS

#include "Huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Function to compress
void huffmancompress(FILE* inputFileName, FILE* outputFileName) {
    struct node* list = NULL;
    
    // Compression logic
    list = generate_count(inputFileName, list);
    list = generate_tree(list);
    generate_code(list);
    list = free_tree(list);
    generate_outputFileName(inputFileName, outputFileName, list);
}

// Function to decompress
void huffmandecompress(FILE* inputFileName, FILE* outputFileName) {
    struct node* list = NULL;
    
    // Decompression logic
    list = de_get_sym_list(inputFileName);
    de_generate_outputFileName(inputFileName, outputFileName, list);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("wrong number of parameters.\n");
        exit(0);
    }

    FILE* inputFileName = fopen(argv[1], "rb");
    if (!inputFileName) {
        printf("cannot open source file.\n");
        exit(0);
    }

    fseek(inputFileName, 0, SEEK_END);
    gl_inputFileName_length = ftell(inputFileName);
    fseek(inputFileName, 0, SEEK_SET);

    FILE* outputFileName = fopen(argv[2], "wb");
    if (!outputFileName) {
        printf("cannot open des file.\n");
        exit(0);
    }

    char op;
    printf("compress(y) or decompress(n) or quit(q)?[y/n/q]");
    do {
        op = getchar();
    } while (op != 'y' && op != 'n' && op != 'q');

    if (op == 'q') {
        printf("Thanks for using.");
    } else if (op == 'y') {
        huffmancompress(inputFileName, outputFileName);
    } else if (op == 'n') {
        huffmandecompress(inputFileName, outputFileName);
    }

    if (fclose(inputFileName)) {
        printf("cannot close source file!\n");
        exit(0);
    }

    if (fclose(outputFileName)) {
        printf("cannot close des file!\n");
        exit(0);
    }

    return 0;
}

struct node* generate_count(FILE* f, struct node* list)
{
    printf("counting");
    int count = 0;
    char ch;
    struct node* content_node;

    while (fread(&ch, sizeof(char), 1, f) == 1)
    {
        count++;
        if (count % (gl_inputFileName_length / 10 + 1) == 0)
            printf(".");

        content_node = content_char(list, ch);
        if (content_node)
            content_node->count++;
        else
        {
            gl_total_node_num++;
            list = insert_node(list, create_new_node(ch, 1));
        }
    }

    printf("\n");
    return list;
}

struct node* generate_tree(struct node* list)
{
    printf("generate_tree");

    for (int i = 1; i < gl_total_node_num; i++)
    {
        if (i % (gl_total_node_num / 10 + 1) == 0)
            printf(".");

        struct node* sm_left = get_smallest_node(list);
        struct node* sm_right = get_smallest_node(list);
        struct node* new_node = create_new_node('\0', sm_left->count + sm_right->count);
        new_node->left = sm_left;
        new_node->right = sm_right;

        list = insert_node(list, new_node);
    }

    printf("\n");
    return list;
}

void generate_code(struct node* list)
{
    if (!list)
        return;

    if (list->left)
    {
        strcat(list->left->code, list->code);
        strcat(list->left->code, "0");
        generate_code(list->left);
    }

    if (list->right)
    {
        strcat(list->right->code, list->code);
        strcat(list->right->code, "1");
        generate_code(list->right);
    }
}

struct node* free_tree(struct node* list)
{
    struct node* free_node = list;

    while (list->left && list->right)
    {
        free_node = list;
        list = list->next;
        free(free_node);
    }

    return list;
}

void generate_outputFileName(FILE* sf, FILE* df, struct node* list)
{
    printf("generate_outputFileName");

    struct node* index = list;
    while (index)
    {
        fprintf(df, "%d %s ", index->sym, index->code);
        index = index->next;
    }

    fprintf(df, "%d %s\n", END_SYM_FLAG, "END");

    char ch, des_ch = '\0';
    int des_ch_length = 0;
    fseek(sf, 0, SEEK_SET);
    int count = 0;

    while (fread(&ch, sizeof(char), 1, sf) == 1)
    {
        count++;
        if (count % (gl_inputFileName_length / 10 + 1) == 0)
            printf(".");

        struct node* content_node = content_char(list, ch);
        if (!content_node)
        {
            printf("error:cannot match with sym list\n");
            exit(0);
        }

        char* current_code = content_node->code;
        for (int i = 0; i < strlen(current_code); i++)
        {
            des_ch = des_ch << 1;
            des_ch_length++;

            if (current_code[i] == '1')
                des_ch |= (char)1;

            if (des_ch_length == 8)
            {
                if (!fwrite(&des_ch, sizeof(char), 1, df))
                {
                    printf("error:cannot write to des file.\n");
                    exit(0);
                }

                des_ch_length = 0;
                des_ch = 0;
            }
        }
    }

    if (des_ch_length != 0)
    {
        des_ch = des_ch << BIT_WIDTH_CHAR - des_ch_length;
        if (!fwrite(&des_ch, sizeof(char), 1, df))
        {
            printf("error:cannot write to des file.\n");
            exit(0);
        }
    }

    fprintf(df, "%d", des_ch_length);
    printf("\n");
}

struct node* de_get_sym_list(FILE* f)
{
    printf("getting sym list...\n");

    char code[MAX_CODE_LENGTH];
    int ch_int;
    struct node* list = NULL;

    fscanf(f, "%d %s", &ch_int, code);

    while (ch_int != END_SYM_FLAG)
    {
        struct node* new_node = create_new_node((char)ch_int, 0);
        strcpy(new_node->code, code);
        list = insert_node(list, new_node);
        fscanf(f, "%d %s", &ch_int, code);
    }

    fgetc(f);
    return list;
}

void de_generate_outputFileName(FILE* sf, FILE* df, struct node* list)
{
    printf("generating des file");

    char ch;
    char temp_code[MAX_CODE_LENGTH] = {'\0'};
    int last_length;
    long current_file_index, file_length;
    current_file_index = ftell(sf);
    fseek(sf, -((long)sizeof(char)), SEEK_END);
    file_length = ftell(sf);
    last_length = fgetc(sf) - '0';
    fseek(sf, current_file_index, SEEK_SET);

    char mask = ((char)1) << BIT_WIDTH_CHAR - 1;
    char append[] = {'0', '\0'};
    struct node* the_node;

    for (int i = current_file_index; i < file_length - 1; i++)
    {
        if (i % ((file_length - current_file_index) / 10 + 1) == 0)
            printf(".");

        if (fread(&ch, sizeof(char), 1, sf) != 1)
        {
            if (ferror(sf))
                printf("error:cannot read file.\n");
            if (feof(sf))
                printf("end of reading file.\n");
            exit(0);
        }

        for (int j = 0; j < BIT_WIDTH_CHAR; j++)
        {
            append[0] = '0' + ((ch & mask) == 0 ? 0 : 1);
            strcat(temp_code, append);
            ch = ch << 1;

            the_node = content_code(list, temp_code);
            if (the_node)
            {
                if (fwrite(&(the_node->sym), sizeof(char), 1, df) != 1)
                {
                    printf("error:failed to write file.\n");
                    exit(0);
                }

                temp_code[0] = '\0';
            }
        }
    }

    if (fread(&ch, sizeof(char), 1, sf) != 1)
    {
        printf("error:cannot read file.\n");
        exit(0);
    }

    for (int i = 0; i < last_length; i++)
    {
        append[0] = '0' + ((ch & mask) == 0 ? 0 : 1);
        strcat(temp_code, append);
        ch = ch << 1;

        the_node = content_code(list, temp_code);
        if (the_node)
        {
            if (fwrite(&(the_node->sym), sizeof(char), 1, df) != 1)
            {
                printf("error:failed to write file.\n");
                exit(0);
            }

            temp_code[0] = '\0';
        }
    }

    if (strlen(temp_code) != 0)
    {
        printf("error:code error.\n");
        exit(0);
    }

    printf("\n");
}

struct node* content_char(struct node* list, char ch)
{
    while (list)
    {
        if (list->sym == ch)
        {
            return list;
        }
        list = list->next;
    }

    return NULL;
}

struct node* content_code(struct node* list, const char* code)
{
    while (list)
    {
        if (!strcmp(list->code, code))
        {
            return list;
        }
        list = list->next;
    }

    return NULL;
}

struct node* create_new_node(char ch, int count)
{
    struct node* new_node = (struct node*)malloc(sizeof *new_node);
    if (!new_node)
    {
        printf("error:failed to malloc.\n");
        exit(0);
    }

    new_node->code[0] = '\0';
    new_node->sym = ch;
    new_node->count = count;
    new_node->next = NULL;
    new_node->checked = 0;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

struct node* insert_node(struct node* list, struct node* new_node)
{
    if (list)
        new_node->next = list;

    return new_node;
}

void print_list(struct node* list)
{
    printf("All nodes...\n");
    int count = 0;

    while (list)
    {
        count++;
        printf("count:\t%d \tsym:\t%d\t checked:\t%d\t %s\n",
               list->count, list->sym, list->checked, list->code);
        list = list->next;
    }

    printf("print node count: %d\n", count);
}

struct node* get_smallest_node(struct node* list)
{
    while (list && list->checked)
        list = list->next;

    struct node* smallest = list;

    while (list)
    {
        if (!list->checked && (list->count < smallest->count))
            smallest = list;

        list = list->next;
    }

    if (smallest)
        smallest->checked++;

    return smallest;
}