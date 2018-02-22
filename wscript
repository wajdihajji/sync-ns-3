# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('sync', ['core'])
    module.source = [
        'helper/vm-helper.cc',
        'model/dc.cc',
        'model/hypervisor.cc',
        'model/mb.cc',
        'model/policy.cc',
        'model/vm.cc',
        ]

    module_test = bld.create_ns3_module_test_library('sync')
    module_test.source = [
        'test/sync-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'sync'
    headers.source = [
        'helper/vm-helper.h',
        'model/dc.h',
        'model/hypervisor.h',
        'model/mb.h',
        'model/policy.h',
        'model/vm.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()
