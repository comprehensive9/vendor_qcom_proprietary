/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Dial.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Marshal_write_ulong) {
    Marshal marshal;
    unsigned long ul = 0x8141211180402010;
    size_t current_size = 0;

    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);

    ASSERT_NE(marshal.write(ul), Marshal::Result::FAILURE);
    current_size += sizeof(ul);
    ASSERT_EQ(marshal.dataSize(), current_size);
}

TEST(Marshalling, Marshal_write) {
    Marshal marshal;
    uint8_t  ui8 = 0x10;
    uint16_t ui16 = 0x2010;
    uint32_t ui32 = 0x80402010;
    uint64_t ui64 = 0x8141211180402010;
    char s[] = "01234567890\000\001\002\003\004";
    std::string is( s,sizeof(s));

    int intv = 0xf0f1f2f3;

    size_t curr_size = 0;
    char out[] =
        "10" // ui8
        "2010"       // ui16
        "80402010"   // ui32
        "8141211180402010" //ui32
        "00000011" // string length (including ending null)
        "3031323334353637383930000102030400" // string value
        "f0f1f2f3"   // intv
        ;
    std::string outs(out, sizeof(out));
    std::string expected = QtiRilStringUtils::fromHex(out);

    ASSERT_NE(marshal.write(ui8), Marshal::Result::FAILURE);
    curr_size += sizeof(ui8);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_NE(marshal.write(ui16), Marshal::Result::FAILURE);
    curr_size += sizeof(ui16);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_NE(marshal.write(ui32), Marshal::Result::FAILURE);
    curr_size += sizeof(ui32);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_NE(marshal.write(ui64), Marshal::Result::FAILURE);
    curr_size += sizeof(ui64);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_NE(marshal.write(is), Marshal::Result::FAILURE);
    curr_size += is.size() + sizeof(uint32_t);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_NE(marshal.write(intv), Marshal::Result::FAILURE);
    curr_size += sizeof(intv);
    ASSERT_EQ(marshal.dataSize(), curr_size);

    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, Marshal_read) {
    Marshal marshal;
    uint8_t  ui8  = 0;  uint8_t ui8xp  = 0x10;
    uint16_t ui16 = 0; uint16_t ui16xp = 0x2010;
    uint32_t ui32 = 0; uint32_t ui32xp = 0x80402010;
    uint64_t ui64 = 0; uint64_t ui64xp = 0x8141211180402010;
    char s[] = "01234567890\000\001\002\003\004";
    std::string sout; std::string sxp(s, sizeof(s));
    int intv = 0; int intvxp = 0xf0f1f2f3;
    char *ibf =
        "10"               // ui8
        "2010"             // ui16
        "80402010"         // ui32
        "8141211180402010" // ui64
        "00000011"         // string length (including ending null)
        "3031323334353637383930000102030400" // string value
        "f0f1f2f3"         // intv
        ;
    std::string ibfs(ibf, sizeof(s));
    std::string input_buffer = QtiRilStringUtils::fromHex(ibf);

    ASSERT_EQ(marshal.setData(input_buffer), 0);
    ASSERT_EQ(marshal.dataSize(), input_buffer.size());

    ASSERT_EQ(marshal.read(ui8), Marshal::Result::SUCCESS);
    ASSERT_EQ(ui8, ui8xp);

    ASSERT_EQ(marshal.read(ui16), Marshal::Result::SUCCESS);
    ASSERT_EQ(ui16, ui16xp);

    ASSERT_EQ(marshal.read(ui32), Marshal::Result::SUCCESS);
    ASSERT_EQ(ui32, ui32xp);

    ASSERT_EQ(marshal.read(ui64), Marshal::Result::SUCCESS);
    ASSERT_EQ(ui64, ui64xp);

    ASSERT_EQ(marshal.read(sout), Marshal::Result::SUCCESS);
    ASSERT_EQ(sout, sxp);

    ASSERT_EQ(marshal.read(intv), Marshal::Result::SUCCESS);
    ASSERT_EQ(intv, intvxp);
}

