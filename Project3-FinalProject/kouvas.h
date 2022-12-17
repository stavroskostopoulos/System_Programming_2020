
/* Stavros Kostopoulos sdi1700068 */

#ifndef KOUVAS
#define KOUVAS

#include "dentro.h"

#define RECORDHASHENTRIES 1000
#define COUNTRYHASHENTRIES 10
#define DISEASEHASHENTRIES 10


typedef struct Bucket_node{
    char* name;
    //RBT
    redblackPtr rbtree;
}Bucket_node;

typedef Bucket_node *Bucket_node_ptr;

typedef struct Bucket{
    int bucket_size;
    Bucket_node_ptr *bucket_node_array;
    struct Bucket *next;
}Bucket;

typedef Bucket *Bucket_ptr;



int check_bucket_freespace(int hash_position, Bucket_ptr *hashtable, int bucketSize);//Check if there is free space in the last bucket of a specific hashtable position|RETURNS 1 if there is FREE SPACE OR 0 if there is not
int check_if_bucketnode_with_this_name_exists(char* name, Bucket_ptr *hashtable, int hashtable_position, int size_of_bucket);//A function to check whether a disease or country with a specific name already exists in its hashtable position bucket |RETURNS 1 if it exists OR 0 if it does not exist
Bucket_node_ptr create_bucket_node( char* name);
Bucket_ptr create_bucket_in_hashtable( int size_of_bucket);//A function to create a bucket(NOT A BUCKET NODE) for the hash table
void record_to_hash_tables_insert( ptrList root, Bucket_ptr *diseasehash, Bucket_ptr *countryhash, int bucketSize, int diseaseHashtableNumOfEntries, int countryHashtableNumOfEntries);//A function to do all the hash table stuff
Bucket_node_ptr find_bucketnode(char* name, Bucket_ptr *hashtable, int hashtable_position, int size_of_bucket);//A function to find a bucket node that already exists in its hashtable position bucket |RETURNS the bucket node | DO NOT USE IF YOU ARE NOT SURE THAT THE BUCKET NODE EXISTS
void destroy_bucket_node(Bucket_node_ptr bnode);//A function to destroy a bucket node
void destroy_bucket(Bucket_ptr kouvas, int bucketSize);//A function to destroy the bucket(s) of a specific postion of the hash table
void new_record_to_hash_tables_insert( ptrList root, Bucket_ptr *diseasehash, Bucket_ptr *countryhash, int bucketSize, int diseaseHashtableNumOfEntries, int countryHashtableNumOfEntries);//A function to insert a new record to the hash tables
void recordID_to_record_hash_insert(ptrList temp, char* recordID, Bucket_ptr *recordhash, int bucketSize);//A function to insert a recordID in the record hash table


#endif
