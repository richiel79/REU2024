#############################################################################################################################################
# Author: Richard Lara								 #######################################################################################################################
#------------------------------------------------------------------------------- #######################################################################################################################
# Date: July 10, 2024								 #######################################################################################################################
#------------------------------------------------------------------------------- #######################################################################################################################
# Description: This file contains the logic explanations of the simc_heep.C code #######################################################################################################################
#______________________________________________________________________________  #######################################################################################################################


The simc_heep.C code was developed to analyze the ROOTFile's of H(e,e'p) coincidence ROOTFiles produced by using Jlab's Monte Carlo Simulation (SIMC).
These ROOTfiles were produce after first checking the elctron beam energy offsets and angles in the standard_kinematics file and the d2_heep_scan_rad_#.data had no discrepencies.

The simc_heep.C code is written to work EXCLUSIVELY in the same directory where the ROOTFiles are stored, hence the while-for loop inside the simc_heep function that searches for the file with the corresponding delta_scan number inputted by the user.

The simc_heep.C code only produces weighted histograms of the following variables: Em, Q2, W, Pmx, Pmy, Pmz. These histograms are saved to an output ROOTfile that's automatically stored in the worksim/analyzed directory. (This line of code can be changed such that the analyzed file can  be stored in the same directory as the original ROOTFile)



***NOTE****
The code still needs to be looked over and possibly optimized in some areas such as the while-for loop to reduce the run time.
