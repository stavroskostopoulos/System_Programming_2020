
/* Stavros Kostopoulos sdi1700068 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mylib.h"
#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "query.h"
#include "topk.h"


ptrTopk create_top_k_node(int count, char* name){// A function to create a node for the topk array

    ptrTopk temp;
    temp = (ptrTopk)malloc(sizeof(topk));

    temp->count = count;
    temp->name = strdup(name);

    return temp;
}

void destroy_topk(ptrTopk node){//A function to destroy a node for the topk array
    free(node->name);

    free(node);
}
void destroy_heap_tree_node(ptrheaptree node){// A function to destroy a single node of the heap tree
    free(node);
}

ptrheaptree create_heap_tree_node(ptrTopk array_of_topk_element){//A function to crete a heap tree node
    ptrheaptree newtreenode;
    newtreenode = (ptrheaptree)malloc(sizeof(heaptree));

    newtreenode->data = array_of_topk_element;
    newtreenode->left = NULL;
    newtreenode->right = NULL;
    newtreenode->parent = NULL;

    newtreenode->me_left = 0;
    newtreenode->me_right = 0;

    return newtreenode;

}

ptrheaptree fillHeapFromArray(ptrTopk *array_of_topk, int index, int limit){//A function to construct the heap tree reading from the array_of_topk

    //we have inserted all the elements
    if(index >= limit)
        return NULL;

    ptrheaptree node = create_heap_tree_node(array_of_topk[index]);
    array_of_topk[index]->treenode = node;
    //printf("\nindex: %d %d %s\n", index, array_of_topk[index]->treenode->data->count, array_of_topk[index]->treenode->data->name);
    //printf("ti les me index %d deixnw se %p opou to treenode: %p\n", index, array_of_topk[index]->treenode, node);

    //fill left sub-tree
    //printf("eiami o %d me left to %d kai right %d\n", index, 2*index+1, 2*index+2);

    node->left = fillHeapFromArray(array_of_topk, 2*index + 1, limit);

    //node->left->parent = node;
    if(node->left!=NULL){
        node->left->parent = node;
        node->left->me_left = 1;//So know the left child knows it's a left child
    }

    //fill right subtree
    node->right = fillHeapFromArray(array_of_topk, 2*index + 2, limit);
    //node->right->parent = node;
    if(node->right!=NULL){
        node->right->parent = node;
        node->right->me_right = 1;//So know the right child knows it's a right child
    }

    //return the node
    //printf("eimai o index %d me %p kai exw left to %p kai right to %p KAI EIMAI LEFT %d\n", index, node, node->left, node->right, node->me_left);



    return node;

}

void preorder(ptrheaptree root)
{
    if(root==NULL)return;
    max_heapify(root,NULL);
    preorder(root->left);
    preorder(root->right);
}

void swapper(ptrheaptree node1, ptrheaptree node2)
{
    ptrTopk temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}

void max_heapify(ptrheaptree root,ptrheaptree prev)
{
    if(root==NULL){
        return ;
    }
    max_heapify(root->left,root);
    max_heapify(root->right,root);
    if(prev!=NULL && root->data->count > prev->data->count)
    {
        swapper(root,prev);
    }
}

void heaptree_operation( ptrheaptree root, int k, char* country, int limit, ptrTopk *array_of_topk_element){//Limit is the number of elements in the array_of_topk
    //1) print the root(which is the largest number)
    //2) swap values(topK struct pointer) with the last heap tree node)
    //3) destroy the pointer(the node) of the last heap tree node
    //We will do that k times
    //printf("Top-k diseases in %s:\n\n", country);
    int z=0;
    for(z=0 ; z<k ; z++){
        if( z==(limit-1) ){
            //printf("No more diseases in %s!\n", country);
            return;
        }
        //printf("%d %d %d\n", limit, z, (limit-1)-z);

    /*1)*/

        preorder(root);
        //printf("%s cases:   %d\n", root->data->name, root->data->count);
        printf("%s %d\n", root->data->name, root->data->count);

    /*2)*/

        root->data = array_of_topk_element[(limit-1)-z]->treenode->data;

        //printf("count: %d %s\n", root->data->count, root->data->name);

    /*3)*/

        if( (array_of_topk_element[(limit-1)-z]->treenode->me_left) == 1){//An auto to treenode pou molis antallaksame einai left child, prin to kanoume free prepei o parent na exei left = NULL
            array_of_topk_element[(limit-1)-z]->treenode->parent->left = NULL;
        }else if( (array_of_topk_element[(limit-1)-z]->treenode->me_right) == 1){//An auto to treenode pou molis antallaksame einai right child, prin to kanoume free prepei o parent na exei right = NULL
            array_of_topk_element[(limit-1)-z]->treenode->parent->right = NULL;
        }

        free(array_of_topk_element[(limit-1)-z]->treenode);

    }


}

void destroyheaptree(ptrheaptree root){
    if (root == NULL){
        return;
    }

    destroyheaptree(root->left);
    destroyheaptree(root->right);


    //destroy_rbtree_list(root);

    free(root);
}
