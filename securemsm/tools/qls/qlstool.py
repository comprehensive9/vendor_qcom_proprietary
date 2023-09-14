# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

import os
import sys
import struct
import argparse
import fnmatch
import hashlib
import base64

# Name & version of the tool
QLS_TOOL_NAME = 'qlstool'
QLS_TOOL_VERSION = '2.0'

__version__ = QLS_TOOL_NAME + ' ' + QLS_TOOL_VERSION

# Path definitions
DEFAULT_QLS_DIR_PATH = os.getcwd()

# Tool configuration
FILE_NAME = 'qweslicstore.bin'
SECTOR_SIZE_IN_BYTES = 4096
PARTITION_SIZE_IN_SECTORS = 29
TOTAL_NUM_OF_PARTITIONS = 2
TOTAL_NUM_OF_SECTORS = 64
SLOT_SIZE_IN_SECTORS = 3
MIN_LICENSES = 1
MAX_LICENSES = int(((((PARTITION_SIZE_IN_SECTORS - 1 - 4) / SLOT_SIZE_IN_SECTORS) / 2) * TOTAL_NUM_OF_PARTITIONS) - 1)
PARTITION_ID_START = 0
PARTITION_OFFSET = 1
RESERVED = 0x00000000000000000
PFM_FILE_MAXSIZE = 10240

MAGIC_1 = b'PTBL'
MAGIC_2 = 0x00000100

COPY_1 = 1
COPY_2 = 2

LOG_PARTI_1_COPY_1 = {0: 0x2000, 1: 0x5000, 2: 0x8000, 3: 0xb000}
LOG_PARTI_1_COPY_2 = {0: 0x10000, 1: 0x13000, 2: 0x16000, 3: 0x19000}
LOG_PARTI_2_COPY_1 = {4: 0x1F000, 5: 0x22000, 6: 0x25000, 7: 0x28000}
LOG_PARTI_2_COPY_2 = {4: 0x2D000, 5: 0x30000, 6: 0x33000, 7: 0x36000}

Copy_1_Offsets = [LOG_PARTI_1_COPY_1, LOG_PARTI_2_COPY_1]
Copy_2_Offsets = [LOG_PARTI_1_COPY_2, LOG_PARTI_2_COPY_2]

MAX_FIDS_SUPPORTED = 16
MAX_SERIAL_NUMBER_SIZE = 21
TOC_INDEX = 0
TOC_ENTRY_SIZE = 140
TOC_ENTRY_SERIAL_SIZE = 64

STORE_COUNT = 5
STORE_SIZE = 122880
SLOT_SIZE = 12288
TOC_PARTITION = 'store_0'
MAX_LICENSES_HLOS = int(((STORE_COUNT * STORE_SIZE) / SLOT_SIZE) - 1)


def getHeader():
    """Creates a header for the partition."""
    return (struct.pack('4s', MAGIC_1) +
            struct.pack('I', MAGIC_2) +
            struct.pack('I', (TOTAL_NUM_OF_PARTITIONS * PARTITION_SIZE_IN_SECTORS)) +
            struct.pack('I', TOTAL_NUM_OF_PARTITIONS) + getHdrEntries())


def getHdrEntries():
    """Creates an entries based on number of logical partitions configured."""
    i = 0
    data = bytearray()
    partitionOffset = PARTITION_OFFSET
    while (i < TOTAL_NUM_OF_PARTITIONS):
        data.extend((struct.pack('I', PARTITION_ID_START + i) +
                     struct.pack('HH', partitionOffset, PARTITION_SIZE_IN_SECTORS) +
                     struct.pack('Q', RESERVED)))
        partitionOffset = partitionOffset + PARTITION_SIZE_IN_SECTORS
        i += 1

    return data


def getOffset(slot, copy):
    """Returns an offset for the corresponding given slot and copy."""
    if copy == COPY_1:
        for index in range(len(Copy_1_Offsets)):
            for key, value in Copy_1_Offsets[index].items():
                if slot == key:
                    return value
                else:
                    continue

    elif copy == COPY_2:
        for index in range(len(Copy_2_Offsets)):
            for key, value in Copy_2_Offsets[index].items():
                if slot == key:
                    return value
                else:
                    continue

    else:
        return


def createEmptyPartition(path, size):
    """Creates an empty partition."""
    qls = open(path, "w+b")
    bytes = [0] * (size)
    qls.write(struct.pack('%iB' % (size), *bytes))
    qls.close()


