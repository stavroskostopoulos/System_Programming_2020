
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




int main(int argc, char *argv[]){

    //printf("\n\n");


    char * patientfile;
    int diseaseHashtableNumOfEntries=0;
    int countryHashtableNumOfEntries=0;
    int bucketSize=0;

/*Command line arguments check*/

    if(argc!=9){
            printf("Please check your command line arguments.\n\n\n");
            return 1;
    }
/*--------------------------*/

/*Get command line arguments*/
    int i;
    for(i=0 ; i<9 ; i++){

        if( !strcmp(argv[i],"-p") ){                                            /* Get input file name from command line*/
            	patientfile = malloc( (strlen(argv[i+1])+1) *sizeof(char) );
            	strcpy(patientfile, argv[i+1]);
            	printf("Input file: %s\n", patientfile);
        }

        //Get diseaseHashtableNumOfEntries
        if( !strcmp(argv[i],"-h1") ){
            diseaseHashtableNumOfEntries = atoi(argv[i+1]);
        }

        //Get diseaseHashtableNumOfEntries
        if( !strcmp(argv[i],"-h2") ){
            countryHashtableNumOfEntries = atoi(argv[i+1]);
        }

        //Get bucketSize
        if( !strcmp(argv[i],"-b") ){
            bucketSize = atoi(argv[i+1]);
        }

    }
/*--------------------------*/


/* Open input file patientfile */

    FILE *fp = fopen(patientfile, "r");

    if( fp == NULL ) {
      perror("Error: ");
      return -1;
   }

/*----------------------------*/

/* Read all the file records */

    //char recordID[20];
    //char patientFirstName[20];
    //char diseaseID[20];
    //char country[20];
    //char entryDate[20];
    //char exitDate[20];

    char* recordID = (char*)malloc(20*sizeof(char));
    char* patientFirstName = (char*)malloc(20*sizeof(char));
    char* patientLastName = (char*)malloc(20*sizeof(char));
    char* diseaseID = (char*)malloc(20*sizeof(char));
    char* country = (char*)malloc(20*sizeof(char));
    char* entryDate = (char*)malloc(20*sizeof(char));
    char* exitDate = (char*)malloc(20*sizeof(char));

    ptrList root;//The root node of the patient list
    ptrList temp;//To attach the new nodes of the list
    i=0;

    unsigned int hash_number;//The number that the hash function returns

/*--------------------------*/

/* RecordId hashtable */

    Bucket_ptr *recordhash;//Create disease hash table
    recordhash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*RECORDHASHENTRIES);

    for(i=0 ; i<RECORDHASHENTRIES ; i++){//Create all the buckets of the hash table
        recordhash[i] = create_bucket_in_hashtable(bucketSize);
    }

/*----------------------------*/

    i=0;
/* Get the records from the patientfile until you get EOF */

    while(fscanf(fp, "%s %s %s %s %s %s %s\n", recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate)!=EOF){//Get the records from the patientfile until you get EOF


        if(i==0){//For the first record create the first node
            root = createListNode(recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);
            i++;
            //print_patientList(root);

        }else{//If it is not the first record,attach the new record to the list
            temp = createListNode(recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);
            attach_patient_to_List( root, temp);
        }


        //Lets check if a same recordId exists in any bucket
        hash_number = hash(recordID)%RECORDHASHENTRIES;

        if( (check_if_bucketnode_with_this_name_exists( recordID, recordhash, hash_number, bucketSize)==1) ){//If there is a recordID equal tothis one

            printf("\n\nA patient with recordID %s already exists!\nExiting...\n\n", recordID);



            for(i=0 ; i<RECORDHASHENTRIES ; i++){//Destroy recordID buckets
                destroy_bucket(recordhash[i], bucketSize);
            }


            free(recordID);
            free(patientFirstName);
            free(patientLastName);
            free(diseaseID);
            free(country);
            free(entryDate);
            free(exitDate);

            free(patientfile);
            destroy_patient_list(root);
            fclose(fp);

            return -1;
        }else{//If there's not such recordID
            //Insert it to the record hash table
            recordID_to_record_hash_insert(temp, recordID, recordhash, bucketSize);
        }




        //printf("%s %s %s %s %s %s %s\n", recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);

        //printf("%s %d\n", diseaseID, hash(diseaseID));
    }



    free(recordID);
    free(patientFirstName);
    free(patientLastName);
    free(diseaseID);
    free(country);
    free(entryDate);
    free(exitDate);

