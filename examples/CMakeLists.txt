set(examples
    e2sim-integration-example
    l3-rrc-example
    oran-interface-example
    encode-decode-indication
    ric-control-function-desc
    ric-indication-messages
    test-wrappers
)
foreach(
  example
  ${examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${liboran-interface}
  )
endforeach()