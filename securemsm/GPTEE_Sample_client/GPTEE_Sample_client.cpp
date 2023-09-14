/**
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "GPTEE_Sample_client.h"

static int QTEEC_TEST_initialize_context_test(void)
{
  TEEC_Context context = {};
  TEEC_Result result = 0xFFFFFFFF;

  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS)
  {
    QTEEC_SAMPLE_LOGE("TEEC_InitializeContext failed in test: %s\n", __func__);
    return result;
  }

  QTEEC_SAMPLE_LOGD("TEEC_InitializeContext passed in test: %s\n", __func__);
  TEEC_FinalizeContext(&context);

  return result;
}

static int QTEEC_TEST_open_close_session(void) {
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Result result = 0xFFFFFFFF;
  uint32_t returnOrigin = 0xFFFFFFFF;
  TEEC_UUID uuid = {};

  memscpy(&uuid, sizeof(TEEC_UUID), &gpSample2UUID, sizeof(TEEC_UUID));

  /* [1] Connect to TEE */
  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS)
  {
    return result;
  }

  /* [2] Open a Session with the TEE application. */
  /* No connection data needed for TEEC_LOGIN_USER. */
  /* No payload, and do not want cancellation. */
  result = TEEC_OpenSession(&context, &session, &uuid, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS)
  {
    TEEC_FinalizeContext(&context);
    return result;
  }

  /* [3] Close the Session with the TEE application. */
  TEEC_CloseSession(&session);
  /* [4] Tidy up resources */
  TEEC_FinalizeContext(&context);

  return result;
}

static int QTEEC_TEST_multiple_session(void) {
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session_array[10];
  TEEC_Result result = 0xFFFFFFFF;
  uint32_t returnOrigin = 0xFFFFFFFF;
  int i = 0, j = 0;
  TEEC_UUID uuid = {};

  memscpy(&uuid, sizeof(TEEC_UUID), &gpSample2UUID, sizeof(TEEC_UUID));
  /* [1] Connect to TEE  */
  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS) goto cleanup1;
  for (i = 0; i < 2; i++) {
    /* [2] Open a Session with the TEE application. */
    /* No connection data needed for TEEC_LOGIN_USER. */
    /* No payload, and do not want cancellation. */
    for (j = 0; j < 3; j++) {
      result = TEEC_OpenSession(&context, &session_array[j], &uuid, TEEC_LOGIN_USER, NULL, NULL,
                                &returnOrigin);

      if (result != TEEC_SUCCESS) goto cleanup2;
    }
    for (j = 0; j < 3; j++) {
      /* [3] Close the Session with the TEE application. */
      TEEC_CloseSession(&session_array[j]);
    }
  }
/* [4] Tidy up resources */
cleanup2:
  TEEC_FinalizeContext(&context);
cleanup1:
  return result;
}

static void print_text(char const* const intro_message, void const* text_addr, unsigned int size) {
  QTEEC_SAMPLE_LOGD("%s @ address = %p\n", intro_message, text_addr);
  for (unsigned int i = 0; i < size; i++) {
    QTEEC_SAMPLE_LOGD("%2x ", ((uint8_t const*)text_addr)[i]);
    if ((i & 0xf) == 0xf) QTEEC_SAMPLE_LOGD("\n");
  }
  QTEEC_SAMPLE_LOGD("\n");
}

static int QTEEC_TEST_TEEC_AllocateSharedMemory(void) {
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Operation operation = {};
  TEEC_Result result = 0xFFFFFFFF;
  TEEC_SharedMemory commsSM = {};
  uint32_t returnOrigin = 0xFFFFFFFF;
  TEEC_UUID uuid = {};
  uint32_t command = GP_SAMPLE_BUFFER_MULTIPLY_TEST;
  int cmpBufferRet = -1;
  uint8_t checkBuffer[BUFFERSIZE] = {};

  memscpy(&uuid, sizeof(TEEC_UUID), &gpSample2UUID, sizeof(TEEC_UUID));
  /* [1] Connect to TEE */
  result = TEEC_InitializeContext(NULL, &context);

  if (result != TEEC_SUCCESS) goto cleanup1;

  /* [2] Open session with TEE application */
  /* Open a Session with the TEE application. */
  /* No connection data needed for TEEC_LOGIN_USER. */
  /* No payload, and do not want cancellation. */
  result = TEEC_OpenSession(&context, &session, &uuid, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS) goto cleanup2;

  /* [3] Initialize the Shared Memory buffers */
  /* [a] Communications buffer. */
  commsSM.size = BUFFERSIZE;
  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  /* Use TEE Client API to allocate the underlying memory buffer. */

  result = TEEC_AllocateSharedMemory(&context, &commsSM);
  if (result != TEEC_SUCCESS) goto cleanup3;
  /* Initialize buffer to all 1's */
  memset(commsSM.buffer, 0x1, commsSM.size);
  /* Initialize checkBuffer to all 1's */
  memset(checkBuffer, 0x1, commsSM.size);

  /* [4] Issue commands to operate on allocated and registered buffers  */
  /* [a] Set the parameter types */
  operation.paramTypes = 0;
  operation.paramTypes =
      TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_PARTIAL_INOUT, TEEC_NONE, TEEC_NONE);
  /* [b] Set the multiplier value input param[0]
  and buffer param[1] to operate on */
  operation.params[0].value.a = 42;
  operation.params[1].memref.parent = &commsSM;
  operation.params[1].memref.offset = 0;
  operation.params[1].memref.size = BUFFERSIZE;

  print_text("Initial data buffer", commsSM.buffer, 128);

  /* [c] Issue command to multiply commsSM by
  value set  in first parameter. */
  result = TEEC_InvokeCommand(&session, command, &operation, &returnOrigin);
  if (result != TEEC_SUCCESS) goto cleanup4;

  /* do the multiply locally */
  for (size_t cnt = 0; cnt < commsSM.size; ++cnt) {
    *(checkBuffer + cnt) *= operation.params[0].value.a;
  }
  cmpBufferRet = memcmp(commsSM.buffer, checkBuffer, commsSM.size);

  print_text("Modified data buffer", commsSM.buffer, 128);