/*------------------------------------------------------*/


/*           Create the hash tables and manage them     */

    Bucket_ptr *diseasehash;//Create disease hash table
    diseasehash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*diseaseHashtableNumOfEntries);

    for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){//Create all the buckets of the hash table
        diseasehash[i] = create_bucket_in_hashtable(bucketSize);
    }

    Bucket_ptr *countryhash;//Create country hash table
    countryhash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*countryHashtableNumOfEntries);

    for(i=0 ; i<countryHashtableNumOfEntries ; i++){//Create all the buckets of the hash table
        countryhash[i] = create_bucket_in_hashtable(bucketSize);
    }

    //Pass records to hash tables
    record_to_hash_tables_insert(root, diseasehash, countryhash, bucketSize, diseaseHashtableNumOfEntries, countryHashtableNumOfEntries);

    //print_patientList(root);

    //printf("\n\n\nTELOS VAMOOOOOOOOOOOOOOOOOOOOOOOOOS\n\n\n");


/*------------------------------------------------------*/

/*                 ------Queries------                  */

    char* what_to_do = (char*)malloc(30*sizeof(char));

    while(1){
        printf("\n");
        scanf("%s", what_to_do);


/* globalDiseaseStats */
        if(!strcmp(what_to_do, "/globalDiseaseStats")){

            //printf("\n\n");



            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* date;
            char* date1;
            char* date2;

            int count=0;//So we know when we get date1 and date2


            date = strtok(line, " ");



            while(date!=NULL){
                count++;//increment count

                if(count==1){//Get teh first date
                    date1 = (char*)malloc((strlen(date)+1)*sizeof(date));
                    strcpy(date1, date);
                }else if(count==2){//Get teh second date
                    date2 = (char*)malloc((strlen(date1)+1)*sizeof(date));
                    strcpy(date2, date);
                }

                date = strtok(NULL, " ");//Get the next token(date)

            }


            /* Errors and function calls*/

            if(count==0){//No dates at all
                //Now let's go over all the disease hash table
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    all_diseases_of_a_bucket(diseasehash[i], bucketSize);
                }
                free(line);

            }else if(count==1){//You got only one date,porint an error

                printf("\nPlease type both dates and try again\n");
                free(date1);
                free(line);
                continue;

            }else if(count==2){//You got both dates,let's do it




                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){
                    printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(line);

                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);


                //printDate(tempdate1);
                //printf("\n");
                //printDate(tempdate2);
                //printf("\n");

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }else{//If date1>date2
                        printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }
                }



                //Now let's go over all the disease hash table
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    global_disease_stats(diseasehash[i], bucketSize, tempdate1, tempdate2);
                }



                free(tempdate1);
                free(tempdate2);
                free(date1);
                free(date2);
                free(line);
            }else{//You got more arguments than you wanted,print an error
                printf("\nPlease check your query arguments.\n");
                free(date1);
                free(date2);
                free(line);
                continue;
            }





        }else
/*--------------------*/

