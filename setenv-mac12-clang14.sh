#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/views/LCG_102/arm64-mac12-clang131-opt/setup.sh

# export PATH=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/CMake/3.20.0/arm64-mac12-clang131-opt/bin:$PATH
# source_dir=$PWD
# cd /cvmfs/sft.cern.ch/lcg/releases/LCG_102/Geant4/11.0.2/arm64-mac12-clang131-opt/bin/
# source /cvmfs/sft.cern.ch/lcg/releases/LCG_102/Geant4/11.0.2/arm64-mac12-clang131-opt/bin/geant4.sh
# cd $source_dir
# source /cvmfs/sft.cern.ch/lcg/releases/LCG_102/ROOT/6.26.04/arm64-mac12-clang131-opt/ROOT-env.sh

export HEPMC_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/hepmc3/3.2.4/arm64-mac12-clang131-opt
export FASTJET_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/fastjet/3.4.0p1/arm64-mac12-clang131-opt
export PYTHIA_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/MCGenerators/pythia8/307p1/arm64-mac12-clang131-opt

export PYTHIA8=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/MCGenerators/pythia8/307p1/arm64-mac12-clang131-opt
export PYTHIA8DATA=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/MCGenerators/pythia8/307p1/arm64-mac12-clang131-opt/share/Pythia8/xmldoc
export ROOT_INCLUDE_PATH=/cvmfs/sft.cern.ch/lcg/releases/LCG_102/hepmc3/3.2.4/arm64-mac12-clang131-opt/include:$ROOT_INCLUDE_PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HEPMC_DIR/lib/:$PWD/lib:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LD_LIBRARY_PATH
