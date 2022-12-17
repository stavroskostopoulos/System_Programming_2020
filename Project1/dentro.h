
/* Stavros Kostopoulos sdi1700068 */
#ifndef RBDENTRO
#define RBDENTRO



#include "mylib.h"

typedef struct redblack
{
    ptrList patient;
    char color;
    struct redblack* left;
    struct redblack* right;
    struct redblack* parent;
    struct redblack* next;
}redblack;


typedef struct redblack *redblackPtr;


/*Red Black Tree Functions*/

void LeftRotate(redblackPtr *root,redblackPtr x);
void rightRotate( redblackPtr *root, redblackPtr y);
void insertFixUp( redblackPtr *root, redblackPtr z);
void insert( redblackPtr *root, ptrList patient);
//void printinorder(redblackPtr root, FILE *fout);
//void printrbtnode(redblackPtr node, FILE *fout, char *aat);
//void printroot(redblackPtr *root);
void destroyrbt(redblackPtr root);
//redblackPtr searchrbt(redblackPtr root, char *aat, FILE *fout);
//redblackPtr getnodes(redblackPtr root);
void destroy_rbtree_list(redblackPtr list);//A function to delete the *next nodes of a rbtree node(se periptwsh pou upirxan diplotypa)
int count_rbtree_nodes(redblackPtr root);//A function to count the nodes of a RBT
void printinorder(redblackPtr root);
int count_rbtree_nodes_with_dates(redblackPtr root, ptrDate date1, ptrDate date2);//A function to count the nodes of a RBT between two dates
int count_rbtree_nodes_with_country(redblackPtr root, char* country);//A function to count the nodes of a RBT in a specific country
int count_rbtree_nodes_with_disease(redblackPtr root, char* disease);//A function to count the nodes of a RBT in a specific diseaseID


#endif
