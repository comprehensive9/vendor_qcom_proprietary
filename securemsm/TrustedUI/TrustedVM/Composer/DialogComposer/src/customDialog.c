/*===========================================================================
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "ITuiComposer.h"
#include "utils.h"
#include "TUILog.h"
#include "timesafe_strncmp.h"
#include "memscpy.h"
#include "secure_memset.h"

static uint8_t* logoBuff = NULL;
static uint32_t logoSize = 0;
static uint8_t* indicatorBuff = NULL;
static uint32_t indicatorSize = 0;

const Color_t COLOR_RED = {
    .format = COLOR_FORMAT_RGBA,
    .rgba_color = {
        .red = 0xFF, .green = 0x00, .blue = 0x00, .alpha = 0xFF,
    }};

const Color_t COLOR_BLACK = {
    .format = COLOR_FORMAT_RGBA,
    .rgba_color = {
        .red = 0x00, .green = 0x00, .blue = 0x00, .alpha = 0xFF,
    }};

#define CUSTOM_INPUT_BUFFER_SIZE 10

// names of non-input controls use explicitly via layout manager API
const char* BUTTON_PAY = "button_pay";
const char* LABEL_CARD_STATUS = "label_card_status";

// the order of items assumes order of focus movement
typedef enum {
    CARD_INPUT_NUM1 = 0,
    CARD_INPUT_NUM2,
    CARD_INPUT_NUM3,
    CARD_INPUT_NUM4,
    CARD_INPUT_MM,
    CARD_INPUT_YY,
    CARD_INPUT_CVV,
    CARD_INPUT_LAST
} credit_card_input_id_t;

typedef struct credit_card_input {
    credit_card_input_id_t id;
    char* name;
    uint32_t expected_num_digits;
    credit_card_input_id_t next_input_id;
    uint8_t buffer[CUSTOM_INPUT_BUFFER_SIZE];
} credit_card_input_t;

static credit_card_input_t credit_card_data[] = {
    {CARD_INPUT_NUM1, "input_num1", 4, CARD_INPUT_NUM2},
    {CARD_INPUT_NUM2, "input_num2", 4, CARD_INPUT_NUM3},
    {CARD_INPUT_NUM3, "input_num3", 4, CARD_INPUT_NUM4},
    {CARD_INPUT_NUM4, "input_num4", 4, CARD_INPUT_MM},
    {CARD_INPUT_MM, "input_mm", 2, CARD_INPUT_YY},
    {CARD_INPUT_YY, "input_yy", 2, CARD_INPUT_CVV},
    {CARD_INPUT_CVV, "input_cvv", 3, CARD_INPUT_LAST /* no next input */},
};
const size_t credit_card_data_size =
    sizeof(credit_card_data) / sizeof(credit_card_input_t);

const credit_card_input_t* credit_card_input_get_by_id(
    credit_card_input_id_t id)
{
    if (id >= CARD_INPUT_LAST) {
        return NULL;
    }

    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        if (credit_card_data[i].id == id) {
            return &credit_card_data[i];
        }
    }
    // not found
    return NULL;
}

static const credit_card_input_t* credit_card_input_get_by_name(
    const char* name)
{
    if (NULL == name) {
        return NULL;
    }

    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        if (0 == timesafe_strncmp(credit_card_data[i].name, name,
                                  (strlen(credit_card_data[i].name) + 1))) {
            return &credit_card_data[i];
        }
    }
    // not found
    return NULL;
}

static inline uint32_t is_valid_month(char month_msd, char month_lsd)
{
    return ((month_msd == '0' && month_lsd >= '0' && month_lsd <= '9') ||
            (month_msd == '1' && month_lsd >= '0' && month_lsd <= '2'));
}

static void credit_card_clear_input_buffers()
{
    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        secure_memset(credit_card_data[i].buffer, 0,
                      sizeof(credit_card_data[i].buffer));
    }
}

static uint32_t credit_card_is_card_number_checksum_valid(
    const uint8_t* card_number, size_t size)
{
    if (NULL == card_number) {
        return false;
    }

    // For demo code we just check the card number is not all zeros
    // (0000-0000-0000-0000)
    // Should be replace by implementation of a real number card validation
    // method
    for (uint32_t i = 0; i < size; i++)
        if ('0' != card_number[i]) {
            return true;
        }

    return false;
}

