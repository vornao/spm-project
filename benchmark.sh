# run spm_project with different parameters

# create file "benchmark.csv" if it doesn't exist, and write the header

if [ ! -f benchmark.csv ]; then
    echo "n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_read, time_writing, time_total_no_rw, time_total_rw, exec_type" > benchmark.csv
fi


# 1. run the program with different number of threads
echo "*** Running with different number of threads ***"
for (( i = 1; i < 256; i*2 )); do
    echo "Running with $i threads..."
    ./spm_project input.txt i 1 i gmr
done

echo "Running benchmark with fastflow"
for (( i = 1; i < 256; i*2 )); do
    ./spm_project input.txt i 1 i ff
done
