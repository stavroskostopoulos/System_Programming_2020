/* Stavros Kostopoulos sdi1700068 */

#ifndef QUERIE
#define QUERIE

#include "kouvas.h"
#include "dentro.h"
#include "topk.h"

/* /globalDiseaseStats */

void all_diseases_of_a_bucket(Bucket_ptr bucket, int size_of_bucket);//A function to print the number of cases for every disease of a bucket
void global_disease_stats(Bucket_ptr bucket, int size_of_bucket, ptrDate date1, ptrDate date2);//A function to print the number of cases for every disease of a bucket between two dates

/* /diseaseFrequency     */

void disease_frequency(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2);//A function to print the number of the cases of a virus
int count_rbtree_nodes_with_dates_and_country(redblackPtr root, ptrDate date1, ptrDate date2, char* country);//A function to count the nodes of a RBT between two dates of a specific country
void disease_frequency_bycountry(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2, char* country);//A function to print the number of the cases of a virus


/* /numCurrentPatients*/

void current_patients(Bucket_ptr bucket, int size_of_bucket);//A function to print the number of cases without exitDAte
int count_rbtree_nodes_no_exitDate(redblackPtr root);//A function to count the nodes of a RBT without exitDate




/*  /topk-Diseases    */

int count_buketnodes_of_bucket_with_country(Bucket_ptr bucket, int size_of_bucket, char* country);//Count the diseases of a country
void names_and_counts_of_buckets_to_ptrtok(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* country);//A function to pass the number of cases and the name of each disease of a bucket to ptrTok(specific country)

int count_buketnodes_of_bucket_with_country_and_dates(Bucket_ptr bucket, int size_of_bucket, char* country, ptrDate date1, ptrDate date2);//Count the diseases of a country(with dates)
void names_and_counts_of_buckets_to_ptrtok_with_dates(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* country, ptrDate date1, ptrDate date2);//A function to pass the number of cases and the name of each disease of a bucket to ptrTok (with dates)

/*     /topk-Countries   */
int count_buketnodes_of_bucket_with_disease(Bucket_ptr bucket, int size_of_bucket, char* disease);//Count the countries taht have cases of a specific disease
void names_and_counts_of_buckets_to_ptrtok_for_topkcountry(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* disease);//A function to pass the number of cases and the name of each country of a bucket to ptrTok

int count_rbtree_nodes_with_dates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease);//A function to count the nodes of a RBT between two dates with  a specific disease
int count_buketnodes_of_bucket_with_disease_and_dates(Bucket_ptr bucket, int size_of_bucket, char* disease, ptrDate date1, ptrDate date2);//Count the countries taht have cases of a specific disease(with dates)
void names_and_counts_of_buckets_to_ptrtok_for_topkcountry_with_dates(ptrTopk *array_of_topk, Bucket_ptr* hash, int size_of_bucket, int HashtableNumOfEntries, char* disease, ptrDate date1, ptrDate date2);//A function to pass the number of cases and the name of each disease of a bucket to ptrTok (with dates)











#endif
