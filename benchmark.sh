#!/bin/bash
# run spm_project with different parameters

# create file "benchmark.csv" if it doesn't exist, and write the header

# go to build directory

if [ ! -f benchmark.csv ]; then
    rm benchmark.csv
    touch benchmark.csv
    echo "n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_read, time_writing, time_total_no_rw, time_total_rw, exec_type" > benchmark.csv
fi



# 1. run the program with different number of threads
echo "*** Running with different number of threads ***"
for (( i = 2; i < 130; i=i+2 )) do
    echo "Running with $i threads..."
    ./build/spm_project ./files/input.160M.txt $i 1 $i gmr
    sleep 1
done

echo "Running benchmark with fastflow"
for (( i = 2; i < 130; i= i+2 )) do
    ./build/spm_project ./files/input.160M.txt $i 1 $i ff
    sleep 1
done