// Fill the provided buffer with the full (16 digits) credit card number
static uint32_t credit_card_fill_card_number_from_input(
    uint8_t* card_number_buffer, size_t buffer_size)
{
    const uint8_t* text = NULL;
    uint32_t textByteSize = 0;
    uint32_t textCharSize = 0;
    uint32_t current_buffer_index = 0;

    if (NULL == card_number_buffer) {
        TUILOGE("%s: card_number_buffer is NULL",
                 __func__);
        return false;
    }

    for (uint32_t id = CARD_INPUT_NUM1; id <= CARD_INPUT_NUM4; id++) {
        const credit_card_input_t* input = credit_card_input_get_by_id(id);
        if (input == NULL) {
            TUILOGE("credit_card_input_get_by_id() returned null value");
            return false;
        }
        layout_mgr_err_t layout_err_val = layout_mgr_get_text(
            input->name, &text, &textByteSize, &textCharSize);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_get_text API error - %d", __func__,
                     layout_err_val);
            return false;
        }
        for (uint32_t i = 0; i < input->expected_num_digits && i < textByteSize;
             i++) {
            if (current_buffer_index >= buffer_size) {
                TUILOGE("Credit card number buffer is too small");
                return false;
            }
            card_number_buffer[current_buffer_index] = text[i];
            card_number_buffer++;
        }
    }
    return true;
}

// Validates if the user can complete the dialog, highlight inputs with errors
// if any
// Returns TUI_SUCCESS if dialog passes the validation
static qsee_tui_dialog_ret_val_t credit_card_dialog_validate()
{
    const uint8_t* text = NULL;
    uint32_t textByteSize = 0;
    uint32_t textCharSize = 0;
    uint32_t hasEmptyInput = true;
    qsee_tui_dialog_ret_val_t retVal = TUI_SUCCESS;
    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        const credit_card_input_t* input = &credit_card_data[i];
        layout_mgr_err_t layout_err_val = layout_mgr_get_text(
            input->name, &text, &textByteSize, &textCharSize);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_get_text API error - %d", __func__,
                     layout_err_val);
            retVal = TUI_LAYOUT_ERROR;
        }
        if (0 == textCharSize) {
            hasEmptyInput = true;
            retVal = TUI_ILLEGAL_INPUT;
        } else if (textCharSize != input->expected_num_digits) {
            layout_err_val = layout_mgr_set_text_color(input->name, COLOR_RED);
            if (layout_err_val != LAYOUT_MGR_SUCCESS) {
                TUILOGE("%s: layout_mgr_set_text_color API error - %d",
                         __func__, layout_err_val);
                retVal = TUI_LAYOUT_ERROR;
            } else {
                retVal = TUI_ILLEGAL_INPUT;
            }
        } else if (input->id == CARD_INPUT_MM &&
                   !is_valid_month(text[0], text[1])) {
            layout_err_val = layout_mgr_set_text_color(input->name, COLOR_RED);
            if (layout_err_val != LAYOUT_MGR_SUCCESS) {
                TUILOGE("%s: layout_mgr_set_text_color API error - %d",
                         __func__, layout_err_val);
                retVal = TUI_LAYOUT_ERROR;
            } else {
                retVal = TUI_ILLEGAL_INPUT;
            }
        }
    }

    // if all the input fields are valid we need to check for card number
    // checksum
    if (retVal == TUI_SUCCESS) {
        uint8_t card_number[16] = {0};
        credit_card_fill_card_number_from_input(card_number,
                                                sizeof(card_number));
        if (!credit_card_is_card_number_checksum_valid(card_number,
                                                       sizeof(card_number))) {
            // number is invalid - let's show the warning string
            layout_mgr_err_t layout_err_val =
                layout_mgr_set_show_flag(LABEL_CARD_STATUS, true);
            if (layout_err_val != LAYOUT_MGR_SUCCESS) {
                TUILOGE("%s: layout_mgr_set_show_flag API error - %d",
                         __func__, layout_err_val);
                retVal = TUI_LAYOUT_ERROR;
            } else {
                retVal = TUI_ILLEGAL_INPUT;
            }
        }
    }

    return retVal;
}

