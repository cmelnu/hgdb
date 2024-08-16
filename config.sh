#!/bin/bash

usage(){
   echo
   echo "This script will allow the user to set a certain configuration, depending on which example program is to be run."
   echo "After executing the following command, a few environmental variables (which specify paths to both an exec. file and a source file) will be exported,"
   echo "plus the desired example will be compiled for later usage."
   echo
   echo "	Usage: . ./config.sh name_prog"
   echo
}


if [ "$#" -eq 0 ]; then
   usage
   return
fi


#We compile one of the examples
pushd $PWD/examples
   if [ "$1" = "suma" ]; then
      make suma
   elif [ "$1" = "vars" ]; then
      make vars
   fi
popd


#Here, we set the appropriate environment
#--SOURCE_PATH: needed by our debugger in order to find the source code file
#--EXEC_PATH: needed in order to find the executable file
if [ "$1" != "" ]; then
   export PATH=$PATH:.
   export EXEC_PATH=$PWD/examples/$1
   export SOURCE_PATH=examples/$1.c
fi