def updatePartition(path, offset, data):
    """Updates the partition with the given data and at the given offset."""
    qls = open(path, "r+b")
    qls.seek(offset)
    qls.write(data)
    qls.close()


def filterUniqueFiles(path):
    """Skip the duplicate pfm files and returns the dict that contains unique pfmfliles list"""
    pfm_unique = {}
    pfm_total = fnmatch.filter(os.listdir(path), '*.pfm')
    for file in pfm_total:
        file_path = os.path.join(path, file)
        file_hash = hashlib.md5(open(file_path, 'rb').read()).hexdigest()
        if file_hash not in pfm_unique:
            pfm_unique[file_hash] = file
        else:
            print('Skipping "%s" file as it is a duplicate of "%s"' % (file, pfm_unique[file_hash]))
    return pfm_unique


def readPFM(filename):
    pem = ""
    in_cert = False

    with open(filename, "r")as f:
        for l in f.readlines():
            if in_cert:
                if 0 == l.find("-----END CERTIFICATE-----"):
                    break
                else:
                    pem += l
            else:
                if 0 == l.find("-----BEGIN CERTIFICATE-----"):
                    in_cert = True
    return bytearray(base64.decodestring(pem))

TAG_MASK = 0x1f
def readDERIdent(der):
    # for now, just return tag
    # print("ident byte = 0x%x" % (der[0]))
    return (der[0] & TAG_MASK, der[1:])

def readUnsignedBytes(der, length):
    if length < 1 or length > 4:
        raise Exception("Unexpected length %d bytes" % (length))
    val = 0
    for i in range(length):
        val = (val << 8) | der[i]
    return (val, der[length:])

def readDERLen(der):
    length_byte = der[0]
    if length_byte == 0xff or length_byte == 0x80:
        raise Exception("Invalid or indefinite length")
    if length_byte < 0x80:
        return (length_byte, der[1:])
    return(readUnsignedBytes(der[1:], length_byte & 0x7f))

def readDERItem(der):
    (ident, der) = readDERIdent(der)
    (length, der) = readDERLen(der)
    # print("length = 0x%x" % (length))
    return (ident, der[0:length], der[length:])

# From section 8.3.2:
# If ... an integer value encoding consit[s] of more than one octet, then:
# the bits of the first octet and bit 8 of the second octed:
#   a) shall not be all ones
#   b) shall not be all zeroes
#
# These rules ensure that an integer value is always incoded in the smallest
# possible number of octets.
#
# The upshot of this is that -1 will always be encoded with length 1 (02 01 FF),
# while -129 will be encoded with length 2 as (02 02 FF 7F),
# And e.g. 65535 will be encoded with length 3.  (02 03 00 FF FF)

def readLargePositiveInteger(der):
    (ident, octets, remainder) = readDERItem(der)
    if ident != 2:
        raise Exception("ident %d when 2 was expected" % (ident))
    if len(octets) >= 1:
        if octets[0] & 0x80 == 0x80:
            raise Exception("Unexpected negative integer")
        if octets[0] == 0:
            # trim leading octet
            return (octets[1:], remainder)
    return (octets, remainder)

def findFids(der_bytes):
    fid_oid_bytes = bytearray([0x2b, 0x06, 0x01, 0x04, 0x01, 0x8b, 0x29, 0x0c, 0x01])
    fid_oid_idx = der_bytes.find(fid_oid_bytes)
    if fid_oid_idx < 0:
        raise Exception("No FIDS found")
    fid_val_idx = fid_oid_idx+len(fid_oid_bytes)
    fid_bytes = der_bytes[fid_val_idx:]
    (ident, fid_value, _) = readDERItem(fid_bytes)
    if ident != 4 or len(fid_value) % 4 != 0:
        raise Exception("Invalid fid value")
    range_list = []
    for i in range(0, len(fid_value),4):
        range_list.append(struct.unpack(">I", fid_value[i:i+4])[0])
    print("FIDS ", range_list)
    return range_list