/* [5] Tidy up resources */
cleanup4:
  TEEC_ReleaseSharedMemory(&commsSM);
cleanup3:
  TEEC_CloseSession(&session);
cleanup2:
  TEEC_FinalizeContext(&context);
cleanup1:
  return (result || cmpBufferRet);
}

static int QTEEC_TEST_TEEC_RegisterSharedMemory(void) {
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Operation operation = {};
  TEEC_Result result = 0xFFFFFFFF;
  unsigned char test_buffer[BUFFERSIZE] = {};
  TEEC_SharedMemory commsSM = {};
  uint32_t returnOrigin = 0xFFFFFFFF;
  TEEC_UUID uuid = {};
  uint32_t command = GP_SAMPLE_BUFFER_MULTIPLY_TEST ;  // CMD_BASIC_MULT_DATA
  int cmpBufferRet = -1;
  uint8_t checkBuffer[BUFFERSIZE] = {};

  memscpy(&uuid, sizeof(TEEC_UUID), &gpSample2UUID, sizeof(TEEC_UUID));
  /* [1] Connect to TEE */
  result = TEEC_InitializeContext(NULL, &context);

  if (result != TEEC_SUCCESS) goto cleanup1;

  /* [2] Open session with TEE application */
  /* Open a Session with the TEE application. */
  /* No connection data needed for TEEC_LOGIN_USER. */
  /* No payload, and do not want cancellation. */
  result = TEEC_OpenSession(&context, &session, &uuid, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS) goto cleanup2;

  /* [3] Initialize the Shared Memory buffers */
  /* [a] Communications buffer. */
  commsSM.size = BUFFERSIZE;
  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  commsSM.buffer = test_buffer;

  /* Initialize buffer to all 1's */
  memset(commsSM.buffer, 0x1, commsSM.size);
  /* Initialize checkBuffer to all 1's */
  memset(checkBuffer, 0x1, commsSM.size);

  /* Use TEE Client API to register the underlying memory buffer. */
  result = TEEC_RegisterSharedMemory(&context, &commsSM);
  if (result != TEEC_SUCCESS) goto cleanup3;

  /* [4] Issue commands to operate on allocated and registered buffers  */
  /* [a] Set the parameter types */
  operation.paramTypes = 0;
  operation.paramTypes =
      TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_PARTIAL_INOUT, TEEC_NONE, TEEC_NONE);
  /* [b] Set the multiplier value input param[0]
  and buffer param[1] to operate on */
  operation.params[0].value.a = 42;
  operation.params[1].memref.parent = &commsSM;
  operation.params[1].memref.offset = 0;
  operation.params[1].memref.size = BUFFERSIZE;

  print_text("Initial data buffer", test_buffer, 128);

  /* [c] Issue command to multiply commsSM by
  value set  in first parameter. */
  result = TEEC_InvokeCommand(&session, command, &operation, &returnOrigin);
  if (result != TEEC_SUCCESS) goto cleanup4;

  /* do the multiply locally */
  for (size_t cnt = 0; cnt < commsSM.size; ++cnt) {
    *(checkBuffer + cnt) *= operation.params[0].value.a;
  }
  cmpBufferRet = memcmp(test_buffer, checkBuffer, commsSM.size);

  print_text("Modified data buffer", test_buffer, 128);

/* [5] Tidy up resources */
cleanup4:
  TEEC_ReleaseSharedMemory(&commsSM);
