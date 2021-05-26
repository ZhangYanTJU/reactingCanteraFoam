#ifndef CT_CONFIG_H
#define CT_CONFIG_H

//---------------------------- Version Flags ------------------//
// Cantera version -> this will be a double-quoted string value
#define CANTERA_VERSION "2.5.2b1"

// Just the major + minor version (i.e. 2.2 instead of 2.2.0)
#define CANTERA_SHORT_VERSION "2.5"

//------------------------ Fortran settings -------------------//

// define types doublereal, integer, and ftnlen to match the
// corresponding Fortran data types on your system. The defaults
// are OK for most systems

typedef double doublereal;   // Fortran double precision
typedef int integer;      // Fortran integer
typedef int ftnlen;       // Fortran hidden string length type

// Fortran compilers pass character strings in argument lists by
// adding a hidden argument with the length of the string. Some
// compilers add the hidden length argument immediately after the
// CHARACTER variable being passed, while others put all of the hidden
// length arguments at the end of the argument list. Define this if
// the lengths are at the end of the argument list. This is usually the
// case for most unix Fortran compilers, but is (by default) false for
// Visual Fortran under Windows.
#define STRING_LEN_AT_END

// Define this if Fortran adds a trailing underscore to names in object files.
// For linux and most unix systems, this is the case.
#define FTN_TRAILING_UNDERSCORE 1


#define CT_SUNDIALS_VERSION 53

//-------- LAPACK / BLAS ---------

#define LAPACK_FTN_STRING_LEN_AT_END 1
#define LAPACK_NAMES_LOWERCASE 1
#define LAPACK_FTN_TRAILING_UNDERSCORE 1
/* #undef CT_USE_LAPACK */

/* #undef CT_USE_SYSTEM_EIGEN */
/* #undef CT_USE_SYSTEM_FMT */
/* #undef CT_USE_SYSTEM_YAMLCPP */
#define CT_USE_DEMANGLE 1

//--------- operating system --------------------------------------

// The configure script defines this if the operating system is Mac
// OS X, This used to add some Mac-specific directories to the default
// data file search path.
/* #undef DARWIN */

// Identify whether the operating system is Solaris
// with a native compiler
/* #undef SOLARIS */

//---------- C++ Compiler Variations ------------------------------

// This define is needed to account for the variability for how
// static variables in templated classes are defined. Right now
// this is only turned on for the SunPro compiler on Solaris.
// in that system , you need to declare the static storage variable.
// with the following line in the include file
//
//    template<class M> Cabinet<M>* Cabinet<M>::s_storage;
//
// Note, on other systems that declaration is treated as a definition
// and this leads to multiple defines at link time
/* #undef NEEDS_GENERIC_TEMPL_STATIC_DECL */

//-------------- Optional Cantera Capabilities ----------------------

//    Enable Sundials to use an external BLAS/LAPACK library if it was
//    built to use this option
#define CT_SUNDIALS_USE_LAPACK 0

#endif
