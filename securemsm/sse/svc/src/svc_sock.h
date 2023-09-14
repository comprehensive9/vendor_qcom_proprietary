/*
 * Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <hidl/HidlSupport.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Secure UI socket communication library.
 *
 * This library provides socket-based communication between the HLOS entities
 * cooperating for the Secure UI.
 *
 * It has overall 3 families of functions, which are expected to be called
 * in 3 different scenarios.
 * In addition to them, there are a set of common functions:
 * - The control functions,
 *    @see svc_sock_init
 *    @see svc_sock_terminate
 * - Functions for synchronous communication
 *    @see svc_sock_send_notification
 *    @see svc_sock_send_command
 *
 * 1. Functions callable from a fully native entity (e.g. Secure UI Listener)
 * This is the most complete interface, and designed for streamlined interaction
 * with the calling applications.
 * Functions in this family are:
 * - The notification callbacks
 *    @see svc_sock_notification_callback_t,
 *    @see svc_sock_command_callback_t
 * - and the functions to register them
 *    @see svc_sock_register_notification_callback
 *    @see svc_sock_register_command_callback
 *
 * 2. Functions callable from a Java/JNI environment, in which callbacks to
 * the Java world are hard to perform. In this environment callbacks are not
 * used, and the caller is expected to wait on an internally managed message
 * queue.
 * Functions in this family are:
 *    @see svc_sock_wait_for_message
 *    @see svc_sock_send_response
 *
 * 3. A single function to be used by an external entity (neither the Listener
 * nor the Android Service, but e.g. an external application) to request a
 * possibly active Secure UI session to terminate. This entity does not need
 * to allocate a stable socket to receive in turn notifications or commands,
 * so control functions aren't effectively needed here, and this function can
 * be called autonomously, with no previous call to @see svc_sock_init:
 *    @see svc_sock_send_notification_noreg
 *
 * */

typedef struct {
    uint8_t bDisableOverlay;
    uint8_t bToRender;
    uint32_t colorFormat;
    uint32_t u32Height;
    uint32_t u32Width;
    uint32_t u32HPos;
    uint32_t u32VPos;
    uint64_t bufHandle;
} secdisp_ip;

typedef struct {
    int32_t status;
    uint16_t u16NumBuffers;
    uint32_t u32Height;
    uint32_t u32Width;
    uint64_t bufHandle;
    uint32_t u32Stride;
    uint32_t u32PixelWidth;
    android::hardware::hidl_handle fd;
} secdisp_op;

/**
 * @brief Callback for received notifications.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param src_name[in]  Name (sun_path) of the originating socket
 * @param src_len[in]   Name length
 * */
typedef void (*svc_sock_notification_callback_t)(uint8_t const id,
                                                 uint8_t const payload,
                                                 char const *const src_name,
                                                 size_t const src_len);

/**
 * @brief Callback for received commands.
 * If response is requested, the response parameter will be not-NULL and the
 * function is expected to assign a value to it before returning.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param input[in]     input data for the secure display operations
 * @param src_name[in]  Name (sun_path) of the originating socket
 * @param src_len[in]   Name lengt
 * @param response[out] 1 byte response
 *
 * @return 0 on success, negative errno otherwise
 * */
typedef int (*svc_sock_command_callback_t)(uint8_t const id,
                                           uint8_t const payload,
                                           secdisp_ip input,
                                           char const *const src_name,
                                           size_t const src_len);

/**
 * @brief Initialize the socket library.
 * Initialize the local socket, sets the default remote socket address,
 * starts the receiver thread.
 * Names to be used for local and remote sockets must be valid AF_UNIX
 * socket names.
 *
 * @param localname[in]       Name for local socket
 * @param localnamelen[in]    Its length
 * @param remotename[in]      Name for remote socket
 * @param remotenamelen[in]   Its length
 * @param filter[in]          1 to filter out all received messages not
 * originating
 *                            from the specified remote socket
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_init(char const *const localname, size_t const localnamelen,
                  char const *const remotename, size_t const remotenamelen,
                  uint32_t filter);

/**
 * @brief Closes the socket library.
 * Closes the local socket and stops the receiver thread.
 * */
int svc_sock_terminate(char *lname);

