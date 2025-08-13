./deps/TSUNAMI_PACT/bin/generate_dataset 1000000 128 1
awk '{print (NR % 2 ? ">" : "<") $0}' sequencesWfa2Format.txt > sequencesWfaGpuFormat.seq
ncu --target-processes all --set full -o NcuProfiles/tsunamiNcu128Reads ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 21 0
ncu --target-processes all --set full -o NcuProfiles/wfaGpuNcu128Reads ./deps/WFA-GPU/bin/wfa.affine.gpu -i sequencesWfaGpuFormat.seq -g 4,6,2 -x 

./deps/TSUNAMI_PACT/bin/generate_dataset 400000 1024 1
awk '{print (NR % 2 ? ">" : "<") $0}' sequencesWfa2Format.txt > sequencesWfaGpuFormat.seq
ncu --target-processes all --set full -o NcuProfiles/tsunamiNcu1KReads ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 200 0
ncu --target-processes all --set full -o NcuProfiles/wfaGpuNcu1KReads ./deps/WFA-GPU/bin/wfa.affine.gpu -i sequencesWfaGpuFormat.seq -g 4,6,2 -x 

./deps/TSUNAMI_PACT/bin/generate_dataset 50000 10240 1
awk '{print (NR % 2 ? ">" : "<") $0}' sequencesWfa2Format.txt > sequencesWfaGpuFormat.seq
ncu --target-processes all --set full -o NcuProfiles/tsunamiNcu10KReads ./deps/TSUNAMI_PACT/bin/wfa_gpu 4 6 2 sequences.txt 400 0
ncu --target-processes all --set full -o NcuProfiles/wfaGpuNcu10KReads ./deps/WFA-GPU/bin/wfa.affine.gpu -i sequencesWfaGpuFormat.seq -g 4,6,2 -x
