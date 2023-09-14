'''
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Created on Apr 28, 2022

@author: shivpati
'''
from __future__ import absolute_import

import jinja2
import os
import datetime

import mpgen_mpdb
import const
import mpgen_catalog
import mpgen_kernel
import calendar
import time
import cbor2

# load the template

template_loader = jinja2.FileSystemLoader(searchpath = os.path.join(os.path.dirname(__file__), "templates"))
template_env = jinja2.Environment(loader = template_loader)
template = template_env.get_template("rtic_mp.template.c")  # this should throw right away if template is not found
template_header = template_env.get_template("rtic_mp_header.template.h")  # this should throw right away if template is not found
template_dts = template_env.get_template("rtic_dts_payload.template.c")  # this should throw right away if template is not found

def generate_rtic_dts(kernel):
    kernel_base = int(kernel.get_symbol(".head.text").value, 16)
    # Note: it is possible that someone mixes the build environments and when generating the DTB, MP structure in kernel is not available
    mp_offset = "0x0"
    sha256 = "0"
    mp_va_addr = "0x0"
    mp_size = "0x0"
    try:
        mp_offset = "0x%x" % (int(kernel.get_symbol("rtic_mp").value, 16) - kernel_base)
        sha256 = kernel.get_symbal_sha256("rtic_mp")
        mp_va_addr = "0x%x" % int(kernel.get_symbol("rtic_mp").value, 16)
        mp_size = "0x%x" % int(kernel.get_symbol("rtic_mp").size, 16)
    except mpgen_kernel.ExceptionSymbolNotFound as e:
        print "WARNING! Symbol `%s` is missing. It seems that kernel missing an MP. Generating empty RTIC DTS." % (e)

    ret = template_dts.render(datetime = datetime, VERSION = const.VERSION, kernel = kernel, mp_va_addr = mp_va_addr, mp_offset = mp_offset, mp_size = mp_size, sha256 = sha256)  # pylint: disable=maybe-no-member
    return ret

def convertFlagsStrToInt(mp_attrs):
    ret = 0
    attrs = mp_attrs.replace("|", " ")
    for flag in attrs.split(" "):
        if flag == "MP_SECTION_ATTRIBUTE_CODE":
            ret = ret | 1 << 4
        if flag == "MP_SECTION_ATTRIBUTE_ENFORCE":
            ret = ret | 1 << 5
        if flag == "MP_SECTION_ATTRIBUTE_NQHEE" or flag == "MP_SECTION_ATTRIBUTE_KP":
            ret = ret | 1 << 6
        if flag == "MP_SECTION_ATTRIBUTE_NRTIC" or flag == "MP_SECTION_ATTRIBUTE_ATTESTATION":
            ret = ret | 1 << 7
        if flag == "MP_SECTION_ATTRIBUTE_NONSTRICT":
            ret = ret | 1 << 10
        if flag == "MP_SECTION_ATTRIBUTE_ALLOW_ONE":
            ret = ret | 1 << 12
    return ret


def appendMPtolist(mp, mp_type, mplist):
    sha_bstr = ""

    if mp.sha256 != None:
        for sha_byte_str in mp.sha256.split(":"):
            sha_byte_int = int(sha_byte_str, 16)
            sha_byte_char = chr(sha_byte_int)
            sha_bstr += sha_byte_char

    mplist.append([
        mp.name,
        mp_type,
        int(mp.vaddr, 16),
        int(mp.size, 16),
        convertFlagsStrToInt(mp.attrs),
        sha_bstr
    ])

