#### APP ####
add_executable(dupcfinder "main.cpp")
target_link_libraries(dupcfinder common)
set_target_properties(dupcfinder PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