/**
 * @brief get the current status of sockets
 * @return false when socket is not initialized
 *         true  when socket is initialized
 * */

bool svc_sock_get_status();

/**
 * @brief Send a notification message.
 * Send a notification message to a remote socket.
 * If no destination name is specified (i.e. NULL), the notification is sent to
 * the default remote address specified during @see svc_sock_init
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param dest_name[in] Name (sun_path) of the destination socket
 * @param dest_len[in]  Name length
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_send_notification(uint8_t const id, uint8_t const payload,
                               char const *const dest_name,
                               size_t const dest_len);

/**
 * @brief Send a notification message from an anonymous temporary socket.
 * Send a notification message to a remote socket from a temporarily created
 * anonymous socket, which is closed before the function terminated.
 * This function is meant to be used without the need to a previous call
 * to @see svc_sock_init
 * The destination name in this case has to be specified.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param dest_name[in] Name (sun_path) of the destination socket
 * @param dest_len[in]  Name length
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_send_notification_noreg(uint8_t const id, uint8_t const payload,
                                     char const *const dest_name,
                                     size_t const dest_len);

/**
 * @brief Send a command message.
 * Send a command message to a remote socket.
 * If no destination name is specified (i.e. NULL), the notification is sent to
 * the default remote address specified during @see svc_sock_init.
 * The response to the command (1 byte) is returned via the response parameter.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param dest_name[in] Name (sun_path) of the destination socket
 * @param dest_len[in]  Name length
 * @param input[in]     input data for the secure display operations
 * @param output[out]   output data returned by the  secure display operations
 * @param response[out] Response
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_send_command(uint8_t const id, uint8_t const payload,
                          char const *const dest_name, size_t const dest_len,
                          secdisp_ip *input, secdisp_op *output,
                          uint8_t *response);

/**
 * @brief Register the notification callback.
 * Register the user-provided function as notification callback.
 * A NULL pointer can be passed to unregister a previously registered callback.
 *
 * @param cback[in]   Callback
 * */
void svc_sock_register_notification_callback(
    svc_sock_notification_callback_t cback);

/**
 * @brief Register the command callback.
 * Register the user-provided function as command callback.
 * A NULL pointer can be passed to unregister a previously registered callback.
 *
 * @param cback[in]   Callback
 * */
void svc_sock_register_command_callback(svc_sock_command_callback_t cback);

/**
 * @brief Wait for a message.
 * Wait for a message to be received.
 * The message (and optionally its originating socket name) are passed out in
 * the
 * output parameters.
 * If the name of the originating socket isn't required, a NULL can be passed.
 *
 * @param mex[out]          4 byte message
 * @param mex_len[in|out]   length of the message buffer
 * @param input[in]     input data for the secure display operations
 * @param src_name[out]     Name (sun_path) of the originating socket
 * @param src_len[out]      Name length
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_wait_for_message(uint8_t *mex, size_t const mex_len,
                              secdisp_ip *input, char *src_name,
                              size_t *src_len);

/**
 * @brief Send the response to a command message.
 * Send the response to a command message to a remote socket.
 * If no destination name is specified (i.e. NULL), the message is sent to
 * the default remote address specified during @see svc_sock_init.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param dest_name[in] Name (sun_path) of the destination socket
 * @param dest_len[in]  Name length
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_send_response(uint8_t const id, uint8_t const payload,
                           char const *const dest_name, size_t const dest_len);

/**
 * @brief Send the response to a secure display command message.
 * Send the response to a command message to a remote socket.
 * If no destination name is specified (i.e. NULL), the message is sent to
 * the default remote address specified during @see svc_sock_init.
 *
 * @param id[in]        1 byte identifier
 * @param payload[in]   1 byte payload
 * @param output[out]   output data returned by the  secure display operations
 * @param dest_name[in] Name (sun_path) of the destination socket
 * @param dest_len[in]  Name length
 *
 * @return 0 on success, negative errno otherwise
 * */
int svc_sock_send_response_getdisp(uint8_t const id, uint8_t const payload,
                                   secdisp_op *output,
                                   char const *const dest_name,
                                   size_t const dest_len);

#ifdef __cplusplus
}
#endif
