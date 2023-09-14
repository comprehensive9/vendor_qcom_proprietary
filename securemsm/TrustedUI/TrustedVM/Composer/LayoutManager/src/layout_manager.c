/********************************************************************
  ---------------------------------------------------------------------
  Copyright (c) 2013-2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ----------------------------------------------------------------------
 *********************************************************************/
/*===========================================================================

  EDIT HISTORY FOR FILE

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/29/17   ac      Fixed kw issues
  08/04/16   sn      Fixed layout manager cleanup issue
  07/07/16   dr      Add orientation support and move to use the display_manager
  05/22/16   sn      Bug fixes: event type of touched object, copying font path
  04/11/16   sn      API refactoring
  ===========================================================================*/

#include "layout_manager.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif
#include <time.h>
#include "LayoutUtils.h"
#include "TUIHeap.h"
#include "TUILog.h"
#include "font_manager.h"
#include "font_manager_common.h"
#include "layout_manager_internal.h"
#include "secure_display_renderer.h"
#include "memscpy.h"
#include "secure_memset.h"
#include "ITuiComposer.h"

#ifdef ENABLE_PROFILING
#undef ENABLE_PROFILING
#define ENABLE_PROFILING 0
#endif

//#define PROFILING

#define SHOW_LAST_CHAR_TIMEOUT (500)  // in millisecond
#define DEL_TIMEOUT (500)             // in millisecond
#define NO_TIMEOUT (-1)
#define NUM_BUFFERS (10000)

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

#define EXITV(x)                                                              \
    {                                                                         \
        unsigned long long delay = 0;                                         \
        TIME_STOP;                                                            \
        int32_t eval_x = (x);                                                 \
        TUILOGD("%s- (0x%08X) (%llu ms)", __func__, eval_x, \
               delay);                                                        \
        return eval_x;                                                        \
    }

#define FIND_OBJECT_OR_EXIT(name, pObject)                                   \
    {                                                                        \
        if (NULL == name) {                                                  \
            TUILOGE("%s: received null pointer parameter", \
                   __func__);                                                \
            EXITV(LAYOUT_MGR_GOT_NULL_INPUT);                                \
        }                                                                    \
        int32_t get_obj_retval = get_object_by_name(name, &pObject);         \
        if (LAYOUT_MGR_SUCCESS != get_obj_retval || NULL == pObject) {       \
            TUILOGE("%s failed to find object named %s",   \
                   __func__, name);                                          \
            EXITV(get_obj_retval);                                           \
        }                                                                    \
    }

#define VALIDATE_LAYOUT_LOADED_OR_EXIT                                      \
    {                                                                       \
        if (NULL == layout) {                                               \
            TUILOGE("%s: no layout is loaded", __func__); \
            EXITV(LAYOUT_MGR_LAYOUT_NOT_LOADED);                            \
        }                                                                   \
    }
/* checks if the the object type supports hint parameter (input) */
#define CHECK_HINT_SUPPORTED_OR_EXIT(obj)                                   \
    {                                                                       \
        if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {                        \
            TUILOGE("%s: object %s has no hint (hint is supported only for " \
                   "input objects)",                                        \
                   __func__, obj->name);                                    \
            EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);                       \
        }                                                                   \
    }
#define CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj)                    \
    {                                                           \
        if (obj->type == LAYOUT_OBJECT_TYPE_IMAGE) {            \
            TUILOGE("%s: object %s is  not of a supported type " \
                   "(label/input/button)",                      \
                   __func__, obj->name);                        \
            EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);           \
        }                                                       \
    }

/* possible operations to be done as a result of timeout event */
typedef enum timeout_operation {
    TIMEOUT_OP_FIRST = 0,
    TIMEOUT_OP_HIDE_LAST = TIMEOUT_OP_FIRST,
    TIMEOUT_OP_DEL, /* long press on the del button */

    TIMEOUT_OP_MAX = 0x7FFFFFFF
} timeout_operation_t;

/* current active finger on the screen */
typedef enum {
    ACTIVE_FINGER_0 = 0,
    ACTIVE_FINGER_1 = 1,
    ACTIVE_FINGER_NONE = 0x7FFFFFFF
} active_finger_t;

/* globals */
static LayoutPage_t* layout = NULL;

static LayoutObject_t* focused_input = NULL;
static LayoutObject_t* pressed_button = NULL;

static LayoutObject_t* timeout_obj = NULL;
static timeout_operation_t timeout_operation = TIMEOUT_OP_MAX;
static int32_t default_timeout = NO_TIMEOUT;
static int32_t curr_timeout = NO_TIMEOUT;
static unsigned long long timeout_start = 0;

/* bunch of buffers to be allocated for object labels */
static uint8_t* layout_buffers[NUM_BUFFERS];
static uint32_t curr_buffer = 0;

static layout_mgr_err_t set_label_buffer(LayoutObject_t* obj, uint8_t* buffer,
                                         uint32_t len, uint8_t is_hint);
static layout_mgr_err_t set_text_font(LayoutObject_t* obj,
                                      const char* font_path,
                                      const uint8_t* text, uint32_t len,
                                      uint8_t is_hint);
static layout_mgr_err_t check_exceeding_text(LayoutObject_t* obj,
                                             const uint8_t* text, uint32_t len,
                                             uint32_t* fitting_len,
                                             uint32_t* char_length,
                                             uint8_t is_hint);
static layout_mgr_err_t get_text(LayoutObject_t* obj, const uint8_t** text,
                                 uint32_t* len, uint32_t* char_length,
                                 uint8_t is_hint);
#ifdef LEVM
#define QSEE_KDF_NOT_SUPPORTED -1
static int qsee_kdf(const void* key, unsigned int key_len, const void* label,
                    unsigned int label_len, const void* context,
                    unsigned int context_len, void* output,
                    unsigned int output_len)
{
    return QSEE_KDF_NOT_SUPPORTED;
}
#endif

/**************************************************************************************/

void free_layout_buffers()
{
    uint32_t i;
    ENTER;
    if (curr_buffer <= NUM_BUFFERS) {
        for (i = 0; i < curr_buffer; i++) {
            tui_free(layout_buffers[i]);
        }
    }
    curr_buffer = 0;
    EXIT;
}

/**
  @brief
  Internal function used to set the current focused input object to the given
  one.
  */
static void set_focus(LayoutObject_t* obj)
{
    ENTER;
    if (NULL == obj) {
        TUILOGE("set_focus got null object");
        EXIT;
    }

    /* remove the graphic focus from the previous focused input object*/
    if (focused_input != NULL) {
        focused_input->input.status = LAYOUT_INPUT_STATUS_UNSELECTED;
        focused_input->dirty = true;
    }

    focused_input = obj;

    /* set the graphic focus to the current focused input object*/
    focused_input->input.status = LAYOUT_INPUT_STATUS_SELECTED;
    focused_input->dirty = true;
    EXIT;
}

/***************************************************************************************/

/**
  @brief
  Validates layout size can fit current display resolution and orientation using
  the TouchUtils api.
  */
layout_mgr_err_t layout_mgr_validate_layout(uint32_t screen_width,
                                            uint32_t screen_height)
{
    uint32_t retVal;
    SecureTouch_InputStatus_t inputStatus;
    LayoutCores_t refs;
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == layout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = verifyLayout(layout, &inputStatus, &refs, screen_width, screen_height);

    if (retVal) {
        TUILOGE("verifyLayout failed with status = %d",
               retVal);
        EXITV(LAYOUT_MGR_BAD_LAYOUT);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/***************************************************************************************/

/**
  @brief
  Internal function for finding an object in the layout, by its name.
  Be noticed that we search also keyboard objects, but only in the currently
  active keyboard.
  */
static layout_mgr_err_t get_object_by_name(const char* name,
                                           LayoutObject_t** obj)
{
    uint32_t i;
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == name || NULL == obj) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    /* first set obj to NULL, so we can check it afterward*/
    *obj = NULL;

    /* goes over the layout objects and compares the names*/
    for (i = 0; i < layout->number; i++) {
        if (!strncmp(layout->objects[i].name, name, OBJ_NAME_MAX_LEN)) {
            *obj = &(layout->objects[i]);
            TUILOGD("%s: object name %s found", __func__,
                   (*obj)->name);
            EXITV(LAYOUT_MGR_SUCCESS);
        }
    }

    if (layout->numberKeyboards > 0 &&
        layout->activeKeyboard < MAX_KEYBOARD_LAYOUTS &&
        layout->activeKeyboard < layout->numberKeyboards) {
        /* goes over the active keyboard and compares the names*/

        for (i = 0; i < layout->keyboards[layout->activeKeyboard].number &&
                    i < MAX_LAYOUT_OBJECTS_IN_KEYBOARD; i++) {
            if (!strncmp(
                    layout->keyboards[layout->activeKeyboard].objects[i].name,
                    name, OBJ_NAME_MAX_LEN)) {
                *obj = &(layout->keyboards[layout->activeKeyboard].objects[i]);
                EXITV(LAYOUT_MGR_SUCCESS);
            }
        }
    }
    EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
}
/***************************************************************************************/

/**
  @brief
  Internal function for finding a keyboard in the layout, by its name.
  */
static layout_mgr_err_t get_keyboard_by_name(const char* name,
                                             uint32_t* keyboard_idx)
{
    uint32_t i;
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == name || NULL == keyboard_idx) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* goes over the layout keyboard and compares the names*/
    for (i = 0; i < layout->numberKeyboards; i++) {
        if (!strncmp(layout->keyboards[i].name, name, MAX_KEYBOARD_NAME_LEN)) {
            *keyboard_idx = i;
            EXITV(LAYOUT_MGR_SUCCESS);
        }
    }

    EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
}

