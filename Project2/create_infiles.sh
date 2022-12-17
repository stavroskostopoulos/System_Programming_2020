#!/bin/bash

echo "Hello World!"

#Argument number check
if [ "$#" -ne 5 ]; then
  echo "Lousi"
  exit 1
fi

#argument 3 is input_dir name

#create the input_dir directory
mkdir $3



#argument 1
diseasefile=$1

#argument 2 is countries file
countriesfile=$2


# argument 4 is numFilesperDirectory
filesperdir=$4

# argument 5 is numRecordsPerFile
recordsperfile=$5

recordid=800

#Read countries one by one
while read line; do

    # make country folder
    mkdir $3/$line

    for ((n=0;n<filesperdir;n++))
    do
        #generate random date
        date="$((RANDOM%30+1))-$((RANDOM%12+1))-$((RANDOM%30+1990))"

        #create the file
        touch $3/$line/$date

        for ((k=0;k<recordsperfile;k++))
        do
            #record ID
            echo $recordid
            recordid=$(($recordid + 1))


            #ENTER/EXIT

            #Generate one random number equal to 0 or 1. If equal to 1 then add ENTER in the .txt record,else add EXIT
            if [ $((RANDOM % 2)) -eq 1 ]; then
                state="ENTER"
            else
                state="EXIT"
            fi


            #Create Name

            #random length apo tria ews dwdeka
            namelength=$((RANDOM%10+3))
            name=$(cat /dev/urandom | tr -dc 'a-z' | fold -w $namelength | head -n 1)


            #Create Surname

            #random length apo tria ews dwdeka
            surnamelength=$((RANDOM%10+3))
            surname=$(cat /dev/urandom | tr -dc 'a-z' | fold -w $surnamelength | head -n 1)

            echo $name
            echo $surname


            #Pick a random disease using shuf function(Source:https://shapeshed.com/unix-shuf/)
            echo "NAI SE LEW"
            disease=$(shuf -n 1 $1)
            echo "OXI PALI"
            echo $disease
            echo "OXI RE"

            #Generate random age in range [18,88]
            age=$((RANDOM%71+18))
            echo $age

            echo $recordid $state $name $surname $disease $age >> $3/$line/$date
        done
    done


done < $countriesfile
