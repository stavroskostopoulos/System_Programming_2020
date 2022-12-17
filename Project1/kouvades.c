
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



Bucket_ptr create_bucket_in_hashtable( int size_of_bucket){//A function to create a bucket(NOT A BUCKET NODE) for the hash table

    Bucket_ptr newbucket;
    newbucket = (Bucket_ptr)malloc(sizeof(Bucket));

    newbucket->bucket_size = size_of_bucket;

    newbucket->next = NULL;



    newbucket->bucket_node_array = (Bucket_node_ptr*)malloc( ( (size_of_bucket)/sizeof(Bucket_node) )*sizeof(Bucket_node));


    int k=0;
    for(k=0 ; k<(size_of_bucket/(sizeof(Bucket_node))) ; k++){
        //printf("\n%d\n", k);
        newbucket->bucket_node_array[k] = NULL;
    }
    //printf("\n\n\nTIIIIIIIIsadASDASDASDSII\n\n\n");

    //newbucket->bucket_node_array = NULL;

    return newbucket;

}


Bucket_node_ptr create_bucket_node( char* name){

    Bucket_node_ptr newbucket_node;
    newbucket_node = (Bucket_node_ptr)malloc(sizeof(Bucket_node));

    newbucket_node->name = (char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(newbucket_node->name, name);


    //RBT

    //newbucket_node->rbtree = (redblackPtr)malloc(sizeof(redblack));
    newbucket_node->rbtree = NULL;

    return newbucket_node;

}

int check_if_bucketnode_with_this_name_exists(char* name, Bucket_ptr *hashtable, int hashtable_position, int size_of_bucket){//A function to check whether a disease or country with a specific name already exists in its hashtable position bucket |RETURNS 1 if it exists OR 0 if it does not exist

    int i=0;
    int value_to_return=0;

    Bucket_ptr temp;


    temp = hashtable[hashtable_position];

    //printf("NAI TI %s %d\n", name, hashtable_position);
    //printf("\nSDFSDFSDAFSDA %s\n", temp->bucket_node_array[i]->name);

    while(temp!=NULL){
        i=0;

        //printf("\nSDFSDFSDAFSDA %s\n", temp->bucket_node_array[i]->name);
        while( (temp->bucket_node_array[i] != NULL) && ( i<( size_of_bucket/(sizeof(Bucket_node)) ) ) ){//Check in the bucket node array
            //printf("\nSDFSDFSDAFSDA %s\n", temp->bucket_node_array[i]->name);
            if( !strcmp(temp->bucket_node_array[i]->name, name) ){ return 1; }

            i++;
        }

        //i=0;
        temp=temp->next;
    }

    //Maybe it is in next bucket of this specific hashtable position
    //if( hashtable[hashtable_position]->next != NULL ){
        //If a next bucket exists...
        //value_to_return = check_if_bucketnode_with_this_name_exists( name, hashtable[hashtable_position]->next, hashtable_position);
    //}





    return value_to_return;

}

Bucket_node_ptr find_bucketnode(char* name, Bucket_ptr *hashtable, int hashtable_position, int size_of_bucket){//A function to find a bucket node that already exists in its hashtable position bucket |RETURNS the bucket node | DO NOT USE IF YOU ARE NOT SURE THAT THE BUCKET NODE EXISTS
    //H filosofia ths synarthshs einai apromoia me thn check_if_bucketnode_with_this_name_exists (vlepe thn check_if_bucketnode_with_this_name_exists gia ton tropo leitourgias ths)
    int i=0;
    int value_to_return=0;

    Bucket_ptr temp;


    temp = hashtable[hashtable_position];


    while(temp!=NULL){
        i=0;

        while( (temp->bucket_node_array[i] != NULL) && ( i<( size_of_bucket/(sizeof(Bucket_node)) ) ) ){


            if( !strcmp(temp->bucket_node_array[i]->name, name) ){ return temp->bucket_node_array[i]; }

            i++;
        }




        temp=temp->next;
    }


}

int check_bucket_freespace(int hash_position, Bucket_ptr *hashtable, int bucketSize){//Check if there is free space in the last bucket of a specific hashtable position|RETURNS 1 if there is FREE SPACE OR 0 if there is not

    Bucket_ptr temp;
    temp = hashtable[hash_position];

    while(temp->next!=NULL){//Go to the last bucket of this hash table position
        temp=temp->next;
    }

    int contained_bucketnodes=0;
    int i=0;
    while(temp->bucket_node_array[i]!=NULL){
        contained_bucketnodes++;
        i++;
    }

    //So now we know how many bucket nodes we have in this specific bucket
    if( ( bucketSize / (sizeof(Bucket_node) ) > contained_bucketnodes) ){//There is free space
        return 1;
    }else{//Den xwraei
        return 0;
    }

}



void record_to_hash_tables_insert( ptrList root, Bucket_ptr *diseasehash, Bucket_ptr *countryhash, int bucketSize, int diseaseHashtableNumOfEntries, int countryHashtableNumOfEntries){//A function to do all the hash table stuff

    ptrList temp;
    temp=root;

    int disease_hash_position=0;
    int country_hash_position=0;

    Bucket_node_ptr bucket_node_temp;
    int disease_exists_in_bucket=0;
    int country_exists_in_bucket=0;

    int pipa=0;
    while(temp !=NULL ){//Gia ena ena record kane oti xreiazetai gia disease kai country hash tables
        pipa++;
        //Disease hash table

        disease_hash_position = hash(temp->diseaseID)%diseaseHashtableNumOfEntries;
        disease_exists_in_bucket = check_if_bucketnode_with_this_name_exists( temp->diseaseID, diseasehash, disease_hash_position, bucketSize);
        if(disease_exists_in_bucket == 1){//If it exists in the bucket
            //Insert in RBT


            //Find the Bucket node we were looking for
            bucket_node_temp = find_bucketnode( temp->diseaseID, diseasehash, disease_hash_position, bucketSize);

            //And insert the RBtree node sto idi uparxon RBT
            insert(&(bucket_node_temp->rbtree), temp);
            //printf("YPARXW  HDH DISEASE %s\n",  temp->diseaseID);
            //printinorder(bucket_node_temp->rbtree);


        }else{//If it does not exist!
            //An den uparxei, prepei na tsekarw an uparxei xwros
            int free_space = check_bucket_freespace(disease_hash_position, diseasehash, bucketSize);

            if(free_space==1){//If there is free space
                //Do nothing
                Bucket_ptr tempono;
                tempono = diseasehash[disease_hash_position];

                while(tempono->next!=NULL){//Pame sto telos ths listas
                    tempono=tempono->next;
                }

                int contained_bucketnodes=0;
                int i=0;

                while(tempono->bucket_node_array[i]!=NULL){
                    contained_bucketnodes++;
                    i++;
                }
                //Now that we know the numebr of bucket nodes in this bucket
                //We can attach the new bucket node
                tempono->bucket_node_array[contained_bucketnodes] = create_bucket_node(temp->diseaseID);

                //Telos,create the RBT
                insert(&(tempono->bucket_node_array[contained_bucketnodes]->rbtree), temp);


            }else{//if there is NOT free space
                Bucket_ptr tempito;

                //Kanw create bucket kai to vazw sto telos ths Bucket list

                tempito = diseasehash[disease_hash_position];

                while(tempito->next!=NULL){//Pame sto telos ths listas
                    tempito=tempito->next;
                }

                tempito->next = create_bucket_in_hashtable(bucketSize);//Kai dhmiourgoume to neo bucket kai ginetai attach sto telos ths listas
                tempito=tempito->next;//Phgaine sto neo bucket

                //Twra prepei na kanw create to bucket node
                tempito->bucket_node_array[0] = create_bucket_node(temp->diseaseID);
                strcpy(tempito->bucket_node_array[0]->name, temp->diseaseID);
                //printf("NO FREE SPACE KAI FADSADASD %s\n", temp->diseaseID);

                //Telos,create the RBT
                insert(&(tempito->bucket_node_array[0]->rbtree), temp);

            }
        }
            /*-------------------------------------------------------------------------------------------------------------*/

            //Country hash table
            country_hash_position = hash(temp->country)%countryHashtableNumOfEntries;
            country_exists_in_bucket = check_if_bucketnode_with_this_name_exists( temp->country, countryhash, country_hash_position, bucketSize);

            if(country_exists_in_bucket == 1){//If it exists in the bucket

                //Insert in RBT

                //Find the Bucket node we were looking for
                bucket_node_temp = find_bucketnode( temp->country, countryhash, country_hash_position, bucketSize);

                //And insert the RBtree node sto idi uparxon RBT
                insert(&(bucket_node_temp->rbtree), temp);

            }else{//If it does not exist!

                //An den uparxei, prepei na tsekarw an uparxei xwros

                int free_space = check_bucket_freespace(country_hash_position, countryhash, bucketSize);

                if(free_space==1){//If there is free space
                    //Do nothing
                    Bucket_ptr tempono;
                    tempono = countryhash[country_hash_position];

                    while(tempono->next!=NULL){//Pame sto telos ths listas
                        tempono=tempono->next;
                    }

                    int contained_bucketnodes=0;
                    int i=0;

                    while(tempono->bucket_node_array[i]!=NULL){//Let's find the number of bucket nodes contained in this bucket
                        contained_bucketnodes++;
                        i++;
                    }
                    //Now that we know the numebr of bucket nodes in this bucket
                    //We can attach the new bucket node

                    tempono->bucket_node_array[contained_bucketnodes] = create_bucket_node(temp->country);


                    //Telos,create the RBT
                    insert(&(tempono->bucket_node_array[contained_bucketnodes]->rbtree), temp);


                }else{//if there is NOT free space
                    Bucket_ptr tempito;


                    //Kanw create bucket kai to vazw sto telos ths Bucket list
                    tempito = countryhash[country_hash_position];

                    while(tempito->next!=NULL){//Pame sto telos ths listas
                        tempito=tempito->next;
                    }

                    tempito->next = create_bucket_in_hashtable(bucketSize);//Kai dhmiourgoume to neo bucket kai ginetai attach sto telos ths listas
                    tempito=tempito->next;//Phgaine sto neo bucket

                    //Twra prepei na kanw create to bucket node
                    tempito->bucket_node_array[0] = create_bucket_node(temp->country);

                    //Telos,create the RBT
                    insert(&(tempito->bucket_node_array[0]->rbtree), temp);


                }

            }


        temp=temp->next;//Go for the next record
    }
}

void recordID_to_record_hash_insert(ptrList temp, char* recordID, Bucket_ptr *recordhash, int bucketSize){//A function to insert a recordID in the record hash table


    int record_hash_position=0;


    Bucket_node_ptr bucket_node_temp;
    int record_exists_in_bucket=0;


        //Record hash table

        record_hash_position = hash(recordID)%RECORDHASHENTRIES;

        //prepei na tsekarw an uparxei xwros
        int free_space = check_bucket_freespace(record_hash_position, recordhash, bucketSize);

        if(free_space==1){//If there is free space
            //Do nothing
            Bucket_ptr tempono;
            tempono = recordhash[record_hash_position];

            while(tempono->next!=NULL){//Pame sto telos ths listas
                tempono=tempono->next;
            }

            int contained_bucketnodes=0;
            int i=0;

            while(tempono->bucket_node_array[i]!=NULL){
                contained_bucketnodes++;
                i++;
            }
                //Now that we know the numebr of bucket nodes in this bucket
                //We can attach the new bucket node
            tempono->bucket_node_array[contained_bucketnodes] = create_bucket_node(recordID);

            //Telos,create the RBT
            insert(&(tempono->bucket_node_array[contained_bucketnodes]->rbtree), temp);

        }else{//if there is NOT free space
            Bucket_ptr tempito;

                //Kanw create bucket kai to vazw sto telos ths Bucket list

            tempito = recordhash[record_hash_position];

            while(tempito->next!=NULL){//Pame sto telos ths listas
                    tempito=tempito->next;
            }

            tempito->next = create_bucket_in_hashtable(bucketSize);//Kai dhmiourgoume to neo bucket kai ginetai attach sto telos ths listas
            tempito=tempito->next;//Phgaine sto neo bucket

            //Twra prepei na kanw create to bucket node
            tempito->bucket_node_array[0] = create_bucket_node(recordID);
            strcpy(tempito->bucket_node_array[0]->name, recordID);
            //printf("NO FREE SPACE KAI FADSADASD %s\n", temp->diseaseID);

            //Telos,create the RBT
            insert(&(tempito->bucket_node_array[0]->rbtree), temp);


        }

}

void new_record_to_hash_tables_insert( ptrList root, Bucket_ptr *diseasehash, Bucket_ptr *countryhash, int bucketSize, int diseaseHashtableNumOfEntries, int countryHashtableNumOfEntries){//A function to do all the hash table stuff

    ptrList temp;
    temp=root;

    int disease_hash_position=0;
    int country_hash_position=0;

    Bucket_node_ptr bucket_node_temp;
    int disease_exists_in_bucket=0;
    int country_exists_in_bucket=0;



        //Disease hash table

        disease_hash_position = hash(temp->diseaseID)%diseaseHashtableNumOfEntries;
        disease_exists_in_bucket = check_if_bucketnode_with_this_name_exists( temp->diseaseID, diseasehash, disease_hash_position, bucketSize);
        if(disease_exists_in_bucket == 1){//If it exists in the bucket
            //Insert in RBT


            //Find the Bucket node we were looking for
            bucket_node_temp = find_bucketnode( temp->diseaseID, diseasehash, disease_hash_position, bucketSize);

            //And insert the RBtree node sto idi uparxon RBT
            insert(&(bucket_node_temp->rbtree), temp);
            //printf("YPARXW  HDH DISEASE %s\n",  temp->diseaseID);
            //printinorder(bucket_node_temp->rbtree);


        }else{//If it does not exist!
            //An den uparxei, prepei na tsekarw an uparxei xwros
            int free_space = check_bucket_freespace(disease_hash_position, diseasehash, bucketSize);

            if(free_space==1){//If there is free space
                //Do nothing
                Bucket_ptr tempono;
                tempono = diseasehash[disease_hash_position];

                while(tempono->next!=NULL){//Pame sto telos ths listas
                    tempono=tempono->next;
                }

                int contained_bucketnodes=0;
                int i=0;

                while(tempono->bucket_node_array[i]!=NULL){
                    contained_bucketnodes++;
                    i++;
                }
                //Now that we know the numebr of bucket nodes in this bucket
                //We can attach the new bucket node
                tempono->bucket_node_array[contained_bucketnodes] = create_bucket_node(temp->diseaseID);

                //Telos,create the RBT
                insert(&(tempono->bucket_node_array[contained_bucketnodes]->rbtree), temp);


            }else{//if there is NOT free space
                Bucket_ptr tempito;

                //Kanw create bucket kai to vazw sto telos ths Bucket list

                tempito = diseasehash[disease_hash_position];

                while(tempito->next!=NULL){//Pame sto telos ths listas
                    tempito=tempito->next;
                }

                tempito->next = create_bucket_in_hashtable(bucketSize);//Kai dhmiourgoume to neo bucket kai ginetai attach sto telos ths listas
                tempito=tempito->next;//Phgaine sto neo bucket

                //Twra prepei na kanw create to bucket node
                tempito->bucket_node_array[0] = create_bucket_node(temp->diseaseID);
                strcpy(tempito->bucket_node_array[0]->name, temp->diseaseID);
                //printf("NO FREE SPACE KAI FADSADASD %s\n", temp->diseaseID);

                //Telos,create the RBT
                insert(&(tempito->bucket_node_array[0]->rbtree), temp);

            }
        }
            /*-------------------------------------------------------------------------------------------------------------*/

            //Country hash table
            country_hash_position = hash(temp->country)%countryHashtableNumOfEntries;
            country_exists_in_bucket = check_if_bucketnode_with_this_name_exists( temp->country, countryhash, country_hash_position, bucketSize);

            if(country_exists_in_bucket == 1){//If it exists in the bucket

                //Insert in RBT

                //Find the Bucket node we were looking for
                bucket_node_temp = find_bucketnode( temp->country, countryhash, country_hash_position, bucketSize);

                //And insert the RBtree node sto idi uparxon RBT
                insert(&(bucket_node_temp->rbtree), temp);

            }else{//If it does not exist!

                //An den uparxei, prepei na tsekarw an uparxei xwros

                int free_space = check_bucket_freespace(country_hash_position, countryhash, bucketSize);

                if(free_space==1){//If there is free space
                    //Do nothing
                    Bucket_ptr tempono;
                    tempono = countryhash[country_hash_position];

                    while(tempono->next!=NULL){//Pame sto telos ths listas
                        tempono=tempono->next;
                    }

                    int contained_bucketnodes=0;
                    int i=0;

                    while(tempono->bucket_node_array[i]!=NULL){//Let's find the number of bucket nodes contained in this bucket
                        contained_bucketnodes++;
                        i++;
                    }
                    //Now that we know the numebr of bucket nodes in this bucket
                    //We can attach the new bucket node

                    tempono->bucket_node_array[contained_bucketnodes] = create_bucket_node(temp->country);


                    //Telos,create the RBT
                    insert(&(tempono->bucket_node_array[contained_bucketnodes]->rbtree), temp);


                }else{//if there is NOT free space
                    Bucket_ptr tempito;


                    //Kanw create bucket kai to vazw sto telos ths Bucket list
                    tempito = countryhash[country_hash_position];

                    while(tempito->next!=NULL){//Pame sto telos ths listas
                        tempito=tempito->next;
                    }

                    tempito->next = create_bucket_in_hashtable(bucketSize);//Kai dhmiourgoume to neo bucket kai ginetai attach sto telos ths listas
                    tempito=tempito->next;//Phgaine sto neo bucket

                    //Twra prepei na kanw create to bucket node
                    tempito->bucket_node_array[0] = create_bucket_node(temp->country);

                    //Telos,create the RBT
                    insert(&(tempito->bucket_node_array[0]->rbtree), temp);


                }

            }



}

void destroy_bucket_node(Bucket_node_ptr bnode){//A function to destroy a bucket node
    free(bnode->name);
    destroyrbt(bnode->rbtree);



}

void destroy_bucket(Bucket_ptr kouvas, int bucketSize){//A function to destroy the bucket(s) of a specific postion of the hash table

    if(kouvas==NULL){ return; }

    destroy_bucket(kouvas->next, bucketSize);


    int contained_bucketnodes=0;
    int i=0;

    while(kouvas->bucket_node_array[i]!=NULL){
        contained_bucketnodes++;
        i++;
    }

    for(i=0 ; i<contained_bucketnodes ; i++ ){
        destroy_bucket_node(kouvas->bucket_node_array[i]);
        free(kouvas->bucket_node_array[i]);
    }
    free(kouvas->bucket_node_array);
    free(kouvas);

}
