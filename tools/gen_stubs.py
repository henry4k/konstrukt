#!/usr/bin/env python3.3

import re
import sys
import os.path
import argparse

class CParameter():
    def __init__(self, name, type):
        self.name = name
        self.type = type

    def __str__(self):
        return '{} {}'.format(
            self.type,
            self.name)

    def __repr__(self):
        return "CParameter(name='{}', type='{}')".format(
            self.name,
            self.type)

class CFunction():
    def __init__(self, name, return_type, parameters):
        self.name = name
        self.return_type = return_type
        self.parameters = parameters

    def __str__(self):
        return '{} {}({})'.format(
            self.return_type,
            self.name,
            ', '.join(str(p) for p in self.parameters))

    def __repr__(self):
        return "CFunction(name='{}', return_type='{}', parameters=[{}])".format(
            self.name,
            self.return_type,
            ', '.join(repr(p) for p in self.parameters))

parameter_pattern = re.compile(r'''
    (?P<type> [a-zA-Z_:][a-zA-Z_:*0-9 ]+)
    \s+
    (?P<name> [a-zA-Z_:]+)
    ''', re.VERBOSE)

function_pattern = re.compile(r'''
    ^\s*
    (?P<return_type> [a-zA-Z_:][a-zA-Z_:*0-9 ]+)
    \s+
    (?P<name> [a-zA-Z_:0-9]+)
    \s*
    \(
    (?P<parameters> .*)
    \)
    \s*
    ;\s*$
    ''', re.VERBOSE)

def log(type, format, *args):
    message = str.format(format, *args)
    output = sys.stdout
    if type != 'INFO':
        output = sys.stderr
    print(str.format('{}: {}', type, message), file=output)

def parse_cparameters(parameters):
    for parameter_string in parameters.split(','):
        parameter_string = parameter_string.strip()
        parameter_match = parameter_pattern.search(parameter_string)
        if parameter_match:
            name = parameter_match.group('name')
            type = parameter_match.group('type')
            yield CParameter(name=name, type=type)
        else:
            if parameter_string == '...':
                raise RuntimeError('Variadic arguments not supported yet.')
            raise RuntimeError('Can\'t parse parameter: "'+parameter_string+'" (This is probably a bug.)')

def try_parse_cfunction(function_string):
    function_match = function_pattern.search(function_string)
    if function_match:
        name = function_match.group('name')

        return_type = function_match.group('return_type')
        parameters = list(parse_cparameters(function_match.group('parameters')))
        return CFunction(name=name,
                         return_type=return_type,
                         parameters=parameters)
    else:
        if 'class' in function_string:
            raise RuntimeError('Found a class definition. Methods can\'t be stubbed yet.')
        return None

def get_cfunction_stub_pointer_name(function):
    return function.name+'_stub'

def write_cfunction_stub_pointer(function, file, extern):
    stub_pointer_template = None
    if extern:
        stub_pointer_template = \
            'extern {return_type} (*{pointer_name})({parameters});\n'
    else:
        stub_pointer_template = \
            '{return_type} (*{pointer_name})({parameters}) = NULL;\n'

    pointer_name = get_cfunction_stub_pointer_name(function)
    parameters = ', '.join(p.type for p in function.parameters)
    file.write(stub_pointer_template.format(
        return_type=function.return_type,
        pointer_name=pointer_name,
        parameters=parameters))

def write_cfunction_stub_implementation(function, file):
    implementation_template = \
'''{return_type} {name}({parameter_declarations})
{{
    if(!{pointer_name})
        dummyAbortTest(DUMMY_FAIL_TEST, "Called {name} without stub callback.");
    return {pointer_name}({parameter_names});
}}
'''
    pointer_name = get_cfunction_stub_pointer_name(function)
    parameter_declarations = ', '.join(str(p) for p in function.parameters)
    parameter_names = ', '.join(p.name for p in function.parameters)
    file.write(implementation_template.format(
        return_type=function.return_type,
        name=function.name,
        parameter_declarations=parameter_declarations,
        parameter_names=parameter_names,
        pointer_name=pointer_name))

def get_stub_header_name(language, name):
    return name+'_stub.h'

def get_stub_implementation_name(language, name):
    return name+'_stub.'+language

def write_stub_header(language, name, header_name, functions):
    file_name = get_stub_header_name(language, name)
    with open(file_name, 'w', encoding='UTF-8') as file:
        file.write('#include "{}"\n'.format(header_name))
        file.write('\n')
        for function in functions:
            write_cfunction_stub_pointer(function, file, extern=True)

def write_stub_implementation(language, name, functions):
    file_name = get_stub_implementation_name(language, name)
    with open(file_name, 'w', encoding='UTF-8') as file:
        file.write('#include <stddef.h> // NULL\n')
        file.write('#include <dummy/core.h> // dummyAbortTest\n')
        file.write('#include "{}"\n'.format(get_stub_header_name(language, name)))
        file.write('\n')
        for function in functions:
            write_cfunction_stub_pointer(function, file, extern=False)
            write_cfunction_stub_implementation(function, file)
            file.write('\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate stubs for C functions.')
    parser.add_argument('--lang',
                        default='c',
                        choices=['c','cpp'])
    parser.add_argument('headers',
                        metavar='header',
                        nargs='+')
    args = parser.parse_args()

    lang = args.lang
    headers = args.headers
    exit_code = 0

    for header in headers:
        module_name = os.path.splitext(os.path.basename(header))[0]

        with open(header, 'r', encoding='UTF-8') as file:
            functions = []
            for line_number, line in enumerate(file):
                location = str.format('{}:{}', header, line_number)
                try:
                    function = try_parse_cfunction(line)
                    if function:
                        functions.append(function)
                        log('INFO', '{} Found {}', location, function)
                except RuntimeError as error:
                    log('WARN', '{} {}', location, error)
                    exit_code = 1

        write_stub_header(language=lang,
                          name=module_name,
                          header_name=header,
                          functions=functions)

        write_stub_implementation(language=lang,
                                  name=module_name,
                                  functions=functions)

    sys.exit(exit_code)