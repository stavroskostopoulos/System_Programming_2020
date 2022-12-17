

/* Stavros Kostopoulos sdi1700068 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include  <fcntl.h>
#include  <error.h>

#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"




/*------------------------ Summary Stats ------------------------*/

int all_diseases_of_a_bucket_country_and_age_and_date(Bucket_node_ptr bucketnode, int size_of_bucket, char* country, int down_age_limit, int up_age_limit, ptrDate date){

    int num_of_krousmata=0;

            if( bucketnode != NULL){

                num_of_krousmata = count_rbtree_nodes_with_age_and_country_and_date(bucketnode->rbtree, country, down_age_limit, up_age_limit, date);
                //printf("%d\n", num_of_krousmata);

                //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                return num_of_krousmata;
            }


}

/*---------------------------------------------------------------*/


/*------------------ NAMED FIFO PIPES PROTOCOL ------------------*/

void write_message(int fd, char* message, int buffersize){//To prwtokollo gia opoiodhpote write se ena pipe

    int nwrite;

    char info[10];
    char* msginfo;

    sprintf(info, "%zu", strlen(message));

    msginfo = (char*)malloc( (strlen(info)+1)*sizeof(char) );
    strcpy(msginfo, info);

    char* final_message;

    final_message = (char*)malloc( (strlen(msginfo)+1+strlen(message)+1)*sizeof(char) );

    sprintf(final_message, "%s&%s", msginfo, message);
    
    //Write the final message in the pipe
    if( ( nwrite = write(fd, final_message, strlen(final_message)+1) ) == -1 ){
            perror("Error in writing");

            free(msginfo);
            return;
    }




}



char* read_message(int fd, int buffersize){//To prwtokollo gia opoiodhpote read se ena pipe

    int firsttime=0;
    char* tempstring;
    char* temp2;
    char* info;
    int nread=0;
    int flag=0;//to let me know whether i allocated memory for tempstring

    char buf[buffersize+1];

    read(fd, buf, buffersize);

    buf[buffersize]='\0';


    //Prwta prepei na parw ta info
    //To info einai ths morfhs 450&


    while( (strchr(buf, '&'))==NULL ){

        flag=1;//to let me know whether i allocated memory for tempstring

        if(firsttime==0){
            tempstring = strdup(buf);
            memset(buf, 0, sizeof(buf));

            read(fd, buf, buffersize);
            buf[buffersize]='\0';


            firsttime=1;
            continue;
        }else{
          //Prwta enwnw tempstring me buf
          temp2 = strdup(tempstring);
          free(tempstring);

          tempstring = (char*)malloc( (strlen(temp2)+strlen(buf)+1)*sizeof(char) );
          //Enwnw ta strings
          strcpy(tempstring, temp2);
          strcat(tempstring, buf);

          free(temp2);
          memset(buf, 0, sizeof(buf));

          read(fd, buf, buffersize);    buf[buffersize]='\0';

          buf[buffersize]='\0';



        }


    }







    //The last read stored in buf the end of the string that contains the & character(info)
    //So lets store it in tempstring

    if(flag==1){
        temp2 = strdup(tempstring);
        free(tempstring);

        tempstring = (char*)malloc( (strlen(temp2)+strlen(buf)+1)*sizeof(char) );
        //Enwnw ta strings
        strcpy(tempstring, temp2);
        strcat(tempstring, buf);

        free(temp2);
        memset(buf, 0, sizeof(buf));



    }else{//let buf be the tempstring
        tempstring=strdup(buf);
        memset(buf, 0, sizeof(buf));
    }








    char* up_comstr;

    int upcoming_len;// to plithos twn bytes pou tha eiani to string

    char* rest;

    //From the string we got we got to seperate the info and the rest of the string
    int count=0;

    char* token;
    token = strtok(tempstring, "&");


    while(token!=NULL){
        if(count==0){
            upcoming_len = atoi(token);
            up_comstr = strdup(token);
        }else if(count==1){
            rest = strdup(token);
        }

        count++;
        token = strtok(NULL, "&");//Get the next token
    }



    if(rest[(strlen(rest)-1)]=='!'){
        return rest;
    }

    if(upcoming_len==1){
        return rest;
    }

    //We may already have the whole string(in case of big buffersize number)
    char* less;
    if(upcoming_len <= (buffersize-strlen(up_comstr)+1) ){
        less = strdup(rest);
        free(rest);
        return less;
    }


    //Now we got the rest of the string in rest


    firsttime=0;

    char* final_string;
    final_string = (char*)malloc( (upcoming_len+1)*sizeof(char) );


    memset(buf, 0, sizeof(buf));

    //Gotta read the complete string and store it in final_string

    while( (nread=read(fd, buf, buffersize)) != 0 ){
        buf[buffersize]='\0';



        if( (rest!=NULL) && (firsttime==0) ){//Check if there actually was something in the string "rest"
            strcpy(final_string, rest);
            free(rest);

            firsttime=1;
        }


        //Attach the bytes read to the final_string
        strcat(final_string, buf);





        memset(buf, 0, sizeof(buf));




        if(strlen(final_string)>=upcoming_len){//If you have read all bytes
            break;
        }
    }




    return final_string;
}