cleanup3:
  TEEC_CloseSession(&session);
cleanup2:
  TEEC_FinalizeContext(&context);
cleanup1:
  return (result || cmpBufferRet);
}

static void print_UUID(const TEEC_UUID uuid) {
  QTEEC_SAMPLE_LOGD("%X-%X-%X-%X%X%X%X%X%X%X%X\n", uuid.timeLow, uuid.timeMid, uuid.timeHiAndVersion,
         uuid.clockSeqAndNode[0], uuid.clockSeqAndNode[1], uuid.clockSeqAndNode[2],
         uuid.clockSeqAndNode[3], uuid.clockSeqAndNode[4], uuid.clockSeqAndNode[5],
         uuid.clockSeqAndNode[6], uuid.clockSeqAndNode[7]);
}

static TEEC_Result QTEEC_TEST_Open_Close_Session_Ex(TEEC_UUID uuid)
{
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Result result = 0xFFFFFFFF;
  uint32_t returnOrigin = 0xFFFFFFFF;
  /* [1] Connect to TEE */
  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS) goto cleanup1;

  /* [2] Open a Session with the TEE application. */
  /* No connection data needed for TEEC_LOGIN_USER. */
  /* No payload, and do not want cancellation. */
  result = TEEC_OpenSession(&context, &session, &uuid, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS) goto cleanup2;

  /* [3] Close the Session with the TEE application. */
  TEEC_CloseSession(&session);

/* [4] Tidy up resources */
cleanup2:
  TEEC_FinalizeContext(&context);
cleanup1:
  return result;
}

static int QTEEC_TEST_open_close_session_gpsample_10times(void) {
  TEEC_UUID uuid = {};
  TEEC_Result result = 0xFFFFFFFF;
  memscpy(&uuid, sizeof(TEEC_UUID), &gpSample2UUID, sizeof(TEEC_UUID));
  for (size_t i = 0; i < 10; i++) {
    QTEEC_SAMPLE_LOGD("Test Open/Close Session for gpsample: %lu th: ", i+1);
    print_UUID(uuid);
    result = QTEEC_TEST_Open_Close_Session_Ex(uuid);
    if (result)
    {
      return result;
    }
  }

  return result;
}

static int QTEEC_TEST_load_unknown_UUID(void) {
  TEEC_Result expectedResult = 0xFFFF0000;
  TEEC_Result result = 0;
  for (size_t i = 0; i < sizeof(unknownUUIDs) / sizeof(unknownUUIDs[0]); i++) {
    TEEC_UUID uuid = {};
    memscpy(&uuid, sizeof(TEEC_UUID), &unknownUUIDs[i], sizeof(TEEC_UUID));
    QTEEC_SAMPLE_LOGD("Test Open/Close Session for unknownUUIDs: ");
    print_UUID(uuid);
    result = QTEEC_TEST_Open_Close_Session_Ex(uuid);
    if (result != expectedResult)
    {
      return -1;
    }
  }
  return 0;
}

static void* send_cancel_request(void* arg)
{
  QTEEC_SAMPLE_LOGD("IN CANCEL THREAD\n");
  usleep(100);
  TEEC_RequestCancellation((TEEC_Operation *)arg);
  return 0;
}

static int QTEEC_TEST_invoke_cmd_cancellation_test(void)
{
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Result result = 0xFFFFFFFF;
  uint32_t returnOrigin = 0;
  TEEC_Operation operation = {};
  uint32_t command = GP_SAMPLE_WAIT_TEST;
  pthread_t reqcancel_thread;

  operation.paramTypes =
      TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

  operation.started = 0;

  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS) {
    QTEEC_SAMPLE_LOGE("TEEC_InitializeContext failed in test: %s\n", __func__);
    return result;
  }

  result = TEEC_OpenSession(&context, &session, &gpSample2UUID, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS) {
    QTEEC_SAMPLE_LOGE("TEEC_OpenSession failed in test: %s, retval: 0x%x\n", __func__, result);
    TEEC_FinalizeContext(&context);
    return result;
  }

  /* create a new thread to cancel the command */
  result = pthread_create(&reqcancel_thread,
                        NULL,
                        send_cancel_request,
                        &operation);
  if ( 0 != result ) {
    QTEEC_SAMPLE_LOGE("Error: Creating a pthread in gp_reqcancel_start is failed!\n");
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
    return -1;
  }
  operation.started = 0;

  result = TEEC_InvokeCommand(&session, command, &operation, &returnOrigin);
  if (result != TEEC_ERROR_CANCEL) {
    QTEEC_SAMPLE_LOGE("TEEC_InvokeCommand failed to be canceled in test: %s, retval: 0x%x\n", __func__, result);
  }

  pthread_join(reqcancel_thread, NULL);

  TEEC_CloseSession(&session);
  TEEC_FinalizeContext(&context);
  return result;
}

