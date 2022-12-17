
/* Stavros Kostopoulos sdi1700068 */

#ifndef PATIENT
#define PATIENT


typedef struct myDate{
    int day;
    int month;
    int year;
}myDate;

typedef myDate *ptrDate;

typedef struct PatientsList{
    int recordID;
    char* patientFirstName;
    char* patientLastName;
    char* diseaseID;
    char* country;
    ptrDate entryDate;
    ptrDate exitDate;
    int age;
    char* status;
    struct PatientsList *next;
}PatientsList;

typedef PatientsList *ptrList;


void setDate( ptrDate my_date, char* date);//A function to set the date values in myDate struct from a date string ( "25-1-2019" )
void printDate(ptrDate my_date);//A function to print a date
ptrList createListNode( char* recordID, char* patientFirstName, char* patientLastName, char* diseaseID, char* country, char* entryDate, char* age, char* status);//A function to create a patient node for the List
void printListNode(ptrList newNode);//A function to print a patient node
void attach_patient_to_List( ptrList root, ptrList newNode/*char* recordID, char* patientFirstName, char* patientLastName, char* diseaseID, char* country, char* entryDate, char* exitDate*/);//A function to attach a new patient Node to the end of the list
void print_patientList(ptrList root);//A function to print the whole list
int compare_dates ( ptrDate d1, ptrDate  d2);// A function to compare dates | If d1 is before d2 returns -1 | If d2 is before d1 returns 1
void destroy_patient_list(ptrList patient);//A functiont o destroy our patient list
int valid_date(char* date);//A function to check whether a date is valid|Returns 0 if it is NOT valid Or it returns 1 if it is valid



#endif
