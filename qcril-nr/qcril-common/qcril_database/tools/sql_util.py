"""
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
import collections
import warnings
import logging
import os
import re

copyright_header = """/*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

"""


# property/configuration type
PropertyObj = collections.namedtuple('PropertyObj', 'name, value_type, def_val, valid_values, reboot')

class SQLUtil(object):
    def __init__(self):
        self.output_sql_file = ""
        self.output_sql_version = None
        self.prop = dict()

    def __get_each_file(self, path):
        for root, dirs, files in os.walk(path):
            select_files = filter(re.compile(r'.*_config\.sql').match, files)
            for each_file in sorted(select_files, key=lambda f: float(re.findall(r'\d+\.*\d*', f)[0])):
                yield os.path.join(root, each_file)

    def __handle_insert_cmd(self, settings):
        logging.debug('Handling INSERT commands...')
        if len(settings) != 2:
            raise RuntimeError("INSERT cmd syntax error!")
        # Get item and value list. Also strip off spaces and unnecessary " and ' around string
        item_list = [item.strip("'\"") for item in "".join(settings[0].split()).split(',')]
        val_list = [item.strip("'\"") for item in "".join(settings[1].split()).split(',')]
        if len(item_list) != len(val_list):
            raise RuntimeError("INSERT cmd syntax error!")

        name = value_type = def_val = valid_values = reboot = None
        for idx, item in enumerate(item_list):
            if item == 'property':
                name = val_list[idx]
            elif item == 'def_val':
                def_val = val_list[idx]

        if name is None:
            raise RuntimeError("INSERT cmd syntax error: no property name specified")
        logging.debug('Handling INSERT commands: property: {}, def_val: {}'.format(name, def_val))
        if name != 'qcrildb_version':
            self.prop[name] = PropertyObj(name=name, value_type=value_type,
                    def_val=def_val, valid_values=valid_values, reboot=reboot);

    def __handle_update_cmd(self, settings):
        logging.debug('Handling UPDATE commands...')
        if len(settings) != 2:
            raise RuntimeError("UPDATE cmd syntax error!")
        # get assign list and strip off unnecessary spaces
        assign_list = "".join(settings[0].split()).split(',')
        keyword = "".join(settings[1].split()).strip("'\"")

        #if not self.prop.has_key(keyword): // comment out for python3 compatibility
        if not keyword in self.prop:
            warnings.warn('UPDATE with no matches')
            return
        name, value_type, def_val, valid_values, reboot = self.prop[keyword].name, \
                self.prop[keyword].value_type, self.prop[keyword].def_val, \
                self.prop[keyword].valid_values, self.prop[keyword].reboot
        for assign_statement in assign_list:
            # check length first?
            item, val = assign_statement.split('=')[:2]
            if item == 'def_val':
                def_val = val[1:-1];  # strip first and last "s. refer __generate_update_cmd
                if def_val == "":
                    def_val = None
        logging.debug('Handling UPDATE commands: property: {} val: {}'.format(name, def_val))
        self.prop[keyword] = PropertyObj(name=name, value_type=value_type,
                def_val=def_val, valid_values=valid_values, reboot=reboot);

    def __handle_delete_cmd(self, settings):
        logging.debug('Handling DELETE commands...')
        keyword = "".join(settings[0].split()).strip("'\"")
        #if self.prop.has_key(keyword): // comment out for python3 compatibility
        logging.debug('Handling DELETE commands: property: {}'.format(keyword))
        if keyword in self.prop:
            del self.prop[keyword]
        else:
            warnings.warn('DELETE with no matches')

    def __parse_sql_cmds(self, cmd):
        """ parse the SQL cmd and update the internal dictionary """
        sql_cmd = " ".join(cmd.split())
        # the insert regex will include "INSERT OR REPLACE" and "INSERT OR IGNORE"
        insert_regex = re.compile(r"^INSERT.*qcril_properties_table\s*\((.*)\)\s*VALUES\s*\((.*)\)")
        update_regex = re.compile(r"^UPDATE\s+qcril_properties_table\s+SET\s+(.*)WHERE\s+property=(.*);")
        delete_regex = re.compile(r"^DELETE\s+FROM\s+qcril_properties_table\s+WHERE\s+property=(.*);")

        res = insert_regex.match(sql_cmd)
        if (res):
            self.__handle_insert_cmd(res.groups())
            return

        res = update_regex.match(sql_cmd)
        if (res):
            self.__handle_update_cmd(res.groups())
            return

        res = delete_regex.match(sql_cmd)
        if (res):
            self.__handle_delete_cmd(res.groups())

    def load_sql_files(self, path, xml_version):
        """Load all SQL files alphabetically in the given path,
        and 'execute' each line of the cmd """
        db_version = None
        for each_file in self.__get_each_file(path):
            res = re.match('(.*)_config', os.path.basename(each_file))
            if res:
                db_version = float(res.groups()[0])
                # so only load lower version of sql cmd files (in case of DEV modification or
                # Non-OTA upgrade case, the last sql cmd file will be re-generated)
                if db_version >= xml_version:
                    logging.info('XML modifcation without uprev')
                    break
            logging.debug('Loading sql file : {}'.format(each_file))
            with open(each_file, 'r') as fp:
                for lines in fp:
                    self.__parse_sql_cmds(lines)
        return db_version

    def set_output_sqlfile_info(self, version, path):
        """Generate the output SQL cmd file"""
        file_name = '{}_config.sql'.format(version)
        self.output_sql_version = version
        self.output_sql_file = os.path.join(path, file_name)

    def __same_config(self, config1, config2):
        # TODO: more fileds to be added in the future
        logging.debug('is same: config1({}, {}) and config2({}, {})'.format(
            config1.name, config1.def_val, config2.name, config2.def_val))
        return config1.name == config2.name \
            and config1.def_val == config2.def_val

    def __generate_update_cmd(self, config1, config2):
        settings = ''
        # TODO: more fields to be added in the future
        if config1.def_val != config2.def_val:
            if config2.def_val is not None:
                settings += 'def_val="{}", '.format(config2.def_val)
            else:
                settings += 'def_val="", '
        settings = settings.strip(', ')

        return 'UPDATE qcril_properties_table SET {} WHERE property="{}";'.format(settings, config2.name)

    def __generate_insert_cmd(self, config):
        item = 'property'
        value = '"{}"'.format(config.name)
        if config.def_val is not None:
            item += ', def_val'
            value += ', "{}"'.format(config.def_val)
        # TODO: more fields to be added in the future

        return 'INSERT OR REPLACE INTO qcril_properties_table({}) VALUES({});'.format(item, value)

    def __generate_delete_cmds(self, fp):
        for key in self.prop:
            cmd = 'DELETE FROM qcril_properties_table WHERE property="{}";'.format(key)
            fp.write(cmd + '\n')

    def generate_diff_sql_cmds(self, prop_list):
        logging.info('Generating: {}'.format(self.output_sql_file))
        with open(self.output_sql_file, "w") as fp:
            fp.write(copyright_header)
            fp.write('CREATE TABLE IF NOT EXISTS qcril_properties_table '
                     '(property TEXT PRIMARY KEY NOT NULL, def_val TEXT, value TEXT);\n');
            fp.write("INSERT OR REPLACE INTO qcril_properties_table(property, def_val) VALUES"
                    "('qcrildb_version',{});\n".format(self.output_sql_version))
            for item in prop_list:
                #if self.prop.has_key(item.name): // comment out for python3 compatibility
                if item.name in self.prop:
                    if not self.__same_config(item, self.prop[item.name]):
                        fp.write(self.__generate_update_cmd(self.prop[item.name], item) + '\n')
                    del self.prop[item.name]
                else:
                    fp.write(self.__generate_insert_cmd(item) + '\n')
            # generate the delete SQL cmds that do not exist in newer version
            self.__generate_delete_cmds(fp)