TEST(Marshalling, Marshal_read_write) {
    Marshal marshal;
    uint8_t  ui8  = 0;  uint8_t ui8xp  = 0x10;
    uint16_t ui16 = 0; uint16_t ui16xp = 0x2010;
    uint32_t ui32 = 0; uint32_t ui32xp = 0x80402010;
    uint64_t ui64 = 0; uint64_t ui64xp = 0x8141211180402010;
    char s[] = "01234567890\000\001\002\003\004";
    std::string sout; std::string sxp(s, sizeof(s));
    int intv = 0; int intvxp = 0xf0f1f2f3;
    size_t cur_size = 0;

    ASSERT_EQ(marshal.write(ui8xp), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui8xp);
    ASSERT_EQ(marshal.dataSize(), cur_size);

    ASSERT_EQ(marshal.write(ui16xp), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui16xp);
    ASSERT_EQ(marshal.dataSize(), cur_size);

    ASSERT_EQ(marshal.write(ui32xp), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui32xp);
    ASSERT_EQ(marshal.dataSize(), cur_size);

    ASSERT_EQ(marshal.write(ui64xp), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui64xp);
    ASSERT_EQ(marshal.dataSize(), cur_size);

    ASSERT_EQ(marshal.write(sxp), Marshal::Result::SUCCESS);
    cur_size += sizeof(uint32_t); // write<std::string> writes the size of the string
    cur_size += sxp.size();
    ASSERT_EQ(marshal.dataSize(), cur_size);

    ASSERT_EQ(marshal.write(intvxp), Marshal::Result::SUCCESS);
    cur_size += sizeof(intvxp);
    ASSERT_EQ(marshal.dataSize(), cur_size);

    marshal.setDataPosition(0);
    cur_size = 0;

    ASSERT_EQ(marshal.read(ui8), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui8xp);
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(ui8xp, ui8);

    ASSERT_EQ(marshal.read(ui16), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui16);
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(ui16xp, ui16);

    ASSERT_EQ(marshal.read(ui32), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui32);
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(ui32xp, ui32);

    ASSERT_EQ(marshal.read(ui64), Marshal::Result::SUCCESS);
    cur_size += sizeof(ui64);
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(ui64xp, ui64);

    ASSERT_EQ(marshal.read(sout), Marshal::Result::SUCCESS);
    cur_size += sizeof(uint32_t);
    cur_size += sout.size();
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(sxp, sout);

    ASSERT_EQ(marshal.read(intv), Marshal::Result::SUCCESS);
    cur_size += sizeof(intv);
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(intvxp, intv);

}
TEST(Marshalling, Marshal_read_write_char_star) {
    Marshal marshal;
    static const char constexpr TEST_STR[] = "This is a string to write";
    static const constexpr size_t TEST_STR_SZ = sizeof(TEST_STR);
    char ein[] =
        "\x00\x00\x00\x19"             // length of string
        "This is a string to write"    // String value
        ;
    const char *in = TEST_STR;
    std::string expectedin{ein,sizeof(ein) - 1};
    ASSERT_EQ(marshal.write(in), Marshal::Result::SUCCESS);
    size_t cur_size = sizeof(int32_t) + TEST_STR_SZ - 1;
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(expectedin, (std::string)marshal);

    char *out = nullptr;
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    std::string ins(in), outs(out);
    delete [] out;
    ASSERT_EQ(ins,outs);
}

TEST(Marshalling, Marshal_read_write_char_array) {
    Marshal marshal;
    static const char constexpr TEST_STR[] = "This is a string to write";
    static const constexpr size_t TEST_STR_SZ = sizeof(TEST_STR);
    char ein[] =
        "\x00\x00\x00\x19"             // length of string
        "This is a string to write"    // String value
        ;
    const char (&in)[TEST_STR_SZ] = TEST_STR;
    std::string expectedin{ein,sizeof(ein) - 1};
    ASSERT_EQ(marshal.write(in), Marshal::Result::SUCCESS);
    size_t cur_size = sizeof(int32_t) + TEST_STR_SZ  - 1;
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(expectedin, (std::string)marshal);

    char out[TEST_STR_SZ];
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    std::string ins(in), outs(out);
    ASSERT_EQ(ins,outs);

    char shortout[5];

    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read(shortout), Marshal::Result::SUCCESS);
    outs = std::string{shortout};
    ASSERT_EQ(outs.size(), 4); // There should always be space for the null character
    std::string expected{TEST_STR,4};
    ASSERT_EQ(outs,expected);
}