/*****************************************************************************************/

/**
  @brief
  Internal function for finding an object in the layout by its coordinates,
  using the TouchUtils api.
  */
static void get_object_by_coordinates(uint32_t x, uint32_t y,
                                      LayoutObject_t** obj)
{
    ENTER;

    if (NULL == layout || NULL == obj) {
        TUILOGE("get_object_by_coordinates received null pointer parameter; "
               "layout = %p, obj = %d",
               (void*)layout, obj);
        EXIT;
    }

    /* first set obj to NULL, so we can check it afterward*/
    *obj = NULL;

    findObject(x, y, layout, obj);
    EXIT;
}

/*******************************************************************************************/

/**
  @brief
  Internal function for appending text from pressed button data, to some input
  object label.
  */
static layout_mgr_err_t char_append(Text_t* label, LayoutButtonData_t data)
{
    uint64_t long_res;
    uint32_t text_bytes_length;
    int32_t retVal;
    ENTER;

    if (NULL == label) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = get_utf8_len(label->text, label->curr_len, &text_bytes_length, NULL);
    if (retVal < 0) {
        TUILOGE("char_append: get_utf8_len returned %d",
               retVal);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    /* first check the buffer size of the label */
    long_res = (uint64_t)text_bytes_length + data.lowerLength;
    if (label->max_buff_size < long_res) {
        TUILOGE("char_append - added data overflows the buffer; required = "
               "%llu, buffer size = %d",
               long_res, label->max_buff_size);
        EXITV(LAYOUT_MGR_TEXT_EXCEED_BUFFER);
    }

    memscpy(label->text + text_bytes_length,
            label->max_buff_size - text_bytes_length, data.lower.enc_bytes,
            data.lowerLength);
    label->curr_len++;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/******************************************************************************************/

/**
  @brief
  External function for deleting the last character of the focused input object.
  */

layout_mgr_err_t layout_mgr_delete_input_last_character()
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;
    if (NULL == focused_input) {
        EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
    }
    if (focused_input->label.curr_len < 1) {
        /* nothing to be done */
        EXITV(LAYOUT_MGR_SUCCESS);
    }
    focused_input->label.curr_len -= 1;
    focused_input->dirty = true;

    if (focused_input->input.displayMode != DISP_MODE_VISIBLE) {
        focused_input->label.visibility.visible = TEXT_HIDE_ALL;
    }
    EXITV(LAYOUT_MGR_SUCCESS);
}

/********************************************************************************************/

static void set_timeout_event(LayoutObject_t* _timeout_obj,
                              timeout_operation_t _timeout_operation,
                              int32_t* timeout, int32_t time)
{
    ENTER;
    timeout_obj = _timeout_obj;
    timeout_operation = _timeout_operation;
    *timeout = time;
    curr_timeout = time;
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    timeout_start = start_time.tv_sec;
    EXIT;
}

/********************************************************************************************/

/**
  @brief
  Internal function for evaluating timeout operation, triggered by timeout
  event.
  Be noticed that it also can be triggered by touch up event that comes before
  the timeout is over.
  */
static layout_mgr_err_t eval_timeout_event()
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;
    if (NULL == timeout_obj) {
        TUILOGE("%s: No timeout action was set", __func__);
        EXITV(LAYOUT_MGR_GENERAL_ERROR);
    }

    switch (timeout_operation) {
        case TIMEOUT_OP_HIDE_LAST:
            timeout_obj->label.visibility.visible = TEXT_HIDE_ALL;
            timeout_obj->dirty = true;
            break;

        case TIMEOUT_OP_DEL:
            /* catch long press on the del button to clear the input */
            TUILOGD("eval_timeout_event TIMEOUT_OP_DEL");
            if (NULL == focused_input || (0 == focused_input->label.curr_len)) {
                /* no focused input to clear, nothing to do */
                TUILOGD("eval_timeout_event clear button with no focused input "
                       "object or input to clear");
                break;
            }
            focused_input->label.curr_len = 0;
            focused_input->dirty = true;
            break;

        default:
            TUILOGE("eval_timeout_event unsupported timeout operation = %d",
                   timeout_operation);
            EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/*******************************************************************************************/

/**
  @brief
  Internal function for evaluating touch up event on a button.
  */
static layout_mgr_err_t eval_up_touch_event_button(LayoutObject_t* obj,
                                                   int32_t* timeout)
{
    layout_mgr_err_t retVal;
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == obj || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* ignores disabled button */
    if (LAYOUT_BUTTON_STATUS_DISABLED == obj->button.status) {
        EXITV(LAYOUT_MGR_SUCCESS);
    }

    switch (obj->button.type) {
        case LAYOUT_BUTTON_TYPE_DATA:
            if (NULL == focused_input) {
                /* no focused input to get the data, nothing to do */
                TUILOGE("eval_up_touch_event_button data button with no focused "
                       "input");
                break;
            }
            /* appending the text of the data button to the focused input
             * label*/
            retVal = char_append(&(focused_input->label), obj->button.data);
            if (LAYOUT_MGR_SUCCESS == retVal) {
                focused_input->dirty = true;
                if (DISP_MODE_VIS_THEN_HID ==
                    focused_input->input.displayMode) {
                    /* if visible then hidden is required we set a timeout
                     * operation to hide the last char, and now we show it*/
                    focused_input->label.visibility.visible =
                        TEXT_SHOW_ONLY_LAST;
                    set_timeout_event(focused_input, TIMEOUT_OP_HIDE_LAST,
                                      timeout, SHOW_LAST_CHAR_TIMEOUT);
                }
            }
            EXITV(LAYOUT_MGR_SUCCESS);

        case LAYOUT_BUTTON_TYPE_FUNCTION:
            switch (obj->button.function.type) {
                case LAYOUT_BUTTON_FUNCTION_BACKSPACE:
                    retVal = layout_mgr_delete_input_last_character();
                    if (retVal < 0) {
                        TUILOGE("%s: layout_mgr_delete_input_last_character "
                               "returned  %d",
                               __func__, retVal);
                        EXITV(retVal);
                    }
                    break;

                case LAYOUT_BUTTON_FUNCTION_CLEAR:
                    if (NULL == focused_input ||
                        (0 == focused_input->label.curr_len)) {
                        /* no focused input to clear, nothing to do */
                        TUILOGD("eval_up_touch_event_button clear button with "
                               "no focused input object or input to clear");
                        break;
                    }
                    focused_input->label.curr_len = 0;
                    focused_input->dirty = true;
                    break;

                /*  pressing these function buttons is propagated to the logic
                 * layer */
                case LAYOUT_BUTTON_FUNCTION_SHIFT:
                    if (layout->numberKeyboards > 0 &&
                        layout->activeKeyboard < MAX_KEYBOARD_LAYOUTS &&
                        layout->activeKeyboard < layout->numberKeyboards) {
                        layout->activeKeyboard = (layout->activeKeyboard + 1) %
                                                 layout->numberKeyboards;
                        layout->dirtyKeyboard = true;
                    }
                    break;
                case LAYOUT_BUTTON_FUNCTION_ENTER:
                    EXITV(LAYOUT_MGR_ENTER_PRESSED);

                case LAYOUT_BUTTON_FUNCTION_CANCEL:
                    EXITV(LAYOUT_MGR_CANCEL_PRESSED);

                case LAYOUT_BUTTON_FUNCTION_F1:
                    EXITV(LAYOUT_MGR_F1_PRESSED);

                case LAYOUT_BUTTON_FUNCTION_F2:
                    EXITV(LAYOUT_MGR_F2_PRESSED);

                case LAYOUT_BUTTON_FUNCTION_F3:
                    EXITV(LAYOUT_MGR_F3_PRESSED);
                default:
                    TUILOGE("eval_up_touch_event_button invalid function button");
                    break;
            }
            break;

        default:
            TUILOGE("eval_up_touch_event_button invalid button type");
            break;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/********************************************************************************************/

/**
  @brief
  Internal function for evaluating touch up event on an input object.
  */
static layout_mgr_err_t eval_up_touch_event_input(LayoutObject_t* obj,
                                                  int32_t* timeout)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == obj || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    set_focus(obj);

    EXITV(LAYOUT_MGR_SUCCESS);
}

/********************************************************************************************/

/**
  @brief
  Internal function for evaluating touch up event.
  */
static layout_mgr_err_t eval_up_touch_event(LayoutObject_t* obj,
                                            int32_t* timeout)
{
    ENTER;
    layout_mgr_err_t rv = LAYOUT_MGR_SUCCESS;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == obj || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* we first check for waiting timeout operation, before we evaluate the
     * touch up event */
    if (timeout_obj != NULL) {
        TUILOGD("eval_up_touch_event zeroing timeout");
        if (TIMEOUT_OP_HIDE_LAST == timeout_operation) {
            eval_timeout_event();
        }
        set_timeout_event(NULL, TIMEOUT_OP_MAX, timeout, default_timeout);
    }

    switch (obj->type) {
        case LAYOUT_OBJECT_TYPE_BUTTON:
            rv = eval_up_touch_event_button(obj, timeout);
            break;

        case LAYOUT_OBJECT_TYPE_INPUT:
            rv = eval_up_touch_event_input(obj, timeout);
            break;

        default:
            rv = LAYOUT_MGR_SUCCESS;
            break;
    }
    EXITV(rv);
}

/*******************************************************************************************/

/**
  @brief
  Internal function for evaluating touch down event.
  */
static layout_mgr_err_t eval_down_touch_event(LayoutObject_t* obj,
                                              int32_t* timeout)
{
    ENTER;
    VALIDATE_LAYOUT_LOADED_OR_EXIT;
    if (NULL == obj || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    switch (obj->type) {
        case LAYOUT_OBJECT_TYPE_BUTTON:
            /* key blinking handling */
            if (LAYOUT_BUTTON_STATUS_RELEASED == obj->button.status) {
                obj->button.status = LAYOUT_BUTTON_STATUS_PRESSED;
                obj->dirty = true;

                /* save it so the next event will turn it bake to released */
                pressed_button = obj;
            }
            if (LAYOUT_BUTTON_TYPE_FUNCTION == obj->button.type &&
                LAYOUT_BUTTON_FUNCTION_BACKSPACE == obj->button.function.type &&
                LAYOUT_BUTTON_STATUS_DISABLED != obj->button.status) {
                /* set timer for catching long press on the del button */
                set_timeout_event(obj, TIMEOUT_OP_DEL, timeout, DEL_TIMEOUT);
            }

            break;

        default:
            break;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/*****************************************************************************************/

/**
  @brief
  Internal function for evaluating touch move event.
  Currently does the same as down event.
  */
static layout_mgr_err_t eval_move_touch_event(LayoutObject_t* obj,
                                              int32_t* timeout)
{
    layout_mgr_err_t rv;
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == obj || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    rv = eval_down_touch_event(obj, timeout);
    EXITV(rv);
}

/********************************************************************************************/
/**
  @brief
  External function that loads a layout struct and screen properties and checks
  its validity.
  */
layout_mgr_err_t layout_mgr_load_layout(LayoutPage_t* layoutPage)
{
    layout_mgr_err_t retVal;
    ENTER;

    if (NULL == layoutPage) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* initialize the globals */
    layout = layoutPage;
    layout->renderAll = true;
    pressed_button = NULL;
    timeout_obj = NULL;
    /* set the focused input object */
    if (layout->focused_input < layout->number &&
        LAYOUT_OBJECT_TYPE_INPUT ==
            layout->objects[layout->focused_input].type) {
        focused_input = &(layout->objects[layout->focused_input]);
        focused_input->input.status = LAYOUT_INPUT_STATUS_SELECTED;
        // TUILOGE("focused_input->input.status : %d",
        // focused_input->input.status);
    } else {
        focused_input = NULL;
    }
    EXITV(LAYOUT_MGR_SUCCESS);
}

/**************************************************************************************/

/**
  @brief
  External function that updates the layout following a given touch/timeout
  event.
  Outputs the current focused object name, and the required timeout (in
  milliseconds) for the next timeout event;
  timeout 0 means no timeout.
  */
layout_mgr_err_t layout_mgr_eval_event(event_t event, void* eventParams,
                                       layout_mgr_touched_obj_t* touchedObject,
                                       int32_t* timeout)
{
    ENTER;
    tuiComposerTouchInput_t* fingers = NULL;
    tuiComposerTouchData_t active_finger = {0};
    static active_finger_t prev_active_finger = ACTIVE_FINGER_NONE;
    LayoutObject_t* obj = NULL;
    layout_mgr_err_t retVal = LAYOUT_MGR_SUCCESS;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == touchedObject || NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* only timeout event or touch up event zeroing the timeout, for all other
     * we save it the same */
    *timeout = curr_timeout; /* this is only the default value, this value may
                                be changed when evaluating the event */

    touchedObject->objectName = NULL;
    touchedObject->eventType = LAYOUT_MGR_EVENT_NONE;

    if (NULL == timeout_obj) {
        set_timeout_event(NULL, TIMEOUT_OP_MAX, timeout, default_timeout);
    }

    if ((event == TOUCH_EVENT) && eventParams && timeout_obj) {
        fingers = (tuiComposerTouchInput_t *)eventParams;
        if (TLAPI_TOUCH_EVENT_NONE == fingers->finger[0].event &&
            TLAPI_TOUCH_EVENT_NONE == fingers->finger[1].event) {
            struct timeval start_time;
            gettimeofday(&start_time, NULL);
            if (start_time.tv_sec - timeout_start > curr_timeout) {
                TUILOGE("Turning a no key press into a timeout");
                event = TIMEOUT_EVENT;
            }
        }
    }

    switch (event) {
        case TIMEOUT_EVENT:
            /* we first check if there is waiting timeout operation from timeout
             * we asked */
            TUILOGD("%s:%d - DEBUG: TIMEOUT_EVENT",
                       __func__, __LINE__);
            if (timeout_obj != NULL) {
                TUILOGE("layout_mgr_eval_event got TIMEOUT_EVENT");
                retVal = eval_timeout_event();
                set_timeout_event(NULL, TIMEOUT_OP_MAX, timeout,
                                  default_timeout);
            } else {
                /* we didn't ask this timeout */
                TUILOGE( "layout_mgr_eval_event got unexpected timeout event");
                retVal = LAYOUT_MGR_UNEXPECTED_TIMEOUT;
            }
            break;

        case TOUCH_EVENT:
            TUILOGD("%s:%d - DEBUG: TOUCH_EVENT",
                       __func__, __LINE__);
            if (NULL == eventParams) {
                TUILOGE("%s: received null pointer parameter",
                       __func__);
                retVal = LAYOUT_MGR_GOT_NULL_INPUT;
                break;
            }

            fingers = (tuiComposerTouchInput_t *)eventParams;

            /* both fingers are inactive */
            if (TLAPI_TOUCH_EVENT_NONE == fingers->finger[0].event &&
                TLAPI_TOUCH_EVENT_NONE == fingers->finger[1].event) {
                /* invalid event is ignored */
                TUILOGD("layout_mgr_eval_event got invalid touch event");
                break;
            }

            /* only finger 0 is active */
            if (TLAPI_TOUCH_EVENT_NONE != fingers->finger[0].event &&
                TLAPI_TOUCH_EVENT_NONE == fingers->finger[1].event) {
                active_finger = fingers->finger[0];
                prev_active_finger = ACTIVE_FINGER_0;
                TUILOGD("layout_mgr_eval_event active finger is finger 0");
            }

            /* only finger 1 is active */
            if (TLAPI_TOUCH_EVENT_NONE == fingers->finger[0].event &&
                TLAPI_TOUCH_EVENT_NONE != fingers->finger[1].event) {
                active_finger = fingers->finger[1];
                prev_active_finger = ACTIVE_FINGER_1;
                TUILOGD("layout_mgr_eval_event active finger is finger 1");
            }

            /* both fingers are active */
            if (TLAPI_TOUCH_EVENT_NONE != fingers->finger[0].event &&
                TLAPI_TOUCH_EVENT_NONE != fingers->finger[1].event) {
                if (prev_active_finger == ACTIVE_FINGER_NONE) {
                    /* invalid event is ignored */
                    TUILOGD("layout_mgr_eval_event layout_mgr_eval_event got "
                           "invalid touch event");
                    break;
                }

                TUILOGD("layout_mgr_eval_event active finger did not change");
                if (prev_active_finger == ACTIVE_FINGER_0) {
                    active_finger = fingers->finger[0];
                }
                if (prev_active_finger == ACTIVE_FINGER_1) {
                    active_finger = fingers->finger[1];
                }
            }

            /* handling the key blinking. if a button was pressed, we turn it
             * back to released mode */
            if (pressed_button != NULL) {
                TUILOGD("layout_mgr_eval_event releasing button");
                pressed_button->button.status = LAYOUT_BUTTON_STATUS_RELEASED;
                pressed_button->dirty = true;
                pressed_button = NULL;
            }

            /* transform native touch coordinates into current dialog
             * coordinates according to orientation */
            uint32_t screen_x = active_finger.xCoOrdinate;
            uint32_t screen_y = active_finger.yCoOrdinate;

            /* seeks for object in the touch event location */
            get_object_by_coordinates(screen_x, screen_y, &obj);

            if (NULL == obj || !(obj->show)) {
                /* no object was found, event is ignored */
                TUILOGD("layout_mgr_eval_event no object was found at (%d, %d)",
                       screen_x, screen_y);
                break;
            }

            touchedObject->objectName = obj->name;

            /* valid event on an object, if there was a timeout event, and it
             * ticked, process it here first*/
            struct timeval start_time;
            gettimeofday(&start_time, NULL);
            if ((timeout_obj != NULL) &&
                (start_time.tv_sec - timeout_start > curr_timeout)) {
                eval_timeout_event();
                set_timeout_event(NULL, TIMEOUT_OP_MAX, timeout,
                                  default_timeout);
            }

            /* we take only the first finger */

            if (TLAPI_TOUCH_EVENT_UP & active_finger.event) {
                touchedObject->eventType = LAYOUT_MGR_EVENT_UP;
                retVal = eval_up_touch_event(obj, timeout);
                TUILOGD("layout_mgr_eval_event evaluating touch up event at "
                       "(%u,%u) returned %d, timeout = %d",
                       screen_x, screen_y, retVal, *timeout);
                break;
            }

            if (TLAPI_TOUCH_EVENT_DOWN & active_finger.event) {
                touchedObject->eventType = LAYOUT_MGR_EVENT_DOWN;
                retVal = eval_down_touch_event(obj, timeout);
                TUILOGD("layout_mgr_eval_event evaluating touch down event at "
                       "(%u,%u) returned %d, timeout = %d",
                       screen_x, screen_y, retVal, *timeout);
                break;
            }

            if (TLAPI_TOUCH_EVENT_MOVE & active_finger.event) {
                touchedObject->eventType = LAYOUT_MGR_EVENT_MOVE;
                retVal = eval_move_touch_event(obj, timeout);
                TUILOGD("layout_mgr_eval_event evaluating touch move event at "
                       "(%u,%u) returned %d, timeout = %d",
                       screen_x, screen_y, retVal, *timeout);
                break;
            }

            break;

        default:
           TUILOGE("layout_mgr_eval_event got invalid event type");
            retVal = LAYOUT_MGR_SUCCESS;
            break;
    }

    EXITV(retVal);
}

/***************************************************************************************/

/**
  @brief
  External function that wraps a call to the renderer to render the current
  layout to the given secure buffer.
  */
layout_mgr_err_t layout_mgr_render_layout(const screen_surface_t* surface)
{
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;
    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == surface) {
        TUILOGE("%s: received null surface parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = render_layout(layout, surface);
    if (SEC_RENDER_STRING_TOO_BIG == retVal) {
        TUILOGE("render_layout failed, got too long string");
        EXITV(LAYOUT_MGR_TEXT_EXCEED_OBJECT);
    }

    if (retVal == SEC_RENDER_UNCHANGED) {
        TUILOGE("render_layout unchanged");
        EXITV(LAYOUT_MGR_UNCHANGED);
    }

    if (retVal != SEC_RENDER_SUCCESS) {
        TUILOGE("render_layout failed with status = %d",
               retVal);
        EXITV(LAYOUT_MGR_BAD_LAYOUT);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/********************************************************************************************/
layout_mgr_err_t layout_mgr_render_bg_image(const screen_surface_t* surface)
{
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;
    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == surface) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = render_bg_image(layout, surface);

    if (retVal != SEC_RENDER_SUCCESS) {
        TUILOGE("render_bg_image failed with status = %d",
               retVal);
        EXITV(LAYOUT_MGR_BAD_LAYOUT);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

void layout_mgr_release_bg_image()
{
    release_bg_image();
}

/*******************************************************************************************/

/** to do: update doc
  @brief
  External function for setting the buffer of an input object.
  Use the is_hint flag to set the default label .
  Notice that this function doesn't set the text itself (curr_len is zeroed).
  */
layout_mgr_err_t layout_mgr_set_buffer_for_input(const char* objectName,
                                                 uint8_t* buffer, uint32_t len)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (len != 0 && NULL == buffer) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
       TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    EXITV(set_label_buffer(obj, buffer, len, false));
}

/****************************************************************************************/

/**
  @brief
  External function for setting the show flag of some object in the layout.
  */
layout_mgr_err_t layout_mgr_set_show_flag(const char* objectName, uint32_t show)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (show != obj->show) {
        if (show) {
            obj->show = true;
        } else {
            obj->show = false;
        }
        obj->dirty = true;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/****************************************************************************************/

/**
  @brief
  External function for setting the status of some button object in the layout.
  */
layout_mgr_err_t layout_mgr_set_button_disabled(const char* objectName,
                                                uint32_t disabled)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
       TUILOGE("%s: object named %s is not a button object, type = %d",
               __func__, objectName, obj->type);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (disabled && (LAYOUT_BUTTON_STATUS_DISABLED != obj->button.status)) {
        obj->button.status = LAYOUT_BUTTON_STATUS_DISABLED;
        obj->dirty = true;
    } else if (!disabled &&
               (LAYOUT_BUTTON_STATUS_DISABLED == obj->button.status)) {
        obj->button.status = LAYOUT_BUTTON_STATUS_RELEASED;
        obj->dirty = true;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/****************************************************************************************/

/**
  @brief
  External function for setting focused input object in the layout.
  */
layout_mgr_err_t layout_mgr_set_focus_input(const char* objectName)
{
    LayoutObject_t* obj;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    set_focus(obj);

    EXITV(LAYOUT_MGR_SUCCESS);
}

/*******************************************************************************************/

/**
  @brief
  External function for setting the image buffer and the alignment of some image
  object in the layout.
  */
layout_mgr_err_t layout_mgr_set_image(const char* objectName,
                                      const uint8_t* imageBuffer,
                                      VerticalAlignment_t vAlign,
                                      HorizontalAlignment_t hAlign)
{
    LayoutObject_t* obj = NULL;
    sec_render_err_t retVal;
    uint32_t imWidth, imHeight;
    ENTER;

    if (NULL == imageBuffer) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_IMAGE) {
        TUILOGE("%s: object named %s is not of the supported type (image)",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (vAlign > LAYOUT_ALIGN_V_LAST || hAlign > LAYOUT_ALIGN_H_LAST) {
        TUILOGE("layout_mgr_set_image unsupported alignments vAlign = %d, "
               "hAlign = %d",
               vAlign, hAlign);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    retVal = check_render_image_object(*obj, imageBuffer, &imWidth, &imHeight);
    if (retVal < 0) {
        TUILOGE("%s: image (%u x %u) can't be rendered ",
               __func__, imWidth, imHeight);
        if (SEC_RENDER_ERROR_FILL_EXCEED == retVal) {
            EXITV(LAYOUT_MGR_IMAGE_EXCEED_OBJECT);
        } else {
            EXITV(LAYOUT_MGR_INVALID_IMAGE);
        }
    }

    obj->bgImage.image_buffer = (uint8_t*)imageBuffer;
    obj->bgImage.v_align = vAlign;
    obj->bgImage.h_align = hAlign;
    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/*****************************************************************************************/

/**
  @brief
  External function for setting the active keyboard in the layout.
  */
layout_mgr_err_t layout_mgr_set_active_keyboard(const char* keyboardName)
{
    uint32_t idx = MAX_KEYBOARD_LAYOUTS;
    int32_t retVal;
    ENTER;

    if (NULL == keyboardName) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = get_keyboard_by_name(keyboardName, &idx);
    if (retVal != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s failed to find keyboard named %s",
               __func__, keyboardName);
        EXITV(retVal);
    }

    layout->activeKeyboard = idx;
    layout->dirtyKeyboard = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/***********************************************************************************************/

/**
  @brief
  External function for keyboard randomization in the layout.
  */
layout_mgr_err_t layout_mgr_randomize_keyboard(const char* keyboardName,
                                               uint8_t isOneTime)
{
    uint32_t idx = MAX_KEYBOARD_LAYOUTS, j, rand = 0;
    int32_t i = 0;
    Text_t label;
    LayoutButtonData_t data;
    int32_t retVal;
    ENTER;

    if (NULL == keyboardName) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    retVal = get_keyboard_by_name(keyboardName, &idx);
    if (retVal != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s failed to find keyboard named %s",
               __func__, keyboardName);
        EXITV(retVal);
    }
    for (i = layout->keyboards[idx].number - 1; i > 0; i--) {
        if (LAYOUT_BUTTON_TYPE_FUNCTION ==
            layout->keyboards[idx].objects[i].button.type) {
            continue;
        }

        /* Randomize only one-time per device */
        if (isOneTime) {
            const char* label_str =
                "Layout Manager Randomize Keyboad pin Label";
            uint32_t new_index = 0;
            int ret;

            ret = qsee_kdf(NULL, 0, (const void*)label_str, strlen(label_str),
                           (const void*)(&i), sizeof(i), &new_index,
                           sizeof(new_index));
            if (ret != 0) {
                TUILOGE("layout_mgr_randomize_keyboard not supported: %d", ret);
                EXITV(LAYOUT_MGR_FAILED_GEN_RAND);
            }

            j = new_index % (i + 1);
        } else { /* Randomize each time anew */
            //TODO: Might need to use new API
            srandom(time(NULL));
            uint32_t rand = random();
            j = rand % (i + 1);
        }

        if (LAYOUT_BUTTON_TYPE_FUNCTION ==
            layout->keyboards[idx].objects[j].button.type) {
            continue;
        }

        /*replacing the label*/
        label = layout->keyboards[idx].objects[i].label;
        layout->keyboards[idx].objects[i].label =
            layout->keyboards[idx].objects[j].label;
        layout->keyboards[idx].objects[j].label = label;

        /*replacing the data*/
        data = layout->keyboards[idx].objects[i].button.data;
        layout->keyboards[idx].objects[i].button.data =
            layout->keyboards[idx].objects[j].button.data;
        layout->keyboards[idx].objects[j].button.data = data;
    }
    layout->dirtyKeyboard = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/************************************************************************************************/

/**
  @brief
  External function for getting the length of the text label of some input
  object in the layout.
  The length is returned in bytes and also in characters.
  */
layout_mgr_err_t layout_mgr_get_input_len(const char* objectName,
                                          uint32_t* chars_len,
                                          uint32_t* bytes_len)
{
    const uint8_t* text = NULL;
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    EXITV(get_text(obj, &text, bytes_len, chars_len, false));
}

/**********************************************************************************************/

/**
  @brief
  External function for setting the length of the text label of some input
  object in the layout,
  to be lower or equal to the given maximum length.
  The maximum length is in characters.
  */
layout_mgr_err_t layout_mgr_truncate_input_string(const char* objectName,
                                                  uint32_t max_chars_len)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (obj->label.curr_len > max_chars_len) {
        obj->label.curr_len = max_chars_len;
        obj->dirty = true;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/**********************************************************************************************/
static layout_mgr_err_t clear_input_buffer(LayoutObject_t* obj)
{
    ENTER;

    if (NULL == obj) {
        TUILOGE("%s received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (obj->label.text != NULL) {
        secure_memset(obj->label.text, 0, obj->label.max_buff_size);
        obj->label.curr_len = 0;
        obj->dirty = true;
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************************/
/**
  @brief
  External function for setting the buffer of an input object to zero.
  */
layout_mgr_err_t layout_mgr_clear_input_buffer(const char* objectName)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(clear_input_buffer(obj));
}

/******************************************************************************************/

layout_mgr_err_t layout_mgr_get_buffer_of_input(const char* objectName,
                                                const uint8_t** buffer,
                                                uint32_t* len)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == buffer || NULL == len) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (LAYOUT_OBJECT_TYPE_INPUT != obj->type) {
        TUILOGE("%s: supports only input type", __func__);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *buffer = obj->label.text;
    *len = obj->label.max_buff_size;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*********************************************************************************************/

layout_mgr_err_t layout_mgr_set_input_hidden_char(const char* objectName,
                                                  utf8_char_t hidden_char)
{
    LayoutObject_t* obj = NULL;
    int32_t retVal;
    uint32_t len, fitting_len, char_length, hidden_char_len;
    utf8_char_t original_hidden_char = {0};
    TextVisiblityOptions_t original_visiblity = 0;
    uint8_t* text_to_check;
    uint32_t text_len_to_check;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (LAYOUT_OBJECT_TYPE_INPUT != obj->type) {
        TUILOGE("%s: supports only input type", __func__);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    retVal = get_utf8_len((uint8_t*)&(hidden_char.enc_bytes[0]), 1,
                          &hidden_char_len, NULL);
    if (retVal < 0) {
        TUILOGE("%s: get_utf8_len returned %d", __func__,
               retVal);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    retVal = get_utf8_len(obj->label.text, obj->label.curr_len, &len, NULL);
    if (retVal < 0) {
        TUILOGE("%s: get_utf8_len returned %d", __func__,
               retVal);
        EXITV(LAYOUT_MGR_GENERAL_ERROR);
    }

    original_hidden_char = obj->label.visibility.hiddenChar;
    obj->label.visibility.hiddenChar = hidden_char;

    if (0 == len) {
        text_to_check = (uint8_t*)&(hidden_char.enc_bytes[0]);
        text_len_to_check = hidden_char_len;
    } else {
        text_to_check = obj->label.text;
        text_len_to_check = len;
    }

    retVal = check_exceeding_text(obj, text_to_check, text_len_to_check,
                                  &fitting_len, &char_length, false);
    if (retVal < 0) {
        TUILOGE("%s: exceeding check returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        obj->label.visibility.hiddenChar = original_hidden_char;
        EXITV(retVal);
    }

    original_visiblity = obj->label.visibility.visible;
    obj->label.visibility.visible = TEXT_HIDE_ALL;
    retVal = check_exceeding_text(obj, obj->label.text, len, &fitting_len,
                                  &char_length, false);
    obj->label.visibility.visible = original_visiblity;
    if (retVal < 0) {
        TUILOGE("%s: exceeding check returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        obj->label.visibility.hiddenChar = original_hidden_char;
        EXITV(retVal);
    }

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/****************************************************************************************/

layout_mgr_err_t layout_mgr_get_input_hidden_char(const char* objectName,
                                                  utf8_char_t* hidden_char)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == hidden_char) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (LAYOUT_OBJECT_TYPE_INPUT != obj->type) {
        TUILOGE("%s: supports only input type", __func__);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *hidden_char = obj->label.visibility.hiddenChar;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*****************************************************************************************/
/**
  @brief
  External function for setting the display mode of some input object in the
  layout.
  */
layout_mgr_err_t layout_mgr_set_input_display_mode(
    const char* objectName, LayoutInputDisplayMode_t mode)
{
    int32_t retVal;
    uint32_t fitting_len = 0, char_length = 0, len = 0;
    LayoutObject_t* obj = NULL;
    LayoutInputDisplayMode_t original_mode;
    TextVisiblityOptions_t original_visiblity;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (mode > DISP_MODE_LAST) {
      TUILOGE("layout_mgr_set_input_display_mode: unsupported mode (=%d)",
               mode);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    retVal = get_utf8_len(obj->label.text, obj->label.curr_len, &len, NULL);
    if (retVal != SEC_RENDER_SUCCESS) {
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    original_mode = obj->input.displayMode;
    obj->input.displayMode = mode;

    original_visiblity = obj->label.visibility.visible;
    if (DISP_MODE_VISIBLE == obj->input.displayMode) {
        obj->label.visibility.visible = TEXT_SHOW_ALL;
    } else {
        obj->label.visibility.visible = TEXT_HIDE_ALL;
    }

    retVal = check_exceeding_text(obj, obj->label.text, len, &fitting_len,
                                  &char_length, false);
    if (retVal < 0) {
       TUILOGE("%s: exceeding check returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        obj->input.displayMode = original_mode;
        obj->label.visibility.visible = original_visiblity;
        EXITV(retVal);
    }

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/***************************************************************************************/

layout_mgr_err_t layout_mgr_get_input_display_mode(
    const char* objectName, LayoutInputDisplayMode_t* mode)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == mode) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *mode = obj->input.displayMode;
    EXITV(LAYOUT_MGR_SUCCESS);
}

/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_input_status(const char* objectName,
                                             LayoutInputStatus_t* input_status)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == input_status) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("%s: object named %s is not of the supported type (input)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *input_status = obj->input.status;
    EXITV(LAYOUT_MGR_SUCCESS);
}

/***********************************************************************************/
static layout_mgr_err_t check_exceeding_text(LayoutObject_t* obj,
                                             const uint8_t* text, uint32_t len,
                                             uint32_t* fitting_len,
                                             uint32_t* char_length,
                                             uint8_t is_hint)
{
    sec_render_err_t retval;
    layout_mgr_err_t check_result = LAYOUT_MGR_GENERAL_ERROR;
    uint32_t missing_char_code = 0;
    Text_t* label;
    ENTER;

    if (NULL == obj || (NULL == text && len != 0) || NULL == fitting_len ||
        NULL == char_length) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);

    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    *fitting_len = 0;
    *char_length = 0;

    if (0 == len) {
        EXITV(LAYOUT_MGR_SUCCESS);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    /* if there is dynamic font we should load it with the examined text */
    if (label->fontPath != NULL &&
        (obj->type != LAYOUT_OBJECT_TYPE_INPUT || is_hint)) {
        layout_mgr_err_t font_retval;
        font_retval = set_text_font(obj, label->fontPath, text, len, is_hint);
        if (font_retval != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: setting font for test returned  %d, for object named "
                   "%s, buffer length = %d",
                   __func__, font_retval, obj->name, len);
            EXITV(LAYOUT_MGR_DYNAMIC_FONT_ERROR);
        }
    }

    retval = check_render_text_object(*obj, text, len, is_hint, fitting_len,
                                      &missing_char_code, char_length);
    TUILOGD("%s: fitting_len = %u, len = %u", __func__,
           *fitting_len, len);

    if (missing_char_code != 0) {
        EXITV(LAYOUT_MGR_FONT_MISSING_CHAR);
    }

    switch (retval) {
        case SEC_RENDER_SUCCESS:
            check_result = LAYOUT_MGR_SUCCESS;
            break;
        case SEC_RENDER_STRING_TOO_BIG:
            check_result = LAYOUT_MGR_TEXT_EXCEED_OBJECT;
            break;
        case SEC_RENDER_UNSUPPORTED:
            check_result = LAYOUT_MGR_UNSUPPORTED;
            break;
        case SEC_RENDER_BAD_STRING:
            check_result = LAYOUT_MGR_INVALID_UTF8_STR;
            break;
        default:
            check_result = LAYOUT_MGR_GENERAL_ERROR;
    }

    EXITV(check_result);
}

/**********************************************************************************/

layout_mgr_err_t layout_mgr_check_exceeding_text(const char* objectName,
                                                 const uint8_t* text,
                                                 uint32_t len,
                                                 uint32_t* fitting_len,
                                                 uint32_t* char_length)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(
        check_exceeding_text(obj, text, len, fitting_len, char_length, false));
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_check_exceeding_text_hint(const char* objectName,
                                                      const uint8_t* text,
                                                      uint32_t len,
                                                      uint32_t* fitting_len,
                                                      uint32_t* char_length)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    CHECK_HINT_SUPPORTED_OR_EXIT(obj);

    EXITV(check_exceeding_text(obj, text, len, fitting_len, char_length, true));
}
/**********************************************************************************/
/**
  @brief
  Internal function for setting the text of a specific label.
  The given length is in bytes.
  */

static layout_mgr_err_t copy_text_to_label(Text_t* label, const uint8_t* text,
                                           uint32_t len)
{
    uint32_t text_chars_length = 0, i, curr_char_len;
    int32_t retVal;
    ENTER;

    if (NULL == label || NULL == text) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    if (len > label->max_buff_size) {
        TUILOGE("%s: text length = %u exceeds max buffer = %u", __func__, len,
               label->max_buff_size);
        EXITV(LAYOUT_MGR_TEXT_EXCEED_BUFFER);
    }

    for (i = 0; i < len;) {
        retVal = get_utf8_len(text + i, 1, &curr_char_len, NULL);
        if (retVal < 0) {
            TUILOGE("%s: get_utf8_len returned %d", __func__,
                   retVal);
            EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
        }
        i += curr_char_len;
        text_chars_length++;
    }

    memscpy(label->text, label->max_buff_size, text, len);
    label->curr_len = text_chars_length;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/*********************************************************************************/

/**
  @brief
  External function for setting the text of the label of some object in the
  layout.
  Use the is_hint flag to set the default label of an input object.
  The given length is in bytes.
  */
static layout_mgr_err_t set_label_text(LayoutObject_t* obj, const uint8_t* text,
                                       uint32_t len, uint8_t is_hint)
{
    layout_mgr_err_t retVal;
    Text_t* label = NULL;

    ENTER;

    if (NULL == obj) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    if (0 == len) {
        /* nothing to be done */
        TUILOGD("%s: got zero length text", __func__);
        EXITV(LAYOUT_MGR_SUCCESS);
    }

    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    retVal = copy_text_to_label(label, text, len);
    if (LAYOUT_MGR_SUCCESS == retVal) {
        obj->dirty = true;
    }

    EXITV(retVal);
}

/**********************************************************************************/

/**
  @brief
  Internal function for setting the buffer of some label.
  */
static layout_mgr_err_t set_label_buffer(LayoutObject_t* obj, uint8_t* buffer,
                                         uint32_t len, uint8_t is_hint)
{
    Text_t* label;
    ENTER;
    if (NULL == obj || (NULL == buffer && len != 0)) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    label->text = buffer;
    label->max_buff_size = len;
    label->curr_len = 0;

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/***********************************************************************************/

/*@brief
  Internal function that allocates a buffer and saves its address for cleanup
  */
static layout_mgr_err_t alloc_label_buffer(uint32_t buffer_len,
                                           uint8_t** buffer)
{
    ENTER;

    if (NULL == buffer) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    *buffer = NULL;

    if (0 == buffer_len) {
        EXITV(LAYOUT_MGR_SUCCESS);
    }

    /* allocates the buffer */
    if (curr_buffer < NUM_BUFFERS) {
        layout_buffers[curr_buffer] = (uint8_t*)tui_malloc(buffer_len);
        if (NULL == layout_buffers[curr_buffer]) {
            TUILOGE("%s: tui_malloc failed for curr_buffer =%u", __func__,
                   curr_buffer);
            EXITV(LAYOUT_MGR_MEM_ALLOC_ERROR);
        }
        *buffer = layout_buffers[curr_buffer];
        curr_buffer++;
    } else {
        TUILOGE("%s: run out of buffers for curr_buffer =%u",
               __func__, curr_buffer);
        EXITV(LAYOUT_MGR_MEM_ALLOC_ERROR);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}
/************************************************************************************/

/*@brief
  Internal function that allocates and sets a buffer to an object label
  */
static layout_mgr_err_t alloc_set_label_buffer(LayoutObject_t* obj,
                                               uint32_t buffer_len,
                                               uint8_t is_hint)
{
    ENTER;
    int32_t retVal;
    uint8_t* buffer = NULL;

    if (NULL == obj) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    retVal = alloc_label_buffer(buffer_len, &buffer);
    if (retVal < 0) {
        TUILOGE("%s: alloc_label_buffer with len = %u failed for obj = %s",
               __func__, buffer_len, obj->name);
        EXITV(retVal);
    }

    /* sets the buffer in the object */
    EXITV(set_label_buffer(obj, buffer, buffer_len, is_hint));
}

/*********************************************************************************/
static layout_mgr_err_t set_text(LayoutObject_t* obj, const uint8_t* buffer,
                                 uint32_t len, uint8_t is_hint)
{
    int32_t retVal;
    uint32_t fitting_len, char_length;
    ENTER;

    if (NULL == obj || (NULL == buffer && len != 0)) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    /* another way to clear the input buffer is to set zero length text */
    if (!is_hint && LAYOUT_OBJECT_TYPE_INPUT == obj->type && 0 == len) {
        EXITV(clear_input_buffer(obj));
    }

    retVal = check_exceeding_text(obj, buffer, len, &fitting_len, &char_length,
                                  is_hint);
    if (retVal < 0) {
        TUILOGE("%s: exceeding check returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        EXITV(retVal);
    }

    /* allocates and sets the buffer */
    if (obj->type != LAYOUT_OBJECT_TYPE_INPUT || is_hint) {
        retVal = alloc_set_label_buffer(obj, len, is_hint);
        if (retVal < 0) {
            TUILOGE("%s: alloc_set_label_buffer returned  %d, for object named "
                   "%s, buffer length = %u",
                   __func__, retVal, obj->name, len);
            EXITV(retVal);
        }
    }

    /* sets the text */
    retVal = set_label_text(obj, buffer, len, is_hint);
    if (retVal < 0) {
        TUILOGE("%s: layout_mgr_set_label_text returned  %d, for object named %s",
            __func__, retVal, obj->name);
        EXITV(retVal);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}

/***********************************************************************************/
layout_mgr_err_t layout_mgr_set_text(const char* objectName,
                                     const uint8_t* buffer, uint32_t len)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text(obj, buffer, len, false));
}

layout_mgr_err_t layout_mgr_set_text_hint(const char* objectName,
                                          const uint8_t* buffer, uint32_t len)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text(obj, buffer, len, true));
}

/***********************************************************************************/

static layout_mgr_err_t get_text(LayoutObject_t* obj, const uint8_t** text,
                                 uint32_t* len, uint32_t* char_length,
                                 uint8_t is_hint)
{
    sec_render_err_t retVal;
    Text_t* label = NULL;
    ENTER;

    if (NULL == obj || NULL == text || NULL == len || NULL == char_length) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    *text = label->text;
    *char_length = label->curr_len;

    retVal = get_utf8_len(*text, *char_length, len, NULL);
    if (retVal < 0) {
        TUILOGE("%s: get_utf8_len returned %d", __func__,
               retVal);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*******************************************************************************/

layout_mgr_err_t layout_mgr_get_text(const char* objectName,
                                     const uint8_t** text, uint32_t* len,
                                     uint32_t* char_length)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(get_text(obj, text, len, char_length, false));
}
/*********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_hint(const char* objectName,
                                          const uint8_t** text, uint32_t* len,
                                          uint32_t* char_length)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(get_text(obj, text, len, char_length, true));
}

/**********************************************************************************/
/**
  @brief
  Internal function used to create a font if needed and set it to an object.
  */
static layout_mgr_err_t set_text_font(LayoutObject_t* obj,
                                      const char* font_path,
                                      const uint8_t* text, uint32_t len,
                                      uint8_t is_hint)
{
    Text_t* label = NULL;
    font_mgr_err_t retVal;
    uint32_t char_length;
    gdFontExt* helper_font = NULL;
    const char* helper_text = " ";
    uint8_t empty_text = false;
    int32_t ret_val;
    uint8_t* path_buffer = NULL;

    ENTER;

    /* text is allowed to be null as long as the length is zero as well */
    if (NULL == obj || (NULL == text && len != 0) || NULL == font_path) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* hint option is supported only in input objects */
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    /* dynamic font is not supported for input objects (except for the hint
     * label) */
    if (LAYOUT_OBJECT_TYPE_INPUT == obj->type && !is_hint) {
        TUILOGE("%s: does not support input type", __func__);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    /* if there is no text, we check that the font is valid with helper text */
    if (NULL == text || 0 == len) {
        text = (uint8_t*)helper_text;
        len = strlen(helper_text);
        empty_text = true;
    }

    char_length = gdGetUTF8StringLen(text, len);
    /* at this stage there should not be zero length text */
    if (0 == char_length) {
        TUILOGE("%s: gdGetUTF8StringLen returned 0",
               __func__);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    /* if the given font path matches the object original font path, we may use
     * the static font to save time */
    if (label->fontPath != NULL && (0 == strcmp(font_path, label->fontPath))) {
        helper_font = (gdFontExt*)(label->font);
    }

    retVal = font_mgr_prepare_gdfont(font_path, text, char_length, &helper_font);
    if (retVal != FONT_MGR_SUCCESS &&
        !(retVal == FONT_MGR_NO_FONT_ENTRY_FOR_CHAR && empty_text)) {
        TUILOGE("%s: font_manager_create_gdfont returned %d",
               __func__, retVal);
        if (retVal == FONT_MGR_NO_FONT_ENTRY_FOR_CHAR) {
            EXITV(LAYOUT_MGR_FONT_MISSING_CHAR);
        }
        EXITV(LAYOUT_MGR_DYNAMIC_FONT_ERROR);
    }

    label->font = helper_font;

    ret_val = alloc_label_buffer(MAX_FILENAME_LEN, &path_buffer);
    if (ret_val < 0 || NULL == path_buffer) {
        TUILOGE("%s: alloc_label_buffer with len = %u failed for obj = %s",
               __func__, MAX_FILENAME_LEN, obj->name);
        EXITV(ret_val);
    }
    memscpy(path_buffer, MAX_FILENAME_LEN, font_path, strlen(font_path) + 1);
    label->fontPath = (char*)path_buffer;

    EXITV(LAYOUT_MGR_SUCCESS);
}

/************************************************************************************/

layout_mgr_err_t set_text_font_generic(LayoutObject_t* obj,
                                       const char* font_path, uint8_t is_hint)
{
    int32_t retVal;
    uint32_t len = 0, fitting_len, char_length;
    Text_t original_label;
    Text_t* label = NULL;
    ENTER;

    if (NULL == obj || NULL == font_path) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    original_label = *label;

    retVal = get_utf8_len(label->text, label->curr_len, &len, NULL);
    if (retVal < 0) {
        TUILOGE("%s: get_utf8_len returned %d", __func__,
               retVal);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    retVal = set_text_font(obj, font_path, label->text, len, is_hint);
    if (retVal != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s: set_text_font returned %d", __func__,
               retVal);
        EXITV(retVal);
    }

    retVal = check_exceeding_text(obj, label->text, len, &fitting_len,
                                  &char_length, is_hint);
    if (retVal < 0) {
        TUILOGE("%s: exceeding test returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        *label = original_label;
        EXITV(retVal);
    }

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*****************************************************************************/

layout_mgr_err_t layout_mgr_set_text_font(const char* objectName,
                                          const char* font_path)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_font_generic(obj, font_path, false));
}

/*******************************************************************************/

layout_mgr_err_t layout_mgr_set_text_font_hint(const char* objectName,
                                               const char* font_path)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_font_generic(obj, font_path, true));
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_font(const char* objectName,
                                          const char** font_path)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == font_path) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    *font_path = obj->label.fontPath;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_font_hint(const char* objectName,
                                               const char** font_path)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == font_path) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    CHECK_HINT_SUPPORTED_OR_EXIT(obj);

    *font_path = obj->input.default_label.fontPath;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/***********************************************************************************/

static layout_mgr_err_t set_text_color(LayoutObject_t* obj, Color_t color,
                                       uint8_t is_hint)
{
    ENTER;

    if (NULL == obj) {
        TUILOGE("%s: failed to find  object named %s",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
    }
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (color.format != COLOR_FORMAT_RGBA) {
        TUILOGE("%s: color format not supported %d", __func__,
               color.format);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    if (is_hint) {
        obj->input.default_label.color = color;
    } else {
        obj->label.color = color;
    }

    if (LAYOUT_OBJECT_TYPE_BUTTON == obj->type &&
        obj->button.status < (LAYOUT_BUTTON_STATUS_LAST + 1)) {
        obj->button.stateColor[obj->button.status].fgColor = color;
    }

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_set_text_color(const char* objectName,
                                           Color_t color)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_color(obj, color, false));
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_set_text_color_hint(const char* objectName,
                                                Color_t color)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_color(obj, color, true));
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_color(const char* objectName,
                                           Color_t* color)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == color) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    *color = obj->label.color;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_color_hint(const char* objectName,
                                                Color_t* color)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == color) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    CHECK_HINT_SUPPORTED_OR_EXIT(obj);

    *color = obj->input.default_label.color;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

static layout_mgr_err_t set_text_alignment(LayoutObject_t* obj,
                                           VerticalAlignment_t v_align,
                                           HorizontalAlignment_t h_align,
                                           uint8_t is_hint)
{
    ENTER;

    if (NULL == obj) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (v_align > LAYOUT_ALIGN_V_LAST || h_align > LAYOUT_ALIGN_H_LAST) {
        TUILOGE("%s: alignment values are not supported %d , %d", __func__,
               v_align, h_align);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    if (is_hint) {
        obj->input.default_label.h_align = h_align;
        obj->input.default_label.v_align = v_align;
    } else {
        obj->label.h_align = h_align;
        obj->label.v_align = v_align;
    }
    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_set_text_alignment(const char* objectName,
                                               VerticalAlignment_t v_align,
                                               HorizontalAlignment_t h_align)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_alignment(obj, v_align, h_align, false));
}
/*********************************************************************************/

layout_mgr_err_t layout_mgr_set_text_alignment_hint(
    const char* objectName, VerticalAlignment_t v_align,
    HorizontalAlignment_t h_align)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_alignment(obj, v_align, h_align, true));
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_alignment(const char* objectName,
                                               VerticalAlignment_t* v_align,
                                               HorizontalAlignment_t* h_align)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == v_align || NULL == h_align) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    *h_align = obj->label.h_align;
    *v_align = obj->label.v_align;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/***********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_alignment_hint(
    const char* objectName, VerticalAlignment_t* v_align,
    HorizontalAlignment_t* h_align)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == v_align || NULL == h_align) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    CHECK_HINT_SUPPORTED_OR_EXIT(obj);

    *h_align = obj->input.default_label.h_align;
    *v_align = obj->input.default_label.v_align;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/***************************************************************************************/

static layout_mgr_err_t set_text_line_breaking(LayoutObject_t* obj,
                                               uint32_t allow_line_breaking,
                                               uint8_t is_hint)
{
    int32_t retVal;
    uint32_t fitting_len = 0, char_length = 0, len = 0;
    uint32_t original_allow_line_breaking;
    Text_t* label = NULL;
    ENTER;

    if (NULL == obj) {
        TUILOGE("%s: failed to find  object named %s",
               __func__, obj->name);
        EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
    }
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    if (is_hint) {
        CHECK_HINT_SUPPORTED_OR_EXIT(obj);
    }

    if (is_hint) {
        label = &(obj->input.default_label);
    } else {
        label = &(obj->label);
    }

    if (allow_line_breaking == label->bAllowLineBreaking) {
        EXITV(LAYOUT_MGR_SUCCESS);
    }

    original_allow_line_breaking = label->bAllowLineBreaking;

    retVal = get_utf8_len(label->text, label->curr_len, &len, NULL);
    if (retVal < 0) {
        TUILOGE("%s: get_utf8_len returned %d", __func__,
               retVal);
        EXITV(LAYOUT_MGR_INVALID_UTF8_STR);
    }

    label->bAllowLineBreaking = allow_line_breaking;

    retVal = check_exceeding_text(obj, label->text, len, &fitting_len,
                                  &char_length, is_hint);
    if (retVal < 0) {
        TUILOGE("%s: exceeding check returned  %d, for object named %s, "
               "fitting_len = %u, char_length = %u",
               __func__, retVal, obj->name, fitting_len, char_length);
        label->bAllowLineBreaking = original_allow_line_breaking;
        EXITV(retVal);
    }

    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/************************************************************************************/

layout_mgr_err_t layout_mgr_set_text_line_breaking(const char* objectName,
                                                   uint32_t allow_line_breaking)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_line_breaking(obj, allow_line_breaking, false));
}

/**********************************************************************************/

layout_mgr_err_t layout_mgr_set_text_line_breaking_hint(
    const char* objectName, uint32_t allow_line_breaking)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    EXITV(set_text_line_breaking(obj, allow_line_breaking, true));
}

/***********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_line_breaking(
    const char* objectName, uint32_t* allow_line_breaking)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == allow_line_breaking) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    *allow_line_breaking = obj->label.bAllowLineBreaking;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*********************************************************************************/

layout_mgr_err_t layout_mgr_get_text_line_breaking_hint(
    const char* objectName, uint32_t* allow_line_breaking)
{
    LayoutObject_t* obj = NULL;

    ENTER;

    if (NULL == allow_line_breaking) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);
    CHECK_IF_TEXT_SUPPORTED_OR_EXIT(obj);
    CHECK_HINT_SUPPORTED_OR_EXIT(obj);

    *allow_line_breaking = obj->input.default_label.bAllowLineBreaking;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_layout_flavor(LayoutFlavor_t* flavor)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == flavor) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    *flavor = layout->flavor;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_get_layout_size(uint32_t* layout_height,
                                            uint32_t* layout_width)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == layout_height || NULL == layout_width) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    *layout_width = layout->layoutBg.rect.x2 - layout->layoutBg.rect.x1 + 1;
    *layout_height = layout->layoutBg.rect.y2 - layout->layoutBg.rect.y1 + 1;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_active_keyboard(const char** keyboardName)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == keyboardName) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    if ((layout->activeKeyboard >= layout->numberKeyboards) ||
        (layout->numberKeyboards > MAX_KEYBOARD_LAYOUTS)) {
        *keyboardName = NULL;
        EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
    }

    *keyboardName = layout->keyboards[layout->activeKeyboard].name;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_focus_input(const char** focused_input_name)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == focused_input_name) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    /* returns the current focused input object*/
    if (NULL == focused_input) {
        *focused_input_name = NULL;
        EXITV(LAYOUT_MGR_OBJECT_NOT_FOUND);
    }

    *focused_input_name = focused_input->name;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/***************************************************************************/

layout_mgr_err_t layout_mgr_get_show_flag(const char* objectName,
                                          uint32_t* show)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == show) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    *show = obj->show;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/*******************************************************************************/

layout_mgr_err_t layout_mgr_get_object_size(const char* objectName,
                                            uint32_t* object_height,
                                            uint32_t* object_width)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == object_height || NULL == object_width) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    *object_width = obj->rect.x2 - obj->rect.x1 + 1;
    *object_height = obj->rect.y2 - obj->rect.y1 + 1;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*******************************************************************************/

layout_mgr_err_t layout_mgr_get_button_type(const char* objectName,
                                            LayoutButtonType_t* type)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == objectName || NULL == type) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("%s: object named %s is not of the supported type (button)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *type = obj->button.type;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/***********************************************************************************/

layout_mgr_err_t layout_mgr_get_button_function(
    const char* objectName, LayoutButtonFunctionType_t* function)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == function) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("%s: object named %s is not of the supported type (button)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (obj->button.type != LAYOUT_BUTTON_TYPE_FUNCTION) {
        TUILOGE("%s: button %s is not of the supported type (function)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    *function = obj->button.function.type;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/********************************************************************************/

layout_mgr_err_t layout_mgr_get_button_data(const char* objectName,
                                            utf8_char_t* data)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == data) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("%s: object named %s is not of the supported type (button)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (obj->button.type != LAYOUT_BUTTON_TYPE_DATA) {
        TUILOGE("%s: button %s is not of the supported type (data)", __func__,
               objectName);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    *data = obj->button.data.lower;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/*************************************************************************************/

layout_mgr_err_t layout_mgr_set_button_state(const char* objectName,
                                             LayoutButtonStatus_t state)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("%s: object named %s is not of the supported type (button)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    if (state > LAYOUT_BUTTON_STATUS_LAST) {
        TUILOGE("%s: button state value is not supported %d , %d", __func__,
               state);
        EXITV(LAYOUT_MGR_UNSUPPORTED);
    }

    obj->button.status = state;
    obj->label.color = obj->button.stateColor[state].fgColor;
    obj->dirty = true;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/*******************************************************************************/

layout_mgr_err_t layout_mgr_get_button_state(const char* objectName,
                                             LayoutButtonStatus_t* state)
{
    LayoutObject_t* obj = NULL;
    ENTER;

    if (NULL == state) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("%s: object named %s is not of the supported type (button)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    *state = obj->button.status;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_get_image(const char* objectName,
                                      const uint8_t** image_buffer,
                                      VerticalAlignment_t* v_align,
                                      HorizontalAlignment_t* h_align,
                                      uint32_t* image_height,
                                      uint32_t* image_width)
{
    LayoutObject_t* obj = NULL;
    sec_render_err_t retVal;
    ENTER;

    if (NULL == image_buffer || NULL == v_align || NULL == h_align ||
        NULL == image_height || NULL == image_width) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }

    FIND_OBJECT_OR_EXIT(objectName, obj);

    if (obj->type != LAYOUT_OBJECT_TYPE_IMAGE) {
        TUILOGE("%s: object named %s is not of the supported type (image)",
               __func__, objectName);
        EXITV(LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED);
    }

    retVal = check_render_image_object(*obj, obj->bgImage.image_buffer,
                                       image_width, image_height);
    if (retVal) {
        TUILOGE("%s: check_render_image_object returned %d",
               __func__, retVal);
        EXITV(LAYOUT_MGR_GENERAL_ERROR);
    }

    *image_buffer = obj->bgImage.image_buffer;
    *v_align = obj->bgImage.v_align;
    *h_align = obj->bgImage.h_align;

    EXITV(LAYOUT_MGR_SUCCESS);
}
/**********************************************************************************/

layout_mgr_err_t layout_mgr_set_default_timeout(int32_t timeout)
{
    ENTER;
    default_timeout = timeout;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/***********************************************************************************/

layout_mgr_err_t layout_mgr_get_default_timeout(int32_t* timeout)
{
    ENTER;
    if (NULL == timeout) {
        TUILOGE("%s: received null pointer parameter",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    *timeout = default_timeout;
    EXITV(LAYOUT_MGR_SUCCESS);
}
/************************************************************************************/

layout_mgr_err_t layout_mgr_get_layout_bgColor(Color_t* bgColor)
{
    ENTER;

    VALIDATE_LAYOUT_LOADED_OR_EXIT;

    if (NULL == bgColor) {
        TUILOGE("%s: received null pointer parameter bgColor",
               __func__);
        EXITV(LAYOUT_MGR_GOT_NULL_INPUT);
    }
    memscpy(bgColor, sizeof(*bgColor), &layout->layoutBg.bgColor,
            sizeof(layout->layoutBg.bgColor));

    EXITV(LAYOUT_MGR_SUCCESS);
}
