'''
Created on May 7, 2018

@author: amavrin
'''
# Copyright (c) 2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

from __future__ import absolute_import
import mpgen_shell
import mpgen_kernel
import mpgen_catalog
import mpgen_kconfig  # Kconfig
import mpgen_mpdb
import re
import os.path
import sys
from collections import OrderedDict

class Dwarf:
	"""
	Simple DWARF parser to extract kernel info from obj files
	"""
	def __init__(self):
		self.task_struct_offsets = {}
		return

	def get_cu_info_list(self, vmlinux):
		"""
		Parses first DWARF info level that is represents top DIE with CU info.
		Gets the list of all top DIE's that includes CU file names and their build paths (as well as some other info)
		@ret List of dicts with CU properties
		"""
		raw = mpgen_shell.dsystemf("%s '%s' --dwarf=info --dwarf-depth=1", mpgen_kernel.OBJDUMP, vmlinux)
		# print raw
		l_cus = re.findall(r'Compilation Unit .*?\: \.\.\.', raw, re.DOTALL)

		# parse each record and add to an object list
		res = []
		for cu in l_cus:
			tmp = {}
			for line in cu.split('\n'):
				# print line
				# find properties for DW_TAG_compile_unit, etc.
				# note it is possible to parse out the CU properties like length, Abbrev Offset, etc., though we are not interested in this for now
				# print re.findall(r'\s*\<[abcdefABCDEF\d]+\>\s+(\(.*?\)\:)?(\w+)\s+\: (.*)', line)
				ret = re.findall(r'\s*\<[abcdefABCDEF\d]+\>\s+(\w+)\s+\: (.*)', line)
				if len(ret):
					key, val = ret[0]
					# filter out the note in braces
					ret = re.findall(r'\(.*?\)\: (.*)', val)
					if len(ret):
						val = ret[0]
					tmp[key] = val

			res.append(tmp)

		return res

	def get_cu_file_names(self, props):
		"""
		Filters out the props and returns tuple with compile unit name and it's compile path
		i.e. (DW_AT_name, DW_AT_comp_dir) - note for some compile units DW_AT_comp_dir may not be defined, these are skipped
		"""
		ret = [(rec['DW_AT_name'], rec['DW_AT_comp_dir']) for rec in props if 'DW_AT_comp_dir' in rec.keys()]
		return ret

	def find_task_struct_offsets(self, obj_files):
		"""
		Gets the dwarf info for the obj files and parses them to extract offsets for task_struct fields.
		First extracts the task_struct blocks from the dwarf info and then parses them to find the offsets.
		Returns map of offsets if all of them are found else returns empty.
		"""
		task_struct = 'task_struct'
		task_struct_offset_keys = [': state', ': pid', ': parent', ': comm']
		task_struct_offset_dict = {}
		root_tag = '<1>'

		for obj_file in obj_files:
			raw = mpgen_shell.dsystemf("%s '%s' --dwarf=info", mpgen_kernel.OBJDUMP, obj_file)
			raw_lines = raw.split('\n')
			root_tag_indices = [i for i, l in enumerate(raw_lines) if root_tag in l]
			task_struct_blocks = []
			for i, block_index in enumerate(root_tag_indices):
				if block_index+1 < len(raw_lines) and task_struct in raw_lines[block_index+1]:
					if i+1 < len(root_tag_indices):
						task_struct_blocks.append(tuple((block_index+1, root_tag_indices[i+1])))
					else:
						task_struct_blocks.append(tuple((block_index+1, len(raw_lines))))

			for block in task_struct_blocks:
				begin = block[0]
				end = block[1]
				for i, line in enumerate(raw_lines[begin:end]):
					for key in task_struct_offset_keys:
						if line.endswith(key):
							if begin+i+4 < len(raw_lines) and 'DW_AT_data_member_location:' in raw_lines[begin+i+4]:
								offset_parts = raw_lines[begin+i+4].split(':')
								try:
									task_struct_offset_dict[task_struct + key] = int(offset_parts[-1])
								except ValueError:
									return {}
				if len(task_struct_offset_dict) == len(task_struct_offset_keys):
					return task_struct_offset_dict

		return {}

	def filter_obj_files(self, filter_list, cu_tuples):
		"""
		Returns paths to obj files for selected compile paths.
		Extracts section of the obj file path from the cu file path
		and returns them if there is an obj file present in those locations.
		"""
		obj_files = []
		for task_struct_ref in filter_list:
			for cu_tuple in cu_tuples:
				if (len(cu_tuple) == 2) and (task_struct_ref in cu_tuple[0]):
					cu_file_path = cu_tuple[0]
					obj_dir_path = cu_tuple[1]
					root_dir = obj_dir_path.split('/')[-1]
					cu_file_path_pos = cu_file_path.find(root_dir)
					if cu_file_path_pos != -1 and (cu_file_path_pos + len(root_dir) + 1) < len(cu_file_path):
						obj_file_path = cu_file_path[cu_file_path_pos + len(root_dir) + 1:]
						file_ext = obj_file_path.find('.')
						if file_ext != -1:
							obj_file_path = obj_dir_path + '/' + obj_file_path[:file_ext] + '.o'
							if os.path.isfile(obj_file_path):
								obj_files.append(obj_file_path)

		return obj_files

	def get_offsets(self, offset_names):
		offsets = []
		for offset_name in offset_names:
			offset = -1
			if offset_name in self.task_struct_offsets:
				offset = self.task_struct_offsets[offset_name]
			offsets.append(offset)

		return offsets

	def wu_list_by_symbol(self, symbol, obj_files):
		"""
		Simple DWARF variable parser
		"""
		for obj_file in obj_files:

			# parse out all die's
			raw = mpgen_shell.dsystemf("%s '%s' --dwarf=info", mpgen_kernel.OBJDUMP, obj_file)

			dies = OrderedDict()
			split = re.split(r"\<([abcdef\d]+)\>\<([abcdef\d]+)\>\:", raw, flags = re.DOTALL | re.MULTILINE)
			for n in range(1, len(split), 3):
				dies[int(split[n + 1], base = 16)] = {'s': int(split[n], base = 16), 't': split[n + 2], 'i': int(split[n + 1], base = 16)}

			good = []
			ptr = []

			def find_pointer(die, pos):
				# print pos, '<-pos', die['s'], die['i'], die['t']

				if 'DW_TAG_pointer_type' in die['t']:
					# print >> sys.stderr, "FOUND", pos
					ptr.append(pos)
					return pos
				if 'DW_TAG_structure_type' in die['t'] or 'DW_TAG_array_type' in die['t']:
					i = dies.keys().index(die['i'])
					for k in dies.keys()[i + 1:]:
						if dies[k]['s'] != die['s'] + 1: break
						ret = find_pointer(dies[k], pos)
						# get number of items in array
						if 'DW_TAG_array_type' in die['t']:
							if isinstance(ret, tuple):
								for i in range(0, ret[1]):
									# same as DW_AT_type - go inside the array
									pos = find_pointer(dies[int(re.findall(r'DW_AT_type.*?\<([xabcdef\d]+)>', die['t'])[0], base = 16)], pos)
					if 'DW_TAG_structure_type' in die['t']:
						pos += int(re.findall(r'DW_AT_byte_size.*?([\d]+)', die['t'])[0])

				if 'DW_AT_data_member_location' in die['t']:
					pos += int(re.findall(r'DW_AT_data_member_location.*?([\d]+)', die['t'])[0])

                                if 'DW_TAG_subrange_type' in die['t']:
                                        # do not follow the array time
                                        return pos, int(re.findall(r'DW_AT_count.*?([\d]+)', die['t'])[0])

				if 'DW_AT_type' in die['t']:
					return find_pointer(dies[int(re.findall(r'DW_AT_type.*?\<([xabcdef\d]+)>', die['t'])[0], base = 16)], pos)

				good.append(pos)
				return pos

			# find the target item by name
			for die in dies.values():
				if ('DW_TAG_variable' in die['t']) and re.findall(r"DW_AT_name.*?:\s%s\n" % (symbol), die['t']):
					find_pointer(die, 0)

			# find_pointer(dies[0x199], 0)
			# print "GOOD", good
			# print 'good sorted', sorted(good)
			# print "PTR", ptr
			# print "ptr sorted", sorted(ptr)

			# find ranges
			start = 0
			rngs = []
			for g in sorted(good):
				for p in sorted(ptr):
					start = start if start < g else g
					if g > p:
						if start < p:
							if start not in ptr:
								rngs.append((start, p))
							start = min([gg for gg in good if gg > p])
			# final range
			if rngs[-1][1] < start and max(good) != start:
				rngs.append((start, max(good)))

			# print rngs
			return rngs
		# nothin found
		return []

	def get_dwarf(self, vmlinux, cat):
		if mpgen_kernel.OBJDUMP is None:
			print "ERROR: objdump not defined\n"
			sys.exit(1)
		self.l_props = self.get_cu_info_list(vmlinux)
		self.cu_tuples = self.get_cu_file_names(self.l_props)
		# get KP process stack unwinding data
		obj_files = self.filter_obj_files(cat.kp_proc_stack_obj_search_inc_filter, self.cu_tuples)
		self.task_struct_offsets = self.find_task_struct_offsets(obj_files)
		# dwarf WU's
		self.wu_dwarf = []
		for wud in cat:
			if wud.mp_type is not mpgen_mpdb.WR_ONCE_UNKNOWN_DWARF: continue
			obj_files = self.filter_obj_files(wud.obj_search_inc_filter, self.cu_tuples)
			wud.wu_ranges = self.wu_list_by_symbol(wud.name, obj_files)

