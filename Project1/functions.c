
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



void setDate( ptrDate my_date, char* date){//A function to set the date values in myDate struct from a date string ( "25-1-2019" )

    char s[2] = "-";
    char* token;

    char* protectdate;
    protectdate = (char*)malloc((strlen(date)+1)*sizeof(char));
    strcpy(protectdate, date);


    int i=0;

    token = strtok(protectdate, s);//Use strtok to separate the date string


    while( token != NULL ) {

      //printf( " %s\n", token );
      if(i==0){ my_date->day = atoi(token); }else//If i==0 get the day
      if(i==1){ my_date->month = atoi(token); }else//If i==1 get the month
      if(i==2){ my_date->year = atoi(token); }    //If i==2 get the year

     token = strtok(NULL, s);//Get the next token

      i++;//Increment i

   }

   if(i==0){//If there was not a date
       my_date->day = 0;
       my_date->month = 0;
       my_date->year = 0;
   }

   free(protectdate);

}

void printDate(ptrDate my_date){//A function to print a date
    printf("%d-%d-%d", my_date->day, my_date->month,  my_date->year);
}


ptrList createListNode( char* recordID, char* patientFirstName, char* patientLastName, char* diseaseID, char* country, char* entryDate, char* exitDate){//A function to create a patient node for the List

    ptrList newNode;

    newNode = (ptrList)malloc(sizeof(PatientsList));

    newNode->recordID = atoi(recordID);

    newNode->patientFirstName = (char*)malloc((strlen(patientFirstName)+1)*sizeof(char));
    strcpy(newNode->patientFirstName, patientFirstName);

    newNode->patientLastName = (char*)malloc((strlen(patientLastName)+1)*sizeof(char));
    strcpy(newNode->patientLastName, patientLastName);

    newNode->diseaseID = (char*)malloc((strlen(diseaseID)+1)*sizeof(char));
    strcpy(newNode->diseaseID, diseaseID);

    newNode->country = (char*)malloc((strlen(country)+1)*sizeof(char));
    strcpy(newNode->country, country);

    newNode->entryDate = (ptrDate)malloc(sizeof(myDate));
    setDate(newNode->entryDate, entryDate);

    newNode->exitDate = (ptrDate)malloc(sizeof(myDate));
    setDate(newNode->exitDate, exitDate);

    newNode->next = NULL;

    return newNode;

}

void attach_patient_to_List( ptrList root, ptrList newNode/*char* recordID, char* patientFirstName, char* patientLastName, char* diseaseID, char* country, char* entryDate, char* exitDate*/){//A function to attach a new patient Node to the end of the list

    //ptrList newNode;
    //newNode = createListNode(recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);

    ptrList tempo = root;

    while( tempo->next != NULL){//Go to the last node of the list

        tempo = tempo->next;

    }

    tempo->next = newNode;

}


void printListNode(ptrList newNode){//A function to print a patient node
    printf("%d %s %s %s %s ", newNode->recordID, newNode->patientFirstName, newNode->patientLastName, newNode->diseaseID, newNode->country);
    printDate(newNode->entryDate);
    printf(" ");
    printDate(newNode->exitDate);
    printf("\n");


}


void print_patientList(ptrList root){//A function to print the whole list

    ptrList temp = root;

    while( temp->next != NULL){//Go to the last node of the list

        printListNode(temp);

        temp = temp->next;

    }
}


int compare_dates ( ptrDate d1, ptrDate  d2){// A function to compare dates | If d1 is before d2 returns -1 | If d2 is before d1 returns 1`


    if (d1->year < d2->year)
       return -1;

    else if (d1->year > d2->year)
       return 1;

    if (d1->year == d2->year)
    {
         if (d1->month < d2->month)
              return -1;
         else if (d1->month > d2->month)
              return 1;
         else if (d1->day < d2->day)
              return -1;
         else if(d1->day > d2->day)
              return 1;
         else
              return 0;
    }
}

void destroy_patient_list(ptrList patient){//A functiont o destroy our patient list

    if(patient==NULL){ return; }

    destroy_patient_list(patient->next);
    free(patient->patientFirstName);
    free(patient->patientLastName);
    free(patient->diseaseID);
    free(patient->country);
    free(patient->entryDate);
    free(patient->exitDate);
    free(patient);
}

int valid_date(char* date){//A function to check whether a date is valid|Returns 0 if it is NOT valid Or it returns 1 if it is valid
    ptrDate midat;
    midat = (ptrDate)malloc(sizeof(myDate));

    setDate(midat, date);

    if( (midat->day<=0) || (midat->month<=0) || (midat->year<=0) ){
        free(midat);
        return 0;
    }

    if( (midat->month==1) || (midat->month==3) || (midat->month==5) || (midat->month==7) || (midat->month==8) || (midat->month==10) || (midat->month==12) ){
        if( (midat->day > 31) ){//Gia tous parapanw mhnes pou exoun 31 meres
            free(midat);
            return 0;
        }
    }else{//Gia tous mhnes pou exoun 30 meres
        if( (midat->day > 30) ){ free(midat); return 0; }
    }

    free(midat);
    return 1;//ola good
}
