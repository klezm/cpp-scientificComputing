#! /usr/bin/env bash

# 
# Small script to send spam mails using mutt.
# got probably a bit carried away from the actual assignment :)
# @author: Lars Groeber
# 
# Usage:
#       SCRIPTNAME [-T %Y%m%d%H%M] [-t time-interval-in-min] [-m max-mails] [-d]
#                  (-s subject/email/body) | (-f subject/file-with-emails/body)
# 
# Example: SCRIPTNAME -T 201610301200 -t 5 -m 10 -s subject/name@example/"body with spaces"
#           sends 1 mail every 5 minutes to name@example.com starting at 
#           10/30/2016 12:00 until 10 mails are sent. 
#           
# Use -t 0 to send all emails immediately.
# Use -d for debugging (no mails are sent).
# 

# terminate on error
set -e

#### GLOBALS ####

maxMails=5
timeInterval=1  # in minutes
nameOfScript="./LarsGroeber_problem1.sh"
timestamp=`date '+%Y%m%d%H%M'`
FS="/" # field seperator
debug=1

usage="Usage:\
            \tTHIS-SCRIPT \t[-T %Y%m%d%H%M] [-t time-interval-in-min] [-m max-mails] [-d]\n\
            \t\t\t(-s subject${FS}email${FS}body) | (-f subject${FS}file-with-emails${FS}body)\n\
            \tDefaults: max-mails: $maxMails, time-interval: $timeInterval\n
            \tRemember to surround text with spaces by quotes!"

#### FUNCTIONS ####

# function which sends the mails
# @param: subject
#         email
#         body
function sendMail
{
  if test $debug; then
    echo "sent mail: $1/$2/$3" # debug
  else
    echo "$3" | mutt -s "$1" -- "$2"
  fi
}

# function which waits until a given timestamp is in the past
# @param: timestamp to check with format %Y%m%d%H%M
function waitTimeStamp
{
  secondsDiff=$(( $1 - `date '+%Y%m%d%H%M'` ))

  while test $secondsDiff -gt 0; do
    echo "Waiting..."
    sleep 5
    secondsDiff=$(( $1 - `date '+%Y%m%d%H%M'` ))
  done
}

# simple implementation, takes only arguments from commandline as inputs
# @param: subject
#         email
#         body
function simpleSpam
{
  # split input
  IFS=$FS read -r -a inputArr <<< $1

  # check if three parameters were given
  if test ${#inputArr[@]} -ne 3; then
    echo "Invalid number of arguments!" >&2
    echo "Usage: -s subject${FS}email${FS}body" >&2
    exit 2
  fi
 
  waitTimeStamp $timestamp

  subject=${inputArr[0]}
  email=${inputArr[1]}
  body=${inputArr[2]}

  for (( i = 1; i <= $maxMails; i++ )); do
    sendMail "$subject" "$email" "$body" # quotes important here
    echo "Sent "$i". mail to \""$email"\". $(( $maxMails-$i )) mail(s) left."
         
    # go to sleep if there are mails left to send
    if test $i -ne $maxMails; then 
      echo "Going to sleep for "$timeInterval"min."
      sleep $(( $timeInterval*60 )); 
    fi
  done
}

# this implimentation takes a file with adresses and sends mails to each of them
# @param: subject
#         file-with-emails
#         body
function advancedSpam
{
  IFS=$FS read -r -a inputArr <<< $1
  
  if test ${#inputArr[@]} -ne 3; then
    echo "Invalid number of arguments!" >&2
    echo "Usage: -f subject${FS}path-to-file${FS}body" >&2
    echo "Remember to surround text with spaces by quotes." >&2
    exit 2
  fi

  subject=${inputArr[0]}
  pathToFile=${inputArr[1]}
  body=${inputArr[2]}

  if test ! -e $pathToFile || test ! -r $pathToFile; then 
    echo "File does not exist!"
    exit 1
  fi

  emails=""

  # read file line by line into $emails
  while read -r line; do
    emails="$emails $line"
  done < "$pathToFile"

  emailsArr=( $emails )

  waitTimeStamp $timestamp  

  for (( i = 1; i <= $maxMails; i++ )); do
    # here we send one email per entry in file
    for email in ${emailsArr[@]}; do
      sendMail "$subject" "$email" "$body"
      echo "Sent "$i". mail to \""$email"\". $(( $maxMails-$i )) mail(s) left."
    done
    
    if test $i -ne $maxMails; then 
      echo "Going to sleep for "$timeInterval"min."
      sleep $(( $timeInterval*60 )); 
    fi
  done
}

#### END FUNCTIONS ####

cmd=""
param=""

# go through arguments
while getopts "f:s:t:T:m:d" opt; do
  case $opt in
    T)
      timestamp=$OPTARG
      ;;
    t)
      timeInterval=$OPTARG
      ;;
    m)
      maxMails=$OPTARG
      ;;
    f)
      cmd="advancedSpam"
      param=$OPTARG
      ;;
    s)
      cmd="simpleSpam"
      param=$OPTARG
      ;;
    d)
      debug=0
      ;;
  esac
done

# show usage information if no parameters are specified
if test $# -eq 0 || test $cmd == ""; then
  echo -e $usage >&2
  exit 1
fi

# quotes are important here
$cmd "$param"

exit 0