
add_executable(example example.cpp)
target_link_libraries(example logger)

add_executable(iterCat iteratorCategories.cpp)
target_link_libraries(iterCat logger)

add_executable(inserterExample inserterExample.cpp)
target_link_libraries(inserterExample logger)
