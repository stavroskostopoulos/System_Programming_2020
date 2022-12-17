
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



/* /globalDiseaseStats */
void all_diseases_of_a_bucket(Bucket_ptr bucket, int size_of_bucket){//A function to print the number of cases for every disease of a bucket

    Bucket_ptr temp;
    temp = bucket;

    int num_of_krousmata;

    int k=0;
    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes(temp->bucket_node_array[k]->rbtree);
                //printf("%d\n", num_of_krousmata);

                //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                printf("%s %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
            }
        }


        temp = temp->next;
    }

}

void global_disease_stats(Bucket_ptr bucket, int size_of_bucket, ptrDate date1, ptrDate date2){//A function to print the number of cases for every disease of a bucket

    Bucket_ptr temp;
    temp = bucket;

    int num_of_krousmata;

    int k=0;
    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_with_dates(temp->bucket_node_array[k]->rbtree, date1, date2);
                //printf("%d\n", num_of_krousmata);
                //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                printf("%s %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                
            }
        }


        temp = temp->next;
    }

}



/*-----------------------*/

/*     /topk-Diseases    */

int count_buketnodes_of_bucket_with_country(Bucket_ptr bucket, int size_of_bucket, char* country){//Count the diseases of a country
    Bucket_ptr temp;
    temp = bucket;

    int disease_count=0;
    int num_of_krousmata=0;
    int k=0;


    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_with_country(temp->bucket_node_array[k]->rbtree, country);

                if(num_of_krousmata!=0){ disease_count++; }//An vethike estw enas patient me authn thn arrwstia(bucket_node_array[k]->rbtree arrwstia) sto country pou theloume,count this disease
            }
        }


        temp = temp->next;
    }

    return disease_count;
}


void names_and_counts_of_buckets_to_ptrtok(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* country){//A function to pass the number of cases and the name of each disease of a bucket to ptrTok

    Bucket_ptr temp;
    int i=0;

    int array_index=0;

    for(i=0 ; i<HashtableNumOfEntries ; i++){
        temp = hash[i];

        int num_of_krousmata;

        int k=0;

        while(temp!=NULL){

            for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

                if( temp->bucket_node_array[k] != NULL){


                    num_of_krousmata = 0;
                    num_of_krousmata = count_rbtree_nodes_with_country(temp->bucket_node_array[k]->rbtree, country);
                    //printf("%d\n", num_of_krousmata);
                    //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                    if(num_of_krousmata!=0){

                        array_of_topk[array_index] = create_top_k_node( num_of_krousmata, temp->bucket_node_array[k]->name);

                        array_index++;
                    }
                }
            }


            temp = temp->next;
        }
    }
}



//With dates Now


int count_buketnodes_of_bucket_with_country_and_dates(Bucket_ptr bucket, int size_of_bucket, char* country, ptrDate date1, ptrDate date2){//Count the diseases of a country(with dates)
    Bucket_ptr temp;
    temp = bucket;

    int disease_count=0;
    int num_of_krousmata=0;
    int k=0;


    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_with_dates_and_country(temp->bucket_node_array[k]->rbtree, date1, date2, country);

                if(num_of_krousmata!=0){ disease_count++; }//An vethike estw enas patient me authn thn arrwstia(bucket_node_array[k]->rbtree arrwstia) sto country pou theloume,count this disease
            }
        }


        temp = temp->next;
    }

    return disease_count;
}

void names_and_counts_of_buckets_to_ptrtok_with_dates(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* country, ptrDate date1, ptrDate date2){//A function to pass the number of cases and the name of each disease of a bucket to ptrTok (with dates)

    Bucket_ptr temp;
    int i=0;

    int array_index=0;

    for(i=0 ; i<HashtableNumOfEntries ; i++){
        temp = hash[i];

        int num_of_krousmata;

        int k=0;

        while(temp!=NULL){

            for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

                if( temp->bucket_node_array[k] != NULL){


                    num_of_krousmata = 0;
                    num_of_krousmata = count_rbtree_nodes_with_dates_and_country(temp->bucket_node_array[k]->rbtree, date1, date2, country);
                    //printf("%d\n", num_of_krousmata);
                    //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                    if(num_of_krousmata!=0){

                        array_of_topk[array_index] = create_top_k_node( num_of_krousmata, temp->bucket_node_array[k]->name);

                        array_index++;
                    }
                }
            }


            temp = temp->next;
        }
    }
}
/*-----------------------*/

/*     /topk-Countries   */

int count_buketnodes_of_bucket_with_disease(Bucket_ptr bucket, int size_of_bucket, char* disease){//Count the countries taht ahve xases of a specific disease
    Bucket_ptr temp;
    temp = bucket;

    int country_count=0;
    int num_of_krousmata=0;
    int k=0;


    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_with_disease(temp->bucket_node_array[k]->rbtree, disease);

                if(num_of_krousmata!=0){ country_count++; }//An vethike estw enas patient me authn thn arrwstia(bucket_node_array[k]->rbtree arrwstia) sto country pou theloume,count this disease
            }
        }


        temp = temp->next;
    }

    return country_count;
}

void names_and_counts_of_buckets_to_ptrtok_for_topkcountry(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* disease){//A function to pass the number of cases and the name of each country of a bucket to ptrTok

    Bucket_ptr temp;
    int i=0;

    int array_index=0;

    for(i=0 ; i<HashtableNumOfEntries ; i++){
        temp = hash[i];

        int num_of_krousmata;

        int k=0;

        while(temp!=NULL){

            for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

                if( temp->bucket_node_array[k] != NULL){


                    num_of_krousmata = 0;
                    num_of_krousmata = count_rbtree_nodes_with_disease(temp->bucket_node_array[k]->rbtree, disease);
                    //printf("%d\n", num_of_krousmata);
                    //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                    if(num_of_krousmata!=0){

                        array_of_topk[array_index] = create_top_k_node( num_of_krousmata, temp->bucket_node_array[k]->name);

                        array_index++;
                    }
                }
            }


            temp = temp->next;
        }
    }
}