TEST(Marshalling, Marshal_read_write_binary_string) {
    Marshal marshal;
    static const char constexpr TEST_STR[] =
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
    ;
    static const constexpr size_t TEST_STR_SZ = sizeof(TEST_STR) - 1;
    char ein[] =
        "\x00\x00\x01\x00"             // length of string
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
        ;
    const char *in = TEST_STR;
    std::string expectedin{ein,sizeof(ein) - 1};
    ASSERT_EQ(marshal.write(in,TEST_STR_SZ), Marshal::Result::SUCCESS);
    size_t cur_size = sizeof(int32_t) + TEST_STR_SZ;
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    ASSERT_EQ(expectedin, (std::string)marshal);

    char *out = nullptr;
    size_t outsz = 0;
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.readAndAlloc(out,outsz), Marshal::Result::SUCCESS);
    ASSERT_NE(out, nullptr);
    std::string ins(in), outs(out);
    ASSERT_EQ(ins,outs);
    delete [] out;
}

TEST(Marshalling, Marshal_read_write_pointer_array) {
    Marshal marshal;
    size_t cur_size = 0;
    char *strings[] = {
        "First",
        "Second",
        "Third"
    };
    size_t dim = sizeof(strings)/sizeof(char *);
    char *out[3];

    ASSERT_EQ(marshal.write(strings), Marshal::Result::SUCCESS);
    cur_size += sizeof(int32_t) // length of array
        + sizeof(int32_t) // length of first string
        + sizeof("First") - 1 // first string
        + sizeof(int32_t) // length of second string
        + sizeof("Second") - 1
        + sizeof(int32_t)
        + sizeof("Third") - 1
        ;
    ASSERT_EQ(marshal.dataPosition(), cur_size);
    marshal.setDataPosition(0);
    ASSERT_EQ(marshal.read<char *>(out), Marshal::Result::SUCCESS);
    for(int i = 0; i < dim; i++) {
        size_t len_strings_i = std::char_traits<char>::length(strings[i]);
        size_t len_out_i = std::char_traits<char>::length(out[i]);
        std::string strings_i(strings[i]);
        std::string out_i(out[i]);
        ASSERT_EQ(strings_i, out_i);
    }
}

TEST(Marshalling, Marshal_concatenation) {
    Marshal marshal;
    int ints[] = {
        10,
        20,
        30,
    };
    ASSERT_EQ(marshal.write(ints), Marshal::Result::SUCCESS);
    size_t cur_size = sizeof(int32_t) + sizeof(ints);
    ASSERT_EQ(marshal.dataPosition(), cur_size);

    Marshal marshal2;
    int ints2[] = {
        40,
        50,
        60
    };
    ASSERT_EQ(marshal2.write(ints2), Marshal::Result::SUCCESS);
    size_t cur_size2 = sizeof(int32_t) + sizeof(ints);
    ASSERT_EQ(marshal2.dataPosition(), cur_size2);
    marshal2.setDataPosition(0);

    marshal += marshal2;
    ASSERT_EQ(marshal.dataSize(), cur_size + cur_size2);

    marshal.setDataPosition(0);
    int out = 0;
    int32_t sz = 0;

    ASSERT_EQ(marshal.read(sz), Marshal::Result::SUCCESS);
    ASSERT_EQ(sz, sizeof(ints) / sizeof(ints[0]));
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints[0]);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints[1]);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints[2]);

    ASSERT_EQ(marshal.read(sz), Marshal::Result::SUCCESS);
    ASSERT_EQ(sz, sizeof(ints) / sizeof(ints[0]));
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints2[0]);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints2[1]);
    ASSERT_EQ(marshal.read(out), Marshal::Result::SUCCESS);
    ASSERT_EQ(out, ints2[2]);

}
