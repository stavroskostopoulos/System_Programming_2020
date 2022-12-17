

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




    char* info;//the number of bytes that we are going to write
    info = (char*)malloc( 7*sizeof(char) );

    sprintf( info, "%zu@", (strlen(message)+1) );
    //printf("\n\n%s\n\n\n", info);

    //Lets write the number of bytes that we are going to write (info)
    if( strlen(info) < buffersize ){//If the string fits in the buffer just write it

        if( ( nwrite = write(fd, info, buffersize) ) == -1 ){
            perror("Error in writing");


            //free(introduction);
            //free(tempstring);
            //free(datetemp);
            //free(info);
            //closedir(folder);

            return;
        }

    }else{

    }

    free(info);

//Afou dwsame to info pame na dwsoume to summary stats string(tempstring)
    if( strlen(message) < buffersize ){//If the string fits in the buffer just write it

        if( ( nwrite = write(fd, message, buffersize) ) == -1 ){
            perror("Error in writing");
            //free(introduction);
            //free(tempstring);
            //free(datetemp);
            //closedir(folder);
            //free(info);

            return;
        }

    }else{//If it does not fit

        int n_times;
        //Diapistwnoume poses fores tha xreiastei na kanoume write gia na graftei olo to string
        if( (strlen(message)%(buffersize-1))==0 ){// -1 sto buffer size gia na afhsoume xwro aki gia ton termatiko xarakthra

            n_times = (strlen(message)/(buffersize-1));
        }else{
            n_times = (strlen(message)/(buffersize-1)) + 1;

        }

        //Kai kanoume to write
        int x;
        for(x=0 ; x<n_times ; x++){
            char* packet;
            packet = strndup( (message + (x*(buffersize-1))), (buffersize-1));

            if( ( nwrite = write(fd, packet, buffersize) ) == -1 ){
                //perror("Error in writing");
                //free(introduction);
                //free(tempstring);
                //free(datetemp);
                //free(packet);
                //free(info);

                //closedir(folder);
                free(packet);
                return;
            }
            //printf("%s\n", packet);
            free(packet);

        }
        //printf("\nEGRAPSA\n");

    }
}



char* read_message(int fd, char* buf, int buffersize){//To prwtokollo gia opoiodhpote read se ena pipe
    int firsttime=0;
    char* tempstring;
    char* temp2;

    //Prwta prepei na parw ta info
    //To info einai ths morfhs 450@

    // printf("meta to tiiii         %s\n\n", buf);

    //while( (strchr(buf, '@'))==NULL ){



        //printf("KATAPINlEEEE %s\n\n", buf);


      //  if(firsttime==0){
          //  tempstring = strdup(buf);
          //  memset(buf, 0, sizeof(buf));

          //  read(fd, buf, buffersize);

          //  firsttime=1;
          //  continue;
    //    }else{
          //Prwta enwnw tempstring me buf
          //  temp2 = strdup(tempstring);
          //  free(tempstring);

          //  tempstring = (char*)malloc( (strlen(temp2)+strlen(buf)+1)*sizeof(char) );
            //Enwnw ta strings
          //  strcpy(tempstring, temp2);
          //  strcat(tempstring, buf);

          //  free(temp2);
          //  memset(buf, 0, sizeof(buf));

          //  read(fd, buf, buffersize);


    //    }


  //  }


    //Edw exw to info,dhladh to plithos twn bytes pou tha eiani to string twn summary stats
    //Sto telos toy tempstring twra vrisketai o xarakthras '@'
    //Ton ksefortwnomai



    int upcoming_len;

    if(firsttime!=0){//An mphkes mesa sto apo panw while loop
        tempstring[(strlen(tempstring)-1)] = '\0';
        upcoming_len = atoi(tempstring);

        free(tempstring);
    }else{
        char* num_of_bytes;
        num_of_bytes=strdup(buf);
        num_of_bytes[(strlen(num_of_bytes)-1)] = '\0';
        //buf[(strlen(buf)-1)] = '\0';





        //printf("\n%s   %s\n", buf, num_of_bytes);
        upcoming_len = atoi(num_of_bytes);

        free(num_of_bytes);

    }

    firsttime=0;

    int n_times;//These are the times we have to use read to get the whole string

    //Twra irthe h wra na paroume to summary stats string
    if(upcoming_len < buffersize){//weneed just 1 time to read
        //just read it
        memset(buf, 0, sizeof(buf));

        read(fd, buf, buffersize);

        tempstring = (char*)malloc( (strlen(buf)+1)*sizeof(char) );
        strcpy(tempstring, buf);

        //printf("Read %d bytes from worker number %d\n", res, i);
        //printf("\n\n%s\n\n", buf);

    }else{


        if( (upcoming_len%(buffersize-1)) == 0 ){
            n_times = (upcoming_len/buffersize-1);
        }else{
            n_times = (upcoming_len/(buffersize-1)) + 1;
        }

        int x;
        char* temp2;
        for(x=0 ; x<n_times ; x++){
            memset(buf, 0, sizeof(buf));

            read(fd, buf, buffersize);

            if(firsttime==0){
                tempstring = strdup(buf);
                firsttime=1;
            }else{
                temp2 = strdup(tempstring);

                free(tempstring);
                tempstring = (char*)malloc( (strlen(temp2)+strlen(buf)+1)*sizeof(char) );

                //Enwnw ta strings
                strcpy(tempstring, temp2);
                strcat(tempstring, buf);


                free(temp2);


            }


        }

    }
    //printf("%s\n", tempstring);
    return tempstring;
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
