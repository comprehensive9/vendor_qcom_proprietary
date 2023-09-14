/*===========================================================================
  Copyright (c) 2013-2016,2019, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#ifndef QSEE_TUI_H_
#define QSEE_TUI_H_

/** @file qsee_tui_dialogs.h
 * @brief
 * This file contains the interfaces to Secure UI GP dialog screens.
 */



#include "SecureTouchLayout.h"
#include "layout_manager.h"
#include "ITuiComposer.h"

/** @addtogroup qsee_gp_screens
  @{ */

/* SecureUI version information */
#define TUI_MAJOR_VERSION 3 /**< Major version. */
#define TUI_MINOR_VERSION 0 /**< Minor version. */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Secure UI dialog return values that indicate the status and error codes
  for the GP screens API. */
typedef enum {
    TUI_GENERAL_ERROR = -6, /**< Secure UI General error. */
    TUI_SECURE_INDICATOR_ERROR = -5, /**< Error in setting the secure indicator for the local
               application. */
    TUI_LAYOUT_ERROR = -4,      /**< Internal layout management error. */
    TUI_ILLEGAL_INPUT = -3,     /**< Illegal input was received. */
    TUI_SYS_EVENT_ABORTED = -2, /**< Secure UI session was aborted by an
                                  external abort request, such as an
                                  incoming call or a power down. */
    TUI_TIMEOUT_ABORTED = -1,  /**< Secure UI session was aborted due to a timeout. */
    TUI_SUCCESS = 0,    /**< Operation was successful. */
    TUI_OK_PRESSED = 1, /**< User pressed the OK button. */
    TUI_CANCEL_PRESSED = 2, /**< User pressed the Cancel button. */
    TUI_RIGHT_PRESSED = 3, /**< User pressed the Right button of the message screen. */
    TUI_LEFT_PRESSED = 4, /**< User pressed the Left button of the message screen. */
    TUI_MIDDLE_PRESSED = 5, /**< User pressed the Middle button of the message screen. */
    TUI_RETVAL_MAX = 0x7FFFFFFF
} qsee_tui_dialog_ret_val_t;

/** @xreflabel{hdr:CustomSecTouchCmdCallback}
  Application callback that implements user customized screen logic.

  @param[in]   layout_mgr_status      Layout Manager event evaluation status.
  @param[in]   curr_touched_object    Name of the currently touched object in
  the
  screen, and the event type: up/down/move.
  @param[out]  final_status           Upon a failure during the callback,
  pointer
  to the status returned in qsee_tui_dialog().

  @return
  Command for the next touch operation: completed, canceled, continue. @newpage
*/
typedef tuiComposerInputStatus_t (*qsee_tui_manage_layout_event_callback_t)(
    layout_mgr_err_t layout_mgr_status,
    layout_mgr_touched_obj_t curr_touched_object,
    qsee_tui_dialog_ret_val_t* final_status);

/** TUI GP screen type. */
typedef enum {
    TUI_DIALOG_MESSAGE,  /**< Message screen. */
    TUI_DIALOG_PIN,      /**< PIN entry screen. */
    TUI_DIALOG_PASSWORD, /**< Login and password entry screen. */
    TUI_DIALOG_CUSTOM,   /**< User-customized screen. */
    TUI_DIALOG_MAX = 0x7FFFFFFF
} qsee_tui_dialog_t;

/** Secure UI keypad mode. */
typedef enum {
    TUI_KEYPAD_NORMAL,
    TUI_KEYPAD_RANDOMIZED,
    TUI_KEYPAD_RANDOMIZED_ONCE, /**< Randomized only one time per device. */
    TUI_KEYPAD_MAX = 0x7FFFFFFF /**< @newpage */
} qsee_tui_keypad_mode_t;

/** Secure UI font heat map mode. */
typedef enum {
    TUI_FONT_HEATMAP_DISABLED, /**< Dynamic font cache loads characters on
                                  demand. */
    TUI_FONT_HEATMAP_ENABLED,  /**< Dynamic font cache is pre-populated with
                                 static font data
                                 (heat map) copied from the layout file for all
                                 dynamic fonts. */
    TUI_FONT_HEATMAP_MAX = 0x7FFFFFFF
} qsee_tui_font_heatmap_mode_t;