/*  diseaseFrequency  */
        if(!strcmp(what_to_do, "/diseaseFrequency")){
            //printf("\n\n");

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;

            char* virusname;
            char* arg2;
            char* arg3;
            char* arg4;


            int count=0;//So we know when we get date1 and date2


            token = strtok(line, " ");

            while(token != NULL){

                if(count==0){
                    virusname = strdup(token);//virusname
                }else if(count==1){
                    arg2 = strdup(token);// date1
                }else if(count==2){
                    arg3 = strdup(token);// date2
                }else if(count==3){// Country was given
                    arg4 = strdup(token);
                }

                count++;
                token = strtok(NULL, " ");//Get the next token
            }

            int disease_hash_position=0;
            int country_hash_position=0;



            if(count==1){
                printf("\nPlease check your query arguments.\n");
                free(virusname);
                free(line);
            }else if(count==2){
                printf("\nPlease check your query arguments.\n");
                free(virusname);
                free(arg2);
                free(line);
            }else if(count==3){
                //arg2 = date1  arg3 = date2


                disease_hash_position = hash(virusname)%diseaseHashtableNumOfEntries;

                if( (check_if_bucketnode_with_this_name_exists(virusname, diseasehash, disease_hash_position, bucketSize)==0) ){//Check if this virus exists
                    free(virusname);
                    free(arg2);
                    free(arg3);
                    free(line);


                    printf("\nNo such virus was found!\n");
                    continue;
                }

                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(arg2)==0 || valid_date(arg3)==0 ){//Check if the dates given are valid
                    printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(arg2);
                    free(arg3);
                    free(line);
                    continue;
                }



                setDate(tempdate1, arg2);
                setDate(tempdate2, arg3);


                //printDate(tempdate1);
                //printf("\n");
                //printDate(tempdate2);
                //printf("\n");

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);
                        free(line);
                        continue;
                    }else{//If date1>date2
                        printf("\n%s is before %s\n", arg3, arg2);
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);
                        free(line);
                        continue;
                    }
                }


                //Everyting is ok with the dates let's move
                disease_frequency(diseasehash, disease_hash_position, virusname, bucketSize, tempdate1, tempdate2);

                free(line);
                free(virusname);
                free(tempdate1);
                free(tempdate2);
                free(arg2);
                free(arg3);
            }else if(count==4){
                //arg4 = country arg2 = date1 arg3 = date2
                disease_hash_position = hash(virusname)%diseaseHashtableNumOfEntries;
                country_hash_position = hash(arg4)%countryHashtableNumOfEntries;



                if( (check_if_bucketnode_with_this_name_exists(virusname, diseasehash, disease_hash_position, bucketSize)==0) ){//Check if this virus exists
                    free(virusname);
                    free(arg2);
                    free(arg3);
                    free(arg4);
                    free(line);

                    printf("\nNo such virus was found!\n");
                    continue;
                }

                if( (check_if_bucketnode_with_this_name_exists( arg4, countryhash, country_hash_position, bucketSize)==0) ){//Check if this country exists
                    free(virusname);
                    free(arg2);
                    free(arg3);
                    free(arg4);
                    free(line);

                    printf("\nNo such country was found!\n");
                    continue;
                }

                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(arg3)==0 || valid_date(arg2)==0 ){//Check if the dates given are valid
                    printf("\nPlease type valid dates!\n");
                    free(line);
                    free(tempdate1);
                    free(tempdate2);
                    free(arg2);
                    free(arg3);
                    free(arg4);
                    continue;
                }



                setDate(tempdate1, arg2);
                setDate(tempdate2, arg3);


                //printDate(tempdate1);
                //printf("\n");
                //printDate(tempdate2);
                //printf("\n");

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        printf("\nThe dates you typed are the same!\n");
                        free(line);
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);
                        free(arg4);
                        continue;
                    }else{//If date1>date2
                        printf("\n%s is before %s\n", arg3, arg2);
                        free(line);
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);
                        free(arg4);
                        continue;
                    }

                }


                //disease_frequency(diseasehash, disease_hash_position, virusname, bucketSize, tempdate1, tempdate2);

                disease_frequency_bycountry(diseasehash, disease_hash_position, virusname, bucketSize, tempdate1, tempdate2, arg4);

                free(tempdate1);
                free(tempdate2);
                free(line);
                free(virusname);
                free(arg2);
                free(arg3);
                free(arg4);
            }else if(count>4){
                printf("\nPlease check your query arguments.\n");
                free(line);
                free(virusname);
                free(arg2);
                free(arg3);
                free(arg4);
            }

        }else
/*--------------------*/

