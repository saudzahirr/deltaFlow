include(FortranCInterface)

FortranCInterface_VERIFY()

FortranCInterface_HEADER(
    ${CMAKE_SOURCE_DIR}/src/algorithms/mangling.H
    MACRO_NAMESPACE    "API_"
    SYMBOL_NAMESPACE   "API_"
        SYMBOLS            GAUSS_SEIDEL NEWTON_RAPHSON
)

file(GLOB FORTRAN_SOURCES *.F)
set_property(GLOBAL APPEND PROPERTY FORTRAN_SOURCES "${FORTRAN_SOURCES}")

#--------------------------------------------------------------------------------#
REGISTER_GLOBAL_SOURCES_AND_INCLUDES(SOURCES INCLUDES)
#--------------------------------------------------------------------------------#