def generate_rtic_mp_cbor(kernel, mpdb):

    # 64bit time stamp from the beginning of epoch
    created_tmstmp = "0x%x" % (calendar.timegm(time.gmtime()))

    version = const.VERSION

    kernel_offset_le = "0"
    try: kernel_offset_le = "%s%s" % (kernel.get_symbol('_kernel_offset_le_hi32').value, kernel.get_symbol('_kernel_offset_le_lo32').value) or "0"
    except Exception:
        pass
    kernel_size_le = "0"
    try: kernel_size_le = "%s%s" % (kernel.get_symbol('_kernel_size_le_hi32').value, kernel.get_symbol('_kernel_size_le_lo32').value) or "0"
    except Exception:
        pass

    banner = kernel.get_banner().replace("\n", "\\n").replace("\0", "\\0")



    ro = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_ONLY]
    ro = sorted(ro, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    wk = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_KNOWN]
    wk = sorted(wk, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    wu = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_UNKNOWN]
    wu = sorted(wu, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    aw = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
    aw = sorted(aw, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    ro_init = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_ONLY_AFTER_INIT]
    ro_init = sorted(ro_init, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    rw = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_WR]
    rw = sorted(rw, key = lambda mp: int(mp.vaddr, 16), reverse = False)

    fulldata = {
        # "version" will end up in position 1 rather than position 0 for some reason in CBOR output
        "version": [
            30,
            20
        ],

        "specs":[

            # 64bit time stamp from the beginning of epoch
            int(created_tmstmp, 16),

            # MPGen tool version
            version,

            # content of the kernel version string
            #banner,

            # el1 va address of the kernel banner
            #int(kernel.get_symbol("linux_banner").value, 16),

            # linux_banner_size
            #int(kernel.get_symbol("linux_banner").size, 16),

            #el1 va address of hlos exception table
            int(kernel.get_symbol("vectors").value, 16),

            # kernel offset
            #int(kernel_offset_le, 16),

            # kernel size
            #int(kernel_size_le, 16),

			#head text addr
            int(kernel.get_symbol("kimage_vaddr").value, 16),

            # kernel base va
            #int(kernel.get_symbol(".head.text").value, 16),

            # false if mpgen detects error/ unsupported kernel features
            mpdb.catalog.is_checkable_kernel,

            # readable mpgen error/warning
            #mpdb.catalog.mp_warning_msg,
        ],
    }

    mplist = []
    for mp in ro:
        appendMPtolist(mp, "ro", mplist)
    for mp in wk:
        appendMPtolist(mp, "wk", mplist)
    for mp in wu:
        appendMPtolist(mp, "wu", mplist)
    for mp in aw:
        appendMPtolist(mp, "aw", mplist)
    for mp in ro_init:
        appendMPtolist(mp, "RO_init", mplist)
    for mp in rw:
        appendMPtolist(mp, "rw", mplist)

    fulldata["MPs"] = mplist

    return cbor2.encoder.dumps(fulldata)

def generate_rtic_mp(kernel, mpdb, dwarf):

    vectors = "0x0"
    try:
        vectors = "0x%x" % int(kernel.get_symbol("vectors").value, 16)
    except mpgen_kernel.ExceptionSymbolNotFound as e:
        err_msg = "WARNING! Important KSymbol `%s` is missing. MP is not usable!" % (e)
        print err_msg
        mpdb.catalog.mark_kernel_non_checkable("%s is missing. MP is not usable!" % (e))
        # On MPGen fatal error, just generate an empty MP source file
        return "\n/* %s */\n\n" % (err_msg)

    kernel_offset_le = "0"
    try: kernel_offset_le = "%s%s" % (kernel.get_symbol('_kernel_offset_le_hi32').value, kernel.get_symbol('_kernel_offset_le_lo32').value) or "0"
    except Exception:
        pass
    kernel_size_le = "0"
    try: kernel_size_le = "%s%s" % (kernel.get_symbol('_kernel_size_le_hi32').value, kernel.get_symbol('_kernel_size_le_lo32').value) or "0"
    except Exception:
        pass

    # symbols __entry_task and __per_cpu_offset may be missing, default it to 0x0
    linux_entry_task_va = "0"
    try: linux_entry_task_va = "%s" % (kernel.get_symbol('__entry_task').value)
    except Exception:
        pass
    linux_per_cpu_offset_va = "0"
    try: linux_per_cpu_offset_va = "%s" % (kernel.get_symbol('__per_cpu_offset').value)
    except Exception:
        pass

    ll = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
    print("generate_rtic_mp, len(aw) == %d" % len(ll))
    for item in ll:
        item.dump()
    banner = kernel.get_banner().replace("\n", "\\n").replace("\0", "\\0")

    # to make it easier to read the template - generate a list of RO features
    # ro = filter(lambda mp: mp.mp_type is mpgen_mpdb.RD_ONLY, mpdb)
    ro = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_ONLY]
    ro = sorted(ro, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    wk = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_KNOWN]
    wk = sorted(wk, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    wu = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_UNKNOWN]
    wu = sorted(wu, key = lambda mp: int(mp.vaddr, 16), reverse = False)
    aw = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
    aw = sorted(aw, key = lambda mp: int(mp.vaddr, 16), reverse = False)

    # filter(lambda mp: mp.mp_type is mpgen_mpdb.RD_ONLY, mpdb) # pylint complains that list comprehension is more efficient

    # 64bit time stamp from the beginning of epoch
    created_tmstmp = "0x%x" % (calendar.timegm(time.gmtime()))

    # Dynamic features. For now use pure C code as a string to initialize dynamic
    dynamic_features = [
        # OEM Unlock and QHEE Report
        ('mp_ou_attributes_t', '''{%s}''' % mpdb.catalog.ou_attributes),
        ('mp_qr_attributes_t', '''{%s}''' % mpdb.catalog.qr_attributes),
        #('mp_kernel_task_struct_offsets_t', '''{%s}''' % ', '.join(map(str, dwarf.get_offsets(['task_struct: state', 'task_struct: pid', 'task_struct: parent', 'task_struct: comm'])))),
    ]

    ret = template.render(dynamic_features = dynamic_features, created_tmstmp = created_tmstmp, datetime = datetime, VERSION = const.VERSION, kernel = kernel, mpdb = mpdb, banner = banner, ro = ro, wk = wk, wu = wu, aw = aw, len = len, kernel_size_le = kernel_size_le, kernel_offset_le = kernel_offset_le, linux_entry_task_va = linux_entry_task_va, linux_per_cpu_offset_va = linux_per_cpu_offset_va, vectors = vectors)  # pylint: disable=maybe-no-member
    return ret

def generate_rtic_mp_header():
    """
    Generates RTIC MP header that to be included by the TA and QHEE
    """

    ret = template_header.render(datetime = datetime, VERSION = const.VERSION)  # pylint: disable=maybe-no-member
    return ret

