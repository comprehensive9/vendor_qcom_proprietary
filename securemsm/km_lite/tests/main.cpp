/* Copyright (c) 2019-2020, 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>

extern "C" int keymaster_aes_sample(bool multi_thread);
extern "C" int keymaster_rsa_sample(bool multi_thread);
extern "C" int keymaster_import_export_sample(bool multi_thread);
extern "C" int keymaster_delete_sample(bool multi_thread);
extern "C" int keymaster_key_upgrade_sample(void);
extern "C" int keymaster_hmac_sample(bool multi_thread);
extern "C" int keymaster_ecdsa_sample(bool multi_thread);
extern "C" int keymaster_multithreading(void);
#define APP_NAME "keymaster_sample"

void usage(void) {
    printf("Keymaster sample usage.\n");
    printf("=======================\n");
    printf("To run AES encrypt/decrypt sample:\n");
    printf("   " APP_NAME " 1\n");
    printf("To run RSA sign/verify sample:\n");
    printf("   " APP_NAME " 2\n");
    printf("To run import/export sample:\n");
    printf("   " APP_NAME " 3\n");
    printf("To run key delete sample:\n");
    printf("   " APP_NAME " 4\n");
    printf("To run key upgrade sample:\n");
    printf("   " APP_NAME " 5\n");
    printf("To run HMAC sample:\n");
    printf("   " APP_NAME " 6\n");
    printf("To run ECDSA sample:\n");
    printf("   " APP_NAME " 7\n");
    printf("To run keymaster multithreading\n");
    printf("   " APP_NAME " 8\n");
}

int main(int argc, char **argv) {
    int err = 0;

    if (argc != 2) {
        usage();
        return -255;
    }

    switch (argv[1][0]) {
        case '1':
            err = keymaster_aes_sample(false);
            break;
        case '2':
            err = keymaster_rsa_sample(false);
            break;
        case '3':
            err = keymaster_import_export_sample(false);
            break;
        case '4':
            err = keymaster_delete_sample(false);
            break;
        case '5':
            err = keymaster_key_upgrade_sample();
            break;
        case '6':
            err = keymaster_hmac_sample(false);
            break;
        case '7':
            err = keymaster_ecdsa_sample(false);
            break;
        case '8':
            err = keymaster_multithreading();
            break;
        default:
            usage();
            err = -1000;
            break;
    }

    if (err != 0) {
        printf("ERROR: km_lite_sample_client cmd %s\n", argv[1][0]);
        return 255;
    }
    return 0;
}
