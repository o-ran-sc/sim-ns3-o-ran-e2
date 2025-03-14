check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/oran-interface-examples-test-suite.cc
        )
endif()

#include_directories(/usr/local/include/e2sim)
#link_directories(/usr/local/lib)
#link_libraries(e2sim)

find_external_library(DEPENDENCY_NAME e2sim
                      HEADER_NAME e2sim.hpp
                      LIBRARY_NAME e2sim
                      SEARCH_PATHS /usr/local/include/e2sim)

if(!${e2sim_FOUND})
    message(WARNING "e2sim is required by oran-interface and was not found" )
    return ()
endif()

include_directories(${e2sim_INCLUDE_DIRS})
message(STATUS "dirs found:  ${e2sim_INCLUDE_DIRS}" )
message(STATUS "libraries found:  ${e2sim_LIBRARIES}" )

build_lib(
    LIBNAME oran-interface
    SOURCE_FILES model/oran-interface.cc
                 helper/oran-interface-helper.cc
                 model/asn1c-types.cc
                 model/function-description.cc
                 model/kpm-indication.cc
                 model/kpm-function-description.cc
                 model/ric-control-message.cc
                 model/ric-control-function-description.cc
                 helper/oran-interface-helper.cc
                 helper/indication-message-helper.cc
                 helper/lte-indication-message-helper.cc
                 helper/mmwave-indication-message-helper.cc
    HEADER_FILES model/oran-interface.h
                 helper/oran-interface-helper.h
                 model/asn1c-types.h
                 model/function-description.h
                 model/kpm-indication.h
                 model/kpm-function-description.h
                 model/ric-control-message.h
                 model/ric-control-function-description.h
                 helper/indication-message-helper.h
                 helper/lte-indication-message-helper.h
                 helper/mmwave-indication-message-helper.h
    LIBRARIES_TO_LINK 
                    ${libcore}
                    ${e2sim_LIBRARIES}
)

