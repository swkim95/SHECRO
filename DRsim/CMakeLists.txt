#----------------------------------------------------------------------------
# Setup the project
project(DRsim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(
  ${HEPMC_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Geant4_INCLUDE_DIR}
  # ${CORSIKA_DIR}/include
)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(DRsim DRsim.cc ${sources} ${headers})
target_link_libraries(
  DRsim ${Geant4_LIBRARIES}
  ${HEPMC_DIR}/lib64/libHepMC3.so
  ${HEPMC_DIR}/lib64/libHepMC3rootIO.so
  # ${CORSIKA_DIR}/lib/unknown/libCorsikaFileIO.a
  # ${CORSIKA_DIR}/lib/unknown/libCorsikaIntern.a
  # ${CORSIKA_DIR}/lib/unknown/libCorsikaROOT.a
  # ${CORSIKA_DIR}/lib/unknown/libCorsikaROOTOut.a
  # ${CORSIKA_DIR}/lib/unknown/libCorsikaToROOT.a
  # ${CORSIKA_DIR}/lib/unknown/libMachineIndependent.a
  # ${CORSIKA_DIR}/lib/unknown/liburqmd.a
  # ${CORSIKA_DIR}/coast/CorsikaROOT/lib_ROOT_Dict.so
  # ${CORSIKA_DIR}/coast/CorsikaROOT/lib_CORSIKA_ROOT.so
  # ${CORSIKA_DIR}/coast/CorsikaIntern/lib_CORSIKA_Intern.so
  # ${CORSIKA_DIR}/coast/CorsikaFileIO/lib_CORSIKA_FileIO.so
  # /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-centos7-gcc48-opt/root/lib/libCint.so
  # ${ROOT_LIBRARIES}
  rootIO
  )

file(GLOB DRsim_MACROS ${PROJECT_SOURCE_DIR}/*.mac)
file(COPY ${DRsim_MACROS} DESTINATION ${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
#add_custom_target(DRsim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DRsim DESTINATION bin)
install(FILES ${DRsim_MACROS} DESTINATION .)