/*  /topk-Diseases    */
        if(!strcmp(what_to_do, "/topk-Diseases")){
            //printf("\n\n");

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;
            int count=0;
            int k=0;
            char* kcountry;
            char* date1;
            char* date2;


            token = strtok(line, " ");


            while(token != NULL){
                if(count==0){
                    k = atoi(token);

                }else if(count==1){
                    kcountry = strdup(token);
                }else if(count==2){
                    date1 = strdup(token);
                }else if(count==3){
                    date2 = strdup(token);
                }


                count++;
                token = strtok(NULL, " ");//Get the next token
            }

            if(count<2){
                printf("\nPlease check your query arguments.\n");
                free(line);

                continue;
            }else if(count==3){
                printf("\nPlease check your query arguments.\n");
                free(kcountry);
                free(date1);
                free(line);

                continue;
            }else if(count>4){
                printf("\nPlease check your query arguments.\n");
                free(kcountry);
                free(date1);
                free(date2);
                free(line);
                continue;
            }else if(count==2){//No dates were given

                //Check if the country given exists
                int country_hash_position=0;
                country_hash_position = hash(kcountry)%countryHashtableNumOfEntries;
                if( (check_if_bucketnode_with_this_name_exists( kcountry, countryhash, country_hash_position, bucketSize)==0) ){//Check if this country exists
                    free(kcountry);
                    free(line);

                    printf("\nNo such country was found!\n");
                    continue;
                }


                int disease_count=0;
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    disease_count += count_buketnodes_of_bucket_with_country(diseasehash[i], bucketSize, kcountry);
                }

                if(disease_count==0){
                    free(kcountry);
                    free(line);
                    continue;
                }

                //Create the array of topk
                //Each element of thearray contains a disease name,and its count of cases
                ptrTopk *array_of_topk;
                array_of_topk = (ptrTopk*)malloc((disease_count)*sizeof(ptrTopk));

                for(i=0 ; i<disease_count ; i++){

                    array_of_topk[i] = NULL;
                }

                //Lest fill the array of topk
                names_and_counts_of_buckets_to_ptrtok(array_of_topk, diseasehash, bucketSize, diseaseHashtableNumOfEntries, kcountry);

                //Print the array
                /*for(i=0 ; i<disease_count ; i++){

                    printf("\n%s %d\n", array_of_topk[i]->name, array_of_topk[i]->count);

                }*/

                ptrheaptree root;
                //Create the heap tree from the array_of_topk
                root = fillHeapFromArray(array_of_topk, 0, disease_count);
                //Do the heap operation(with the tree)
                heaptree_operation(root, k, kcountry, disease_count, array_of_topk);


                //Free the array_of_topk

                for(i=0 ; i<disease_count ; i++){
                    //free(array_of_topk[i]->name);
                    destroy_topk(array_of_topk[i]);

                }
                destroyheaptree(root);

                free(array_of_topk);

                free(kcountry);
                free(line);
                continue;

            }else if(count==4){

                //Check if the country given exists
                int country_hash_position=0;
                country_hash_position = hash(kcountry)%countryHashtableNumOfEntries;
                if( (check_if_bucketnode_with_this_name_exists( kcountry, countryhash, country_hash_position, bucketSize)==0) ){//Check if this country exists
                    free(kcountry);
                    free(date1);
                    free(date2);
                    free(line);

                    printf("\nNo such country was found!\n");
                    continue;
                }



                //Lets check the dates
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){
                    printf("\nPlease type valid dates!\n");

                    free(kcountry);
                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(line);

                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);


                //printDate(tempdate1);
                //printf("\n");
                //printDate(tempdate2);
                //printf("\n");

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        printf("\nThe dates you typed are the same!\n");
                        free(kcountry);

                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }else{//If date1>date2
                        printf("\n%s is before %s\n", date2, date1);
                        free(kcountry);

                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }
                }



                int disease_count=0;
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    disease_count += count_buketnodes_of_bucket_with_country_and_dates(diseasehash[i], bucketSize, kcountry, tempdate1, tempdate2);
                }

                if(disease_count==0){
                    free(kcountry);

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(line);
                    continue;

                }
                //Create the array of topk
                //Each element of thearray contains a disease name,and its count of cases
                ptrTopk *array_of_topk;
                array_of_topk = (ptrTopk*)malloc((disease_count)*sizeof(ptrTopk));

                for(i=0 ; i<disease_count ; i++){

                    array_of_topk[i] = NULL;
                }

                //Lest fill the array of topk
                names_and_counts_of_buckets_to_ptrtok_with_dates(array_of_topk, diseasehash, bucketSize, diseaseHashtableNumOfEntries, kcountry, tempdate1, tempdate2);

                //Print the array
                /*for(i=0 ; i<disease_count ; i++){

                    printf("\n%s %d\n", array_of_topk[i]->name, array_of_topk[i]->count);

                }*/

                ptrheaptree root;
                //Create the heap tree from the array_of_topk
                root = fillHeapFromArray(array_of_topk, 0, disease_count);
                //Do the heap operation(with the tree)
                heaptree_operation(root, k, kcountry, disease_count, array_of_topk);


                //Free the array_of_topk

                for(i=0 ; i<disease_count ; i++){
                    //free(array_of_topk[i]->name);
                    destroy_topk(array_of_topk[i]);

                }
                destroyheaptree(root);

                free(array_of_topk);

                free(tempdate1);
                free(tempdate2);
                free(date1);
                free(date2);
                free(kcountry);
                free(line);
                continue;
            }





        }else
