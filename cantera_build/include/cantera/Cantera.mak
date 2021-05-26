###############################################################################
# Include Snippet for Makefiles
#
#    To create Cantera C++ applications from the install environment
#    include this file into your Makefile environment by putting
#    the line "include Cantera.mak" in your Makefile.
#
#  Main Variables:
#
#      CANTERA_INCLUDES = Variable containing the include path
#      CANTERA_LIBS = List of libraries to include on the link line
#
#      CANTERA_FORTRAN_LIBS = list of libraries to link for Fortran programs
#      CANTERA_FORTRAN_MODS = Directory containing the F90 .mod files
#

CANTERA_VERSION=2.5.2b1

###############################################################################
#        CANTERA CORE
###############################################################################

# The directory where Cantera include files may be found.
#  Include files in application programs should start with:
#     #include "cantera/thermo.h"
#     #include "cantera/kernel/HMWSoln.h"

CANTERA_INSTALL_ROOT=/proj/lufmreact/users/x_yzhan/softwareZY/build/cantera

CANTERA_CORE_INCLUDES=-I$(CANTERA_INSTALL_ROOT)/include

CANTERA_EXTRA_INCLUDES= 

# Required Cantera libraries
CANTERA_CORE_LIBS=-pthread -L/proj/lufmreact/users/x_yzhan/softwareZY/build/cantera/lib -lcantera

CANTERA_CORE_LIBS_DEP = /proj/lufmreact/users/x_yzhan/softwareZY/build/cantera/lib/libcantera.a

CANTERA_EXTRA_LIBDIRS=

CANTERA_CORE_FTN=-L/proj/lufmreact/users/x_yzhan/softwareZY/build/cantera/lib -lcantera_fortran -lcantera

CANTERA_FORTRAN_MODS=$(CANTERA_INSTALL_ROOT)/include/cantera

CANTERA_FORTRAN_SYSLIBS=-lpthread -lstdc++

###############################################################################
#            BOOST
###############################################################################

CANTERA_BOOST_INCLUDES=-I/software/sse/easybuild/prefix/software/Boost/1.61.0-foss-2016b-nsc1/include

###############################################################################
#         CVODE/SUNDIALS LINKAGE
###############################################################################

CANTERA_SUNDIALS_INCLUDE=
CANTERA_SUNDIALS_LIBS= 

###############################################################################
#         BLAS LAPACK LINKAGE
###############################################################################

CANTERA_BLAS_LAPACK_LIBS=

###############################################################################
#      COMBINATIONS OF INCLUDES AND LIBS
###############################################################################

CANTERA_INCLUDES=$(CANTERA_CORE_INCLUDES) $(CANTERA_SUNDIALS_INCLUDE) \
                 $(CANTERA_BOOST_INCLUDES) $(CANTERA_EXTRA_INCLUDES)

CANTERA_TOTAL_INCLUDES = $(CANTERA_INCLUDES)

# Add this into the compilation environment to identify the version number
CANTERA_DEFINES = -DCANTERA_VERSION=2.5.2b1

CANTERA_LIBS=$(CANTERA_CORE_LIBS) \
             $(CANTERA_EXTRA_LIBDIRS) $(CANTERA_SUNDIALS_LIBS) \
             $(CANTERA_BLAS_LAPACK_LIBS)

CANTERA_TOTAL_LIBS=$(CANTERA_LIBS)

CANTERA_TOTAL_LIBS_DEP= $(CANTERA_CORE_LIBS_DEP) \
                        $(CANTERA_SUNDIALS_LIBS_DEP)

CANTERA_FORTRAN_LIBS=$(CANTERA_CORE_FTN) \
                     $(CANTERA_EXTRA_LIBDIRS) $(CANTERA_SUNDIALS_LIBS) \
                     $(CANTERA_BLAS_LAPACK_LIBS) $(CANTERA_FORTRAN_SYSLIBS)

###############################################################################
#  END
###############################################################################
