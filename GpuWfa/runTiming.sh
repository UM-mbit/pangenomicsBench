

NUM_ITERS=5

echo "################################################################################"
echo "RUNNING FOR 128"
echo "################################################################################"
### Build the dataset
# The three parameters at the end are (N_SEQS, SEQ_LEN, ERR_RATE_PERCENT)
./deps/TSUNAMI_PACT/bin/generate_dataset 1000000 128 1 
#generate wfa2 dataset
tail -n +4 sequences.txt > sequencesWfa2Format.txt

### Run the experiments
rm -f tsunamiResults.txt && touch tsunamiResults.txt
rm -f wfa2Results.txt && touch wfa2Results.txt

for i in $(seq 1 $NUM_ITERS); do
  ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 21 1 | grep  "GPU Time" | /usr/bin/grep -oP "\d+\.\d+" >> tsunamiResults.txt
  ./deps/WFA2-lib/build/align_benchmark --algorithm gap-affine-wfa -i sequencesWfa2Format.txt --affine-penalties 0,4,6,2 --num-threads 64 |& grep -Po "Time.Benchmark *\d+\.\d+ ms" | /usr/bin/grep -Po "\d+\.\d+" >> wfa2Results.txt
done

echo "TSUNAMI PACT GPU Time (ms)"
echo "($(paste -sd + tsunamiResults.txt)) * 1000/$NUM_ITERS" | bc -l
echo "WFA2 Time (ms)"
echo "($(paste -sd + wfa2Results.txt)) /$NUM_ITERS" | bc -l



echo "################################################################################"
echo "RUNNING FOR 1K"
echo "################################################################################"
### Build the dataset
#generate tsunami datasets
./deps/TSUNAMI_PACT/bin/generate_dataset 400000 1024 1
#generate wfa2 dataset
tail -n +4 sequences.txt > sequencesWfa2Format.txt

### Run the experiments
rm -f tsunamiResults.txt && touch tsunamiResults.txt
rm -f wfa2Results.txt && touch wfa2Results.txt

for i in $(seq 1 $NUM_ITERS); do
  ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 200 1 | grep  "GPU Time" | /usr/bin/grep -oP "\d+\.\d+" >> tsunamiResults.txt
  ./deps/WFA2-lib/build/align_benchmark --algorithm gap-affine-wfa -i sequencesWfa2Format.txt --affine-penalties 0,4,6,2 --num-threads 64 |& grep -Po "Time.Benchmark *\d+\.\d+ ms" | /usr/bin/grep -Po "\d+\.\d+" >> wfa2Results.txt
done

echo "TSUNAMI PACT GPU Time (ms)"
echo "($(paste -sd + tsunamiResults.txt)) * 1000/$NUM_ITERS" | bc -l
echo "WFA2 Time (ms)"
echo "($(paste -sd + wfa2Results.txt)) /$NUM_ITERS" | bc -l




echo "################################################################################"
echo "RUNNING FOR 10K"
echo "################################################################################"
### Build the dataset
#generate tsunami datasets
./deps/TSUNAMI_PACT/bin/generate_dataset 50000 10240 1
#generate wfa2 dataset
tail -n +4 sequences.txt > sequencesWfa2Format.txt
 
### Run the experiments
#!/usr/bin/bash
rm -f tsunamiResults.txt && touch tsunamiResults.txt
rm -f wfa2Results.txt && touch wfa2Results.txt

for i in $(seq 1 $NUM_ITERS); do
  ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 400 1 | grep  "GPU Time" | /usr/bin/grep -oP "\d+\.\d+" >> tsunamiResults.txt
  ./deps/WFA2-lib/build/align_benchmark --algorithm gap-affine-wfa -i sequencesWfa2Format.txt --affine-penalties 0,4,6,2 --num-threads 64 |& grep -Po "Time.Benchmark *\d+\.\d+ ms" | /usr/bin/grep -Po "\d+\.\d+" >> wfa2Results.txt
done

echo "TSUNAMI PACT GPU Time (ms)"
echo "($(paste -sd + tsunamiResults.txt)) * 1000/$NUM_ITERS" | bc -l
echo "WFA2 Time (ms)"
echo "($(paste -sd + wfa2Results.txt)) /$NUM_ITERS" | bc -l




echo "################################################################################"
echo "RUNNING FOR 130K"
echo "(skipping TSUNAMI for 130K because of memory limitataions. Uncomment lines in this script to run TSUNAMI)"
echo "################################################################################"
#### Build the dataset
# 130592 is average length from wfmash. 500 was found to give about half the time of 1000 units, so we assume this is enough work to be representative
#generate tsunami datasets
./deps/TSUNAMI_PACT/bin/generate_dataset 5000 130000 1
#generate wfa2 dataset
tail -n +4 sequences.txt > sequencesWfa2Format.txt

rm -f tsunamiResults.txt && touch tsunamiResults.txt
rm -f wfa2Results.txt && touch wfa2Results.txt

for i in $(seq 1 $NUM_ITERS); do
  ./deps/WFA2-lib/build/align_benchmark --algorithm gap-affine-wfa -i sequencesWfa2Format.txt --affine-penalties 0,4,6,2 --num-threads 64 |& grep -Po "Time.Benchmark *\d+\.\d+ s" | /usr/bin/grep -Po "\d+\.\d+" >> wfa2Results.txt
  #uncomment to run tsunami for 130K, but it will likely run out of memory
  #./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 2000 1

done

echo "WFA2 Time (ms)"
echo "($(paste -sd + wfa2Results.txt)) * 1000 /$NUM_ITERS" | bc -l
#uncomment to run tsunami for 130K, but it will likely run out of memory
#echo "TSUNAMI PACT GPU Time (ms)"
#echo "($(paste -sd + tsunamiResults.txt)) * 1000/$NUM_ITERS" | bc -l