static int QTEEC_TEST_invoke_cmd_test(GP_SAMPLE_TESTS_IDS command_id)
{
  /* Allocate TEE Client structures on the stack. */
  TEEC_Context context = {};
  TEEC_Session session = {};
  TEEC_Result result = TEEC_ERROR_GENERIC;
  uint32_t returnOrigin = 0;
  TEEC_Operation operation = {};
  uint32_t command = (uint32_t)command_id;

  operation.paramTypes =
      TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

  operation.started = 0;

  result = TEEC_InitializeContext(NULL, &context);
  if (result != TEEC_SUCCESS) {
    QTEEC_SAMPLE_LOGE("TEEC_InitializeContext failed in test: %s\n", __func__);
    return result;
  }

  result = TEEC_OpenSession(&context, &session, &gpSample2UUID, TEEC_LOGIN_USER, NULL, NULL, &returnOrigin);
  if (result != TEEC_SUCCESS) {
    QTEEC_SAMPLE_LOGE("TEEC_OpenSession failed in test: %s, retval: 0x%x\n", __func__, result);
    TEEC_FinalizeContext(&context);
    return result;
  }

  result = TEEC_InvokeCommand(&session, command, &operation, &returnOrigin);
  if (result != TEEC_SUCCESS) {
    QTEEC_SAMPLE_LOGE("TEEC_InvokeCommand failed in test: %s,commad_id: 0x%x, retval: 0x%x\n", __func__,command,result);
  }

  TEEC_CloseSession(&session);
  TEEC_FinalizeContext(&context);
  return result;
}

void usage(void) {
	printf("\n\n");
	printf("The currently implemented tests are:\n");
	printf("\n");
	printf("    Input 1:	TEST CASE NUMBER\n");
	printf("   ------------------------------\n");
	printf("    0 ------->  Dispaly Menu\n");
	printf("    1 ------->  Test Initialize/deinitialize context \n");
	printf("    2 ------->  Test Open/Close Session \n");
	printf("    3 ------->  Test open multiple sessions/close all\n");
	printf("    4 ------->  Test Open/Close Session 10 iterations \n");
	printf("    5 ------->  Test Invoke comamnd with AllocateSharedMemory API\n");
	printf("    6 ------->  Test Invoke comamnd with RegisterSharedMemory API\n");
	printf("    7 ------->  Test request cancellation \n");
	printf("    8 ------->  Test open session for unknown UUIDs \n");
	printf("    9 ------->  Test crypto API \n");
	printf("    10 ------>  Test SFS API \n");
	printf("    11 ------>  Test TA-TA internal API \n");
}

int main(int argc, char **argv) {
  int retval = 0;
  uint32_t passed_tests = 0;
  uint32_t failed_tests = 0;
  if (argc != 2)  {
        usage();
        return -255;
  }

    int test_number = atoi(argv[1]);

    printf("Running test case %d\n", test_number);
    switch (test_number) {
        case 0:
            usage();
            break;
        case 1:
            TEST(QTEEC_TEST_initialize_context_test,TEEC_SUCCESS);
            break;
        case 2:
            TEST(QTEEC_TEST_open_close_session,TEEC_SUCCESS);
            break;
        case 3:
            TEST(QTEEC_TEST_multiple_session,TEEC_SUCCESS);
            break;
        case 4:
            TEST(QTEEC_TEST_open_close_session_gpsample_10times,TEEC_SUCCESS);
            break;
        case 5:
            TEST(QTEEC_TEST_TEEC_AllocateSharedMemory,TEEC_SUCCESS);
            break;
        case 6:
            TEST(QTEEC_TEST_TEEC_RegisterSharedMemory,TEEC_SUCCESS);
            break;
        case 7:
            TEST(QTEEC_TEST_invoke_cmd_cancellation_test,TEEC_ERROR_CANCEL);
            break;
        case 8:
            TEST(QTEEC_TEST_load_unknown_UUID,TEEC_SUCCESS);
            break;
        case 9:
            TEST_CMD(QTEEC_TEST_invoke_cmd_test,GP_SAMPLE_CRYPTO_TEST,TEEC_SUCCESS);
            break;
        case 10:
            TEST_CMD(QTEEC_TEST_invoke_cmd_test,GP_SAMPLE_PERSISTENT_OBJ_BASIC_TEST,TEEC_SUCCESS);
            break;
        case 11:
            TEST_CMD(QTEEC_TEST_invoke_cmd_test,GP_SAMPLE_TA_TA_TEST,TEEC_SUCCESS);
            break;
        default:
            usage();
            retval = -1000;
            break;
    }

    return retval;
}