/** Secure UI dialog orientation relative to the device native screen
  orientation.

  On the phone form factor, native orientation (0 degrees) is commonly a
  portrait orientation. Some dialog designs might also require landscape
  orientation on phone devices. In such cases, the provided layout should
  have the dimensions matching the preferred orientation (such as 2560 width
  and 1600 height) and a 90 degree (or 270 if preferred) rotation specified
  in the common dialog option (qsee_tui_dialog_orienation_t).
  */
typedef enum {
    TUI_ORIENTATION_0,   /**< Native orientation (0 degrees). */
    TUI_ORIENTATION_90,  /**< 90 degrees. */
    TUI_ORIENTATION_180, /**< 180 degrees. */
    TUI_ORIENTATION_270, /**< 270 degrees. */
    TUI_ORIENTATION_MAX = 0x7FFFFFFF
} qsee_tui_dialog_orienation_t;

/** Defines a general buffer parameter.

  If the buffer is used to represent text, the text string must be formatted as
  UTF-8. For the string to be displayed properly on the screen, an appropriate
  font file must be present on the device.
  For instructions on how to create and reference a font file, see the
  <i>QSEE Trusted User Interface Customization Tool User Guide</i>
  @xhyperref{80-P3696-1,(80-P3696-1)}.
  */
typedef struct {
    uint8_t* pu8Buffer; /**< Start address of the buffer. */
    uint32_t u32Len;    /**< Length of the buffer in bytes. @newpagetable */
} qsee_tui_buffer_t;

/** Defines the parameters for user input in a dialog screen. */
typedef struct {
    uint32_t u32MinLen;             /**< Minimum length in characters (UTF-8
                                      encoding can take more than one byte
                                      to represent a character). */
    uint32_t u32MaxLen;             /**< Maximum length in characters. */
    qsee_tui_buffer_t inpValue;     /**< Buffer parameter to be filled with
                                      user input. */
    qsee_tui_buffer_t defaultValue; /**< Buffer parameter that contains the
                                      default value to be shown in the
                                      dialog screen. */
    qsee_tui_buffer_t label;        /**< Buffer parameter that contains the
                                      label text to be shown above the
                                      input object if it is empty. */
    uint32_t* pu32InpValueLen;      /**< Output parameter; the actual length
                                      of the user input in characters. */
} qsee_tui_input_params_t;

/** Defines the parameters for the secure indicator. */
typedef struct {
    uint32_t bDisplayGlobalSecureIndicator; /**< Boolean flag to display the
                                              global secure indicator. */
    qsee_tui_buffer_t secureIndicator;      /**< Secure indicator image buffer
                                              encoded as PNG. @newpagetable */
} qsee_tui_secure_indicator_params_t;

/** Common parameters for all GP screens.

  @note1hang Some of the common parameters are not relevant to custom screens,
  such as the title, description, and logo. Any entered value
  for these parameters is ignored.
  */
typedef struct {
    int32_t n32TimeOut;
    /**< Touch session timeout (in milliseconds), that is, the maximum time to
      wait for user input.

      @values
      - 0 means return immediately
      - -1 means no timeout (wait indefinitely) @tablebulletend */

    qsee_tui_buffer_t title;
    /**< Buffer parameter that contains the title text of the screen. */

    qsee_tui_buffer_t description;
    /**< Description label text. */

    qsee_tui_buffer_t logo;
    /**< Logo image buffer encoded as PNG. */

    qsee_tui_secure_indicator_params_t secureIndicatorParams;
    /**< Secure indicator parameters; contains the optional image buffer
      encoded as PNG. */

    qsee_tui_font_heatmap_mode_t fontHeatmapMode;
    /**< Dynamic font heat map support mode. */

    qsee_tui_dialog_orienation_t orientation;
    /**< Screen orientation. */

    uint32_t width;
    /**< Screen width. */

    uint32_t height;
    /**< Screen height. */
} qsee_tui_dialog_common_params_t;

