#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1;
fi

checkslepc

answerzero ud20-1-0-sl.fee
exitifwrong $?

answer reflected.fee "1.0902 0.5053 0.5313"
exitifwrong $?
