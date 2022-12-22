include(FetchContent)

add_executable(Assembler
    assembler.cpp
)
target_include_directories(Assembler PRIVATE ${SRC_INC_DIR})