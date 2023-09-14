/*===========================================================================
 Copyright (c) 2019, 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "qsee_tui_dialogs.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif
#include "font_manager.h"
#include "layout_manager.h"
#include "layout_manager_internal.h"
#include "TUILog.h"
#include "memscpy.h"
#include "TUIHeap.h"
#include "secure_memset.h"
#include "qsee_tui_layout.h"



#ifdef ENABLE_PROFILING
#undef ENABLE_PROFILING
#define ENABLE_PROFILING 0
#endif
//#define PROFILING

#ifdef PROFILING
#if (ENABLE_PROFILING)
#define TIME_START                   \
    unsigned long long stop;         \
    unsigned long long start;        \
    struct timeval start_time;       \
    gettimeofday(&start_time, NULL); \
    start = start_time.tv_sec

#define TIME_STOP                   \
    struct timeval stop_time;       \
    gettimeofday(&stop_time, NULL); \
    stop = stop_time.tv_sec;        \
    delay = stop - start
#endif
#else
#define TIME_START \
    do {           \
    } while (0)
#define TIME_STOP \
    do {          \
    } while (0)
#endif

#define ENTER                                   \
    TUILOGD("%s+", __func__); \
    TIME_START
#define EXIT                                                         \
    {                                                                \
        unsigned long long delay = 0;                                \
        TIME_STOP;                                                   \
        TUILOGD("%s- (%llu ms)", __func__, delay); \
    }                                                                \
    return;

#define EXITV(x)                                                           \
    {                                                                      \
        unsigned long long delay = 0;                                      \
        TIME_STOP;                                                         \
        TUILOGD("%s- (0x%08X) (%llu ms)", __func__, (x), \
               delay);                                                     \
    }                                                                      \
    return (x);

#define NO_TIMEOUT (-1)
#define MAX_DIALOG_INPUTS (2)
/* TODO: Currently 64KB is supported.
#define MAX_IMAGE_SIZE 400*1024    //400KB */
#define MAX_IMAGE_SIZE 63488 // 62KB

static LayoutPage_t* layout_page = NULL;
static qsee_tui_dialog_t layout_type = TUI_DIALOG_PIN;

static int32_t default_timeout = NO_TIMEOUT;

/* globals to hold the final state and received inputs */
static qsee_tui_dialog_ret_val_t* g_final_status = NULL;
static uint32_t* dialog_input_lengths[MAX_DIALOG_INPUTS];
static uint32_t dialog_input_min_lengths[MAX_DIALOG_INPUTS];
static uint32_t dialog_input_max_lengths[MAX_DIALOG_INPUTS];

/* global to hold the secure indicator */
static qsee_tui_secure_indicator_t secure_indicator = {NULL, 0, 0};

/* global for custom screen logic callback function */
qsee_tui_manage_layout_event_callback_t manage_layout_event_custom = NULL;

/*****************************************************************************************************************/

