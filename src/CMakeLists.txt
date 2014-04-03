# create a library of the main functions
add_library(guamps STATIC guampsio.c gmx_t.c gmxutil.c output.c data.c select.c)

# the api executables
add_executable(guamps_get get.c)
add_executable(guamps_set set.c)
SET_TARGET_PROPERTIES(guamps_get PROPERTIES LINK_SEARCH_START_STATIC 1)
SET_TARGET_PROPERTIES(guamps_get PROPERTIES LINK_SEARCH_END_STATIC 1)
# SET_TARGET_PROPERTIES(guamps_set PROPERTIES LINK_SEARCH_START_STATIC 1)
# SET_TARGET_PROPERTIES(guamps_set PROPERTIES LINK_SEARCH_END_STATIC 1)
target_link_libraries (guamps_get guamps ${GROMACS_LIBRARY} m pthread dl)
target_link_libraries (guamps_set guamps ${GROMACS_LIBRARY} m pthread dl)

# destinations
install(TARGETS guamps_get guamps_set DESTINATION bin)
install(TARGETS guamps                DESTINATION lib)