/*--------------------*/
/*  /topk-Countries    */
        if(!strcmp(what_to_do, "/topk-Countries")){
            //printf("\n\n");

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;
            int count=0;
            int k=0;
            char* kdisease;
            char* date1;
            char* date2;


            token = strtok(line, " ");


            while(token != NULL){
                if(count==0){
                    k = atoi(token);

                }else if(count==1){
                    kdisease = strdup(token);
                }else if(count==2){
                    date1 = strdup(token);
                }else if(count==3){
                    date2 = strdup(token);
                }


                count++;
                token = strtok(NULL, " ");//Get the next token
            }

            if(count<2){
                printf("\nPlease check your query arguments.\n");
                free(line);

                continue;
            }else if(count==3){
                printf("\nPlease check your query arguments.\n");
                free(kdisease);
                free(date1);
                free(line);

                continue;
            }else if(count>4){
                printf("\nPlease check your query arguments.\n");
                free(kdisease);
                free(date1);
                free(date2);
                free(line);
                continue;
            }else if(count==2){//No dates were given

                //Check if the disease given exists
                int disease_hash_position=0;
                disease_hash_position = hash(kdisease)%diseaseHashtableNumOfEntries;
                if( (check_if_bucketnode_with_this_name_exists( kdisease, diseasehash, disease_hash_position, bucketSize)==0) ){//Check if this country exists
                    free(kdisease);
                    free(line);

                    printf("\nNo such disease was found!\n");
                    continue;
                }


                int country_count=0;
                for(i=0 ; i<countryHashtableNumOfEntries ; i++){
                    country_count += count_buketnodes_of_bucket_with_disease(countryhash[i], bucketSize, kdisease);
                }

                if(country_count==0){
                    free(kdisease);
                    free(line);
                    continue;
                }

                //Create the array of topk
                //Each element of thearray contains a disease name,and its count of cases
                ptrTopk *array_of_topk;
                array_of_topk = (ptrTopk*)malloc((country_count)*sizeof(ptrTopk));

                for(i=0 ; i<country_count ; i++){

                    array_of_topk[i] = NULL;
                }

                //Lest fill the array of topk
                names_and_counts_of_buckets_to_ptrtok_for_topkcountry(array_of_topk, countryhash, bucketSize, countryHashtableNumOfEntries, kdisease);

                //Print the array
                /*for(i=0 ; i<disease_count ; i++){

                    printf("\n%s %d\n", array_of_topk[i]->name, array_of_topk[i]->count);

                }*/

                ptrheaptree root;
                //Create the heap tree from the array_of_topk
                root = fillHeapFromArray(array_of_topk, 0, country_count);
                //Do the heap operation(with the tree)
                heaptree_operation(root, k, kdisease, country_count, array_of_topk);


                //Free the array_of_topk

                for(i=0 ; i<country_count ; i++){
                    //free(array_of_topk[i]->name);
                    destroy_topk(array_of_topk[i]);

                }
                destroyheaptree(root);

                free(array_of_topk);

                free(kdisease);
                free(line);
                continue;

            }else if(count==4){//With dates

                //Check if the disease given exists
                int disease_hash_position=0;
                disease_hash_position = hash(kdisease)%diseaseHashtableNumOfEntries;
                if( (check_if_bucketnode_with_this_name_exists( kdisease, diseasehash, disease_hash_position, bucketSize)==0) ){//Check if this country exists
                    free(kdisease);
                    free(line);
                    free(date1);
                    free(date2);

                    printf("\nNo such disease was found!\n");
                    continue;
                }



                //Lets check the dates
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){
                    printf("\nPlease type valid dates!\n");

                    free(kdisease);
                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(line);

                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);


                //printDate(tempdate1);
                //printf("\n");
                //printDate(tempdate2);
                //printf("\n");

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        printf("\nThe dates you typed are the same!\n");
                        free(kdisease);

                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }else{//If date1>date2
                        printf("\n%s is before %s\n", date2, date1);
                        free(kdisease);

                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(line);
                        continue;
                    }
                }



                int country_count=0;
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    country_count += count_buketnodes_of_bucket_with_disease_and_dates(countryhash[i], bucketSize, kdisease, tempdate1, tempdate2);
                }

                if(country_count==0){
                    free(kdisease);

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(line);
                    continue;

                }
                //Create the array of topk
                //Each element of thearray contains a disease name,and its count of cases
                ptrTopk *array_of_topk;
                array_of_topk = (ptrTopk*)malloc((country_count)*sizeof(ptrTopk));

                for(i=0 ; i<country_count ; i++){

                    array_of_topk[i] = NULL;
                }

                //Lest fill the array of topk
                names_and_counts_of_buckets_to_ptrtok_for_topkcountry_with_dates(array_of_topk, countryhash, bucketSize, countryHashtableNumOfEntries, kdisease, tempdate1, tempdate2);

                //Print the array
                /*for(i=0 ; i<disease_count ; i++){

                    printf("\n%s %d\n", array_of_topk[i]->name, array_of_topk[i]->count);

                }*/

                ptrheaptree root;
                //Create the heap tree from the array_of_topk
                root = fillHeapFromArray(array_of_topk, 0, country_count);
                //Do the heap operation(with the tree)
                heaptree_operation(root, k, kdisease, country_count, array_of_topk);


                //Free the array_of_topk

                for(i=0 ; i<country_count ; i++){
                    //free(array_of_topk[i]->name);
                    destroy_topk(array_of_topk[i]);

                }
                destroyheaptree(root);
                free(array_of_topk);

                free(tempdate1);
                free(tempdate2);
                free(date1);
                free(date2);
                free(kdisease);
                free(line);
                continue;
            }


        }else
