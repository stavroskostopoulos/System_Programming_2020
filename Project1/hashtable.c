
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


unsigned int hash(unsigned char *string_to_hash){//The hash function

        unsigned int hash = 5381;
        int c;
        char* str = strdup(string_to_hash);
        char* temp = str;//So we dont move the str pointer,so we can free the allocated memory from strdup later



        while (c = *temp++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */




        free(str);
        return hash;

}
