/********************************************************************
  ---------------------------------------------------------------------
  Copyright (c) 2013-2016,2019, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ----------------------------------------------------------------------
 *********************************************************************/

#include "secure_display_renderer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_ON_LEVM
#include <stringl.h>
#endif
#include <time.h>
#include "TUIHeap.h"
#include "TUILog.h"
#include "font_manager.h"
#include "gd.h"
#include "gd_intern.h"
#include "png.h"
#include "pngstruct.h"
#include "secure_ui_defs.h"
#include "memscpy.h"

#define SUPPORT_TRANSPARENCY 1

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

#define RGBA_BYTES_PER_PIXEL (4)
#define PNG_MAX_SIG_LEN 8
#define ALPHAMAX 255U
#define PNG_HEADER_SUCCESS 1

#ifdef TEST_ON_ANDROID
void PNGAPI png_read_row(png_structrp png_ptr, png_bytep row, png_bytep dsp_row);
void PNGAPI png_read_info(png_structrp png_ptr, png_inforp info_ptr);
#endif

/* allocate max possible space for an image row. Additional 4 bytes are needed
 * to allow
 * gdImageSetRowPixels to safely read beyond the actual data when reading 24bit
 * pixel into 32bit */
static uint8_t rowData[MAX_IMAGE_LINE_WIDTH_IN_PIXELS * RGBA_BYTES_PER_PIXEL +
                       sizeof(uint32_t)];

uint8_t img_array[MAX_IMAGE_SIZE_IN_PIXELS * RGBA_BYTES_PER_PIXEL];

/* holds the pre-rendered background image for faster composition */
static gdImagePtr bg_gdImage = NULL;

/*************************************************************************************/

/**
  @brief
  External function for calculating utf-8 string length in bytes.
  */
sec_render_err_t get_utf8_len(const uint8_t* utf8_string, uint32_t num_chars,
                              uint32_t* length, uint32_t* last_char_index)
{
    uint32_t i;
    int encoded_char;

    if (NULL == length) {
        TUILOGE(
               "get_utf8_len: got null input, utf8_string = %d, length = %d",
               utf8_string, length);
        return SEC_RENDER_ERROR_NULL_INPUT;
    }

    *length = 0;

    if (NULL == utf8_string || 0 == num_chars) {
        TUILOGD("get_utf8_len: got empty string, utf8_string = %d, length = %d",
               utf8_string, length);
        return SEC_RENDER_SUCCESS;
    }

    for (i = 0; i < num_chars; i++) {
        encoded_char = getUnicode(&utf8_string[*length]);
        if (encoded_char == 0) {
            TUILOGE("get_utf8_len: string is not a valid utf-8 i = %d", i);
            return SEC_RENDER_GOT_BAD_INPUT;
        }
        if (last_char_index != NULL) {
            *last_char_index = *length;
        }
        if (0 < encoded_char && encoded_char < 0x80) {
            *length += 1;
        } else if (0x80 <= encoded_char && encoded_char < 0x800) {
            *length += 2;
        } else if (0x800 <= encoded_char && encoded_char < 0x10000) {
            *length += 3;
        } else if (0x10000 <= encoded_char && encoded_char < 0x200000) {
            *length += 4;
        } else if (0x200000 <= encoded_char && encoded_char < 4000000) {
            *length += 5;
            //} else if(0x4000000<=encoded_char && encoded_char<=0x7FFFFFFFF){
        } else if (0x4000000 <= encoded_char) {
            *length += 6;
        }
    }

    return SEC_RENDER_SUCCESS;
}

/********************************************************************************/

/**
  @brief
  Internal function used as callback for getting png image info.
  */
static void read_data_fn(png_structp png_ptr, png_bytep outBytes,
                         png_size_t byteCountToRead)
{
    uint8_t* buffer = (uint8_t*)png_ptr->io_ptr;
    if (NULL == buffer) {
        TUILOGE("read_data_fn got NULL buffer");
        return;
    }

    memscpy((uint8_t*)outBytes, byteCountToRead, buffer, byteCountToRead);
    buffer += byteCountToRead;
    png_ptr->io_ptr = (void*)buffer;
}
/**********************************************************************************/

/**
  @brief
  Internal function that reads and draws an image or a part of it into an area
on the gd image object.
  Note that all the coordinates are absolute outImage coordinates. Image type
can be RGBA/RGB png image.
Parameters:
outImage         - gd image (render surface)
png_ptr, info_ptr,
width, height    - png image parameters
xOffset, yOffset - coordinates of the left, top corner of the image on the
render surface
rect             - (optional) clipping area of the rendered image on the render
surface
*/
static sec_render_err_t ParseImage(gdImagePtr outImage, png_structp png_ptr,
                                   png_infop info_ptr, png_uint_32 width,
                                   png_uint_32 height, uint32_t xOffset,
                                   uint32_t yOffset, int colorType,
                                   const Rect_t* rect)
{
    uint32_t rowIdx;
    int gdRowPixelType = gdPixelTypeUndefined;
    const Rect_t defaultRect = {
        xOffset, yOffset, xOffset + width - 1,
        yOffset + height - 1}; /* assume full image by default */
    ENTER;

    if (colorType != PNG_COLOR_TYPE_RGB && colorType != PNG_COLOR_TYPE_RGBA) {
        TUILOGE("ParseImage got unsupported color type = %d",
               colorType);
        EXITV(SEC_RENDER_UNSUPPORTED);
    }

    if (width > MAX_IMAGE_LINE_WIDTH_IN_PIXELS) {
        TUILOGE("ParseImage got too big png width = %d, "
               "MAX_IMAGE_LINE_WIDTH_IN_PIXELS = %d",
               width, MAX_IMAGE_LINE_WIDTH_IN_PIXELS);
        EXITV(SEC_RENDER_UNSUPPORTED);
    }

    if (NULL == rect) {
        /* if no 'rect' was provided we need to do a full image rendering */
        rect = &defaultRect;
    }

    if (rect->x1 < xOffset || rect->y1 < yOffset ||
        rect->x2 >= xOffset + width || rect->y2 >= yOffset + height) {
        TUILOGE("ParseImage got invalid clipping rectangle");
        EXITV(SEC_RENDER_UNSUPPORTED);
    }

    if (colorType == PNG_COLOR_TYPE_RGB) {
        gdRowPixelType = gdPixelTypeRGB;
    } else if (colorType == PNG_COLOR_TYPE_RGBA) {
        gdRowPixelType = gdPixelTypeRGBA;
    }

    const uint32_t imX = rect->x1;
    const uint32_t pngRowFirst = rect->x1 - xOffset;
    const uint32_t pngRowLast = rect->x2 - xOffset;

    /* read and renderer png image one row at a time.
     * Note that we have to parse the png from the start even if only part of it
     * is rendered
     * */
    for (uint32_t imY = yOffset; imY <= rect->y2; ++imY) {
        png_read_row(png_ptr, (png_bytep)rowData, NULL);

        /* skip png row if above the clipping area */
        if (imY < rect->y1) {
            continue;
        }

        int gdRes = gdImageSetRowPixels(outImage, imX, imY, rowData, pngRowFirst,
                                pngRowLast, gdRowPixelType);
        if (GD_FALSE == gdRes) {
            TUILOGE("gdImageSetRowPixels error while parsing part of the image. "
                   "gdPixelType: %d",
                   gdRowPixelType);
            EXITV(SEC_RENDER_GD_IMAGE_ERROR);
        }
    }

    EXITV(SEC_RENDER_SUCCESS);
}

