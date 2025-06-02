#ifndef UMUM_H
#define UMUM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Citation {
    char citing_paper_id[50];
    char citing_paper_title[256];
    int citation_count;
    struct Citation* next;
} Citation;

typedef struct Paper {
    char title[256];
    char field_of_study[100];
    int year;
    int citation_count;
    Citation* citations_head;
    struct Paper* next;
    struct Paper* prev;
} Paper;

#endif