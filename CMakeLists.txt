find_external_library(
    DEPENDENCY_NAME e2sim
    HEADER_NAME e2sim.hpp
    LIBRARY_NAME e2sim
    PATH_SUFFIXES e2sim
    SEARCH_PATHS /usr/local/include
)

set(source_files
    helper/indication-message-helper.cc
    helper/lte-indication-message-helper.cc
    helper/mmwave-indication-message-helper.cc
    helper/oran-interface-helper.cc
    model/asn1c-types.cc
    model/function-description.cc
    model/kpm-function-description.cc
    model/kpm-indication.cc
    model/oran-interface.cc
    model/ric-control-function-description.cc
    model/ric-control-message.cc
)

set(header_files
    helper/indication-message-helper.h
    helper/lte-indication-message-helper.h
    helper/mmwave-indication-message-helper.h
    helper/oran-interface-helper.h
    model/asn1c-types.h
    model/function-description.h
    model/kpm-function-description.h
    model/kpm-indication.h
    model/oran-interface.h
    model/ric-control-function-description.h
    model/ric-control-message.h
)

set(test_sources
    test/oran-interface-test-suite.cc
)

include_directories(${e2sim_INCLUDE_DIRS})

build_lib(
  LIBNAME oran-interface
  SOURCE_FILES ${source_files}
  HEADER_FILES ${header_files}
  LIBRARIES_TO_LINK
    ${libcore}
    ${e2sim_LIBRARIES}
  TEST_SOURCES ${test_sources}
)

