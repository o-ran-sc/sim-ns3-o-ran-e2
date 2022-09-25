# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

def configure(conf):
    conf.env.append_value('CXXFLAGS', '-I/usr/local/include/e2sim')
    conf.env.append_value("LINKFLAGS", ["-L/usr/local/lib"])
    conf.env.append_value("LIB", ["e2sim"])

def build(bld):
    module = bld.create_ns3_module('oran-interface', ['core'])    
    module.source = [
        'model/oran-interface.cc',
        'model/asn1c-types.cc',
        'model/function-description.cc',
        'model/kpm-indication.cc',
        'model/kpm-function-description.cc',
        'model/ric-control-message.cc',
        'model/ric-control-function-description.cc',
        'helper/oran-interface-helper.cc',
        'helper/indication-message-helper.cc',
        'helper/lte-indication-message-helper.cc',
        'helper/mmwave-indication-message-helper.cc'
        ]

    module_test = bld.create_ns3_module_test_library('oran-interface')
    module_test.source = [
        'test/oran-interface-test-suite.cc',
        ]
    # Tests encapsulating example programs should be listed here
    if (bld.env['ENABLE_EXAMPLES']):
        module_test.source.extend([
        #    'test/oran-interface-examples-test-suite.cc',
             ])

    headers = bld(features='ns3header')
    headers.module = 'oran-interface'
    headers.source = [
        'model/oran-interface.h',
        'model/asn1c-types.h',
        'model/function-description.h',
        'model/kpm-indication.h',
        'model/kpm-function-description.h',
        'model/ric-control-message.h',
        'model/ric-control-function-description.h',
        'helper/oran-interface-helper.h',
        'helper/indication-message-helper.h',
        'helper/lte-indication-message-helper.h',
        'helper/mmwave-indication-message-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')
    

    # bld.ns3_python_bindings()
