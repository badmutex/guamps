# create a library of the main functions
add_library(guamps ${LinkType} guampsio.c gmx_t.c gmxutil.c output.c data.c select.c)

# the api executables
add_executable(guamps_get get.c)
add_executable(guamps_set set.c)
target_link_libraries (guamps_get guamps ${LinkLibs})
target_link_libraries (guamps_set guamps ${LinkLibs})

# destinations
install(TARGETS guamps_get guamps_set DESTINATION bin)
install(TARGETS guamps                DESTINATION lib)
