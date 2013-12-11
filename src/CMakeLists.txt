# create a library of the main functions
add_library(guamps ${LinkType} guampsio.c gmx_t.c gmxutil.c output.c)

# the api executables
add_executable(guamps_get get.c)
add_executable(guamps_set set.c)
target_link_libraries (guamps_get ${GROMACS_LIBRARIES} guamps)
target_link_libraries (guamps_set ${GROMACS_LIBRARIES} guamps)

# destinations
install(TARGETS guamps_get guamps_set DESTINATION bin)
install(TARGETS guamps                DESTINATION lib)