/** Defines the parameters of a configurable button in a dialog screen.. */
typedef struct {
    qsee_tui_buffer_t btnLabel; /**< Label to be shown on the button. */
    uint32_t bShowBtn;          /**< Boolean flag to show the button. */
    uint32_t bDisableBtn;       /**< Boolean flag to disable (gray out) the
                                  button. @newpagetable */
} qsee_tui_button_params_t;

/** Defines the parameters that are specific to the message screen. */
typedef struct {
    qsee_tui_buffer_t msg;
    /**< Buffer parameter that contains the message to be shown on the
      screen. */
    qsee_tui_button_params_t rightBtnParams;
    /**< Parameters of the right button of the message screen. */
    qsee_tui_button_params_t leftBtnParams;
    /**< Parameters of the left button of the message screen. */
    qsee_tui_button_params_t middleBtnParams;
    /**< Parameters of the middle button of the message screen. */
} qsee_tui_dialog_msg_params_t;

/** Defines the parameters that are specific to the pin entry screen. */
typedef struct {
    qsee_tui_input_params_t pin;        /**< Buffer details for storing the user PIN. */
    uint32_t bHideInputBox; /**< Boolean flag to hide the user input on
                              the screen. */
    LayoutInputDisplayMode_t inpMode; /**< User input (for the PIN) display mode. */
    qsee_tui_keypad_mode_t keyPadMode; /**< Keypad mode: Randomized, Randomized
                                         Once, Normal. */
} qsee_tui_dialog_pin_params_t;

/** Defines the parameters that are specific to the login and password entry
  screen.

  @note1hang At least one of the flags in bUserNameInpShow and bPasswordInpShow
  must be set to TRUE; otherwise, it is considered an illegal input.
  */
typedef struct {
    qsee_tui_input_params_t username;         /**< User input parameters for the username. */
    uint32_t bUserNameInpShow; /**< Boolean flag to show the username
                                 input object on the screen. */
    uint32_t bPasswordInpShow; /**< Boolean flag to show the password
                                 input object on the screen. */
    qsee_tui_input_params_t password; /**< User input parameters for password. */
    LayoutInputDisplayMode_t inpMode; /**< Password display mode. @newpagetable */
} qsee_tui_dialog_pswd_params_t;

/** Defines the parameters that are specific to user-customized screen. */
typedef struct {
    qsee_tui_manage_layout_event_callback_t manage_layout_event_custom;
    /**< Callback to be called upon touch event in custom screen, after Layout
      Manager evaluation. */
} qsee_tui_dialog_custom_params_t;

/** Defines the general parameter set for all screens.

  When passing a layout for a GP screen (PIN, login, message), ensure that the
  parameters match the specified layout. For example, if the parameters refer
  to three buttons in the message screen, the layout must contain three
  buttons. This restriction also applies to user name and password input fields
  on login screens.

  For backward compatibility, you can pass NULL for the layout_page. In this
  case, the default layout for the required dialog type is used. The default
  layout of each dialog type is defined by the Customization Tool as follows:
  - If only one layout of this type exists, it is the default.
  - Otherwise, the first layout of each type that was defined in the tool
  is the default.

  If you implicitly select a layout by passing NULL, ensure that a
  corresponding layout exists in the application. For more information, see the
  <i>QSEE Trusted User Interface Customization Tool User Guide</i>
  @xhyperref{80-P3696-1,(80-P3696-1)}).
  */