def createTOCEntry(path, file_name):
    """Returns the TOC entry for the given input pfm file."""
    der_bytes = readPFM(os.path.join(path, file_name))
    toc_empty_entry = struct.pack('%iB' % (TOC_ENTRY_SIZE), *list([0] * TOC_ENTRY_SIZE))

    try:
        fid_range_list = findFids(der_bytes)
        fidrange_count = fid_range_list[0]
        if fidrange_count * 2 > len(fid_range_list):
            raise Exception("Bad FID range count: %d" % fidrange_count)
        fid_count = len(fid_range_list) - 1

        # top-level collection
        (ident, value, der_bytes) = readDERItem(der_bytes)
        # TBS collection
        (ident, value, der_bytes) = readDERItem(value)
        # version
        (ident, value, der_bytes) = readDERItem(value)
        # serial number
        (serial_num, der_bytes) = readLargePositiveInteger(der_bytes)
        print("Serial #", "".join(["%02x" % x for x in serial_num]))
    except Exception as err:
        print('Failed to parse "%s" for TOC entry' % (file_name))
        print(err)
        return toc_empty_entry

    fid_pad = list([0] * (MAX_FIDS_SUPPORTED - fid_count))
    fids = struct.pack('>18I', fid_count, fidrange_count, *(fid_range_list[1:] + fid_pad))

    serial_pad_len = (TOC_ENTRY_SERIAL_SIZE - len(serial_num))
    serial = struct.pack('>I', len(serial_num)) + serial_num + \
             struct.pack('%iB' % serial_pad_len, *list([0] * serial_pad_len))
    return fids + serial


def uefi(args):
    """Generates the uefi license store file."""
    partition = os.path.join(args.output_dir, FILE_NAME)
    createEmptyPartition(partition, (SECTOR_SIZE_IN_BYTES * TOTAL_NUM_OF_SECTORS))
    updatePartition(partition, 0, getHeader())

    if not os.path.exists(args.input_dir):
        print('Input directory and pfm files are not present.\n' +
              'Generated an empty qweslicstore.bin file.')
        return

    pfm_files = filterUniqueFiles(args.input_dir)
    num_pfm_files = len(pfm_files)
    if (num_pfm_files == 0) or (num_pfm_files > MAX_LICENSES):
        print('Generated an empty qweslicstore.bin file.\n' +
              '%d number of pfm files are available.\n' % num_pfm_files +
              'Minimum of %d file or upto maximum of %d files are supported.' % (MIN_LICENSES, MAX_LICENSES))
        return

    index = 0
    file_count = 0
    for file in pfm_files.values():
        index += 1
        file_path = os.path.join(args.input_dir, file)
        file_size = os.path.getsize(os.path.join(args.input_dir, file))
        if (file_size == 0) or (file_size > PFM_FILE_MAXSIZE):
            print(
                '\nSkipping "%s" file, it is either an empty file or it crosses the allowed max file size limit' % file)
            print('Received PFM file size - %d Bytes, Allowed max file size limit - %d Bytes\n' % (
                file_size, PFM_FILE_MAXSIZE))
            continue

        TOC_entry = createTOCEntry(args.input_dir, file)
        updatePartition(partition, (getOffset(TOC_INDEX, COPY_1)) + (file_count * TOC_ENTRY_SIZE), TOC_entry)
        updatePartition(partition, (getOffset(TOC_INDEX, COPY_2)) + (file_count * TOC_ENTRY_SIZE), TOC_entry)
        data = struct.pack('>Q', file_size) + open(file_path, "rb").read()
        updatePartition(partition, getOffset(index, COPY_1), data)
        updatePartition(partition, getOffset(index, COPY_2), data)
        file_count += 1
        continue

    print('Successfully generated a "qweslicstore.bin" with %d pfmfiles in the following path. \n' % file_count +
          '%s' % str(args.output_dir))


def createEmptyStores(path):
    for store in range(STORE_COUNT):
        partition = os.path.join(path, 'store_' + str(store))
        createEmptyPartition(partition, STORE_SIZE)


def getPartitionName(file_count):
    return 'store_' + str(int((file_count + 1) / STORE_COUNT))


def getPartitionOffset(file_count):
    return ((file_count + 1) % STORE_COUNT) * SLOT_SIZE