static uint32_t credit_card_has_empty_input()
{
    const uint8_t* text = NULL;
    uint32_t textByteSize = 0;
    uint32_t textCharSize = 0;
    uint32_t hasEmptyInput = false;
    qsee_tui_dialog_ret_val_t retVal = TUI_SUCCESS;
    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        const credit_card_input_t* input = &credit_card_data[i];
        layout_mgr_err_t layout_err_val = layout_mgr_get_text(
            input->name, &text, &textByteSize, &textCharSize);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_get_text API error - %d", __func__,
                     layout_err_val);
        }
        if (0 == textCharSize) {
            hasEmptyInput = true;
        }
    }
    return hasEmptyInput;
}

static uint32_t credit_card_move_focus_to_next_control(
    const char* control_in_focus)
{
    if (NULL == control_in_focus) {
        TUILOGE("%s got NULL input", __func__);
        return false;
    }

    const credit_card_input_t* input_in_focus =
        credit_card_input_get_by_name(control_in_focus);
    if (NULL == input_in_focus) {
        TUILOGE("%s: can't find input control structure for %s", __func__,
                 control_in_focus);
        return false;
    }

    const credit_card_input_t* next_input_to_focus =
        credit_card_input_get_by_id(input_in_focus->next_input_id);
    if (NULL == next_input_to_focus) {
        // no next control to move focus onto
        return false;
    }

    layout_mgr_err_t layout_err_val =
        layout_mgr_set_focus_input(next_input_to_focus->name);
    if (layout_err_val != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s: layout_mgr_set_focus_input API error - %d", __func__,
                 layout_err_val);
        return false;
    }

    return true;
}

static qsee_tui_dialog_ret_val_t credit_card_update_controls(
    const layout_mgr_touched_obj_t* curr_touched_object)
{
    layout_mgr_err_t layout_err_val = LAYOUT_MGR_SUCCESS;
    // update all controls which doesn't depend on the currently touched object

    if (credit_card_has_empty_input()) {
        // if we have even one empty input disable the PAY button
        layout_err_val = layout_mgr_set_button_state(
            BUTTON_PAY, LAYOUT_BUTTON_STATUS_DISABLED);
    }

    if (NULL == curr_touched_object ||
        curr_touched_object->eventType != LAYOUT_MGR_EVENT_UP) {
        // we don't have anything else to update as this function not called in
        // touch context
        return TUI_SUCCESS;
    }

    // set the pay button state to release as all the fields are filled
    // this should be done only as a response to touch event on the dialog
    if (!credit_card_has_empty_input()) {
        layout_err_val = layout_mgr_set_button_state(
            BUTTON_PAY, LAYOUT_BUTTON_STATUS_RELEASED);
    }

    if (layout_err_val != LAYOUT_MGR_SUCCESS) {
        TUILOGE( "%s: layout_mgr_set_button_state API error - %d", __func__,
                 layout_err_val);
        return TUI_LAYOUT_ERROR;
    }

    // now update controls based on the touched object

    // we need to move focus to then next input if the following happens:
    // - the focused input control has exactly the number of characters it
    // expects
    // - the last character was added to it during this event (any of the data
    // keys pressed)
    utf8_char_t button_data = {0};
    if (LAYOUT_MGR_SUCCESS ==
        layout_mgr_get_button_data(curr_touched_object->objectName,
                                   &button_data)) {
        const char* focused_input_name = NULL;
        const uint8_t* text = NULL;
        uint32_t textByteSize = 0;
        uint32_t textCharSize = 0;

        // set the invalid card message label to hidden
        layout_err_val = layout_mgr_set_show_flag(LABEL_CARD_STATUS, false);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_set_show_flag API error - %d", __func__,
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }

        layout_err_val = layout_mgr_get_focus_input(&focused_input_name);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_get_focus_input API error - %d", __func__,
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
        // set the current color to regular just in case it was changed before
        layout_err_val =
            layout_mgr_set_text_color(focused_input_name, COLOR_BLACK);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_set_text_color API error - %d", __func__,
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
        // check if the focused input field has exactly the number of chars
        layout_err_val = layout_mgr_get_text(focused_input_name, &text,
                                             &textByteSize, &textCharSize);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_get_text API error - %d", __func__,
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
        const credit_card_input_t* focused_input =
            credit_card_input_get_by_name(focused_input_name);
        if (NULL == focused_input) {
            TUILOGE("%s: credit_card_input_get_by_name() returned NULL value",
                     __func__);
            return TUI_LAYOUT_ERROR;
        }
        if (textCharSize == focused_input->expected_num_digits) {
            // try to move focus to the next control
            credit_card_move_focus_to_next_control(focused_input_name);
        }
    }

    return TUI_SUCCESS;
}

