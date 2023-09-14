/*===========================================================================
 * Copyright(c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/**
 * All messages exchanged on the socket are 4 bytes long.
 *
 * They consist of 4 independent uint8 fields.
 * - The 1st field carries the message type
 * - The 2nd field carries the message ID.
 * - The 3rd field carries an optional (depending on the message)
 *   additional payload.
 * - The 4th field is reserved.
 *
 * The following types of messages are envisioned:
 * - Command: requires a response
 *      - may use the second payload field
 * - Reponse: to a previously sent command
 *      - shares the same ID as the command
 *      - uses the payload field
 * - Notification: does not require a response
 *      - may use the payload field
  *
 * List of messages:
 * ****************************************************************************
 * - CALL STATUS
 *    Originator: Android Service (Notification), SecureUILib (Command)
 *    Type: Notification or command
 *    Payload (for the notification and for the response):
 *      OK if no call is in progress, ABORT if a call is in progress
 * ****************************************************************************
 * - ABORT
 *    Originator: Android Service
 *    Type: Notification
 *    Payload: None
 * ****************************************************************************
 * - SCREEN STATUS
 *    Originator: Android Service (Notification), SecureUILib (Command)
 *    Type: Notification or command
 *    Payload (for the notification and for the response):
 *      OK if screen is on, ABORT if screen is off
 * ****************************************************************************
 * - SD STARTING
 *    Originator: Listener
 *    Type: Command / Response
 *    Payload:
 *      Command: None
 *      Response: OK to proceed, ABORT otherwise
 * ****************************************************************************
 *  - SD STOPPED
 *    Originator: Listener
 *    Type: Notification
 *    Payload: None
 * ****************************************************************************
 * */

#define BYTES_IN_MEX (4)
#define BYTES_IN_INPUT (32)

#define SUI_MSG_CMD_MSK 0x08 /* Command, requires response */
#define SUI_MSG_RSP_MSK 0x04 /* Response to a command */
#define SUI_MSG_NOT_MSK 0x02 /* Notification */

/* ID for exchanged messages */
/* IDs for communications initiated by the Android Svc */
/* Call notifications*/
#define SUI_MSG_ID_CALL_STATUS 0x01
/* Abort, for unreported reason (e.g. power) */
#define SUI_MSG_ID_ABORT 0x02
/* Screen status notifications */
#define SUI_MSG_ID_SCREEN_STATUS 0x03
/* IDs for communications initiated by the Listener */
/* Secure Display notifications */
#define SUI_MSG_ID_SD_STARTING 0x10
#define SUI_MSG_ID_SD_STOPPED 0x11
#define SUI_MSG_SEC_DISP_PROPERTIES 0x12
#define SUI_MSG_SEC_DISP_ENQUEUE 0x13
#define SUI_MSG_SEC_DISP_DEQUEUE 0x14
#define SUI_MSG_SEC_DISP_START 0x15
#define SUI_MSG_SEC_DISP_STOP 0x16

/* General purpose IDs */
/* Invalid */
#define SUI_MSG_ID_INVALID 0x00

/* Response codes, in the payload */
#define SUI_MSG_RET_OK 0x00
#define SUI_MSG_RET_ABORT 0x01
#define SUI_MSG_RET_NOP 0x0F
