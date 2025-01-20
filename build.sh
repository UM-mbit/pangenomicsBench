export VTUNE_HOME=/opt/intel/oneapi/vtune/latest

GBV_BUILD_FAILED=0
cd Gbv/deps/GraphAligner
conda env create -f CondaEnvironment_linux.yml
conda activate GraphAligner || GBV_BUILD_FAILED=1
make -j || GBV_BUILD_FAILED=1
conda deactivate
cd ../../..

GBWT_BUILD_FAILED=0
cd Gbwt
# Make SDSL-lite dependency
cd deps/sdsl-lite
BUILD_PORTABLE=1 CXXFLAGS="-O3 -ggdb -g3 -fopenmp" ./install.sh $(pwd) || GBWT_BUILD_FAILED=1
cd ../..
# Make GBWT dependency
cd deps/gbwt 
make -j || GBWT_BUILD_FAILED=1
cd ../..
# Make kernel
make -j || GBWT_BUILD_FAILED=1
cd ..


GSSW_BUILD_FAILED=0
cd Gssw
conda create -n vgPgBench python=3.8 -y
conda activate vgBench || GSSW_BUILD_FAILED=1
cd deps/vg
make -j || GSSW_BUILD_FAILED=1
cd ../..
make -j || GSSW_BUILD_FAILED=1
conda deactivate
cd ..

GWFA_BUILD_FAILED=0
cd Gwfa
make -j || GWFA_BUILD_FAILED=1
cd ..

echo "GBV_BUILD_FAILED=$GBV_BUILD_FAILED"
echo "GBWT_BUILD_FAILED=$GBWT_BUILD_FAILED"
echo "GSSW_BUILD_FAILED=$GSSW_BUILD_FAILED"
echo "GWFA_BUILD_FAILED=$GWFA_BUILD_FAILED"