/**
   @brief
   Internal function for handling the evaluated touch events in LOGIN dialog.
*/
static tuiComposerInputStatus_t manage_layout_event_login(
    layout_mgr_err_t layout_mgr_status, const char* focused,
    qsee_tui_dialog_ret_val_t* final_status)
{
    int32_t retVal;
    uint32_t username_len = 0, password_len = 0;  // in characters
    TUILOGE("DEBUG: Inside manage_layout_event_login");

    /* update the received username length*/

    if (dialog_input_lengths[0] != NULL) {  // if username is shown
        retVal = layout_mgr_get_input_len("input_0", &username_len,
                                          dialog_input_lengths[0]);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: layout_mgr_get_input_len "
                     "returned  %d for username",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }
    }

    /* update the received password length*/
    if (dialog_input_lengths[1] != NULL) {  // if password is shown
        retVal = layout_mgr_get_input_len("input_1", &password_len,
                                          dialog_input_lengths[1]);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: layout_mgr_get_input_len "
                     "returned  %d for password",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }
    }
    TUILOGE("DEBUG: layout_mgr_status - %d", layout_mgr_status);
    switch (layout_mgr_status) {
        case LAYOUT_MGR_ENTER_PRESSED:
            TUILOGD("manage_layout_event_login: validate was pressed");
            *final_status = TUI_OK_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_CANCEL_PRESSED:
            TUILOGD("manage_layout_event_login: cancel was pressed");
            if (dialog_input_lengths[0] != NULL) {  // if username is shown
                *(dialog_input_lengths[0]) = 0;
                retVal = layout_mgr_clear_input_buffer("input_0");
                if (retVal < 0) {
                    TUILOGE("manage_layout_event_login: "
                             "layout_mgr_clear_input_buffer returned  %d for "
                             "username",
                             retVal);
                    *final_status = TUI_LAYOUT_ERROR;
                    return TUI_COMPOSER_INPUT_STATUS_CANCEL;
                }
            }

            if (dialog_input_lengths[1] != NULL) {  // if password is shown
                *(dialog_input_lengths[1]) = 0;
                retVal = layout_mgr_clear_input_buffer("input_1");
                if (retVal < 0) {
                    TUILOGE("manage_layout_event_login: "
                             "layout_mgr_clear_input_buffer returned  %d for "
                             "password",
                             retVal);
                    *final_status = TUI_LAYOUT_ERROR;
                    return TUI_COMPOSER_INPUT_STATUS_CANCEL;
                }
            }

            *final_status = TUI_CANCEL_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_F1_PRESSED:

            retVal = layout_mgr_set_active_keyboard("keyboard_lower");
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_active_keyboard returned  %d for "
                         "keyboard lower case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }

            retVal = layout_mgr_set_button_disabled("ab", 1);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard lower case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("AB", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard upper case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("12", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard numeric",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            break;

        case LAYOUT_MGR_F2_PRESSED:
            retVal = layout_mgr_set_active_keyboard("keyboard_upper");
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_active_keyboard returned  %d for "
                         "keyboard upper case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }

            retVal = layout_mgr_set_button_disabled("ab", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard lower case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("AB", 1);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard upper case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("12", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard numeric",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            break;

        case LAYOUT_MGR_F3_PRESSED:
            TUILOGD("manage_layout_event_login: 12 was pressed moving to numeric");
            retVal = layout_mgr_set_active_keyboard("keyboard_numeric");
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_active_keyboard returned  %d for "
                         "keyboard numeric",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }

            retVal = layout_mgr_set_button_disabled("ab", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard lower case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("AB", 0);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard upper case",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            retVal = layout_mgr_set_button_disabled("12", 1);
            if (retVal < 0) {
                TUILOGE("manage_layout_event_login: "
                         "layout_mgr_set_button_disabled returned  %d for "
                         "keyboard numeric",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                return TUI_COMPOSER_INPUT_STATUS_CANCEL;
            }
            break;
        default:
            break;
    }

    /* set the disabled clear button according to the current focused input */
    if (0 == strcmp(focused, "input_0")) {
        retVal = layout_mgr_set_button_disabled("clear_0", 0);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_button_disabled (0) returned  %d for "
                     "username clear button",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        retVal = layout_mgr_set_button_disabled("clear_1", 1);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_button_disabled (1) returned  %d for "
                     "password clear button",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        /* set the username label to be shown only if a text value exists */
        retVal = layout_mgr_set_show_flag("label_2", (username_len > 0));
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: layout_mgr_set_show_flag "
                     "returned  %d for username upper label",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        retVal = layout_mgr_set_show_flag("label_3", false);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: layout_mgr_set_show_flag "
                     "returned  %d for password upper label",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

    } else if (0 == strcmp(focused, "input_1")) {
        retVal = layout_mgr_set_button_disabled("clear_0", 1);
        if (retVal < 0 && retVal != LAYOUT_MGR_OBJECT_NOT_FOUND) {
            /* it's not mandatory object */
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_button_disabled (1) returned  %d for "
                     "username clear button",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        retVal = layout_mgr_set_button_disabled("clear_1", 0);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_button_disabled (0) returned  %d for "
                     "password clear button",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        retVal = layout_mgr_set_show_flag("label_2", false);
        if (retVal < 0 && retVal != LAYOUT_MGR_OBJECT_NOT_FOUND) {
            TUILOGE("manage_layout_event_login: layout_mgr_set_show_flag "
                     "returned  %d for username upper label",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }

        /* set the password label to be shown only if a text value exists */
        retVal = layout_mgr_set_show_flag("label_3", (password_len > 0));
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: layout_mgr_set_show_flag "
                     "returned  %d for password upper label",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }
    }

    if (username_len > dialog_input_max_lengths[0]) {
        retVal = layout_mgr_truncate_input_string("input_0",
                                                  dialog_input_max_lengths[0]);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_input_len_to_max returned  %d for "
                     "username input",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }
    }

    if (password_len > dialog_input_max_lengths[1]) {
        retVal = layout_mgr_truncate_input_string("input_1",
                                                  dialog_input_max_lengths[1]);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_login: "
                     "layout_mgr_set_input_len_to_max returned  %d for "
                     "password input",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;
        }
    }

    if (username_len < dialog_input_min_lengths[0] ||
        password_len < dialog_input_min_lengths[1]) {
        retVal = layout_mgr_set_button_disabled("validate", 1);
    } else {
        retVal = layout_mgr_set_button_disabled("validate", 0);
    }

    if (retVal < 0) {
        TUILOGE("layout_init_login: layout_mgr_set_button_disabled returned  "
                 "%d for validate button",
                 retVal);
        *final_status = TUI_LAYOUT_ERROR;
        return TUI_COMPOSER_INPUT_STATUS_CANCEL;
    }
    TUILOGE("DEBUG: Returning TUI_COMPOSER_INPUT_STATUS_CONTINUE");

    return TUI_COMPOSER_INPUT_STATUS_CONTINUE;
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for handling the evaluated touch events in GET PIN dialog.
*/
static tuiComposerInputStatus_t manage_layout_event_pin(
    layout_mgr_err_t layout_mgr_status, const char* focused,
    qsee_tui_dialog_ret_val_t* final_status)
{
    int32_t retVal;
    uint32_t pin_len = 0;  // in characters
    ENTER;
    TUILOGD("DEBUG: manage_layout_event_pin");

    /* update the received pin length*/
    retVal =
        layout_mgr_get_input_len("input_0", &pin_len, dialog_input_lengths[0]);
    if (retVal < 0) {
        TUILOGE("manage_layout_event_pin: layout_mgr_get_input_len returned %d",
            retVal);
        *final_status = TUI_LAYOUT_ERROR;
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }
    TUILOGD("DEBUG: layout_mgr_status : %d", layout_mgr_status);
    switch (layout_mgr_status) {
        case LAYOUT_MGR_ENTER_PRESSED:
            TUILOGD("manage_layout_event_pin: validate was pressed");
            *final_status = TUI_OK_PRESSED;
            EXITV(TUI_COMPOSER_INPUT_STATUS_COMPLETE);

        case LAYOUT_MGR_CANCEL_PRESSED:
            TUILOGD("manage_layout_event_pin: cancel was pressed");

            *(dialog_input_lengths[0]) = 0;
            retVal = layout_mgr_clear_input_buffer("input_0");
            if (retVal < 0) {
                TUILOGE("manage_layout_event_pin: "
                         "layout_mgr_clear_input_buffer returned  %d",
                         retVal);
                *final_status = TUI_LAYOUT_ERROR;
                EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
            }

            *final_status = TUI_CANCEL_PRESSED;
            EXITV(TUI_COMPOSER_INPUT_STATUS_COMPLETE);

        default:
            break;
    }

    if (pin_len > dialog_input_max_lengths[0]) {
        retVal = layout_mgr_truncate_input_string("input_0",
                                                  dialog_input_max_lengths[0]);
        if (retVal < 0) {
            TUILOGE("manage_layout_event_pin: layout_mgr_set_input_len_to_max "
                     "returned  %d for pin input",
                     retVal);
            *final_status = TUI_LAYOUT_ERROR;
            EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
        }
    }

    /* sets the validate button enable only if the input length is longer than
     * the minimum */
    if (pin_len < dialog_input_min_lengths[0]) {
        retVal = layout_mgr_set_button_disabled("button_11", 1);
    } else {
        retVal = layout_mgr_set_button_disabled("button_11", 0);
    }
    if (retVal < 0) {
        TUILOGE("manage_layout_event_pin: layout_mgr_set_button_disabled "
                 "returned  %d for pin validate button",
                 retVal);
        *final_status = TUI_LAYOUT_ERROR;
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }
    TUILOGD("DEBUG: Returning TUI_COMPOSER_INPUT_STATUS_CONTINUE");
    EXITV(TUI_COMPOSER_INPUT_STATUS_CONTINUE);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for handling the evaluated touch events in SHOW MESSAGE
   dialog.
*/
static tuiComposerInputStatus_t manage_layout_event_msg(
    layout_mgr_err_t layout_mgr_status, const char* focused,
    qsee_tui_dialog_ret_val_t* final_status)
{
        TUILOGD("DEBUG: manage_layout_event_msg");
     TUILOGD("DEBUG: layout_mgr_status : %d", layout_mgr_status);

    switch (layout_mgr_status) {
        case LAYOUT_MGR_F1_PRESSED:
            TUILOGD("manage_layout_event_msg: left button was pressed");
            *final_status = TUI_LEFT_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_F2_PRESSED:
            TUILOGD("manage_layout_event_msg: right button was pressed");
            *final_status = TUI_RIGHT_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_F3_PRESSED:
            TUILOGD("manage_layout_event_msg: middle button was pressed");
            *final_status = TUI_MIDDLE_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_ENTER_PRESSED:
            TUILOGD("manage_layout_event_msg: ENTER button was pressed");
            *final_status = TUI_OK_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_COMPLETE;

        case LAYOUT_MGR_CANCEL_PRESSED:
            TUILOGD("manage_layout_event_msg: CANCEL button was pressed");
            *final_status = TUI_CANCEL_PRESSED;
            return TUI_COMPOSER_INPUT_STATUS_CANCEL;

        default:
            break;
    }
    TUILOGD("DEBUG: Returning TUI_COMPOSER_INPUT_STATUS_CONTINUE");
    return TUI_COMPOSER_INPUT_STATUS_CONTINUE;
}

/*****************************************************************************************************************/

/**
   @brief
   Callback function for handling the touch/timeout events from the touch
   session.
   First, the layout manager is called to evaluate the event.
   Then, this function calls specific screen function for handling the event,
   according to the dialog type.
*/
tuiComposerInputStatus_t manage_layout_event(tuiComposerInputCommand_t cmd,
                                             tuiComposerTouchInput_t* fingers,
                                             int32_t* timeout)
{
    int32_t retVal = -1;
    layout_mgr_err_t layout_mgr_status, layout_mgr_retval;
    tuiComposerInputStatus_t status = TUI_COMPOSER_INPUT_STATUS_CONTINUE;
    event_t event;
    const char* focused = NULL;
    layout_mgr_touched_obj_t touched_object = {0};
    const screen_surface_t* render_surface = NULL;
    ENTER;

    TUILOGD( "%s: cmd = %d", __func__, cmd);

    if (NULL == g_final_status) {
        TUILOGE("manage_layout_event got NULL g_final_status");
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    if (NULL == timeout) {
        TUILOGE( "manage_layout_event got NULL timeout");
        *g_final_status = TUI_ILLEGAL_INPUT;
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    if (TUI_COMPOSER_INPUT_CMD_DATA != cmd && TUI_COMPOSER_INPUT_CMD_TIMEOUT != cmd) {
        TUILOGE("manage_layout_event: unsupported err number = %d", cmd);
        *g_final_status = TUI_ILLEGAL_INPUT;
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    if (TUI_COMPOSER_INPUT_CMD_TIMEOUT == cmd) {
        event = TIMEOUT_EVENT;
    } else {
        event = TOUCH_EVENT;
    }

    /* call layout manager to evaluate the event */
    layout_mgr_status =
        layout_mgr_eval_event(event, fingers, &touched_object, timeout);
    TUILOGD("%s: %d - DEBUG: manage_layout_event: layout_mgr_status -  %d",
                 __func__, __LINE__, layout_mgr_status);
    if (layout_mgr_status < 0 &&
        !(LAYOUT_MGR_UNEXPECTED_TIMEOUT == layout_mgr_status &&
          TUI_DIALOG_CUSTOM == layout_type)) {
        TUILOGE("manage_layout_event: layout_mgr_eval_event returned  %d",
                 layout_mgr_status);

        if (LAYOUT_MGR_UNEXPECTED_TIMEOUT == layout_mgr_status) {
            *g_final_status = TUI_TIMEOUT_ABORTED;
        } else {
            *g_final_status = TUI_LAYOUT_ERROR;
        }
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    layout_mgr_retval = layout_mgr_get_focus_input(&focused);
    if ((layout_mgr_retval < 0 || NULL == focused) &&
        layout_type != TUI_DIALOG_MESSAGE && layout_type != TUI_DIALOG_CUSTOM) {
        TUILOGE("%s: layout_mgr_get_focused_input returned  %d", __func__,
                 layout_mgr_retval);
        *g_final_status = TUI_LAYOUT_ERROR;
        EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    /* dialog specific status evaluation*/
    TUILOGD( "%s: DEBUG: layout_type -  %d", __func__, layout_type);
    switch (layout_type) {
        case TUI_DIALOG_PASSWORD:
            status = manage_layout_event_login(layout_mgr_status, focused,
                                               g_final_status);
            break;
        case TUI_DIALOG_PIN:
            status = manage_layout_event_pin(layout_mgr_status, focused,
                                             g_final_status);
            break;
        case TUI_DIALOG_MESSAGE:
            status = manage_layout_event_msg(layout_mgr_status, NULL,
                                             g_final_status);
            break;
        case TUI_DIALOG_CUSTOM:
            if (NULL == manage_layout_event_custom) {
                *g_final_status = TUI_LAYOUT_ERROR;
                EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
            }
            status = manage_layout_event_custom(layout_mgr_status,
                                                touched_object, g_final_status);

            break;
        default:
            TUILOGE("manage_layout_event: unsupported layout type = %d",
                     layout_type);
            *g_final_status = TUI_ILLEGAL_INPUT;
            EXITV(TUI_COMPOSER_INPUT_STATUS_CANCEL);
    }

    /* propagate the status in case we should not continue*/
    if (status != TUI_COMPOSER_INPUT_STATUS_CONTINUE) {
        TUILOGD("manage_layout_event: manage layout type %d returned  %d",
                 layout_type, status);
        EXITV(status);
    }

    EXITV(TUI_COMPOSER_INPUT_STATUS_CONTINUE);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for initializing the layout for LOGIN screen using the
   layout manager.
*/
static qsee_tui_dialog_ret_val_t layout_init_login(
    qsee_tui_dialog_pswd_params_t* login_params)
{
    int32_t retVal;
    uint32_t default_username_len, default_password_len;
    uint8_t disable_validate = false;

    if (NULL == login_params) {
        TUILOGE( "layout_init_login got NULL login_params");
        return TUI_ILLEGAL_INPUT;
    }

    if (!login_params->bUserNameInpShow && !login_params->bPasswordInpShow) {
        TUILOGE("layout_init_login both username and password inputs are set "
                 "to be not shown, illegal screen parameters");
        return TUI_ILLEGAL_INPUT;
    }

    if (login_params->bUserNameInpShow) {
        /* sets the buffer in the object */
        retVal = layout_mgr_set_buffer_for_input(
            "input_0", login_params->username.inpValue.pu8Buffer,
            login_params->username.inpValue.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_label_buffer returned  "
                     "%d, for username input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }
        /* sets username default value */
        retVal = layout_mgr_set_text(
            "input_0", login_params->username.defaultValue.pu8Buffer,
            login_params->username.defaultValue.u32Len);
        if (retVal < 0) {
            TUILOGE(
                     "layout_init_login: layout_mgr_set_text returned  %d, for "
                     "username input",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        /* set the username upper label text */
        retVal = layout_mgr_set_text("label_2",
                                     login_params->username.label.pu8Buffer,
                                     login_params->username.label.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_text returned  %d, for "
                     "username upper label",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        /* set the username label to be shown only if a default value exists */
        retVal = layout_mgr_set_show_flag(
            "label_2", (login_params->username.defaultValue.u32Len > 0));
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_show_flag returned  %d "
                     "for username upper label",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        /* sets username default label */
        retVal = layout_mgr_set_text_hint(
            "input_0", login_params->username.label.pu8Buffer,
            login_params->username.label.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_text_hint returned  "
                     "%d, for username label",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        retVal = layout_mgr_get_input_len("input_0", &default_username_len,
            &(login_params->username.defaultValue.u32Len));
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_get_input_len returned  %d "
                     "for username input", retVal);
            return TUI_LAYOUT_ERROR;
        }

        if (default_username_len > login_params->username.u32MaxLen) {
            TUILOGE("layout_init_login: got default username greater than the "
                     "maximum length");
            return TUI_ILLEGAL_INPUT;
        }

        if (default_username_len < login_params->username.u32MinLen) {
            disable_validate = true;
        }

        retVal =
            layout_mgr_set_input_display_mode("input_0", DISP_MODE_VISIBLE);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_input_display_mode "
                     "returned  %d for username input", retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_focus_input("input_0");
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_focus_input returned  "
                     "%d for username input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_button_disabled("clear_0", 0);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_button_disabled "
                     "returned  %d for username clear button",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_button_disabled("clear_1", 1);
        if (retVal < 0) {
            TUILOGE(
                     "layout_init_login: layout_mgr_set_button_disabled "
                     "returned  %d for password clear button",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_show_flag("label_3", false);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_show_flag returned  %d "
                     "for password upper label",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        dialog_input_lengths[0] = login_params->username.pu32InpValueLen;

        if (login_params->username.u32MinLen >
            login_params->username.u32MaxLen) {
            TUILOGE("layout_init_login: minimum username len is greater than "
                     "maximum");
            return TUI_ILLEGAL_INPUT;
        }

        dialog_input_min_lengths[0] = login_params->username.u32MinLen;
        dialog_input_max_lengths[0] = login_params->username.u32MaxLen;

    } else {
        retVal = layout_mgr_set_button_disabled("clear_1", 0);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_button_disabled "
                     "returned  %d for password clear button",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_focus_input("input_1");
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_focus_input returned  "
                     "%d for password input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_show_flag(
            "label_3", (login_params->password.defaultValue.u32Len > 0));
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_show_flag returned  %d "
                     "for password upper label",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        dialog_input_lengths[0] = NULL;
        dialog_input_min_lengths[0] = 0;
        dialog_input_max_lengths[0] = login_params->username.inpValue.u32Len;
    }

    if (login_params->bPasswordInpShow) {
        retVal = layout_mgr_set_buffer_for_input(
            "input_1", login_params->password.inpValue.pu8Buffer,
            login_params->password.inpValue.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_label_buffer returned  "
                     "%d, for password input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        /* sets password default value */
        retVal = layout_mgr_set_text(
            "input_1", login_params->password.defaultValue.pu8Buffer,
            login_params->password.defaultValue.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_text returned  %d, for "
                     "password input",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        /* set the password upper label text */
        retVal = layout_mgr_set_text("label_3",
                                     login_params->password.label.pu8Buffer,
                                     login_params->password.label.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_text returned  %d, for "
                     "password upper label",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        retVal = layout_mgr_set_text_hint(
            "input_1", login_params->password.label.pu8Buffer,
            login_params->password.label.u32Len);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_text_hint returned  "
                     "%d, for password label",
                     retVal);
            return TUI_ILLEGAL_INPUT;
        }

        retVal = layout_mgr_get_input_len(
            "input_1", &default_password_len,
            &(login_params->password.defaultValue.u32Len));
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_get_input_len returned  %d "
                     "for password input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        if (default_password_len > login_params->password.u32MaxLen) {
            TUILOGE("layout_init_login: got default password greater than the "
                     "maximum length");
            return TUI_ILLEGAL_INPUT;
        }

        if (default_password_len < login_params->password.u32MinLen) {
            disable_validate = true;
        }

        if (login_params->inpMode > DISP_MODE_LAST) {
            TUILOGE("layout_init_login: received invalid input mode = %d",
                     login_params->inpMode);
            return TUI_ILLEGAL_INPUT;
        }

        retVal =
            layout_mgr_set_input_display_mode("input_1", login_params->inpMode);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_input_display_mode "
                     "returned  %d for password input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        /* save the pointers of the output parameters, to have there the
         * received username and password lengths */

        dialog_input_lengths[1] = login_params->password.pu32InpValueLen;

        if (login_params->password.u32MinLen >
            login_params->password.u32MaxLen) {
            TUILOGE("layout_init_login: minimum password len is greater than "
                     "maximum");
            return TUI_ILLEGAL_INPUT;
        }

        dialog_input_min_lengths[1] = login_params->password.u32MinLen;
        dialog_input_max_lengths[1] = login_params->password.u32MaxLen;

    } else {
        retVal = layout_mgr_set_show_flag("input_1", false);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_show_flag returned  %d "
                     "for password input",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        retVal = layout_mgr_set_show_flag("clear_1", false);
        if (retVal < 0) {
            TUILOGE("layout_init_login: layout_mgr_set_show_flag returned  %d "
                     "for password clear button",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }

        dialog_input_lengths[1] = NULL;
        dialog_input_min_lengths[1] = 0;
        dialog_input_max_lengths[1] = login_params->password.inpValue.u32Len;
    }

    retVal = layout_mgr_set_button_disabled("validate", disable_validate);
    if (retVal < 0) {
        TUILOGE("layout_init_login: layout_mgr_set_button_disabled returned  "
                 "%d for validate button",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }

    retVal = layout_mgr_set_active_keyboard("keyboard_lower");
    if (retVal < 0) {
        TUILOGE("layout_init_login: layout_mgr_set_active_keyboard returned  "
                 "%d for keyboard lower case",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }

    retVal = layout_mgr_set_button_disabled("ab", 1);
    if (retVal < 0) {
        TUILOGE("layout_init_login: layout_mgr_set_button_disabled returned  "
                 "%d for keyboard lower case",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }
    retVal = layout_mgr_set_button_disabled("AB", 0);
    if (retVal < 0) {
        TUILOGE(
                 "layout_init_login: layout_mgr_set_button_disabled returned  "
                 "%d for keyboard upper case",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }
    retVal = layout_mgr_set_button_disabled("12", 0);
    if (retVal < 0) {
        TUILOGE("layout_init_login: layout_mgr_set_button_disabled returned  "
                 "%d for keyboard numeric",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }

    return TUI_SUCCESS;
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for initializing the layout for GET PIN screen using the
   layout manager.
*/
static qsee_tui_dialog_ret_val_t layout_init_pin(
    qsee_tui_dialog_pin_params_t* pin_params)
{
    int32_t retVal;
    uint32_t default_pin_len;
    ENTER;

    if (NULL == pin_params) {
        TUILOGE( "layout_init_pin got NULL pin_params");
        EXITV(TUI_ILLEGAL_INPUT);
    }

    retVal = layout_mgr_set_buffer_for_input("input_0",
                                             pin_params->pin.inpValue.pu8Buffer,
                                             pin_params->pin.inpValue.u32Len);
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_label_buffer returned  %d",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    /* sets pin default label */
    retVal =
        layout_mgr_set_text_hint("input_0", pin_params->pin.label.pu8Buffer,
                                 pin_params->pin.label.u32Len);
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_text_hint returned  %d, for "
                 "pin label",
                 retVal);
        EXITV(TUI_ILLEGAL_INPUT);
    }

    /* sets pin default value */
    retVal =
        layout_mgr_set_text("input_0", pin_params->pin.defaultValue.pu8Buffer,
                            pin_params->pin.defaultValue.u32Len);
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_text returned  %d", retVal);
        EXITV(TUI_ILLEGAL_INPUT);
    }

    retVal = layout_mgr_get_input_len("input_0", &default_pin_len,
                                      &(pin_params->pin.defaultValue.u32Len));
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_get_input_len returned  %d for "
                 "password input",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    if (default_pin_len > pin_params->pin.u32MaxLen) {
        TUILOGE("layout_init_pin: got default pin greater than the maximum length");
        EXITV(TUI_ILLEGAL_INPUT);
    }

    if (default_pin_len < pin_params->pin.u32MinLen) {
        retVal = layout_mgr_set_button_disabled("button_11", 1);
    } else {
        retVal = layout_mgr_set_button_disabled("button_11", 0);
    }

    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_button_disabled returned  %d "
                 "for pin validate button",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    if (pin_params->inpMode > DISP_MODE_LAST) {
        TUILOGE("layout_init_pin: received invalid input mode = %d",
                 pin_params->inpMode);
        EXITV(TUI_ILLEGAL_INPUT);
    }

    retVal = layout_mgr_set_input_display_mode("input_0", pin_params->inpMode);
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_input_display_mode returned  "
                 "%d for pin input",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    retVal = layout_mgr_set_focus_input("input_0");
    if (retVal < 0) {
        TUILOGE(
                 "layout_init_pin: layout_mgr_set_focus_input returned  %d for "
                 "pin input",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    retVal = layout_mgr_set_button_disabled("correction", 0);
    if (retVal < 0) {
        TUILOGE("layout_init_pin: layout_mgr_set_button_disabled returned  %d "
                 "for pin correction button",
                 retVal);
        EXITV(TUI_LAYOUT_ERROR);
    }

    if (TUI_KEYPAD_RANDOMIZED == pin_params->keyPadMode) {
        retVal = layout_mgr_randomize_keyboard("keyboard_pin", false);
        if (retVal < 0) {
            TUILOGE("layout_init_pin: layout_mgr_randomize_keyboard returned  %d",
                retVal);
            EXITV(TUI_LAYOUT_ERROR);
        }
    }

    if (TUI_KEYPAD_RANDOMIZED_ONCE == pin_params->keyPadMode) {
        retVal = layout_mgr_randomize_keyboard("keyboard_pin", true);
        if (retVal < 0) {
            TUILOGE("layout_init_pin: layout_mgr_randomize_onetime_keyboard "
                     "returned  %d",
                     retVal);
            EXITV(TUI_LAYOUT_ERROR);
        }
    }

    if (pin_params->bHideInputBox) {
        retVal = layout_mgr_set_show_flag("input_0", false);
        if (retVal < 0) {
            TUILOGE("layout_init_pin: layout_mgr_set_show_flag returned  %d",
                     retVal);
            return TUI_LAYOUT_ERROR;
        }
    }

    /* save the pointer of the output parameter, to have there the received pin
     * length*/
    dialog_input_lengths[0] = pin_params->pin.pu32InpValueLen;
    if (pin_params->pin.u32MaxLen < pin_params->pin.u32MinLen) {
        TUILOGE("layout_init_pin: minimum len is greater than maximum");
        EXITV(TUI_ILLEGAL_INPUT);
    }
    dialog_input_min_lengths[0] = pin_params->pin.u32MinLen;
    dialog_input_max_lengths[0] = pin_params->pin.u32MaxLen;

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for initializing the layout for SHOW MESSAGE screen using
   the layout manager.
*/
static qsee_tui_dialog_ret_val_t layout_init_msg_btn(
    char* btnName, qsee_tui_button_params_t btnParams)
{
    int32_t retVal;

    if (NULL == btnName) {
        TUILOGE("layout_init_msg_btn got NULL btnName");
        return TUI_ILLEGAL_INPUT;
    }

    retVal = layout_mgr_set_show_flag(btnName, btnParams.bShowBtn);
    if (LAYOUT_MGR_OBJECT_NOT_FOUND == retVal && !(btnParams.bShowBtn)) {
        return TUI_SUCCESS;
    }
    if (retVal < 0) {
        TUILOGE("layout_init_msg_btn: layout_mgr_set_show_flag returned  %d "
                 "for %s button",
                 retVal, btnName);
        return TUI_LAYOUT_ERROR;
    }

    retVal = layout_mgr_set_text(btnName, btnParams.btnLabel.pu8Buffer,
                                 btnParams.btnLabel.u32Len);
    if (retVal < 0) {
        TUILOGE("layout_init_msg_btn: set_label_with_buffer returned  %d for "
                 "%s button",
                 retVal, btnName);
        return (qsee_tui_dialog_ret_val_t)retVal;
    }

    retVal = layout_mgr_set_button_disabled(btnName, btnParams.bDisableBtn);
    if (retVal < 0) {
        TUILOGE("layout_init_msg_btn: layout_mgr_set_button_disabled returned "
                 " %d for %s button",
                 retVal, btnName);
        return TUI_LAYOUT_ERROR;
    }

    return TUI_SUCCESS;
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for initializing the layout for SHOW MESSAGE screen using
   the layout manager.
*/
static qsee_tui_dialog_ret_val_t layout_init_msg(
    qsee_tui_dialog_msg_params_t* msg_params)
{
    int32_t retVal;

    if (NULL == msg_params) {
        TUILOGE( "layout_init_msg got NULL msg_params");
        return TUI_ILLEGAL_INPUT;
    }

    retVal = layout_mgr_set_text("label_2", msg_params->msg.pu8Buffer,
                                 msg_params->msg.u32Len);
    if (retVal < 0) {
        TUILOGE("layout_init_msg: set_label_with_buffer returned  %d for "
                 "message label",
                 retVal);
        return TUI_LAYOUT_ERROR;
    }

    retVal = layout_init_msg_btn("right", msg_params->rightBtnParams);
    if (retVal < 0) {
        TUILOGE("layout_init_msg: layout_init_msg_btn returned  %d for right "
                 "button",
                 retVal);
        return (qsee_tui_dialog_ret_val_t)retVal;
    }

    retVal = layout_init_msg_btn("left", msg_params->leftBtnParams);
    if (retVal < 0) {
        TUILOGE("layout_init_msg: layout_init_msg_btn returned  %d for left button",
            retVal);
        return (qsee_tui_dialog_ret_val_t)retVal;
    }

    retVal = layout_init_msg_btn("middle", msg_params->middleBtnParams);
    if (retVal < 0) {
        TUILOGE("layout_init_msg: layout_init_msg_btn returned  %d for middle "
                 "button",
                 retVal);
        return (qsee_tui_dialog_ret_val_t)retVal;
    }

    return TUI_SUCCESS;
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for initializing the layout according the dialog type using
   the layout manager.
*/

static qsee_tui_dialog_ret_val_t dialog_layout_init(
    qsee_tui_dialog_params_t* dialogParams)
{
    int32_t retVal;
    uint8_t* indicator = NULL;
    ENTER;

    if (NULL == dialogParams) {
        TUILOGE("dialog_layout_init got NULL dialogParams");
        EXITV(TUI_ILLEGAL_INPUT);
    }

    /* sets the secure indicator image */
    /* If bDisplayGlobalSecureIndicator is true, display the global secure
     * indicator.
     * If the secure indicator doesn't exist or was not retrieved yet, an error
     * is returned.
     * If bDisplayGlobalSecureIndicator is false, the image provided in
     * secureIndicator buffer will be displayed.
     * If no image was provided by the calling app - proceed with no indicator
     * and let the user decide whether to continue or not.
     *
     * Please note: if bDisplayGlobalSecureIndicator is true, the content of
     * secureIndicator buffer
     * will not be checked or displayed in any case. */
    if (dialogParams->dialogCommonParams.secureIndicatorParams
            .bDisplayGlobalSecureIndicator) {
        if ((NULL != secure_indicator.pu8SecureIndicator) &&
            (secure_indicator.bIsValid)) {
            indicator = secure_indicator.pu8SecureIndicator;
        } else {
            TUILOGE("dialog_layout_init: unable to use global secure "
                     "indicator, was not set for this application yet");
            EXITV(TUI_SECURE_INDICATOR_ERROR);
        }
    } else {
        indicator = dialogParams->dialogCommonParams.secureIndicatorParams
                        .secureIndicator.pu8Buffer;
    }

    /* sets the secure indicator */
    if (NULL != indicator) {
        retVal = layout_mgr_set_image("image_0", indicator, LAYOUT_ALIGN_MID,
                                      LAYOUT_ALIGN_CENTER);
        if ((retVal == LAYOUT_MGR_IMAGE_EXCEED_OBJECT) ||
            (retVal == LAYOUT_MGR_INVALID_IMAGE)) {
            TUILOGE("dialog_layout_init: layout_mgr_set_image returned  %d "
                     "for secure indicator image",
                     retVal);
            EXITV(TUI_ILLEGAL_INPUT);
        } else if (retVal < 0) {
            TUILOGE("dialog_layout_init: layout_mgr_set_image returned  %d "
                     "for secure indicator image",
                     retVal);
            EXITV(TUI_LAYOUT_ERROR);
        }
    } else {
        /* no secure indicator image */
        TUILOGD("dialog_layout_init: no secure indicator image");
    }

    /* sets the logo image */
    if (NULL != dialogParams->dialogCommonParams.logo.pu8Buffer) {
        retVal = layout_mgr_set_image(
            "image_1", dialogParams->dialogCommonParams.logo.pu8Buffer,
            LAYOUT_ALIGN_MID, LAYOUT_ALIGN_CENTER);
        if ((retVal == LAYOUT_MGR_IMAGE_EXCEED_OBJECT) ||
            (retVal == LAYOUT_MGR_INVALID_IMAGE)) {
            TUILOGE("dialog_layout_init: layout_mgr_set_image returned  %d "
                     "for logo image",
                     retVal);
            EXITV(TUI_ILLEGAL_INPUT);
        } else if (retVal < 0) {
            TUILOGE("dialog_layout_init: layout_mgr_set_image returned  %d "
                     "for logo image",
                     retVal);
            EXITV(TUI_LAYOUT_ERROR);
        }
    } else {
        /* no logo image */
        TUILOGD( "dialog_layout_init: no logo image");
    }

    /* sets the title */
    retVal = layout_mgr_set_text(
        "label_0", dialogParams->dialogCommonParams.title.pu8Buffer,
        dialogParams->dialogCommonParams.title.u32Len);
    if (retVal < 0) {
        TUILOGE("dialog_layout_init: set_label_with_buffer returned  %d for title",
            retVal);
        EXITV((qsee_tui_dialog_ret_val_t)retVal);
    }

    /* sets the subtitle */
    retVal = layout_mgr_set_text(
        "label_1", dialogParams->dialogCommonParams.description.pu8Buffer,
        dialogParams->dialogCommonParams.description.u32Len);
    if (retVal < 0) {
        TUILOGE("dialog_layout_init: set_label_with_buffer returned  %d for "
                 "description",
                 retVal);
        EXITV((qsee_tui_dialog_ret_val_t)retVal);
    }

    /* dialog specific initializations */
    switch (dialogParams->dialogType) {
        case TUI_DIALOG_PASSWORD:
            retVal = layout_init_login(&(dialogParams->pswdDialogParams));
            break;

        case TUI_DIALOG_PIN:
            retVal = layout_init_pin(&(dialogParams->pinDialogParams));
            break;

        case TUI_DIALOG_MESSAGE:
            retVal = layout_init_msg(&(dialogParams->msgDialogParams));
            break;

        default:
            EXITV(TUI_ILLEGAL_INPUT);
    }

    EXITV((qsee_tui_dialog_ret_val_t)retVal);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for validating that the layout flavor matches the given
   parameters.
*/

static bool validate_layout_flavor(LayoutFlavor_t param_screen_flavor,
                                   LayoutFlavor_t given_layout_flavor)
{
    switch (given_layout_flavor) {
        case LAYOUT_FLAVOR_TYPE_MESSAGE_TRIPLE:
            /* as long as we have enough buttons/inputs for the requested
             * parameters we're good*/
            return (LAYOUT_FLAVOR_TYPE_MESSAGE_TRIPLE == param_screen_flavor ||
                    LAYOUT_FLAVOR_TYPE_MESSAGE_DOUBLE == param_screen_flavor ||
                    LAYOUT_FLAVOR_TYPE_MESSAGE_SINGLE == param_screen_flavor);
        case LAYOUT_FLAVOR_TYPE_MESSAGE_DOUBLE:
            return (LAYOUT_FLAVOR_TYPE_MESSAGE_DOUBLE == param_screen_flavor ||
                    LAYOUT_FLAVOR_TYPE_MESSAGE_SINGLE == param_screen_flavor);
        case LAYOUT_FLAVOR_TYPE_lOGIN_DOUBLE_INPUT:
            return (
                LAYOUT_FLAVOR_TYPE_lOGIN_DOUBLE_INPUT == param_screen_flavor ||
                LAYOUT_FLAVOR_TYPE_lOGIN_SINGLE_INPUT == param_screen_flavor);
        default:
            return (given_layout_flavor == param_screen_flavor);
    }
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for validating that the layout version matches the current
   SecureUI codebase version.
*/

static qsee_tui_dialog_ret_val_t qsee_tui_validate_layout_version(
    LayoutPage_t* layout)
{
    ENTER;

    if (NULL == layout) {
        TUILOGE("qsee_tui_validate_layout_version got NULL layout ");
        EXITV(TUI_ILLEGAL_INPUT);
    }

    TUILOGD("qsee_tui_validate_layout_version: Running SecureUI version %d.%d",
             TUI_MAJOR_VERSION, TUI_MINOR_VERSION);

    if (layout->major_version != TUI_MAJOR_VERSION) {
        TUILOGE("qsee_tui_validate_layout_version: layout version mismatch");
        EXITV(TUI_LAYOUT_ERROR);
    }

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for setting up the initial layout for the dialog screen.
   Note: will setup the touch callback (only if rendering was successful)
*/

static qsee_tui_dialog_ret_val_t dialog_setup(
    qsee_tui_dialog_params_t* dialogParams)
{
    int32_t retVal;
    LayoutFlavor_t screen_flavor = LAYOUT_FLAVOR_TYPE_CUSTOM;
    ENTER;

    if (NULL == dialogParams) {
        TUILOGE("dialog_setup got NULL dialogParams");
        EXITV(TUI_ILLEGAL_INPUT);
    }

    /* sets the default timeout to given parameter */
    default_timeout = dialogParams->dialogCommonParams.n32TimeOut;

    /* sets the global of the layout type */
    layout_type = dialogParams->dialogType;

    /* enable the font cache copy only if heatmap support is requested for this
     * dialog */
    font_mgr_cfg_enable_static_to_dynamic_font_copy(
        dialogParams->dialogCommonParams.fontHeatmapMode ==
        TUI_FONT_HEATMAP_ENABLED);

    /* sets the global of the custom screen callback */
    if (TUI_DIALOG_CUSTOM == layout_type) {
        manage_layout_event_custom =
            dialogParams->customDialogParams.manage_layout_event_custom;
        if (NULL == manage_layout_event_custom ||
            NULL == dialogParams->layout_page) {
            TUILOGE("dialog_setup: got NULL input in custom screen,"
                     " callback = 0x%08X, layout = 0x%08X",
                     manage_layout_event_custom, dialogParams->layout_page);
            EXITV(TUI_ILLEGAL_INPUT);
        }
    }

    /* conclude the screen flavor according to the parameters */
    if (TUI_DIALOG_MESSAGE == layout_type) {
        screen_flavor = LAYOUT_FLAVOR_TYPE_MESSAGE_TRIPLE;

        if (dialogParams->msgDialogParams.leftBtnParams.bShowBtn &&
            dialogParams->msgDialogParams.rightBtnParams.bShowBtn &&
            !(dialogParams->msgDialogParams.middleBtnParams.bShowBtn)) {
            /* only right and left are shown => double button screen */
            screen_flavor = LAYOUT_FLAVOR_TYPE_MESSAGE_DOUBLE;
        }
        if (!(dialogParams->msgDialogParams.leftBtnParams.bShowBtn) &&
            !(dialogParams->msgDialogParams.rightBtnParams.bShowBtn) &&
            dialogParams->msgDialogParams.middleBtnParams.bShowBtn) {
            /* only middle is shown => single button screen */
            screen_flavor = LAYOUT_FLAVOR_TYPE_MESSAGE_SINGLE;
        }
    } else if (TUI_DIALOG_PASSWORD == layout_type) {
        if (dialogParams->pswdDialogParams.bUserNameInpShow) {
            screen_flavor = LAYOUT_FLAVOR_TYPE_lOGIN_DOUBLE_INPUT;
        } else {
            screen_flavor = LAYOUT_FLAVOR_TYPE_lOGIN_SINGLE_INPUT;
        }
    } else if (TUI_DIALOG_PIN == layout_type) {
        screen_flavor = LAYOUT_FLAVOR_TYPE_PIN;
    } else if (TUI_DIALOG_CUSTOM == layout_type) {
        screen_flavor = LAYOUT_FLAVOR_TYPE_CUSTOM;
    } else {
        screen_flavor = LAYOUT_FLAVOR_TYPE_MAX;
    }

    if (NULL == dialogParams->layout_page) {
        /* implicit layout was given, take the default of the required flavor */
        layout_page = create_layout_ex(dialogParams->dialogCommonParams.width,
                                       dialogParams->dialogCommonParams.height,
                                       screen_flavor);
        if (NULL == layout_page) {
            TUILOGE( "dialog_setup: failed create_layout");
            EXITV(TUI_ILLEGAL_INPUT);
        }
    } else {
        /* explicit layout was given, validate the flavor against the dialog
         * parameters */
        if (!validate_layout_flavor(screen_flavor,
                                    dialogParams->layout_page->flavor)) {
            TUILOGE("dialog_setup: mismatching layout flavor and dialog "
                     "parameters");
            EXITV(TUI_ILLEGAL_INPUT);
        }
        layout_page = dialogParams->layout_page;
    }

    retVal = qsee_tui_validate_layout_version(layout_page);
    if (retVal < 0) {
        TUILOGE("dialog_setup: qsee_tui_validate_layout_version returned %d",
                 retVal);
        EXITV(retVal);
    }

    /* sets up the layout manager with the layout */
    retVal = layout_mgr_load_layout(layout_page);
    if (retVal < 0) {
        TUILOGE("dialog_setup: layout_mgr_load_layout returned  %d", retVal);
        EXITV(TUI_ILLEGAL_INPUT);
    }



    retVal = layout_mgr_validate_layout(dialogParams->dialogCommonParams.width,
                                        dialogParams->dialogCommonParams.height);
    if (retVal < 0) {
        TUILOGE("dialog_setup: layout validation failed (%d) for screen "
                 "width:%u, height:%u",
         retVal, dialogParams->dialogCommonParams.width,
                 dialogParams->dialogCommonParams.height);
        EXITV(TUI_LAYOUT_ERROR);
    }

    retVal = layout_mgr_set_default_timeout(default_timeout);
    if (retVal < 0) {
        TUILOGE("dialog_setup: layout_mgr_set_default_timeout returned  %d",
                 retVal);
        EXITV(TUI_ILLEGAL_INPUT);
    }

    /* sets up the layout using the layout manager */
    if (layout_type != TUI_DIALOG_CUSTOM) {
        /* only for GP screens */
        retVal = dialog_layout_init(dialogParams);
        if (retVal < 0) {
            TUILOGE("dialog_setup: dialog_layout_init returned  %d", retVal);
            EXITV((qsee_tui_dialog_ret_val_t)retVal);
        }
    }

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   Internal function for freeing allocated buffers and the touch callback
*/
static void dialog_clean_up()
{
    ENTER;
    layout_mgr_release_bg_image();
    EXIT;
}

/*****************************************************************************************************************/

/**
   @brief
   External function for initializing the secure ui GP screen service.
*/
qsee_tui_dialog_ret_val_t qsee_tui_init()
{
    return TUI_SUCCESS;
}

/*****************************************************************************************************************/

/**
   @brief
   External function for using the 3 GP screens: LOGIN, GET_PIN, and
   SHOW_MESSAGE.
   The given general parameter struct contains the screen type and relevant
   in/output parameters.
   Calling this function only _sets up_ the screen, and the operation will be
   done during the touch session.
   The status parameter outputs the final status of the operation of the GP
   screen,
   with oppose to the return value that reflects the status of setting up the
   screen .
   The user should take the output only after the touch session is finished
   (after UseSecureTouch in HLOS).

*/
qsee_tui_dialog_ret_val_t qsee_tui_dialog(
    qsee_tui_dialog_params_t* dialogParams, qsee_tui_dialog_ret_val_t* status)
{
    qsee_tui_dialog_ret_val_t retVal;

    ENTER;
    if (NULL == dialogParams || NULL == status) {
       TUILOGE("qsee_tui_dialog got NULL input; dialogParams = %d, status = %d",
            dialogParams, status);
        EXITV(TUI_ILLEGAL_INPUT);
    }

    /* first cleanup the previous screen, if there was */
    dialog_clean_up();

    /* save the pointer to output the final status through */
    g_final_status = status;

    /* setup the secure display session and the layout for the dialog screen */
    retVal = dialog_setup(dialogParams);
    if (retVal < 0) {
        TUILOGE("qsee_tui_dialog: dialog_setup returned %d", retVal);
        EXITV(retVal);
    }

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   External function for retrieving the secure indicator struct, which holds a
   pointer to the secure indicator buffer.
   When calling this function the buffer is allocated and the pointer parameter
   is set to point to the struct saved in the service.
   This struct can then be used by the requesting app in order to decapsulate
   the secure indicator into the internal buffer and set its size.
   After a successful decapsulation, the valid field should be set to 1 by the
   requesting app.

 */
qsee_tui_dialog_ret_val_t qsee_tui_get_secure_indicator_buffer(
    qsee_tui_secure_indicator_t** indicator)
{
    ENTER;

    if (NULL == secure_indicator.pu8SecureIndicator) {
        secure_indicator.pu8SecureIndicator =
            (uint8_t*)tui_malloc(MAX_IMAGE_SIZE);

        if (NULL == secure_indicator.pu8SecureIndicator) {
            TUILOGE("qsee_tui_get_secure_indicator_buffer: failed to allocate "
                     "buffer of size %d",
                     MAX_IMAGE_SIZE);
            EXITV(TUI_SECURE_INDICATOR_ERROR);
        }

        secure_indicator.u32IndicatorSize = MAX_IMAGE_SIZE;
        secure_memset(secure_indicator.pu8SecureIndicator, 0,
                      secure_indicator.u32IndicatorSize);
    }

    *indicator = &secure_indicator;

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   External function for releasing the secure indicator buffer and clearing the
   valid and size fields.
   This function should be called when the app no longer needs the secure
   indicator (usually on shut down).

 */
qsee_tui_dialog_ret_val_t qsee_tui_reset_secure_indicator()
{
    ENTER;

    if (NULL != secure_indicator.pu8SecureIndicator) {
        tui_free(secure_indicator.pu8SecureIndicator);
        secure_indicator.pu8SecureIndicator = NULL;
     }
    secure_indicator.u32IndicatorSize = 0;
    secure_indicator.bIsValid = 0;

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/

/**
   @brief
   External function for closing the secure ui GP screen service.
*/
qsee_tui_dialog_ret_val_t qsee_tui_tear_down()
{
    ENTER;

    layout_mgr_release_bg_image();

    /*internal cleanup - has to be called last as it also clears touch
     * callback*/
    dialog_clean_up();

    /* layout manager cleanup */
    free_layout_buffers();

    font_mgr_cleanup();

    EXITV(TUI_SUCCESS);
}

/*****************************************************************************************************************/