//With dates Now

int count_rbtree_nodes_with_dates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease){//A function to count the nodes of a RBT between two dates with  a specific disease

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( (compare_dates(root->patient->entryDate, date1)==1) && (compare_dates(root->patient->entryDate, date2)==-1) && (!strcmp(root->patient->diseaseID, disease)) ){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_with_dates_and_disease(root->left, date1, date2, disease);
        c += count_rbtree_nodes_with_dates_and_disease(root->right, date1, date2, disease);
        c += count_rbtree_nodes_with_dates_and_disease(root->next, date1, date2, disease);
        return c;
    }
}

int count_buketnodes_of_bucket_with_disease_and_dates(Bucket_ptr bucket, int size_of_bucket, char* disease, ptrDate date1, ptrDate date2){//Count the diseases of a country(with dates)
    Bucket_ptr temp;
    temp = bucket;

    int country_count=0;
    int num_of_krousmata=0;
    int k=0;


    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_with_dates_and_disease(temp->bucket_node_array[k]->rbtree, date1, date2, disease);

                if(num_of_krousmata!=0){ country_count++; }//An vethike estw enas patient me authn thn arrwstia(bucket_node_array[k]->rbtree arrwstia) sto country pou theloume,count this disease
            }
        }


        temp = temp->next;
    }

    return country_count;
}

void names_and_counts_of_buckets_to_ptrtok_for_topkcountry_with_dates(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* disease, ptrDate date1, ptrDate date2){//A function to pass the number of cases and the name of each disease of a bucket to ptrTok (with dates)

    Bucket_ptr temp;
    int i=0;

    int array_index=0;

    for(i=0 ; i<HashtableNumOfEntries ; i++){
        temp = hash[i];

        int num_of_krousmata;

        int k=0;

        while(temp!=NULL){

            for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

                if( temp->bucket_node_array[k] != NULL){


                    num_of_krousmata = 0;
                    num_of_krousmata = count_rbtree_nodes_with_dates_and_disease(temp->bucket_node_array[k]->rbtree, date1, date2, disease);
                    //printf("%d\n", num_of_krousmata);
                    //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                    if(num_of_krousmata!=0){

                        array_of_topk[array_index] = create_top_k_node( num_of_krousmata, temp->bucket_node_array[k]->name);

                        array_index++;
                    }
                }
            }


            temp = temp->next;
        }
    }
}

/*-----------------------*/

/* /diseaseFrequency     */

void disease_frequency(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2){//A function to print the number of the cases of a virus

    //Bucket_node_ptr temp = (Bucket_node_ptr)malloc(sizeof(Bucket_node));
    Bucket_node_ptr temp;
    temp = find_bucketnode(virusname, hashtable, hashtable_position, bucketSize);

    int num_of_krousmata;


    num_of_krousmata = count_rbtree_nodes_with_dates(temp->rbtree, date1, date2);

    //printf("Number of %s cases: %d\n", temp->name, num_of_krousmata);
    printf("%s %d\n",  temp->name, num_of_krousmata);

    //free(temp);
    return;

}






int count_rbtree_nodes_with_dates_and_country(redblackPtr root, ptrDate date1, ptrDate date2, char* country){//A function to count the nodes of a RBT between two dates of a specific country

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( (compare_dates(root->patient->entryDate, date1)==1) && (compare_dates(root->patient->entryDate, date2)==-1) && (!strcmp(root->patient->country, country)) ){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_with_dates_and_country(root->left, date1, date2, country);
        c += count_rbtree_nodes_with_dates_and_country(root->right, date1, date2, country);
        c += count_rbtree_nodes_with_dates_and_country(root->next, date1, date2, country);
        return c;
    }
}

void disease_frequency_bycountry(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2, char* country){//A function to print the number of the cases of a virus

    //Bucket_node_ptr temp = (Bucket_node_ptr)malloc(sizeof(Bucket_node));
    Bucket_node_ptr temp;

    temp = find_bucketnode(virusname, hashtable, hashtable_position, bucketSize);

    int num_of_krousmata;


    num_of_krousmata = count_rbtree_nodes_with_dates_and_country(temp->rbtree, date1, date2, country);

    //printf("Number of %s cases in %s: %d\n", temp->name, country, num_of_krousmata);
    printf("%s %d\n",  temp->name, num_of_krousmata);

    return;

}

/*-----------------------*/

/* /numCurrentPatients*/
void current_patients(Bucket_ptr bucket, int size_of_bucket){//A function to print the number of cases without exitDAte

    Bucket_ptr temp;
    temp = bucket;

    int num_of_krousmata;

    int k=0;
    while(temp!=NULL){
        for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){

            if( temp->bucket_node_array[k] != NULL){

                num_of_krousmata = count_rbtree_nodes_no_exitDate(temp->bucket_node_array[k]->rbtree);
                //printf("%d\n", num_of_krousmata);
                //printf("Number of current %s patients: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                printf("%s %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
            }
        }


        temp = temp->next;
    }

}

int count_rbtree_nodes_no_exitDate(redblackPtr root){//A function to count the nodes of a RBT without exitDate

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( ((root->patient->exitDate->day)==0) && ((root->patient->exitDate->month)==0) && ((root->patient->exitDate->year)==0) ){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_no_exitDate(root->left);
        c += count_rbtree_nodes_no_exitDate(root->right);
        c += count_rbtree_nodes_no_exitDate(root->next);
        return c;
    }
}

/*-----------------------*/