/*---------------------------------------------------------------*/

/*-------------------------- QUERIES ----------------------------*/

//Disease Frequency

int disease_frequency(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2){//A function to count the number of the cases of a virus within 2 dates

    //Bucket_node_ptr temp = (Bucket_node_ptr)malloc(sizeof(Bucket_node));
    Bucket_node_ptr temp;
    temp = find_bucketnode(virusname, hashtable, hashtable_position, bucketSize);

    int num_of_krousmata;


    num_of_krousmata = count_rbtree_nodes_with_dates(temp->rbtree, date1, date2);

    return num_of_krousmata;

    //free(temp);


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

int disease_frequency_bycountry(Bucket_ptr *hashtable, int hashtable_position, char* virusname, int bucketSize, ptrDate date1, ptrDate date2, char* country){//A function to print the number of the cases of a virus

    //Bucket_node_ptr temp = (Bucket_node_ptr)malloc(sizeof(Bucket_node));
    Bucket_node_ptr temp;

    temp = find_bucketnode(virusname, hashtable, hashtable_position, bucketSize);

    int num_of_krousmata;


    num_of_krousmata = count_rbtree_nodes_with_dates_and_country(temp->rbtree, date1, date2, country);



    return num_of_krousmata;

}

//searchPatientRecord
char* search_patient(char* record_id, Bucket_ptr *hashtable, int hashtable_position, int bucketSize){//A function that constructs a strings with a patient's info(with specific record ID)

    Bucket_node_ptr temp;

    temp = find_bucketnode(record_id, hashtable, hashtable_position, bucketSize);

    char* result;

    if(temp->rbtree->patient->exitDate->day != 0){//An exei kanei EXIT o patient

        result = (char*)malloc( (strlen(record_id)+1+strlen(temp->rbtree->patient->patientFirstName)+1+strlen(temp->rbtree->patient->patientLastName)+1+strlen(temp->rbtree->patient->diseaseID)+1+3/*age*/+1+10/*DD-MM-YYYY*/+1+10+1)*sizeof(char) );

        sprintf(result, "%s %s %s %s %d %d-%d-%d %d-%d-%d", record_id, temp->rbtree->patient->patientFirstName, temp->rbtree->patient->patientLastName, temp->rbtree->patient->diseaseID, temp->rbtree->patient->age, temp->rbtree->patient->entryDate->day, temp->rbtree->patient->entryDate->month, temp->rbtree->patient->entryDate->year, temp->rbtree->patient->exitDate->day, temp->rbtree->patient->exitDate->month, temp->rbtree->patient->exitDate->year);

        return result;

    }else{//An DEN exei kanei EXIT
        result = (char*)malloc( (strlen(record_id)+1+strlen(temp->rbtree->patient->patientFirstName)+1+strlen(temp->rbtree->patient->patientLastName)+1+strlen(temp->rbtree->patient->diseaseID)+1+3/*age*/+1+10/*DD-MM-YYYY*/+1+2/* "--" */+1)*sizeof(char) );

        sprintf(result, "%s %s %s %s %d %d-%d-%d --", record_id, temp->rbtree->patient->patientFirstName, temp->rbtree->patient->patientLastName, temp->rbtree->patient->diseaseID, temp->rbtree->patient->age, temp->rbtree->patient->entryDate->day, temp->rbtree->patient->entryDate->month, temp->rbtree->patient->entryDate->year);

        return result;
    }

}

//numPatientAdmissions
int count_rbtree_nodes_with_dates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease){//A function to count the nodes of a RBT between two dates of a specific country

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( (compare_dates(root->patient->entryDate, date1)==1) && (compare_dates(root->patient->entryDate, date2)==-1) && (!strcmp(root->patient->diseaseID, disease))){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_with_dates_and_disease(root->left, date1, date2, disease);
        c += count_rbtree_nodes_with_dates_and_disease(root->right, date1, date2, disease);
        c += count_rbtree_nodes_with_dates_and_disease(root->next, date1, date2, disease);
        return c;
    }

}

