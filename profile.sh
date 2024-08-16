#!/usr/bin/bash

set -e
#This function profiles a single kernel and creates an output directory for that
#kernel. 
#@param ROOT_OUT_DIR the top level output directory
#
#The output is structured as follows:
# ROOT_OUT_DIR
# - KernelOuts/ //outputs of kernel for true comparisons
#   - Out/ //The outputs of the kernel for comparison to true
# - Logs/ //the log of the command including the runtime
#   - uarch.log
# - Profiles/ //the raw vtune profile
#   - Uarch
# - Reports/ //reports generated from vtune
function runner {
  set -e
  ROOT_OUT_DIR=$1
  APP=$2

  #setup output directories
  PROFILE_DIR="${ROOT_OUT_DIR}/Profiles"
  KERNEL_OUT_DIR="${ROOT_OUT_DIR}/KernelOuts"
  LOG_DIR="${ROOT_OUT_DIR}/Logs"
  REPORT_DIR="${ROOT_OUT_DIR}/Reports"
  mkdir $ROOT_OUT_DIR
  mkdir $PROFILE_DIR
  mkdir $KERNEL_OUT_DIR
  mkdir $LOG_DIR
  mkdir $REPORT_DIR

  #gives 500mb
  COMMAND="vtune -collect uarch-exploration -start-paused -data-limit=500 -result-dir ${PROFILE_DIR}/Uarch ${APP} 2>&1 | tee ${LOG_DIR}/uarch.log"
  echo $COMMAND
  eval $COMMAND

  mv ./Out $KERNEL_OUT_DIR
  vtune -report summary -inline-mode on -r $PROFILE_DIR/Uarch -report-output $REPORT_DIR/uarchSum.txt

}

rm -rf GbwtOut
GBWT="./Gbwt/bin/gbwt /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt"
runner "GbwtOut" "$GBWT"
rm -rf GsswOut
GSSW="./Gssw/bin/gssw /data2/kaplannp/Genomics/Datasets/Kernels/Gssw"
runner "GsswOut" "$GSSW"