/*/insertPatientRecord*/
        if(!strcmp(what_to_do, "/insertPatientRecord")){
            //printf("\n\n");

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;
            int count=0;

            token = strtok(line, " ");


            while(token != NULL){

                if(count==0){
                    recordID = strdup(token);//virusname
                }else if(count==1){
                    patientFirstName = strdup(token);// [country] OR date1
                }else if(count==2){
                    patientLastName = strdup(token);// date1 OR date2
                }else if(count==3){// Country was given
                    diseaseID = strdup(token);
                }else if(count==4){
                    country = strdup(token);
                }else if(count==5){
                    entryDate = strdup(token);
                }else if(count==6){
                    exitDate = strdup(token);
                }

                count++;
                token = strtok(NULL, " ");//Get the next token
            }

            if(count<7){//Ligotera arguments

                printf("\nPlease check your query arguments.\n");
                free(line);
                if(count>=1) free(recordID);
                if(count>=2) free(patientFirstName);
                if(count>=3) free(patientLastName);
                if(count>=4) free(diseaseID);
                if(count>=5) free(country);
                if(count>=6) free(entryDate);
                continue;
            }else if(count>7){//Parapanw arguments


                printf("\nPlease check your query arguments.\n");
                free(line);
                free(recordID);
                free(patientFirstName);
                free(patientLastName);
                free(diseaseID);
                free(country);
                free(entryDate);
                free(exitDate);
                continue;
            }else if(count==7){//else if(count==7){

                if(!strcmp(exitDate, "-")){//If no exit Date
                    if( valid_date(entryDate)==0  ){//Check if the entrydate given is valid
                        printf("\nPlease type valid dates!\n");

                        free(line);
                        free(recordID);
                        free(patientFirstName);
                        free(patientLastName);
                        free(diseaseID);
                        free(country);
                        free(entryDate);
                        free(exitDate);
                        continue;
                    }
                }else{

                    if( valid_date(entryDate)==0 || valid_date(exitDate)==0 ){//Check if the dates given are valid
                        printf("\nPlease type valid dates!\n");

                        free(line);
                        free(recordID);
                        free(patientFirstName);
                        free(patientLastName);
                        free(diseaseID);
                        free(country);
                        free(entryDate);
                        free(exitDate);
                        continue;
                    }

                    //Check if the dates are valid and Convert date strings to actual dates
                    ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                    ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                    setDate(tempdate1, entryDate);
                    setDate(tempdate2, exitDate);

                    if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                        if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                            printf("\nThe dates you typed are the same!\n");
                            free(line);
                            free(tempdate1);
                            free(tempdate2);
                            free(recordID);
                            free(patientFirstName);
                            free(patientLastName);
                            free(diseaseID);
                            free(country);
                            free(entryDate);
                            free(exitDate);
                            continue;
                        }else{//If date1>date2
                            printf("\n%s is before %s\n", exitDate, entryDate);
                            free(line);
                            free(tempdate1);
                            free(tempdate2);
                            free(recordID);
                            free(patientFirstName);
                            free(patientLastName);
                            free(diseaseID);
                            free(country);
                            free(entryDate);
                            free(exitDate);
                            continue;
                        }
                    }

                    free(tempdate1);
                    free(tempdate2);
                }


                hash_number = hash(recordID)%RECORDHASHENTRIES;

                if( (check_if_bucketnode_with_this_name_exists( recordID, recordhash, hash_number, bucketSize)==1) ){//If there is a recordID equal tothis one
                    printf("\n\nA patient with recordID %s already exists!\nTry again...\n\n", recordID);


                    free(line);
                    free(recordID);
                    free(patientFirstName);
                    free(patientLastName);
                    free(diseaseID);
                    free(country);
                    free(entryDate);
                    free(exitDate);
                    continue;
                }

                //Create new patient

                temp = createListNode(recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);
                attach_patient_to_List(root, temp);

                //insert it in record hash table
                recordID_to_record_hash_insert(temp, recordID, recordhash, bucketSize);

                //insert it in diseash and country hash atbles
                new_record_to_hash_tables_insert(temp, diseasehash, countryhash, bucketSize, diseaseHashtableNumOfEntries, countryHashtableNumOfEntries);

                //printf("\nNew patient inserted!\n");
                printf("Record added\n");

                free(line);
                free(recordID);
                free(patientFirstName);
                free(patientLastName);
                free(diseaseID);
                free(country);
                free(entryDate);
                free(exitDate);
                continue;
            }


        }else