//numPatientDischarges
int count_rbtree_nodes_with_exitdates_and_disease(redblackPtr root, ptrDate date1, ptrDate date2, char* disease){//A function to count the nodes of a RBT between two dates of a specific country

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {
        if(root->patient->exitDate->day==0){//If the patient has not exited yet
            return 0;
        }

        if( (compare_dates(root->patient->exitDate, date1)==1) && (compare_dates(root->patient->exitDate, date2)==-1) && (!strcmp(root->patient->diseaseID, disease))){//If patient entry date >date1 && <date2

            c=1;
        }
        c += count_rbtree_nodes_with_exitdates_and_disease(root->left, date1, date2, disease);
        c += count_rbtree_nodes_with_exitdates_and_disease(root->right, date1, date2, disease);
        c += count_rbtree_nodes_with_exitdates_and_disease(root->next, date1, date2, disease);
        return c;
    }

}

//topk-AgeRanges
int count_rbtree_nodes_with_age_and_disease_and_date(redblackPtr root, char* disease, int down_age_limit, int up_age_limit, ptrDate date1, ptrDate date2){//A function to count the nodes of a RBT with a specific disease in a specific age limit and date range

    int c = 0;

    if (root ==NULL)
        return 0;
    else
    {


        if( !strcmp(root->patient->diseaseID, disease) && (root->patient->age>=down_age_limit) && (root->patient->age<=up_age_limit) && (compare_dates(root->patient->entryDate, date1)==1) && (compare_dates(root->patient->entryDate, date2)==-1) ){
            c=1;
        }
        c += count_rbtree_nodes_with_age_and_disease_and_date(root->left, disease, down_age_limit, up_age_limit, date1, date2);
        c += count_rbtree_nodes_with_age_and_disease_and_date(root->right, disease, down_age_limit, up_age_limit, date1, date2);
        c += count_rbtree_nodes_with_age_and_disease_and_date(root->next, disease, down_age_limit, up_age_limit, date1, date2);
        return c;
    }
}

int agegroup_count_country_disease_and_date(Bucket_node_ptr bucketnode, int size_of_bucket, char* disease, int down_age_limit, int up_age_limit, ptrDate date1, ptrDate date2){//A function to count the numebr of cases of an age group of specific country,disease and date range

    int num_of_krousmata=0;

            if( bucketnode != NULL){

                num_of_krousmata = count_rbtree_nodes_with_age_and_disease_and_date(bucketnode->rbtree, disease, down_age_limit, up_age_limit, date1, date2);
                //printf("%d\n", num_of_krousmata);

                //printf("Number of %s cases: %d\n", temp->bucket_node_array[k]->name, num_of_krousmata);
                return num_of_krousmata;
            }


}
/*---------------------------------------------------------------*/
