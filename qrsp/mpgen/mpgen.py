"""
Copyright (c) 2017, 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
from __future__ import absolute_import

import traceback
import sys

# add package path
import os
from argparse import ArgumentError
sys.path.append(os.path.dirname(os.path.realpath(__file__)))

# CUSTOM EXCEPTIONS
class GreatSuccess(BaseException):  # app successfully complete
    pass

# DBG - TEST command line
"""
# sys.argv.append(r'--wait')
# sys.argv.append(r'-pfe')
# sys.argv.append(r'--debug_regex')
sys.argv.append(r'-v')
sys.argv.append(r'-rn')
sys.argv.append(r'1')
sys.argv.append(r'-pf')
sys.argv.append(r'xxxxxxxxxxx')
sys.argv.append(r'-s')
sys.argv.append(r'marriott')
# """
# DBG - TEST command line


try:
    import datetime
    #*** Execution time ***
    started = datetime.datetime.now()
    cborgen_found = True

    import mpgen_kernel
    import mpgen_dwarf
    import mpgen_xml
    import mpgen_catalog
    import mpgen_populate
    import argparse
    import os
    import mpgen_shell
    import mpgen_cgen
    import mpgen_dtsgen
    import const
    import mpgen_kconfig
    import binascii

    try:
        import mpgen_cborgen

    except ImportError:
        cborgen_found = False
        pass

    def main():
        parser = argparse.ArgumentParser(description = "MPGEN tool\n",
                                        formatter_class = argparse.RawDescriptionHelpFormatter)
        exclusive_group = parser.add_mutually_exclusive_group(required = True)
        exclusive_group.add_argument('--vmlinux', type = str, help = 'path to vmlinux', metavar = '<in_file>')
        exclusive_group.add_argument('--header', type = str, help = 'generates MP header to a given file', metavar = '<out_file>')
        parser.add_argument('--dts_payload', type = bool, help = 'generates MP DTS pay load .C file that contains dts_payload symbol', metavar = '')
        parser.add_argument('--dts', type = bool, help = 'generates MP DTS file. This includes dts pay load generation', metavar = '')
        parser.add_argument('--config', type = argparse.FileType('r'), help = 'path to kernel .config file', required = False, default = '../../../../out/msm-kernel-kalama-consolidate/gki_kernel/common/.config')
        parser.add_argument('--binpath', type = str, help = 'path to cross compiler bin directory', default = '')
        parser.add_argument('--objcopy', type = str, help = 'file name of the `objcopy` utility', default = '../../../../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-objcopy')
        parser.add_argument('--objdump', type = str, help = 'file name of the `objdump` utility', default = '../../../../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-objdump')
        parser.add_argument('--symdump', type = str, help = 'file name of the `objdump` utility for symbols', default = '../../../../prebuilts/clang/host/linux-x86/clang-r437112b/bin/llvm-readelf')
        parser.add_argument('--dd', type = str, help = 'file name of the `dd` utility', default = 'dd')
        parser.add_argument('--cc', type = str, help = 'compiler command line', default = '')

        args, unparsed = parser.parse_known_args()

        if (unparsed): raise Exception("Unknown arguments: %s" % unparsed)

        # Set ARGUMENTS
        mpgen_kernel.OBJCOPY = os.path.join(args.binpath, args.objcopy)
        mpgen_kernel.OBJDUMP = os.path.join(args.binpath, args.objdump)
        mpgen_kernel.SYMDUMP = os.path.join(args.binpath, args.symdump)
        mpgen_shell.DD = args.dd

        print("\n\n")
        print("======================================")
        print("            %s %s         " % (const.NAME, const.VERSION))
        print("======================================")

        # Generate MP header file
        if args.header:
            data = mpgen_cgen.generate_rtic_mp_header()
            with open(args.header, 'wb') as f:
                f.write(data)
                raise GreatSuccess

        # Generate MP object (C)
        vmlinux = args.vmlinux

        # create a kernel python object
        print("KERNEL")
        kernel = mpgen_kernel.Kernel()

        # objcopy the kernel if it is out of date
        print("VMLINUX")
        kernel.get_kernel(vmlinux)

        if args.dts_payload or args.dts:
            # generate dts payload
            with open("rtic_dts_payload.c", "w") as f_cfile:
                c_content = mpgen_cgen.generate_rtic_dts(kernel)
                print("writing DTS C file ...\n")
                f_cfile.write(c_content)

            if args.dts:
                # compile to object
                # mpgen_shell.dsystemf("cat rtic_dts.c  %s %s %s", OBJCOPY, vmlinux, KERNEL_IMAGE)
                print "building DTS pay load"
                mpgen_shell.dsystemf("%s -c rtic_dts_payload.c -o rtic_dts_payload.o" % (args.cc))

                # cat $C_F | /pkg/asw/compilers/gnu/linaro-toolchain/.4.9-2014.06-02_linux-x86/bin/aarch64-linux-gnu-gcc -c -o alex_dummy_dtb.o -x c - && \
                # objcopy -j .rodata -O binary -I elf64-little alex_dummy_dtb.o alex_dummy_dtb.rodata

                # extract dts_payload binary content (assume we have only one structure in .const section, therefore get this section)
                # mpgen_shell.dsystemf("%s -j .const -O binary -I elf64-little alex_dummy_dtb.o alex_dummy_dtb.rodata" % (args.objcopy))
                # mpgen_shell.dsystemf("/usr/local/bin/gobjcopy -I elf64-little -j .const -O binary rtic_dts_payload.o rtic_dts_payload.const")
                mpgen_shell.dsystemf("%s -j .rodata -O binary rtic_dts_payload.o rtic_dts_payload.const" % (args.objcopy))

                # convert binary blob to hex characters
                with open('rtic_dts_payload.const', 'rb') as f:
                    pyaload_bin = f.read()
                    payload_hex = binascii.hexlify(pyaload_bin)
                    payload_hex = " ".join([a + b for a, b in zip(payload_hex[::2], payload_hex[1::2])])

                    # generate dts file
                    with open("rtic_mp.dts", "w") as f:
                        content = mpgen_dtsgen.generate_rtic_mp_dts(payload_hex)
                        f.write(content)
                        # print ">>>>>", content

        else: # 'cfile'
            print("CATALOG")
            if not args.config: raise RuntimeError("--config argument is required for MP .c file generation")
            config = mpgen_kconfig.Kconfig(args.config.read())
            catalog = mpgen_catalog.get_catalog(config)

            print("DWARF")
            dwarf = mpgen_dwarf.Dwarf()
            # calculate task_struct offsets using dwarf parser
            #dwarf.get_dwarf(vmlinux, catalog)  # update catalog with DWARF data
            #*/
            print("POPULATE")
            mpdb = mpgen_populate.get_mpdb(catalog, kernel, config)

            with open("rtic_mp.c", "w") as f_cfile:
                # cfile = mpgen_cfile.Cfile(kernel, mpdb)
                c_content = mpgen_cgen.generate_rtic_mp(kernel, mpdb, dwarf)
                print("WRITE C\n")
                f_cfile.write(c_content)

            print("POPULATE CBOR")
            if cborgen_found:
                with open("qrks_mp.bin", "w") as f_cborfile:
                    cbor_content = mpgen_cborgen.generate_rtic_mp_cbor(kernel, mpdb)
                    print("WRITE CBOR\n")
                    f_cborfile.write(cbor_content)

            else:
                print("CBOR2 MODULE NOT FOUND. MPDATA FILE NOT GENERATED.")

        print("======================================")
        print("            MPGEN PROTO RUN COMPLETE")
        print("======================================")

        # todo: add proper exit code
        # sys.exit(0)  # explicit exit is required for the frozen file
        raise GreatSuccess

    if __name__ == '__main__':
        main()

except GreatSuccess:
    # *** EXECUTION TIME ***
    ended = datetime.datetime.now()
    print 'Complete in %i minutes (%i sec.).' % ((ended - started).seconds / 60, (ended - started).seconds)
    if cborgen_found:
        print 'COMPLETE: GREAT SUCCESSS'
    else:
        print("PLEASE INSTALL THE NECESSARY DEPENDENCIES FOR MPDATA GENERATION -------- pip install cbor2; apt install libc++-dev.")
    exit(0)

except KeyboardInterrupt:
    print >> sys.stderr, "terminated by user"
    exit(1)

except Exception as e:  # pylint: disable=W0703
    # print >> sys.stderr
    #if args.verbose:
    traceback.print_exc(file = sys.stderr)  # BGD
    print >> sys.stderr, "FATAL ERROR: %s" % e
    exit(1)

