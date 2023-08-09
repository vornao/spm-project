#!/bin/bash
# run spm_project with different parameters

# create file "benchmark.csv" if it doesn't exist, and write the header

# go to build directory

# create a variable with filename
FILENAME="./files/benchmark.csv"
INPUT_FILE="./files/input.128M.txt"
OUTPUT_FILE="./files/output.bin"

if [ ! -f $FILENAME ]; then
    rm $FILENAME
    touch $FILENAME
    echo "n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_read, time_writing, time_total_no_rw, time_total_rw, exec_type" > $FILENAME
fi

if [ ! -f $OUTPUT_FILE ]; then
    rm $OUTPUT_FILE
fi



# 1. run the program with different number of threads
echo "*** Running with different number of threads ***"
for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads..."
    ./build/spm_project $INPUT_FILE $i 1 $i map
    rm $OUTPUT_FILE
    sleep 1
done

echo "Running benchmark with fastflow"
for (( i = 2; i < 130; i=i+2 )) do
    ./build/spm_project $INPUT_FILE $i 1 $i ff
    rm $OUTPUT_FILE
    sleep 1
done

<<COMMENT

for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads... and 2 reducers"
    ./build/spm_project $INPUT_FILE $i 2 $i gmr
    rm $OUTPUT_FILE
    sleep 1
done

for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads... and 4 reducers"
    ./build/spm_project $INPUT_FILE $i 4 $i gmr
    rm $OUTPUT_FILE
    sleep 1
done


for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads... and 8 reducers"
    ./build/spm_project $INPUT_FILE $i 8 $i gmr
    rm $OUTPUT_FILE
    sleep 1
done

for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads... and 16 reducers"
    ./build/spm_project $INPUT_FILE $i 16 $i gmr
    sleep 1
done

for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads... and 32 reducers"
    ./build/spm_project $INPUT_FILE $i 32 $i gmr
    rm $OUTPUT_FILE
    sleep 1
done
COMMENT


