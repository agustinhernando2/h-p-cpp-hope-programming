add_library(Unity_test STATIC
    cJSON/tests/unity/src/unity.c
)
add_library(cJSON STATIC
    cJSON/cJSON.c
)

target_include_directories(Unity_test PUBLIC
    cJSON/tests/unity/src
)
target_include_directories(cJSON PUBLIC
cJSON
)
