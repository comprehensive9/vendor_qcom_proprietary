"""
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
import sys
import argparse
import logging
import sql_util as SU
import xml.etree.ElementTree as ET

# c++ related:
not_edit_warning = "//////////////////////Auto Generated file: DO NOT EDIT ///////////////////////\n"
enum_type_name = "property_id_type"
enum_type_last_item = "PROPERTY_ID_MAX"

# keyword for multi instance config
multi_instance_keyword = "%(instance)"

def generate_property_identifier(name):
    return name.replace(".", "_").upper()

def generate_cplusplus_header_file(property_list, file_name):
    """generate c++ header file"""
    logging.info('Generating: {}'.format(file_name))
    with open(file_name, 'w') as fp:
        #write_markings(fp)
        fp.write(not_edit_warning)
        fp.write('#pragma once\n')
        fp.write('#include <stdint.h>\n')
        fp.write("typedef enum property_id {\n");
        for idx, val in enumerate(property_list):
            if idx == 0:
                fp.write('\t' + generate_property_identifier(val.name) + ' = 0,\n')
            else:
                fp.write('\t' + generate_property_identifier(val.name) + ',\n')
        fp.write('\t' + enum_type_last_item + '\n')
        fp.write('} ' + enum_type_name + ';\n')
        fp.write('\n')

        fp.write('#define CONFIG_DATA_AGGRE_INIT { \\\n')
        fp.write('\t{ \\\n')
        for prop in property_list:
            fp.write('\t{"' + prop.name + '", nullptr}, \\\n')
        fp.write('\t} \\\n')
        fp.write('\t}\n')

def strip_file_path(file_name):
    # if not found, it will return -1
    pos = file_name.rfind('/');
    return file_name[pos+1:];

def generate_source_file(property_list, output_path):
    """generate c++ source file"""
    logging.info('Generating: {}'.format(output_path.source_path))
    with open(output_path.source_path, 'w') as fp:
        #write_markings(fp)
        fp.write(not_edit_warning)
        fp.write('#include "' + strip_file_path(output_path.header_path) + '"\n')
        fp.write('#include <array>\n')
        fp.write('\n')
        fp.write('static constexpr uint32_t length = (uint32_t)' + enum_type_name + '::' + enum_type_last_item + ';\n')
        fp.write('static const std::array<std::string, length> property_key_table {\n')
        for prop in property_list:
            fp.write('\t"' + prop.name + '",\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('const std::string get_property_key(' + enum_type_name + ' id) {\n')
        fp.write('\tauto idx = static_cast<uint32_t>(id);\n')
        fp.write('\tif (idx < ' + 'static_cast<uint32_t>(' + enum_type_name + '::' + enum_type_last_item + ')) {\n')
        fp.write('\t\treturn property_key_table[idx];\n')
        fp.write('\t} else {\n')
        fp.write('\t\treturn "";\n')
        fp.write('\t}\n')
        fp.write('}\n')

# generate immutable named tuple for each configuration
def get_config_info(xml_node):
    desc = name = value_type = value = valid_values = reboot = None
    for item in xml_node:
        if (item.tag == 'description'):
            desc = item.text
        elif (item.tag == 'name'):
            name = item.text
        elif (item.tag == 'type'):
            value_type = item.text
        elif (item.tag == 'value'):
            value = item.text
        elif (item.tag == 'validValues'):
            valid_values = item.text
        elif (item.tag == 'rebootRequired'):
            reboot = item.text
    # check mandatory items
    # if (desc == None or name == None or value == None):
    if (desc is None or name is None):
        logging.critical('Something is missing: desc: {}, name: {}'.format(desc, name))
    return SU.PropertyObj(name=name, value_type=value_type, def_val=value, valid_values=valid_values, reboot=reboot)

def is_multi_instance_config(config):
    length = len(multi_instance_keyword)
    # check if name string ends with "%(instance)". If name length is less
    # than the length of keyword, the slice operation will return ''
    return config.name[-length:] == multi_instance_keyword

def add_multi_instance_config(config, config_list):
    max_instance = 2
    old_name = config.name
    #for i in xrange(max_instance): // comment out for python3 compatibility
    for i in range(max_instance):
        new_config = SU.PropertyObj(name=old_name.replace(multi_instance_keyword, str(i)),
            value_type=config.value_type, def_val=config.def_val, valid_values=config.valid_values, reboot=config.reboot)
        config_list.append(new_config)
    return

# return a *list* of items and xml version (use list to keep the same enum/order
# in generated c++ files)
def read_xml_file(xml_file):
    # initialization
    xml_version = None
    config_list = []
    # XML parsing
    tree = ET.parse(xml_file)
    root = tree.getroot()
    for child in root:
        if (child.tag == 'version'):
            xml_version = float(child.text)
            logging.info('XML version {}'.format(xml_version))
        elif (child.tag == 'property'):
            config = get_config_info(child)
            if is_multi_instance_config(config):
                add_multi_instance_config(config, config_list);
            else:
                config_list.append(config)

    if (xml_version is None):
        raise RuntimeError('FATAL: version is missing in XML file!')
    return config_list, xml_version

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Qcril configuration generator')
    parser.add_argument('xml_path', action="store")
    parser.add_argument('--header', action="store", dest="header_path")
    parser.add_argument('--sql', action="store", dest="sql_cmd_path")
    parser.add_argument('-v', '--verbose', action='count', default=0)
    output_path = parser.parse_args()

    if output_path.verbose > 0:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    # read XML file
    xml_list, xml_version = read_xml_file(output_path.xml_path)
    # generate the C++ header file
    if output_path.header_path is not None:
        generate_cplusplus_header_file(xml_list, output_path.header_path)
    # generate the SQL cmds file
    if output_path.sql_cmd_path is not None:
        # read the current SQL file and generate the delta to match the latest xml file
        sql_util = SU.SQLUtil();
        db_version = sql_util.load_sql_files(output_path.sql_cmd_path, xml_version)
        # case "<": OTA/version upgrade
        # case "=": DEV modification
        if (db_version is None or db_version <= xml_version):
            sql_util.set_output_sqlfile_info(xml_version, output_path.sql_cmd_path)
            sql_util.generate_diff_sql_cmds(xml_list)