typedef struct qsee_tui_dialog_params_s {
    qsee_tui_dialog_common_params_t dialogCommonParams;
    /**< Common parameters, such as timeout, title, description, and logo. */

    qsee_tui_dialog_t dialogType;
    /**< Type of dialog screen. */

    /** Union of the parameters of different screen types. */
    union {
        qsee_tui_dialog_msg_params_t msgDialogParams;
        /**< Parameters that are specific to the message screen. */
        qsee_tui_dialog_pin_params_t pinDialogParams;
        /**< Parameters that are specific to the PIN screen. */
        qsee_tui_dialog_pswd_params_t pswdDialogParams;
        /**< Parameters that are specific to the password screen. */
        qsee_tui_dialog_custom_params_t customDialogParams;
        /**< Parameters that are specific to a custom screen. */
    };

    LayoutPage_t* layout_page;
    /**< Layout of the dialog screen (GP screen or custom screen).
      @newpagetable */

} /** @cond */ qsee_tui_dialog_params_t /** @endcond */;

/** @cond */
typedef struct {
    uint8_t* pu8SecureIndicator;
    uint32_t u32IndicatorSize;
    uint32_t bIsValid;
} qsee_tui_secure_indicator_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*******************************************************************************/

/** @xreflabel{hdr:ScreenFunctionDocumentation}
  External function used to initialize the Secure UI GP screen service.

  @note1hang This function must be called before any other GP screen functions
  are called. For a description of the GP screen functions flow,
  see Section @xref{sec:GPScreenCallFlow}.

  @return
#TUI_SUCCESS \n
#TUI_LAYOUT_ERROR

@dependencies
None. @newpage
*/
qsee_tui_dialog_ret_val_t qsee_tui_init(void);

/***********************************************************************************/

/**
  External function for using the three GP screens: LOGIN, GET_PIN, and
  SHOW_MESSAGE. For a description of the GP screens, see Section
@xref{sec:GpScreens}.

  @datatypes
#qsee_tui_dialog_params_s \n
#qsee_tui_dialog_ret_val_t

@param[in,out] dialogParams  Pointer to the general parameters of the dialog,
including the dialog type and the relevant
input/output parameters. For a description of
limitations for this parameter, see
Section @xref{sec:GPScreenParamLim}.
@param[out]    status        Pointer to the final status of the dialog
execution, which is to be read only after the
touch session is finished.

@detdesc
This function does not start the screen operation. It only sets up the
screen so that when the touch session starts, the required screen appears.
The operation completes during the touch session.
@par
@note1hang This function starts a secure display session (if there is no
active session of the previous screen). So, until the touch session starts,
a black screen is displayed.
@par
The status parameter <i>must be read only after the touch session ends</i>.
This includes other output parameters that are contained in the dialogParams
parameter, such as the buffers that are to be filled with the user input.
A notification about the touch session ending is received from the HLOS
after the return of UseSecureTouch().

@return
#TUI_SUCCESS \n
#TUI_LAYOUT_ERROR \n
#TUI_ILLEGAL_INPUT \n
#TUI_SECURE_INDICATOR_ERROR
@par
For descriptions of the return codes, see #sec_ui_err_t.
@par
@note1hang The return value represents the status of setting up the
screen, as opposed to the status parameter, which reflects the final status
of the operation of the GP screen.

@dependencies
Must be called after qsee_tui_init(). @newpage
*/
qsee_tui_dialog_ret_val_t qsee_tui_dialog(
    qsee_tui_dialog_params_t* dialogParams, qsee_tui_dialog_ret_val_t* status);

/*********************************************************************************/

/**
  External function for resetting the secure indicator buffer and clearing
  the valid and size fields. This function must be called when the application
  no longer requires the secure indicator (usually on shutdown).

  @return
#TUI_SUCCESS

@dependencies
None.

@sideeffects
Secure indicator is no longer available (unless it is set again).
*/
qsee_tui_dialog_ret_val_t qsee_tui_reset_secure_indicator(void);

/***********************************************************************/

/**
  External function for closing the Secure UI GP screen service.

  @note1hang If several GP screens must be called one after
  another, call this function only after the final one.

  @return
#TUI_SUCCESS \n
#TUI_LAYOUT_ERROR

@dependencies
None.

@sideeffects
Stops the secure display session.
*/
qsee_tui_dialog_ret_val_t qsee_tui_tear_down(void);

/** @} */ /* end_addtogroup qsee_gp_screens */

#endif /* QSEE_TUI_H_ */
