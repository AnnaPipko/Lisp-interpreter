cmake_minimum_required(VERSION 3.5)
project(lispp)

include(common.cmake)

add_library(lispp-lib
        lispp/lispp.cpp
        lispp/parser.cpp
        lispp/tokenizer.cpp
        lispp/node_types.cpp
        lispp/scope.cpp
        lispp/exceptions.cpp
        lispp/common_functions.cpp)

add_executable(lispp
  lispp/main.cpp)

target_link_libraries(lispp
  lispp-lib)

add_executable(test_lispp
  test/test_tokenizer.cpp
  test/test_parser.cpp
  test/test_boolean.cpp
  test/test_control_flow.cpp
  test/test_eval.cpp
  test/test_integer.cpp
  test/test_lambda.cpp
  test/test_list.cpp
  test/test_symbol.cpp
  test/test_equal.cpp
  catch_main.cpp)

target_link_libraries(test_lispp
  lispp-lib)

add_executable(test_tokenizer
        test/test_tokenizer.cpp
        catch_main.cpp)

target_link_libraries(test_tokenizer
        lispp-lib)

add_executable(test_parser
        test/test_parser.cpp
        catch_main.cpp)

target_link_libraries(test_parser
        lispp-lib)