/************************************************************************************/

/**
  @brief
  Internal function that calculates the numeric representation of a generic
  color.
  */

static sec_render_err_t get_numeric_color(Color_t generic_color,
                                          uint32_t* numeric_color)
{
    ENTER;
    if (NULL == numeric_color) {
        TUILOGE("get_color: got null color");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    switch (generic_color.format) {
        case COLOR_FORMAT_RGBA:
            *numeric_color = gdTrueColorAlpha(
                generic_color.rgba_color.alpha, generic_color.rgba_color.blue,
                generic_color.rgba_color.green, generic_color.rgba_color.red);
            break;
        default:
            TUILOGE("get_color: unsupported color format = %d",
                   generic_color.format);
            EXITV(SEC_RENDER_UNSUPPORTED);
    }

    EXITV(SEC_RENDER_SUCCESS);
}

/*********************************************************************************/

/**
  @brief
  Internal function that draws a box, with background color and frame, to a
  given rectangle.
  */
static sec_render_err_t draw_box(gdImage* img_ptr, Rect_t* rect,
                                 Color_t bgColor, uint32_t u32FrameWidth,
                                 Color_t frameColor)
{
    uint32_t bg_color, frame_color;
    sec_render_err_t retVal;
    ENTER;

    if (NULL == img_ptr || NULL == rect) {
        TUILOGE("draw_box got null input; img_ptr = %d, rect = %d", img_ptr,
               rect);
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    retVal = get_numeric_color(bgColor, &bg_color);
    if (retVal < 0) {
        TUILOGE("draw_box: get_numeric_color returned %d",
               retVal);
        EXITV(retVal);
    }

    retVal = get_numeric_color(frameColor, &frame_color);
    if (retVal < 0) {
        TUILOGE("draw_box: get_numeric_color returned %d",
               retVal);
        EXITV(retVal);
    }

    /* First draw the outer rectangle with the frame color */
    if (u32FrameWidth > 0) {
        gdImageFilledRectangle(img_ptr, rect->x1, rect->y1, rect->x2, rect->y2,
                               frame_color);
    }

    /* Then draw the inner rectangle with the background color */
    gdImageFilledRectangle(img_ptr, rect->x1 + u32FrameWidth,
                           rect->y1 + u32FrameWidth, rect->x2 - u32FrameWidth,
                           rect->y2 - u32FrameWidth, bg_color);

    EXITV(SEC_RENDER_SUCCESS);
}
/****************************************************************************************/

/**
  @brief
  Internal function that calculates the position of some fill in the object
  rectangle.
  */
static sec_render_err_t calc_position(VerticalAlignment_t vAlign,
                                      HorizontalAlignment_t hAlign,
                                      uint32_t objHight, uint32_t objWidth,
                                      uint32_t fillHight, uint32_t fillWidth,
                                      uint32_t* x, uint32_t* y)
{
    ENTER;
    if (NULL == x || NULL == y) {
        TUILOGE("calc_position: got null input");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (objHight < fillHight || objWidth < fillWidth) {
        TUILOGE("calc_position: fill exceeds object rectangle: fill = %d x %d, "
               "object (with margins) = %d x %d",
               fillWidth, fillHight, objWidth, objHight);
        EXITV(SEC_RENDER_ERROR_FILL_EXCEED);
    }

    switch (vAlign) {
        case LAYOUT_ALIGN_TOP:
            *y = 0;
            break;
        case LAYOUT_ALIGN_MID:
            *y = (objHight - fillHight) / 2;
            break;
        case LAYOUT_ALIGN_BTM:
            *y = objHight - fillHight;
            break;
        default:
            TUILOGE("calc_position: unsupported vertical alignment %d", vAlign);
            EXITV(SEC_RENDER_UNSUPPORTED);
    }

    switch (hAlign) {
        case LAYOUT_ALIGN_LEFT:
            *x = 0;
            break;
        case LAYOUT_ALIGN_CENTER:
            *x = (objWidth - fillWidth) / 2;
            break;
        case LAYOUT_ALIGN_RIGHT:
            *x = objWidth - fillWidth;
            break;
        default:
            TUILOGE("calc_position: unsupported horizontal alignment %d",
                   hAlign);
            EXITV(SEC_RENDER_UNSUPPORTED);
    }

    EXITV(SEC_RENDER_SUCCESS);
}

/******************************************************************************************/

/**
  @brief
  Internal function that fills drawn box with text, supporting line wrapping.
  This function can be called with bToRender = false, in this case nothing will
  be rendered.
  One can use this mode to check that the text fits in the rectangle, before
  trying to render it.
  */
static sec_render_err_t render_text(gdImage* img_ptr, Rect_t* rect,
                                    uint32_t margin, Text_t* text,
                                    uint32_t bToRender, uint32_t* fitting_len,
                                    uint32_t* missing_char)
{
    uint32_t x, y, first_line_y, text_color, font_h, curr_line = 0, max_lines,
                                                     i, rect_width;
    uint32_t bytes_to_copy, line_width, rect_hight, total_bytes,
        written_bytes = 0;
    sec_render_err_t retVal;
    int32_t status;
    gdFontExt* ext_font;
    uint8_t is_last_line = false;
    uint32_t line_bytes_len[MAX_LINES_IN_TEXT_LABEL];
    ENTER;

    if ((NULL == img_ptr && bToRender) || NULL == rect || NULL == text ||
        NULL == fitting_len || NULL == missing_char) {
        TUILOGE("render_text got null input; img_ptr = %d, rect = %d, text = %d",
               img_ptr, rect, text);
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    *fitting_len = 0;
    *missing_char = 0;

    ext_font = (gdFontExt*)text->font;
    font_h = ext_font->h;

    retVal = get_numeric_color(text->color, &text_color);
    if (retVal < 0) {
        TUILOGE("render_text: get_numeric_color returned %d",
               retVal);
        EXITV(retVal);
    }

    rect_width = rect->x2 - rect->x1 + 1 - 2 * margin;
    rect_hight = rect->y2 - rect->y1 + 1 - 2 * margin;
    max_lines = rect_hight / font_h;

    /* allocates counter for each line to save the number of bytes to write to
     * it */
    if (0 == max_lines) {
        TUILOGE("render_text got too small rectangle, rect_hight = %d, font_h =%d",
            rect_hight, font_h);
        EXITV(SEC_RENDER_STRING_TOO_BIG);
    }

    if (max_lines > MAX_LINES_IN_TEXT_LABEL) {
        TUILOGE("render_text got too big rectangle, max lines in the rectangle "
               "= %d, MAX_LINES_IN_TEXT_LABEL =%d",
               max_lines, MAX_LINES_IN_TEXT_LABEL);
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    /* gets the length in bytes of the text */
    status = get_utf8_len(text->text, text->curr_len, &total_bytes, NULL);
    if (status < 0) {
        TUILOGE("render_text: get_utf8_len returned %d",
               retVal);
        EXITV(SEC_RENDER_BAD_STRING);
    }

    while (curr_line < max_lines && !is_last_line) {
        /* check if the remaining text can be rendered in one line */
        bytes_to_copy = gdGetMaxLenForWidth(
            ext_font, text->text + written_bytes, total_bytes - written_bytes,
            rect_width, missing_char);
        if (0 == bytes_to_copy) {
            TUILOGE("render_text: missing character unicode = %u",
                   *missing_char);
            EXITV(SEC_RENDER_GOT_BAD_INPUT);
        }

        if (bytes_to_copy == total_bytes - written_bytes) {
            is_last_line = true;
        } else {
            /* if it's not the last line , and line breaking is not allowed,
             * return */
            if (!text->bAllowLineBreaking) {
                *fitting_len = bytes_to_copy;
                break;
            }
            /* if there is a space and this is not the last line we want to
             * break the line in the last space*/
            for (i = bytes_to_copy; i > 0; i--) {
                if ((text->text + written_bytes)[i] == ' ') {
                    bytes_to_copy = i;  // not including the space
                    written_bytes++;    // including the space
                    break;
                }
            }
        }

        line_bytes_len[curr_line] = bytes_to_copy;
        written_bytes += bytes_to_copy;
        *fitting_len = written_bytes;
        curr_line++;
    }

    if (!is_last_line) {
        TUILOGE("render_text: text string is too long for the given rectangle");
        EXITV(SEC_RENDER_STRING_TOO_BIG);
    }

    if (bToRender) {
        /* calculates where to start the first line */
        retVal =
            calc_position(text->v_align, text->h_align, rect_hight, rect_width,
                          font_h * curr_line, rect_width, &x, &first_line_y);
        if (retVal < 0) {
            TUILOGE("render_text: failed calc_position for %d lines, returned %d",
                curr_line, retVal);
            EXITV(retVal);
        }

        written_bytes = 0;

        for (i = 0; i < curr_line; i++) {
            /* skips space in the head of line */
            if (' ' == (text->text + written_bytes)[0] && (i > 0)) {
                written_bytes++;
            }

            /* get line width in pixels to calculate position */
            line_width = gdImageStringVarWidth(
                NULL, ext_font, 0, 0,
                (unsigned char*)(text->text + written_bytes), line_bytes_len[i],
                text_color);

            retVal = calc_position(text->v_align, text->h_align, rect_hight,
                                   rect_width, font_h, line_width, &x, &y);
            if (retVal < 0) {
                TUILOGE("render_text: failed calc_position for line %d returned "
                       "%d, length = %d; rect =(%d,%d:%d,%d)",
                       i, retVal, line_bytes_len[i], rect->x1, rect->y1,
                       rect->x2, rect->y2);
                break;
            }

            line_width = gdImageStringVarWidth(
                img_ptr, ext_font, rect->x1 + x + margin,
                rect->y1 + first_line_y + margin + i * font_h,
                (unsigned char*)(text->text + written_bytes), line_bytes_len[i],
                text_color);
            written_bytes += line_bytes_len[i];
        }
    }

    EXITV(retVal);
}

/****************************************************************************************/

/**
  @brief
  Internal function that fills drawn box with text,
  the text has properties of color, alignments, font, and visibility.
  */
static sec_render_err_t fill_box_text(gdImage* img_ptr, Rect_t* rect,
                                      uint32_t margin, Text_t* text,
                                      uint32_t bToRender, uint32_t* fitting_len,
                                      uint32_t* missing_char)
{
    Text_t text_to_render;
    sec_render_err_t retVal;
    uint32_t i, text_bytes_length, last_char_idx = 0, hidden_char_length,
                                   curr_idx = 0;
    ENTER;

    if ((NULL == img_ptr && bToRender) || NULL == rect || NULL == text ||
        NULL == fitting_len || NULL == missing_char) {
        TUILOGE("fill_box_text got null input; img_ptr = %d, rect = %d, text = %d",
            img_ptr, rect, text);
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }
    *fitting_len = 0;
    *missing_char = 0;

    text_to_render = *text;

    /* check if label is empty */
    if (0 == text->curr_len) {
        TUILOGD("fill_box_text got zero length text, nothing to render");
        EXITV(SEC_RENDER_SUCCESS);
    }

    /* get the bytes length of the text, and the index of the last character */
    retVal = get_utf8_len(text->text, text->curr_len, &text_bytes_length,
                          &last_char_idx);
    if (retVal < 0) {
        TUILOGE("fill_box_text: get_utf8_len returned %d",
               retVal);
        EXITV(SEC_RENDER_BAD_STRING);
    }

    if (TEXT_SHOW_ONLY_LAST == text->visibility.visible ||
        TEXT_HIDE_ALL == text->visibility.visible) {
        unsigned char
            displayText[MAX_HIDDEN_FIELD_LEN_IN_CHARS * UTF8_MAX_CHAR_LEN];

        /* get the bytes length of the hidden character */
        retVal = get_utf8_len(text->visibility.hiddenChar.enc_bytes, 1,
                              &hidden_char_length, NULL);
        if (retVal < 0) {
            TUILOGE("fill_box_text: get_utf8_len returned %d for hidden char",
                   retVal);
            EXITV(SEC_RENDER_BAD_STRING);
        }

        if (text->curr_len > MAX_HIDDEN_FIELD_LEN_IN_CHARS) {
            TUILOGE("fill_box_text: got too long hidden string length = %d, "
                   "MAX_HIDDEN_FIELD_LEN_IN_CHARS = %d",
                   text->curr_len, MAX_HIDDEN_FIELD_LEN_IN_CHARS);
            EXITV(SEC_RENDER_GOT_BAD_INPUT);
        }

        /* fill the display text buffer with the hidden character*/
        for (i = 0; i < text->curr_len; i++) {
            curr_idx = i * hidden_char_length;
            memscpy(displayText + curr_idx, UTF8_MAX_CHAR_LEN,
                    text->visibility.hiddenChar.enc_bytes, hidden_char_length);
        }

        retVal = render_text(NULL, rect, margin, &text_to_render, false,
                             fitting_len, missing_char);
        if (retVal != SEC_RENDER_SUCCESS) {
            TUILOGE("fill_box_text: render_text returned %d, fitting_len = %u, "
                   "missing_char = %u",
                   retVal, *fitting_len, *missing_char);
            EXITV(retVal);
        }

        text_to_render.text = displayText;

        if (TEXT_SHOW_ONLY_LAST == text->visibility.visible) {
            /* make the last character visible */
            memscpy(displayText + curr_idx, UTF8_MAX_CHAR_LEN,
                    text->text + last_char_idx,
                    text_bytes_length - last_char_idx);
        }

    } else if (text->visibility.visible != TEXT_SHOW_ALL) {
        TUILOGE("fill_box_text got unknown visibility");
        EXITV(SEC_RENDER_UNSUPPORTED);
    }

    retVal = render_text(img_ptr, rect, margin, &text_to_render, bToRender,
                         fitting_len, missing_char);
    if (retVal) {
        TUILOGE("fill_box_text: render_text returned %d, fitting_len = %u, "
               "missing_char = %u",
               retVal, *fitting_len, *missing_char);
    }

    EXITV(retVal);
}

/*************************************************************************************/

/**
  @brief

*/
static sec_render_err_t get_png_info(uint8_t* png_buffer,
                                     png_structp* png_struct_ptr,
                                     png_infop* info_ptr, uint32_t* imWidth,
                                     uint32_t* imHeight, int32_t* colorType)
{
    uint32_t status;
    int32_t bitdepth = 0;
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;

    *colorType = -1;
    *imWidth = 0;
    *imHeight = 0;
    *png_struct_ptr = NULL;
    *info_ptr = NULL;

    if (!png_check_sig(png_buffer, PNG_MAX_SIG_LEN)) {
        TUILOGE("render_object failed signature check using png_check_sig()");
        EXITV(SEC_RENDER_ERROR_PNG_SIG);
    }

    *png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (NULL == *png_struct_ptr) {
        TUILOGE("render_object failed reading png struct using "
               "png_create_read_struct()");
        EXITV(SEC_RENDER_ERROR_PNG_INFO);
    }
    do {
        *info_ptr = png_create_info_struct(*png_struct_ptr);
        if (NULL == *info_ptr) {
            TUILOGE("render_object failed creating png info struct using "
                   "png_create_info_struct()");
            retVal = SEC_RENDER_ERROR_PNG_INFO;
            break;
        }

        png_set_read_fn(*png_struct_ptr, png_buffer + PNG_MAX_SIG_LEN,
                        read_data_fn);
        png_set_sig_bytes(*png_struct_ptr, PNG_MAX_SIG_LEN);
        png_read_info(*png_struct_ptr, *info_ptr);

        status = png_get_IHDR(*png_struct_ptr, *info_ptr, imWidth, imHeight,
                              &bitdepth, colorType, NULL, NULL, NULL);

        if (PNG_HEADER_SUCCESS != status) {
            TUILOGE("render_object failed getting png header using png_get_IHDR()");
            retVal = SEC_RENDER_ERROR_PNG_INFO;
            break;
        }

        if (*colorType != PNG_COLOR_TYPE_RGB &&
            *colorType != PNG_COLOR_TYPE_RGBA) {
            TUILOGE("%s received unsupported color type = %d",
                   __func__, *colorType);
            retVal = SEC_RENDER_UNSUPPORTED;
            break;
        }

        if (*imWidth > MAX_IMAGE_LINE_WIDTH_IN_PIXELS) {
            TUILOGE("%s received too big png width = %d, "
                   "MAX_IMAGE_LINE_WIDTH_IN_PIXELS = %d",
                   __func__, *imWidth, MAX_IMAGE_LINE_WIDTH_IN_PIXELS);
            retVal = SEC_RENDER_UNSUPPORTED;
            break;
        }

    } while (0);

    if (retVal) {
        png_destroy_read_struct(png_struct_ptr, info_ptr, NULL);
    }

    EXITV(retVal);
}

/**********************************************************************************/

/**
  @brief

*/
static sec_render_err_t render_layout_bg(LayoutObject_t bg_obj,
                                         gdImage* img_ptr, Rect_t obj_rect)
{
    png_structp png_struct_ptr = NULL;
    png_infop info_ptr = NULL;
    uint32_t imWidth, imHeight;
    int32_t colorType = -1;
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;

    if (NULL != bg_obj.bgImage.image_buffer) {
        if (NULL == bg_gdImage) {
            /* there is no pre-rendered background - so we have to read and
             * render the actual png image */
            retVal = get_png_info(bg_obj.bgImage.image_buffer, &png_struct_ptr,
                                  &info_ptr, &imWidth, &imHeight, &colorType);
            if (retVal < 0) {
                TUILOGE("get_png_info returned %d", retVal);
                EXITV(retVal);
            }
            retVal = ParseImage(img_ptr, png_struct_ptr, info_ptr, imWidth,
                                imHeight, bg_obj.rect.x1, bg_obj.rect.y1,
                                colorType, &obj_rect);
            png_destroy_read_struct(&png_struct_ptr, &info_ptr, NULL);

            if (retVal < 0) {
                TUILOGE("ParseImage returned %d", retVal);
                EXITV(retVal);
            }
        } else {
            /* pre-rendered background is available, so copy the clipped area
             * only */
            if (gdImageCompatibleCopy(img_ptr, bg_gdImage, obj_rect.x1,
                                      obj_rect.y1, obj_rect.x2,
                                      obj_rect.y2) != GD_TRUE) {
                TUILOGE("Error while copying pre-rendered background image");
                EXITV(SEC_RENDER_GD_IMAGE_ERROR);
            }
        }
        EXITV(SEC_RENDER_SUCCESS);

    } else {
        sec_render_err_t rv = draw_box(img_ptr, &obj_rect, bg_obj.bgColor, 0, bg_obj.bgColor);
        EXITV(rv);
    }
}
/*****************************************************************************************************************/

/**
  @brief
  Internal function for calculating the proportion to resize an image.
  */
static sec_render_err_t calc_resize_prop(uint32_t destW, uint32_t destH,
                                         uint32_t srcW, uint32_t srcH,
                                         uint32_t* resizedW, uint32_t* resizedH)
{
    double prop;
    ENTER;
    if (destW > srcW && destH > srcH) {
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    prop = MIN((double)destW / srcW, (double)destH / srcH);

    *resizedW = (uint32_t)MIN((double)srcW * prop, destW);
    *resizedH = (uint32_t)MIN((double)srcH * prop, destH);

    TUILOGD("calc_resize_prop recieved dest (obj) = %d x %d, src (image) = %d x %d",
        destW, destH, srcW, srcH);
    TUILOGD("calc_resize_prop resized image = %d x %d",
           *resizedW, *resizedH);

    EXITV(SEC_RENDER_SUCCESS);
}

/************************************************************************************/

/**
  @brief
  Internal function that loads a font for a layout object
  */
static sec_render_err_t prepare_font_for_object(LayoutObject_t* obj)
{
    font_mgr_err_t retVal;
    gdFontExt* gdFont;
    ENTER;

    if (NULL == obj->label.text) {
        /* if the objects text buffer is NULL , the buffer size should be zero
           non-zero buffer size should not be allowed*/
        if (obj->label.max_buff_size) {
            EXITV(SEC_RENDER_GOT_BAD_INPUT);
        }
        EXITV(SEC_RENDER_SUCCESS);
    }
    if (NULL == obj->label.fontPath || 0 == strlen(obj->label.fontPath)) {
        // for static objects fontPath can be NULL , for dynamic it gets
        // generated
        EXITV(SEC_RENDER_SUCCESS);
    }
    gdFont = (gdFontExt*)(obj->label.font);
    retVal = font_mgr_prepare_gdfont(obj->label.fontPath, obj->label.text,
                                     obj->label.curr_len, &gdFont);
    if (retVal != FONT_MGR_SUCCESS) {
        TUILOGE("prepare_font_for_object font_manager_create_gdfont returned %d",
               retVal);
        EXITV(SEC_RENDER_FONT_ERROR);
    }

    obj->label.font = gdFont;

    EXITV(SEC_RENDER_SUCCESS);
}

/*****************************************************************************************/

static uint32_t get_text_margin(LayoutObject_t obj)
{
    if (LAYOUT_OBJECT_TYPE_INPUT == obj.type &&
        LAYOUT_OBJECT_RENDER_MODE_COLORS == obj.input.renderMode) {
        return MAX(obj.input.focusFrame.frameWidth,
                   obj.input.normalFrame.frameWidth);
    } else if (LAYOUT_OBJECT_TYPE_BUTTON == obj.type) {
        return MAX(obj.button.stateColor[LAYOUT_BUTTON_STATUS_RELEASED]
                       .frame.frameWidth,
                   MAX(obj.button.stateColor[LAYOUT_BUTTON_STATUS_PRESSED]
                           .frame.frameWidth,
                       obj.button.stateColor[LAYOUT_BUTTON_STATUS_DISABLED]
                           .frame.frameWidth));
    }
    return obj.frame.frameWidth;
}

/*****************************************************************************************/
static sec_render_err_t render_image(LayoutObject_t obj, gdImage* img_ptr,
                                     uint8_t* image_buffer, uint8_t bToRender,
                                     uint32_t* imWidth, uint32_t* imHeight)
{
    png_structp png_struct_ptr = NULL;
    png_infop info_ptr = NULL;
    uint32_t x = 0, y = 0, obj_w, obj_h;
    int32_t colorType = -1;
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;

    if ((NULL == img_ptr && bToRender) || NULL == imWidth || NULL == imHeight) {
        TUILOGE("%s got null input", __func__);
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }
    obj_w = obj.rect.x2 - obj.rect.x1 + 1;
    obj_h = obj.rect.y2 - obj.rect.y1 + 1;

    if (NULL == image_buffer) {
        TUILOGD("render_object got null image buffer - no image to render");
        EXITV(SEC_RENDER_SUCCESS);
    }

    retVal = get_png_info(image_buffer, &png_struct_ptr, &info_ptr, imWidth,
                          imHeight, &colorType);
    if (retVal < 0) {
        TUILOGE("get_png_info returned %d", retVal);
        EXITV(retVal);
    }

    do {
        /*check if the image fits the object, and calculates the offsets */
        retVal = calc_position(obj.bgImage.v_align, obj.bgImage.h_align, obj_h,
                               obj_w, *imHeight, *imWidth, &x, &y);
        if (retVal < 0) {
            TUILOGE("render_object: failed calc_position for object = %s, "
                   "returned %d",
                   obj.name, retVal);
            break;
        }

        if (bToRender) {
            retVal = ParseImage(img_ptr, png_struct_ptr, info_ptr, *imWidth,
                                *imHeight, obj.rect.x1 + x, obj.rect.y1 + y,
                                colorType, NULL);
            if (retVal < 0) {
                TUILOGE("ParseImage returned %d", retVal);
                break;
            }
        }

    } while (0);

    png_destroy_read_struct(&png_struct_ptr, &info_ptr, NULL);
    EXITV(retVal);
}

/*****************************************************************************************/
sec_render_err_t check_render_image_object(LayoutObject_t obj,
                                           const uint8_t* image_buffer,
                                           uint32_t* imWidth,
                                           uint32_t* imHeight)
{
    return render_image(obj, NULL, (uint8_t*)image_buffer, false, imWidth,
                        imHeight);
}

/*****************************************************************************************/
/**
  @brief
  Internal function that renders a general object to the secure buffer, it
  checks whether the dirty bit is on,
  if it is, it renders the box of the object, fills it with a background image
  and text.
  */
static sec_render_err_t render_object(LayoutObject_t obj, gdImage* img_ptr)
{
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    uint32_t text_margin, fitting_len = 0, missing_char = 0, obj_w, obj_h,
                          imWidth, imHeight;
    ENTER;

    if (NULL == img_ptr) {
        TUILOGE("render_object got null gdImage pointer");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    obj_w = obj.rect.x2 - obj.rect.x1 + 1;
    obj_h = obj.rect.y2 - obj.rect.y1 + 1;

    retVal = draw_box(img_ptr, &(obj.rect), obj.bgColor, obj.frame.frameWidth,
                      obj.frame.frameColor);
    if (retVal < 0) {
        TUILOGE("render_object - calling draw_box returned %d", retVal);
        EXITV(retVal);
    }

    retVal = render_image(obj, img_ptr, obj.bgImage.image_buffer, true,
                          &imWidth, &imHeight);
    if (retVal < 0) {
        TUILOGE("%s: render_image  (%u x %u) draw_box returned %d", __func__,
               imWidth, imHeight, retVal);
        EXITV(retVal);
    }

    text_margin = get_text_margin(obj);

    if ((LAYOUT_OBJECT_TYPE_INPUT == obj.type) &&
        obj.input.default_label.curr_len > 0) {
        uint8_t* img_buffer = &(img_array[0]);
        gdImage* helper_im;
        uint64_t obj_size = (uint64_t)obj_w * obj_h;
        Rect_t obj_rect_in_zero;

        /* check the limit of the allocated buffer for resizing */
        if (obj_size > MAX_IMAGE_SIZE_IN_PIXELS) {
            TUILOGE("render_object cannot examine the default label length "
                   "against the input object."
                   "input object is too big (%d x %d), the maximal size in "
                   "pixels the helper image is MAX_IMAGE_SIZE_IN_PIXELS = %d",
                   obj_w, obj_h, MAX_IMAGE_SIZE_IN_PIXELS);
            EXITV(SEC_RENDER_ALLOCATION_ERROR);
        }

        helper_im = gdImageCreateTrueColorExt(obj_w, obj_h, (int**)&img_buffer);
        if (NULL == helper_im) {
            TUILOGE("render_object failed to create helper gd image using "
                   "gdImageCreateTrueColorExt()");
            EXITV(SEC_RENDER_GD_IMAGE_ERROR);
        }

        obj_rect_in_zero.x1 = 0;
        obj_rect_in_zero.x2 = obj.rect.x2 - obj.rect.x1;
        obj_rect_in_zero.y1 = 0;
        obj_rect_in_zero.y2 = obj.rect.y2 - obj.rect.y1;

        retVal = fill_box_text(helper_im, &(obj_rect_in_zero), text_margin,
                               &(obj.input.default_label), true, &fitting_len,
                               &missing_char);

        gdImageDestroyEx(helper_im);

        if (retVal < 0) {
            TUILOGE("render_object - calling fill_box_text returned %d when the "
                   "rendering of the default label was examined",
                   retVal);
            EXITV(retVal);
        }
    }

    if ((LAYOUT_OBJECT_TYPE_INPUT == obj.type) && (0 == obj.label.curr_len)) {
        retVal = fill_box_text(img_ptr, &(obj.rect), text_margin,
                               &(obj.input.default_label), true, &fitting_len,
                               &missing_char);
    } else {
        retVal = fill_box_text(img_ptr, &(obj.rect), text_margin, &(obj.label),
                               true, &fitting_len, &missing_char);
    }
    if (retVal < 0) {
        TUILOGE("render_object - calling fill_box_text returned %d for len %u",
               retVal, obj.label.curr_len);
        EXITV(retVal);
    }

    EXITV(SEC_RENDER_SUCCESS);
}
/***********************************************************************************/

sec_render_err_t check_render_text_object(
    LayoutObject_t obj, const uint8_t* text, uint32_t len, uint8_t check_hint,
    uint32_t* fitting_len, uint32_t* missing_char, uint32_t* char_length)
{
    Text_t text_info = {0};
    uint32_t text_margin = get_text_margin(obj);
    sec_render_err_t retval;
    ENTER;

    if (NULL == text || NULL == fitting_len || NULL == missing_char ||
        NULL == char_length) {
        TUILOGE("check_render_text_object got null parameters");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (check_hint && obj.type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("check_render_text_object: got hint for non-input object");
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    if (check_hint) {
        text_info = obj.input.default_label;
    } else {
        text_info = obj.label;
    }

    text_info.text = (uint8_t*)text;
    text_info.max_buff_size = len;
    text_info.curr_len = gdGetUTF8StringLen(text_info.text, len);

    if (0 == text_info.curr_len && len != 0) {
        TUILOGE("%s: gdGetUTF8StringLen returned 0 for %u byte length string",
               __func__, len);
        EXITV(SEC_RENDER_BAD_STRING);
    }

    retval = fill_box_text(NULL, &(obj.rect), text_margin, &text_info, false,
                           fitting_len, missing_char);
    *char_length = gdGetUTF8StringLen(text_info.text, *fitting_len);
    if (0 == *char_length && *fitting_len != 0) {
        TUILOGE("%s: gdGetUTF8StringLen returned 0 for %u byte length string",
               __func__, *fitting_len);
        EXITV(SEC_RENDER_BAD_STRING);
    }
    EXITV(retval);
}

/*****************************************************************************************/
/**
  @brief
  Internal function that renders a button object,
  it draws the needed box according to the button status using draw_box()
  also, it fills the box with the button label (lower/upper) using
  fill_box_text()
  */
static sec_render_err_t render_button_object(LayoutObject_t buttonObj,
                                             gdImage* img_ptr)
{
    ENTER;
    sec_render_err_t rv;
    if (NULL == img_ptr) {
        TUILOGE("render_button_object got null gdImage pointer");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (buttonObj.type != LAYOUT_OBJECT_TYPE_BUTTON) {
        TUILOGE("render_button_object got object of a wrong type %d",
               buttonObj.type);
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    buttonObj.label.color =
        buttonObj.button.stateColor[buttonObj.button.status].fgColor;

    if (LAYOUT_OBJECT_RENDER_MODE_COLORS == buttonObj.button.renderMode) {
        /* set the colors according to the status */
        buttonObj.bgColor =
            buttonObj.button.stateColor[buttonObj.button.status].bgColor;
        buttonObj.frame =
            (buttonObj.button.stateColor[buttonObj.button.status]).frame;
        buttonObj.bgImage.image_buffer = NULL;
    } else {
        /* render the button using an image according to the button state */
        buttonObj.bgImage =
            buttonObj.button.stateBgImage[buttonObj.button.status];
    }

    rv = render_object(buttonObj, img_ptr);
    EXITV(rv);
}
/***********************************************************************************/

/**
  @brief
  Internal function that renders an input object, it draws the needed box
  according to the status flag
  also, it fills the box with the current input (lower/upper) using
  fill_box_text(), with the needed visibility.
  */
static sec_render_err_t render_input_object(LayoutObject_t inputObj,
                                            gdImage* img_ptr)
{
    sec_render_err_t rv;
    ENTER;

    if (NULL == img_ptr) {
        TUILOGE("render_input_object got null gdImage pointer");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (inputObj.type != LAYOUT_OBJECT_TYPE_INPUT) {
        TUILOGE("render_input_object got object of a wrong type %d",
               inputObj.type);
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    if (LAYOUT_OBJECT_RENDER_MODE_COLORS == inputObj.input.renderMode) {
        inputObj.bgImage.image_buffer = NULL;
        /* set the frame according to the focus */
        if (LAYOUT_INPUT_STATUS_SELECTED == inputObj.input.status) {
            inputObj.frame = inputObj.input.focusFrame;
        } else {
            inputObj.frame = inputObj.input.normalFrame;
        }
    } else {
        /* render the background image according to the focus */
        if (LAYOUT_INPUT_STATUS_SELECTED == inputObj.input.status) {
            inputObj.bgImage = inputObj.input.focusImage;
        } else {
            inputObj.bgImage = inputObj.input.normalImage;
        }
    }

    rv = render_object(inputObj, img_ptr);
    EXITV(rv);
}

/*******************************************************************************/

/**
  @brief
  Internal function that renders a keyboard to the secure buffer.
  */
static sec_render_err_t render_keyboard(LayoutObject_t layoutBg,
                                        LayoutObject_t keyboardBg,
                                        LayoutKeyboard_t* keyboard,
                                        gdImage* img_ptr, uint8_t renderAllKeys)
{
    uint32_t i;
    sec_render_err_t retVal;
    ENTER;

    if (NULL == img_ptr) {
        TUILOGE("render_keyboard got null gd image pointer");
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (renderAllKeys) {
        /* first erase the keyboard zone */
        retVal = render_layout_bg(layoutBg, img_ptr, keyboardBg.rect);
        if (retVal != SEC_RENDER_SUCCESS) {
            TUILOGE("render_keyboard failed erasing keyboard zone, "
                   "render_layout_bg returned  %d",
                   retVal);
            EXITV(retVal);
        }

        retVal = render_object(keyboardBg, img_ptr);
        if (retVal != SEC_RENDER_SUCCESS) {
            TUILOGE("render_keyboard failed rendering keyboard background, "
                   "render_object returned  %d",
                   retVal);
            EXITV(retVal);
        }
    }

    for (i = 0; i < keyboard->number; i++) {
        /* goes over the objects in the keyboard, and renders each if it's
         * dirty, or renders them all if renderAllKeys flag is true */
        if (renderAllKeys || keyboard->objects[i].dirty) {
            if (!renderAllKeys) {
#ifdef SUPPORT_TRANSPARENCY
                retVal = render_layout_bg(layoutBg, img_ptr,
                                          keyboard->objects[i].rect);
                if (retVal != SEC_RENDER_SUCCESS) {
                    TUILOGE("render_keyboard failed erasing object (1st step), "
                           "render_layout_bg returned  %d",
                           retVal);
                    EXITV(retVal);
                }

                retVal = render_layout_bg(keyboardBg, img_ptr,
                                          keyboard->objects[i].rect);
                if (retVal != SEC_RENDER_SUCCESS) {
                    TUILOGE("render_keyboard failed erasing object (2nd step), "
                           "render_layout_bg returned  %d",
                           retVal);
                    EXITV(retVal);
                }
#endif
            }
            TUILOGD("render_keyboard is rendering button %s, dirty = %d",
                   keyboard->objects[i].name, keyboard->objects[i].dirty);
            retVal = render_button_object(keyboard->objects[i], img_ptr);
            if (retVal != SEC_RENDER_SUCCESS) {
                TUILOGE("render_keyboard failed rendering button, "
                       "render_button_object returned  %d",
                       retVal);
                EXITV(retVal);
            }
            keyboard->objects[i].dirty = false;
        }
    }

    EXITV(SEC_RENDER_SUCCESS);
}
/************************************************************************************/

/**
  @brief
  External function for rendering the current layout bg image to an external
  buffer. This is an optimization
  to allow faster rendering time of transparent objects.
  */

sec_render_err_t render_bg_image(LayoutPage_t* layout,
                                 const screen_surface_t* surface)
{
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    ENTER;

    if (NULL == layout || NULL == surface || NULL == surface->buffer) {
        TUILOGE("render_bg_image failed to create gd image using "
               "gdImageCreateTrueColorExt()");
        EXITV(SEC_RENDER_GD_IMAGE_ERROR);
    }

    if (bg_gdImage != NULL) {
        gdImageDestroyEx(bg_gdImage);
        bg_gdImage = NULL;
    }

    bg_gdImage = gdImageCreateTrueColorExt(surface->width, surface->height,
                                           (int**)(&surface->buffer));
    if (NULL == bg_gdImage) {
        TUILOGE("render_bg_image failed to create gd image using "
               "gdImageCreateTrueColorExt()");
        EXITV(SEC_RENDER_GD_IMAGE_ERROR);
    }

    retVal = render_object(layout->layoutBg, bg_gdImage);
    if (retVal != SEC_RENDER_SUCCESS) {
        TUILOGE("render_bg_image failed rendering layout background, "
               "render_object returned  %d",
               retVal);
        EXITV(retVal);
    }
    retVal = render_object(layout->keyboardBg, bg_gdImage);
    if (retVal != SEC_RENDER_SUCCESS) {
        TUILOGE("render_bg_image failed rendering keyboard background, "
               "render_object returned  %d",
               retVal);
        EXITV(retVal);
    }

    EXITV(SEC_RENDER_SUCCESS);
}

/*****************************************************************************************/
/**
  @brief
  External function to release the current layout's bg image
  rendered previously using render_bg_image.
  */

void release_bg_image()
{
    if (bg_gdImage != NULL) {
        gdImageDestroyEx(bg_gdImage);
        bg_gdImage = NULL;
    }
}

/***************************************************************************************/

/**
  @brief
  External function that goes over a layout and renders the layout objects into
  the current secure buffer,
  each object will be rendered only if dirty bit is on.
  */

sec_render_err_t render_layout(LayoutPage_t* layout,
                               const screen_surface_t* surface)
{
    uint32_t i;
    sec_render_err_t retVal = SEC_RENDER_SUCCESS;
    uint32_t changed = 0;
    gdImage* img_rect = NULL;
    ENTER;

    if (NULL == surface || NULL == layout || NULL == surface->buffer) {
        TUILOGE("render_layout got null input surface = %d, layout = %d",
               surface, layout);
        EXITV(SEC_RENDER_ERROR_NULL_INPUT);
    }

    if (surface->pixelWidth != RGBA_BYTES_PER_PIXEL) {
        TUILOGE("render_layout got pixelWidth that doesn't match the lower "
               "rendering layer (libgd), pixelWidth = %d",
               surface->pixelWidth);
        EXITV(SEC_RENDER_UNSUPPORTED);
    }

    if (0 == surface->width || 0 == surface->height) {
        TUILOGE("render_layout got bad screen properties height = %d, width = %d",
            surface->height, surface->width);
        EXITV(SEC_RENDER_GOT_BAD_INPUT);
    }

    /* create the gd image object with the secure buffer and the display
     * properties */
    img_rect = gdImageCreateTrueColorExt(surface->width, surface->height,
                                         (int**)(&surface->buffer));
    if (NULL == img_rect) {
        TUILOGE("render_layout failed to create gd image using "
               "gdImageCreateTrueColorExt()");
        EXITV(SEC_RENDER_GD_IMAGE_ERROR);
    }
    do {
        /* fills the buffer with the background color if renderAll is true */
        if (layout->renderAll) {
            retVal = render_object(layout->layoutBg, img_rect);
            if (retVal != SEC_RENDER_SUCCESS) {
                TUILOGE("render_layout failed rendering layout background, "
                       "render_object returned  %d",
                       retVal);
                break;
            }
            changed = 1;
        }

        else {
            /* delete the objects that don't need to be shown and render the
             * background of the dirty objects */
            for (i = 0; i < layout->number; i++) {
                if (layout->objects[i].dirty) {
#ifdef SUPPORT_TRANSPARENCY
                    retVal = render_layout_bg(layout->layoutBg, img_rect,
                                              layout->objects[i].rect);
                    if (retVal != SEC_RENDER_SUCCESS) {
                        TUILOGE("render_layout failed erasing object, "
                               "render_layout_bg returned  %d",
                               retVal);
                        break;
                    }
#endif

                    if (!layout->objects[i].show) {
#ifndef SUPPORT_TRANSPARENCY
                        retVal = render_layout_bg(layout->layoutBg, img_rect,
                                                  layout->objects[i].rect);
                        if (retVal != SEC_RENDER_SUCCESS) {
                            TUILOGE("render_layout failed erasing object, "
                                   "render_layout_bg returned  %d",
                                   retVal);
                            break;
                        }
#endif
                        layout->objects[i].dirty = 0;
                    }
                    changed = 1;
                }
            }
            if (retVal != SEC_RENDER_SUCCESS) {
                break;
            }
        }

        /* renders each object */
        for (i = 0; i < layout->number; i++) {
            if ((layout->renderAll || layout->objects[i].dirty) &&
                layout->objects[i].show) {
                TUILOGD("render_layout object %s",
                       layout->objects[i].name);

                if (LAYOUT_OBJECT_TYPE_TEXT == layout->objects[i].type ||
                    LAYOUT_OBJECT_TYPE_BUTTON == layout->objects[i].type) {
                    retVal = prepare_font_for_object(&layout->objects[i]);
                    if (retVal != SEC_RENDER_SUCCESS) {
                        TUILOGE("render_layout failed, prepare_font_for_object "
                               "returned  %d",
                               retVal);
                        break;
                    }
                }

                switch (layout->objects[i].type) {
                    case LAYOUT_OBJECT_TYPE_BUTTON:
                        retVal =
                            render_button_object(layout->objects[i], img_rect);
                        if (retVal != SEC_RENDER_SUCCESS) {
                            TUILOGE("render_layout failed, render_button_object "
                                   "returned  %d",
                                   retVal);
                        }
                        break;
                    case LAYOUT_OBJECT_TYPE_INPUT:
                        retVal =
                            render_input_object(layout->objects[i], img_rect);
                        if (retVal != SEC_RENDER_SUCCESS) {
                            TUILOGE("render_layout failed, render_input_object "
                                   "returned  %d",
                                   retVal);
                        }
                        break;
                    default: /* image object or a label */
                        retVal = render_object(layout->objects[i], img_rect);
                        if (retVal != SEC_RENDER_SUCCESS) {
                            TUILOGE("render_layout failed, render_object "
                                   "returned  %d",
                                   retVal);
                        }
                        break;
                }

                if (retVal != SEC_RENDER_SUCCESS) {
                    break;
                }
                layout->objects[i].dirty = 0;
                changed = 1;
            }
        }

        if (retVal != SEC_RENDER_SUCCESS) {
            break;
        }

        /* renders the keyboard */
        if ((layout->activeKeyboard < layout->numberKeyboards) &&
            (layout->numberKeyboards < MAX_KEYBOARD_LAYOUTS)) {
            /* render the active keyboard */
            retVal = render_keyboard(
                layout->layoutBg, layout->keyboardBg,
                &(layout->keyboards[layout->activeKeyboard]), img_rect,
                layout->renderAll || layout->dirtyKeyboard);
            if (retVal != SEC_RENDER_SUCCESS) {
                TUILOGE("render_layout failed for keyboard %d, render_keyboard "
                       "returned  %d",
                       layout->activeKeyboard, retVal);
                break;
            }
            layout->dirtyKeyboard = 0;
            changed = 1;
        }

        if (layout->renderAll) {
            layout->renderAll = 0;
        }

    } while (0);

    gdImageDestroyEx(img_rect);
    if (retVal != SEC_RENDER_SUCCESS) {
        EXITV(retVal);
    }

    if (changed) {
        EXITV(SEC_RENDER_SUCCESS);
    } else {
        EXITV(SEC_RENDER_UNCHANGED);
    }
}