/*--------------------*/

/* /numCurrentPatients*/
        if(!strcmp(what_to_do, "/recordPatientExit")){
            //printf("\n\n");

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;
            int count=0;

            token = strtok(line, " ");


            while(token != NULL){


                if(count==0){
                    recordID = strdup(token);
                }else if(count==1){
                    exitDate = strdup(token);
                }
                count++;
                token = strtok(NULL, " ");//Get the next token

            }


            if(count==0){
                printf("\nPlease check your query arguments.\n");

                free(line);
                free(recordID);
                free(exitDate);
                continue;
            }else if(count>2){
                printf("\nPlease check your query arguments.\n");

                free(line);
                free(recordID);
                free(exitDate);
                continue;
            }else{

                hash_number = hash(recordID)%RECORDHASHENTRIES;

                Bucket_node_ptr temp;


                if( (check_if_bucketnode_with_this_name_exists( recordID, recordhash, hash_number, bucketSize)==0) ){//Make sure this record ID exists
                    printf("\n\nNo such patient exists!\nTry again...\n\n");

                    free(line);
                    free(recordID);
                    free(exitDate);
                    continue;
                }

                if(valid_date(exitDate)==0){//If the date given is not valid
                    printf("\nPlease type a valid date!\n");

                    free(line);
                    free(recordID);
                    free(exitDate);
                    continue;
                }



                //Find the bucket node of this patient in the record hash table
                temp = find_bucketnode(recordID, recordhash , hash_number, bucketSize);

                //Let's make sure that the patient does not already have an exitDate
                //if(temp->rbtree->patient->exitDate->day!=0){
                //    printf("\nThe patient already has an exit date...\nHe is cured!\n");

                //    free(line);
                //    free(recordID);
                //    free(exitDate);
                //    continue;
                //}


                //Now we got to check out if new Exit Date is after the entryDate of the patient
                ptrDate tempdate = (ptrDate)malloc(sizeof(myDate));

                setDate(tempdate, exitDate);
                if( compare_dates(temp->rbtree->patient->entryDate, tempdate)==1 ){
                    printf("\nThe exitDate you typed is before the entry date\n");

                    free(line);
                    free(tempdate);
                    free(recordID);
                    free(exitDate);
                    continue;
                }




                //Everything is fine so...
                //Change the exit date
                setDate(temp->rbtree->patient->exitDate, exitDate);

                //printf("\nExit date of %s %s set to: %s\n", temp->rbtree->patient->patientFirstName, temp->rbtree->patient->patientLastName, exitDate);
                printf("Record updated\n");

                free(line);
                free(tempdate);
                free(recordID);
                free(exitDate);
                continue;
            }



        }else
