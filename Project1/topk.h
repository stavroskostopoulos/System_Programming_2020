
/* Stavros Kostopoulos sdi1700068 */

#ifndef TOPKE
#define TOPKE

struct heaptree;

typedef struct topk{
    int count;
    char* name;
    struct heaptree* treenode;
}topk;

typedef topk *ptrTopk;


typedef struct heaptree{
    ptrTopk data;
    int me_left;//To know if it's a left child
    int me_right;//To know if it's a right child
    struct heaptree* left;
    struct heaptree* right;
    struct heaptree* parent;
}heaptree;

typedef heaptree *ptrheaptree;

ptrTopk create_top_k_node(int count, char* name);// A function to create a node for the topk array
void destroy_topk(ptrTopk node);//A function to destroy a node for the topk array
void destroy_heap_tree_node(ptrheaptree node);// A function to destroy a single node of the heap tree
ptrheaptree create_heap_tree_node(ptrTopk array_of_topk_element);//A function to crete a heap tree node
void swapper(ptrheaptree node1, ptrheaptree node2);
void preorder(ptrheaptree root);
ptrheaptree fillHeapFromArray(ptrTopk *array_of_topk, int index, int limit);//A function to construct the heap tree reading from the array_of_topk


void max_heapify(ptrheaptree root,ptrheaptree prev);

void heaptree_operation( ptrheaptree root, int k, char* country, int limit, ptrTopk *array_of_topk_element);//Limit is the number of elements in the array_of_topk
void destroyheaptree(ptrheaptree root);



#endif