def hlos(args):
    """Generates the hlos based license store files."""
    createEmptyStores(args.output_dir)

    if not os.path.exists(args.input_dir):
        print('Input directory and pfm files are not present.\n' +
              'Generated an empty HLOS license store files.')
        return

    pfm_files = filterUniqueFiles(args.input_dir)
    num_pfm_files = len(pfm_files)
    if (num_pfm_files == 0) or (num_pfm_files > MAX_LICENSES_HLOS):
        print('Generated an empty HLOS license store files.\n' +
              '%d number of pfm files are available.\n' % num_pfm_files +
              'Minimum of %d file or upto maximum of %d files are supported.' % (MIN_LICENSES, MAX_LICENSES_HLOS))
        return

    index = 0
    file_count = 0
    for file in pfm_files.values():
        index += 1
        file_path = os.path.join(args.input_dir, file)
        file_size = os.path.getsize(os.path.join(args.input_dir, file))
        if (file_size == 0) or (file_size > PFM_FILE_MAXSIZE):
            print(
                '\nSkipping "%s" file, it is either an empty file or it crosses the allowed max file size limit' % file)
            print('Received PFM file size - %d Bytes, Allowed max file size limit - %d Bytes\n' % (
                file_size, PFM_FILE_MAXSIZE))
            continue

        TOC_entry = createTOCEntry(args.input_dir, file)
        updatePartition(os.path.join(args.output_dir, TOC_PARTITION), file_count * TOC_ENTRY_SIZE, TOC_entry)
        data = struct.pack('>Q', file_size) + open(file_path, "rb").read()
        updatePartition(os.path.join(args.output_dir, getPartitionName(file_count)),
                        getPartitionOffset(file_count), data)
        file_count += 1
        continue

    print('Successfully generated a "store_*" files with %d pfmfiles in the following path. \n' % file_count +
          '%s' % str(args.output_dir))


def main(args):
    """Parses the command line arguments and generates a UEFI or HLOS based license store binary files."""
    print('qlstool launched as: "' + ' '.join(sys.argv) + '"\n')

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    if args.mode == 'uefi':
        uefi(args)
    else:
        hlos(args)

    return


def parseArgs(argv):
    # Specifying the usage and description.
    # noinspection PyTypeChecker
    qlsparser = argparse.ArgumentParser(usage='python %(prog)s [options]',
                                        description='Generates the UEFI or HLOS based license store binary files.',
                                        formatter_class=argparse.RawDescriptionHelpFormatter,
                                        epilog=('\n'
                                                'This tool has been validated on python version 2.7 & 3.7.\n'
                                                'It is currently configured with the following parameters:-\n'
                                                '1) Generates the UEFI or HLOS based license store binaries based on '
                                                'the input "--mode". Default is "UEFI".\n'
                                                '2) Processes *.pfm files from the given input directory only.\n'
                                                '3) It does recognize duplicate pfm files, processes one file and '
                                                'skips the rest of duplicate files.\n'
                                                'UEFI:\n'
                                                '   a) Generates a qweslicstore.bin of size 256KB.\n'
                                                '   b) Supports 2 logical partitions (Data Stores).\n'
                                                '   c) Generates an empty qweslicstore.bin if there are 0 pfm files or '
                                                'greater than 7 pfm files.\n'
                                                'HLOS:\n'
                                                '   a) Generates a "store_*" files of each size 60KB.\n'
                                                '   b) Supports 5 Data Stores.\n'
                                                '   c) Generates an empty "store_*" files if there are 0 pfm files or '
                                                'greater than %d pfm files.' % MAX_LICENSES_HLOS))
    # Specifying the version information.
    qlsparser.add_argument('--version', action='version',
                           version=__version__,
                           help='show the version number and exit')

    # Specifying the input location
    qlsparser.add_argument('-i', '--input_dir', metavar='<dir>',
                           help='directory to process input pfm files. DEFAULT: "./"',
                           default=DEFAULT_QLS_DIR_PATH)

    # Specifying the output location
    qlsparser.add_argument('-o', '--output_dir', metavar='<dir>',
                           help='directory to store output files. DEFAULT: "./"',
                           default=DEFAULT_QLS_DIR_PATH)

    # Specifying the mode of operation
    qlsparser.add_argument('-m', '--mode', type=str, choices=['uefi', 'hlos'],
                           help='mode to generate "UEFI" or "HLOS" based license store. DEFAULT: "uefi"',
                           default='uefi')

    return qlsparser.parse_args()


if __name__ == '__main__':
    try:
        main(parseArgs(sys.argv))

    except Exception as err:
        print('Exception Raised. Exiting!')
        print(err)
        sys.exit(1)

    except KeyboardInterrupt:
        print('Keyboard Interrupt Received. Exiting!')
        sys.exit(1)

    sys.exit(0)