static qsee_tui_dialog_ret_val_t credit_card_dialog_init(LayoutPage_t* page)
{
    layout_mgr_err_t layout_err_val = LAYOUT_MGR_SUCCESS;

    if (NULL == page) {
        TUILOGE( "%s: layout is NULL", __func__);
        return TUI_ILLEGAL_INPUT;
    }

    layout_mgr_err_t layout_res = layout_mgr_load_layout(page);
    if (layout_res < 0) {
        TUILOGE("%s: layout_mgr_load_layout failed with %d", __func__,
                 layout_res);
        return TUI_LAYOUT_ERROR;
    }

    for (uint32_t i = 0; i < credit_card_data_size; i++) {
        credit_card_input_t* input = &credit_card_data[i];
        layout_err_val = layout_mgr_set_buffer_for_input(
            input->name, input->buffer, CUSTOM_INPUT_BUFFER_SIZE);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("%s: layout_mgr_set_buffer_for_input API error - %d",
                     __func__, layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
    }

    static const char* hint_mm = "MM";
    static const char* hint_yy = "YY";
    const credit_card_input_t* input_mm =
        credit_card_input_get_by_id(CARD_INPUT_MM);
    const credit_card_input_t* input_yy =
        credit_card_input_get_by_id(CARD_INPUT_YY);
    if (NULL == input_mm || NULL == input_yy) {
        TUILOGE("%s: error getting input fields",
                 __func__);
        return TUI_LAYOUT_ERROR;
    }
    layout_err_val = layout_mgr_set_text_hint(
        input_mm->name, (const uint8_t*)hint_mm, strlen(hint_mm));
    if (layout_err_val != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s: layout_mgr_set_text_hint API error - %d", __func__,
                 layout_err_val);
        return TUI_LAYOUT_ERROR;
    }
    layout_err_val = layout_mgr_set_text_hint(
        input_yy->name, (const uint8_t*)hint_yy, strlen(hint_yy));
    if (layout_err_val != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s: layout_mgr_set_text_hint API error - %d", __func__,
                 layout_err_val);
        return TUI_LAYOUT_ERROR;
    }

    // set the invalid card message label to hidden
    layout_err_val = layout_mgr_set_show_flag(LABEL_CARD_STATUS, false);
    if (layout_err_val != LAYOUT_MGR_SUCCESS) {
        TUILOGE("%s: layout_mgr_set_show_flag API error - %d", __func__,
                 layout_err_val);
        return TUI_LAYOUT_ERROR;
    }

    // set own secure indicator image if provided
    if (indicatorBuff != NULL) {
        layout_err_val = layout_mgr_set_image(
            "image_0", indicatorBuff, LAYOUT_ALIGN_MID, LAYOUT_ALIGN_CENTER);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("Error setting local secure indicator - %d",
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
    }

    // set dynamic logo image of provided
    if (logoBuff != NULL) {
        layout_err_val = layout_mgr_set_image(
            "image_1", logoBuff, LAYOUT_ALIGN_MID, LAYOUT_ALIGN_CENTER);
        if (layout_err_val != LAYOUT_MGR_SUCCESS) {
            TUILOGE("Error setting dynamic logo - %d",
                     layout_err_val);
            return TUI_LAYOUT_ERROR;
        }
    }

    return credit_card_update_controls(NULL);
}

