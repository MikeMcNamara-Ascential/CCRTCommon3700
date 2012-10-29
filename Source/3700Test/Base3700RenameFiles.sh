#!/bin/sh

if [ "$#" = "3" ]
    then 
    echo "Customer $1"
    echo "Plant $2"
    echo "Machine $3"
    origCust="Customer"
    origPlant="Plant"
    origMach="Machine"
    target="$1.$2.$3"
    selected="$origCust.$origPlant.$origMach"
    currentDir=$(pwd)
    #save current directory from root path
    
    echo "Moving: $selected -> $target"
    linesMatch=`find $currentDir/* -name "$selected*" -type f`
    for lineMatch in $linesMatch
    do
          echo "mv $lineMatch" -> `echo $lineMatch | sed "s/$selected/$target/"`
          mv $lineMatch `echo $lineMatch | sed "s/$selected/$target/"`
    done

    target="$1.$2"
    selected="$origCust.$origPlant"
       
    echo "Moving: $selected -> $target"
    linesMatch=`find $currentDir/* -name "$selected*" -type f`
    for lineMatch in $linesMatch
    do
          echo "mv $lineMatch" -> `echo $lineMatch | sed "s/$selected/$target/"`
          mv $lineMatch `echo $lineMatch | sed "s/$selected/$target/"`
    done
    
    target="$1"
    selected="$origCust"
       
    echo "Moving: $selected -> $target"
    linesMatch=`find $currentDir/* -name "$selected*" -type f`
    for lineMatch in $linesMatch
    do
          echo "mv $lineMatch" -> `echo $lineMatch | sed "s/$selected/$target/"`
          mv $lineMatch `echo $lineMatch | sed "s/$selected/$target/"`
    done
    
else
echo "Error: incorrect syntax ./LinkForCustomer [Customer] [Plant] [Machine]"
fi


   
#link base filename to target filename (this is done autmatically if the rewrite
#directory is linked and the system is restarted, but CTC machine is using mulitple 
#rewrite directories... it saves us a reboot

#for line in $lines
#do
  
#  echo  "`echo $line | sed "s/$orig/""/"` -> `echo $line | sed "s/$orig/$target/"`"
#  ln -sf `echo $line | sed "s/$orig/$target/"` `echo $line | sed "s/$orig/""/"`
#  echo  
#done


#lines=`find ./Configuration/VehicleTest/* -name "$orig*" -type f`
#for line in $lines
#do
#  echo "$line -> `echo $line | sed "s/$orig/$target/"`"
#  ln -sf `echo $line | sed "s/$orig/$target/"` $line
#done