/*--------------------*/

/* /numCurrentPatients*/
        if(!strcmp(what_to_do, "/numCurrentPatients")){
            //printf("\n\n");



            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            char* token;


            int count=0;//So we know when we get date1 and date2


            token = strtok(line, " ");



            while(token!=NULL){///If no [disease] was given,then count stay equal to 0
                count++;//increment count

                if(count==1){//We got a disease argument
                    diseaseID = strdup(token);
                }


                token = strtok(NULL, " ");//Get the next token(date)

            }

            if(count>1){
                printf("\nPlease check your query arguments.\n");
                free(line);
                free(diseaseID);
                continue;
            }else if(count==0){//No [disease] was given
                for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
                    current_patients(diseasehash[i], bucketSize);
                }
                continue;
            }else if(count==1){
                int disease_hash_position=0;
                disease_hash_position = hash(diseaseID)%diseaseHashtableNumOfEntries;



                //Lets check if a virus with this name exists
                if( (check_if_bucketnode_with_this_name_exists(diseaseID, diseasehash, disease_hash_position, bucketSize)==0) ){
                    //If it does not exist
                    printf("\nNo such virus was found!\n");
                    free(line);
                    free(diseaseID);
                    continue;

                }else{//An uparxei
                    int num_of_current_patients=0;
                    Bucket_node_ptr temp;

                    temp = find_bucketnode(diseaseID, diseasehash ,disease_hash_position, bucketSize);

                    num_of_current_patients = count_rbtree_nodes_no_exitDate(temp->rbtree);

                    //printf("Number of current %s patients: %d\n", temp->name, num_of_current_patients);
                    printf("%s %d\n", temp->name, num_of_current_patients);

                    free(line);
                    free(diseaseID);
                    continue;

                }
            }
        }else
/*--------------------*/

/*        exit        */
        if(!strcmp(what_to_do, "/exit")){
            //printf("\n\nBye bye :)\n\n---------------------\n");
            printf("exiting\n");
            break;
        }else{ break; }
/*--------------------*/





















    }




/*------------------------------------------------------*/

/* Free and Exit */

    for(i=0 ; i<RECORDHASHENTRIES ; i++){//Destroy recordID buckets
        destroy_bucket(recordhash[i], bucketSize);
    }

    free(recordhash);

    for(i=0 ; i<diseaseHashtableNumOfEntries ; i++){
        destroy_bucket(diseasehash[i], bucketSize);
    }

    free(diseasehash);

    for(i=0 ; i<countryHashtableNumOfEntries ; i++){
        destroy_bucket(countryhash[i], bucketSize);
    }

    free(countryhash);

    free(what_to_do);



    free(patientfile);
    destroy_patient_list(root);
    fclose(fp);



    return 0;
}
