
/* Stavros Kostopoulos sdi1700068 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"





// Left Rotation
void LeftRotate(redblackPtr *root,redblackPtr x)
{
    if (!x || !x->right)
        return ;
    //y stored pointer of right child of x
    redblackPtr y = x->right;

    //store y's left subtree's pointer as x's right child
    x->right = y->left;

    //update parent pointer of x's right
    if (x->right != NULL)
        x->right->parent = x;

    //update y's parent pointer
    y->parent = x->parent;

    // if x's parent is null make y as root of tree
    if (x->parent == NULL)
        (*root) = y;

    // store y at the place of x
    else if (x == x->parent->left)
        x->parent->left = y;
    else    x->parent->right = y;

    // make x as left child of y
    y->left = x;

    //update parent pointer of x
    x->parent = y;
}


// Right Rotation (Similar to LeftRotate)
void rightRotate( redblackPtr *root, redblackPtr y)
{
    if (!y || !y->left)
        return ;
    redblackPtr x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent =y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

// Utility function to fixup the Red-Black tree after standard BST insertion
void insertFixUp( redblackPtr *root, redblackPtr z)
{
    // iterate until z is not the root and z's parent color is red
    while (z != *root && z != (*root)->left && z != (*root)->right && z->parent->color == 'R')
    {
        redblackPtr y;

        // Find uncle and store uncle in y
        if (z->parent && z->parent->parent && z->parent == z->parent->parent->left)
            y = z->parent->parent->right;
        else
            y = z->parent->parent->left;

        // If uncle is RED, do following
        // (i)  Change color of parent and uncle as BLACK
        // (ii) Change color of grandparent as RED
        // (iii) Move z to grandparent
        if (!y)
            z = z->parent->parent;
        else if (y->color == 'R')
        {
            y->color = 'B';
            z->parent->color = 'B';
            z->parent->parent->color = 'R';
            z = z->parent->parent;
        }

        // Uncle is BLACK, there are four cases (LL, LR, RL and RR)
        else
        {
            // Left-Left (LL) case, do following
            // (i)  Swap color of parent and grandparent
            // (ii) Right Rotate Grandparent
            if (z->parent == z->parent->parent->left &&
                z == z->parent->left)
            {
                char ch = z->parent->color ;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rightRotate(root,z->parent->parent);
            }

            // Left-Right (LR) case, do following
            // (i)  Swap color of current redblack  and grandparent
            // (ii) Left Rotate Parent
            // (iii) Right Rotate Grand Parent
            if (z->parent && z->parent->parent && z->parent == z->parent->parent->left &&
                z == z->parent->right)
            {
                char ch = z->color ;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                LeftRotate(root,z->parent);
                rightRotate(root,z->parent->parent);
            }

            // Right-Right (RR) case, do following
            // (i)  Swap color of parent and grandparent
            // (ii) Left Rotate Grandparent
            if (z->parent && z->parent->parent &&
                z->parent == z->parent->parent->right &&
                z == z->parent->right)
            {
                char ch = z->parent->color ;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                LeftRotate(root,z->parent->parent);
            }

            // Right-Left (RL) case, do following
            // (i)  Swap color of current redblack  and grandparent
            // (ii) Right Rotate Parent
            // (iii) Left Rotate Grand Parent
            if (z->parent && z->parent->parent && z->parent == z->parent->parent->right &&
                z == z->parent->left)
            {
                char ch = z->color ;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                rightRotate(root,z->parent);
                LeftRotate(root,z->parent->parent);
            }
        }
    }
    (*root)->color = 'B'; //keep root always black
}

// Utility function to insert newly redblack in RedBlack tree
void insert( redblackPtr *root, ptrList patient)
{
    //printf("INSERT\n");
    int diplotypo=0;

    // Allocate memory for new redblack
    redblackPtr z = (redblackPtr)malloc(sizeof(struct redblack));




    //Atch the patient list node to the red black tree node
    z->patient = patient;



    z->next = NULL;

    z->left = z->right = z->parent = NULL;

     //if root is null make z as root
    if (*root == NULL)
    {
        //printf("prwthfora\n");
        z->color = 'B';
        (*root) = z;

    }
    else
    {

        redblackPtr y = NULL;
        redblackPtr x = (*root);

        // Follow standard BST insert steps to first insert the redblack
        while (x != NULL)
        {
            y = x;

            //printDate(z->patient->entryDate);
            //printf(" ");
            //printDate(x->patient->entryDate);

            if (compare_dates(z->patient->entryDate, x->patient->entryDate)==-1){
                x = x->left;
                //printf("mphka mikrotero\n");
            }
            else if(compare_dates(z->patient->entryDate, x->patient->entryDate)==1){
                x = x->right;
            }else if(compare_dates(z->patient->entryDate, x->patient->entryDate)==0){//An einai isa ta dates(diplotypo)
                diplotypo=1;

                break;
            }
        }

        if(diplotypo==0){
            z->parent = y;
        }else{
            z->parent = y->parent;
        }


        if (compare_dates(z->patient->entryDate, y->patient->entryDate)==1){
            y->right = z;
            //printf("mphka megalutero\n");
        }
        else if (compare_dates(z->patient->entryDate, y->patient->entryDate)==-1){
            y->left = z;
        }else{//An eixame diplotypo
            redblackPtr temp;
            temp = y;
            while(temp->next!=NULL){//Pame sto telos ths listas
                temp=temp->next;
            }

            temp->next = z;


        }

        z->color = 'R';

        // call insertFixUp to fix reb-black tree's property if it
        // is voilated due to insertion.
        if(diplotypo==0){ insertFixUp(root,z); }//Fix up MONO AN den exw dipolotypo.To diplotypo ousiastika den ephreazei katholou to dentro mas

    }




}


void destroyrbt(redblackPtr root){
    if (root == NULL){
        return;
    }

    destroyrbt(root->left);
    destroyrbt(root->right);


    destroy_rbtree_list(root);

    //free(root);
}

void destroy_rbtree_list(redblackPtr list){//A function to delete the *next nodes of a rbtree node(se periptwsh pou upirxan diplotypa)

    if(list==NULL){return;}
    destroy_rbtree_list(list->next);
    free(list);

}

int count_rbtree_nodes(redblackPtr root){//A function to count the nodes of a RBT

    int c =  1;             //Node itself should be counted
    if (root ==NULL)
        return 0;
    else
    {
        c += count_rbtree_nodes(root->left);
        c += count_rbtree_nodes(root->right);
        c += count_rbtree_nodes(root->next);
        return c;
    }
}

int count_rbtree_nodes_with_dates(redblackPtr root, ptrDate date1, ptrDate date2){//A function to count the nodes of a RBT between two dates

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( (compare_dates(root->patient->entryDate, date1)==1) && (compare_dates(root->patient->entryDate, date2)==-1) ){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_with_dates(root->left, date1, date2);
        c += count_rbtree_nodes_with_dates(root->right, date1, date2);
        c += count_rbtree_nodes_with_dates(root->next, date1, date2);
        return c;
    }
}

int count_rbtree_nodes_with_age_and_country_and_date(redblackPtr root, char* country, int down_age_limit, int up_age_limit, ptrDate date){//A function to count the nodes of a RBT in a specific country in a specific age limit

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( !strcmp(root->patient->country, country) && (root->patient->age>=down_age_limit) && (root->patient->age<=up_age_limit) && (compare_dates(root->patient->entryDate, date)==0) ){//If patient is in country
            c=1;
        }
        c += count_rbtree_nodes_with_age_and_country_and_date(root->left, country, down_age_limit, up_age_limit, date);
        c += count_rbtree_nodes_with_age_and_country_and_date(root->right, country, down_age_limit, up_age_limit, date);
        c += count_rbtree_nodes_with_age_and_country_and_date(root->next, country, down_age_limit, up_age_limit, date);
        return c;
    }
}

int count_rbtree_nodes_with_country(redblackPtr root, char* country){//A function to count the nodes of a RBT in a specific country

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( !strcmp(root->patient->country, country) ){//If patient is in country

            c=1;
        }
        c += count_rbtree_nodes_with_country(root->left, country);
        c += count_rbtree_nodes_with_country(root->right, country);
        c += count_rbtree_nodes_with_country(root->next, country);
        return c;
    }
}

int count_rbtree_nodes_with_disease(redblackPtr root, char* disease){//A function to count the nodes of a RBT in a specific diseaseID

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( !strcmp(root->patient->diseaseID, disease) ){//If patient is in country

            c=1;
        }
        c += count_rbtree_nodes_with_disease(root->left, disease);
        c += count_rbtree_nodes_with_disease(root->right, disease);
        c += count_rbtree_nodes_with_disease(root->next, disease);
        return c;
    }
}

void printinorder(redblackPtr root){

    if (root == NULL){
        return;
    }
    printinorder(root->left);

    printf("%s\n", root->patient->patientFirstName);

    printinorder(root->right);
}