static tuiComposerInputStatus_t credit_card_dialog_event(
    layout_mgr_err_t layout_mgr_status,
    layout_mgr_touched_obj_t curr_touched_object,
    qsee_tui_dialog_ret_val_t* final_status)
{
    *final_status = TUI_SUCCESS;

    // if the pay button (ENTER) was pressed we validate the input and only then
    // allow the dialog to complete
    if (LAYOUT_MGR_ENTER_PRESSED == layout_mgr_status &&
        (TUI_SUCCESS == credit_card_dialog_validate())) {
        *final_status = TUI_OK_PRESSED;
        return TUI_COMPOSER_INPUT_STATUS_COMPLETE;
    }

    // check if the 'SKIP' button was pressed
    if (LAYOUT_MGR_F1_PRESSED == layout_mgr_status) {
        // clear all buffers as we skip validation
        credit_card_clear_input_buffers();
        *final_status = TUI_OK_PRESSED;
        return TUI_COMPOSER_INPUT_STATUS_COMPLETE;
    }

    if (LAYOUT_MGR_CANCEL_PRESSED == layout_mgr_status) {
        *final_status = TUI_CANCEL_PRESSED;
        return TUI_COMPOSER_INPUT_STATUS_CANCEL;
    }

    // update all the dialog controls state
    *final_status = credit_card_update_controls(&curr_touched_object);
    if (*final_status != TUI_SUCCESS) {
        return TUI_COMPOSER_INPUT_STATUS_CANCEL;
    }

    // is it expected to set the final status according to layout manager status
    return TUI_COMPOSER_INPUT_STATUS_CONTINUE;
}

uint32_t getCustomResult(char* msg, uint32_t len)
{
    uint32_t length = 0;
    int32_t count = 0;

    if ((msg == NULL) || (len == 0)) {
        TUILOGE("%s: invalid param", __func__);
        return 0;
    }

    secure_memset(msg, 0, len);

    count = snprintf((char*)msg, len, "card: %s-%s-%s-%s %s/%s cvv:%s",
                     credit_card_input_get_by_id(CARD_INPUT_NUM1)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_NUM2)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_NUM3)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_NUM4)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_MM)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_YY)->buffer,
                     credit_card_input_get_by_id(CARD_INPUT_CVV)->buffer);

    if ((count < 0) || (count >= len)) {
        TUILOGE("%s: truncation happened: %d", __func__, count);
        return 0;
    }

    return strlen(msg);
}

int32_t startCustomCreditCardDialog(tuiComposerParam_t* param,
                                    qsee_tui_dialog_ret_val_t* status)
{
    qsee_tui_dialog_params_t credit_card_params = {0};

    CHECK_STATUS(*status);

    // load dialog based on display resolution. get_layout_by_name() can be
    // used directly if the layout name is known at compile time
    LayoutPage_t* page = getLayout("credit_card", param->width, param->height);
    if (page == NULL) {
        TUILOGE("%s::%d error getLayout('credit_card') for login dialog layout", __func__, __LINE__);
        return TUI_ILLEGAL_INPUT;
    }

    qsee_tui_dialog_ret_val_t res = credit_card_dialog_init(page);
    if (res < 0) {
        return res;
    }

    // TODO: Error handling, load it once, check when these get freed
    logoSize = loadLogo(&logoBuff);

    credit_card_params.layout_page = page;

    credit_card_params.dialogType = TUI_DIALOG_CUSTOM;

    credit_card_params.dialogCommonParams.width = param->width;
    credit_card_params.dialogCommonParams.height = param->height;

    if (param->useSecureIndicator != true) {
        // Use own secure indicator
        indicatorSize = loadIndicator(&indicatorBuff);
        credit_card_params.dialogCommonParams.secureIndicatorParams
            .bDisplayGlobalSecureIndicator = false;
        credit_card_params.dialogCommonParams.secureIndicatorParams
            .secureIndicator.pu8Buffer = indicatorBuff;
        credit_card_params.dialogCommonParams.secureIndicatorParams
            .secureIndicator.u32Len = indicatorSize;
    } else {
        if (loadSecureIndicator() == 0) {
            // Display global secure indicator
            credit_card_params.dialogCommonParams.secureIndicatorParams
                .bDisplayGlobalSecureIndicator = true;
        } else {
            TUILOGE("%s::%d Failed to load SecureIndicator", __func__, __LINE__);
            return TUI_SECURE_INDICATOR_ERROR;
        }
    }

    credit_card_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;
    credit_card_params.customDialogParams.manage_layout_event_custom =
        credit_card_dialog_event;

    return qsee_tui_dialog(&credit_card_params, status);
}