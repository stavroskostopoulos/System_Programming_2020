/* Stavros Kostopoulos sdi1700068 */

#ifndef QUERIE
#define QUERIE

/*------------------------ Summary Stats ------------------------*/

//Count the cases of specific date,country and age for every disease in a bucket
int all_diseases_of_a_bucket_country_and_age_and_date(Bucket_node_ptr bucketnode, int size_of_bucket, char* country, int down_age_limit, int up_age_limit, ptrDate date);

/*---------------------------------------------------------------*/

/*------------------ NAMED FIFO PIPES PROTOCOL ------------------*/

void write_message(int fd, char* message, int buffersize);//To prwtokollo gia opoiodhpote write se ena pipe
char* read_message(int fd, int buffersize);//To prwtokollo gia opoiodhpote read se ena pipe

/*---------------------------------------------------------------*/

/*-------------------------- QUERIES ----------------------------*/

//Disease Frequency
int disease_frequency(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2);//A function to return the number of the cases of a virus
int count_rbtree_nodes_with_dates_and_country(redblackPtr root, ptrDate date1, ptrDate date2, char* country);//A function to count the nodes of a RBT between two dates of a specific country
int disease_frequency_bycountry(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2, char* country);//A function to print the number of the cases of a virus

//searchPatientRecord
char* search_patient(char* record_id, Bucket_ptr *hashtable, int hashtable_position, int bucketSize);//A function that constructs a strings with a patient's info(with specific record ID)

//numPatientAdmissions
int count_rbtree_nodes_with_dates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease);//A function to count the nodes of a RBT between two dates of a specific country

//numPatientDischarges
int count_rbtree_nodes_with_exitdates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease);//A function to count the nodes of a RBT exited between two dates of a specific country


//topk-AgeRanges
int count_rbtree_nodes_with_age_and_disease_and_date(redblackPtr root, char* disease, int down_age_limit, int up_age_limit, ptrDate date1, ptrDate date2);//A function to count the nodes of a RBT with a specific disease in a specific age limit and date range
int agegroup_count_country_disease_and_date(Bucket_node_ptr bucketnode, int size_of_bucket, char* disease, int down_age_limit, int up_age_limit, ptrDate date1, ptrDate date2);//A function to count the numebr of cases of an age group of specific country,disease and date range

/*---------------------------------------------------------------*/






#endif
