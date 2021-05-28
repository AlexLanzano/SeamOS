#! /usr/bin/python3

import sys

def generate_config_header(define_statements):
    config_header = open("config.h", "w")
    config_header.write("#ifndef CONFIG_H\n")
    config_header.writelines(define_statements)
    config_header.write("#endif")
    config_header.close()

def generate_define_statement(config_name, config_value):
    return "#define {} {}\n".format(config_name, config_value)

def is_static_string(string):
    if string[0] == '"' and string[-1] == '"':
        return True
    return False

def invalid_config_value(config_value):
    if not config_value.isnumeric() and not is_static_string(config_value):
        return True
    return False

def invalid_config_name(config_name):
    if config_name[0:6] != "CONFIG":
        return True
    for c in config_name:
        if not c.isalnum() and c != '_':
            return True
    return False

def parse_lines_into_define_statements(lines):
    define_statements = []
    line_number = 0
    for line in lines:
        if line[0] == '#':
            continue

        config_name,config_value = line.split('=', 1)
        config_name = config_name.strip()
        config_value = config_value.strip()

        if invalid_config_name(config_name):
            print("Invalid config name at line {} in config file".format(line_number))
            return -1

        if invalid_config_value(config_value):
            print("Invalid config value at line {} in config file".format(line_number))
            return -1

        define_statements.append(generate_define_statement(config_name, config_value))
        line_number += 1

    return define_statements

def main():
    if len(sys.argv) < 2 and len(sys.argv) > 2:
        sys.exit(-1)

    config_file = open(sys.argv[1], "r")
    lines = config_file.readlines()
    config_file.close()

    define_statements = parse_lines_into_define_statements(lines)
    if define_statements == -1:
        sys.exit(-1)

    generate_config_header(define_statements)

main()
