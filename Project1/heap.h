

/* Stavros Kostopoulos sdi1700068 */

#ifndef TILE
#define TILE

#include "topk.h"


typedef struct heaptree{
    ptrTopk data;
    struct heaptree* left;
    struct heaptree* right;
    struct heaptree* parent;
}heaptree;

typedef heaptree *ptrheaptree;


#endif
