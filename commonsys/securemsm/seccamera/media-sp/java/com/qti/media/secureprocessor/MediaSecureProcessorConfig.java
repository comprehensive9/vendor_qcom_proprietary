/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qti.media.secureprocessor;

import android.util.Log;
import java.util.*;

/**
 * The concept for configuration data marshaling and
 * de-marshaling is borrowed from AOSP Camera MetaData.
 * Reference:
 *   platform/system/media/camera/src/camera_metadata.h[/c]
 *
 */

/**
 * MediaSecureProcessorConfig
 *   This class is a helper class to marshal and de-marshal the
 *   secure processor configurations data to the backing buffer.
 *   An instance of this class is used by {@link
 *   MediaSecureProcessor} class to set/get config
 *   data to HAL service for further consumption by a selected
 *   secure data processing destination. A config entry is
 *   defined a <tag, value> pair. This class provides interface
 *   for marshaling (to be used by client) and de-marshaling (to
 *   be used by service) of config data for a setConfig
 *   operation.
 *
 */
public final class MediaSecureProcessorConfig {
  private static final String TAG = "MediaSecureProcessorConfig";

  /**
   * ConfigEntry
   *   This class is used to return the config entry present in
   *   the config buffer. It consists of TAG, it's TYPE, data size
   *   and actual DATA. Based on the config tag type, the
   *   corresponding data array will hold the config data i.e. if
   *   config data type is U8, u8Data array will hold the config
   *   data of U8 type.
   */
  public static final class ConfigEntry {
    public int tag;
    public int type;
    public int count;
    public byte[] u8Data;
    public int[] i32Data;
    public long[] i64Data;

    public ConfigEntry(int tag_, int type_, int count_, byte[] u8_) {
      tag = tag_;
      type = type_;
      count = count_;
      u8Data = u8_;
    }
    public ConfigEntry(int tag_, int type_, int count_, int[] i32_) {
      tag = tag_;
      type = type_;
      count = count_;
      i32Data = i32_;
    }
    public ConfigEntry(int tag_, int type_, int count_, long[] i64_) {
      tag = tag_;
      type = type_;
      count = count_;
      i64Data = i64_;
    }
  };

  /**
   * Secure processor config classification
   *   Secure processor configs are classified into following
   *   three sections -
   *   1. Image configs: These configs are applicable to
   *                     associated image and can change from
   *                     image to image like timestamp,
   *                     frame_number, exposure_time etc.
   *   2. Session configs: These configs are applied to requested
   *                       session.
   *   3. Custom configs: Clients are allows to add custom
   *                      configs which are transparent to secure
   *                      processor interface. The custom config
   *                      (tag) definitions shall start from
   *                      SECURE_PROCESSOR_CUSTOM_CONFIG_OFFSET.
   *                      Clients are expected to register custom
   *                      configs using setCustomConfigs() method.
   *
   */

  /**
   * Config section: Image configuration.
   * Description: Camera identifier.
   * Type: int32_t
   * Requirement: Mandatory image configuration.
   */
  public static final String IMAGE_CONFIG_CAMERA_ID =
      "secureprocessor.image.config.camera_id";

  /**
  * Config section: Image configuration.
  * Description: Frame number.
  * Type: int64_t
  * Requirement: Mandatory image configuration.
  */
  public final static String IMAGE_CONFIG_FRAME_NUMBER =
      "secureprocessor.image.config.frame_number";

  /**
   * Config section: Image configuration.
   * Description: Frame timestamp.
   * Type: int64_t
   * Requirement: Mandatory image configuration.
   */
  public final static String IMAGE_CONFIG_TIMESTAMP =
      "secureprocessor.image.config.timestamp";

  /**
   * Config section: Image configuration.
   * Description: Frame buffer width.
   * Type: int32_t
   * Requirement: Mandatory image configuration.
   */
  public final static String IMAGE_CONFIG_FRAME_BUFFER_WIDTH =
      "secureprocessor.image.config.frame_buffer_width";

  /**
  * Config section: Image configuration.
  * Description: Frame buffer height.
  * Type: int32_t
  * Requirement: Mandatory image configuration.
  */
  public final static String IMAGE_CONFIG_FRAME_BUFFER_HEIGHT =
      "secureprocessor.image.config.frame_buffer_height";

  /**
   * Config section: Image configuration.
   * Description: Frame buffer stride.
   * Type: int32_t
   * Requirement: Mandatory image configuration.
   */
  public final static String IMAGE_CONFIG_FRAME_BUFFER_STRIDE =
      "secureprocessor.image.config.frame_buffer_stride";

  /**
   * Config section: Image configuration.
   * Description: Frame buffer format
   *              (expects android_pixel_format_t)
   * Type: int32_t
   * Requirement: Mandatory image configuration.
   */
  public final static String IMAGE_CONFIG_FRAME_BUFFER_FORMAT =
      "secureprocessor.image.config.frame_buffer_format";

  /**
   * Config section: Session configuration.
   * Description: Num sensors required in usecase.
   * Type: int32_t
   * Requirement: Optional session configuration.
   *              (default value: 1)
   *              This configuration ensures that number of
   *              requested camera sensors are streaming in secure
   *              state before allowing any data-processing on
   *              secure data processor (aka secure destination).
   */
  public final static String SESSION_CONFIG_NUM_SENSOR =
      "secureprocessor.session.config.num_sensors";

  /**
   * Config section: Session configuration.
   * Description: Usecase identifier.
   * Type: int8_t[]
   * Requirement: Mandatory session configuration.
   *              It identifies the entities to be run on secure
   *              destination for secure data consumption
   *              [/processing].
   */
  public final static String SESSION_CONFIG_USECASE_IDENTIFIER =
      "secureprocessor.session.config.usecase_id";

  /**
   * Offset for custom configuration tag definitions.
   *
   * Clients are allows to add custom configurations which are
   * transparent to MediaSecureProcessorConfig interface. The
   * custom config tag definitions shall start from
   * CUSTOM_CONFIG_OFFSET.
   * Clients are expected to register custom configs using
   * setCustomConfigs() method before using them.
   */
  public final static int CUSTOM_CONFIG_OFFSET = 0x20000;

  /**
   * Method to add a new config entry in the config buffer
   * structure.
   *
   * @param tag selects the respective configTag to add the entry.
   * @param data byte array data to be added in the config buffer
   *             for this specific tag.
   * @param dataCount size of the data set being passed to add in
   *                  the config buffer.
   * @return 0 if the entry was added successfully in the
   *         configuration buffer.
   */
  public native int addEntry(String tag, byte[] data, int dataCount);

  /**
   * Method to add a new config entry in the config buffer
   * structure.
   *
   * @param tag selects the respective configTag to add the entry
   * @param data integer array data to be added in the config
   *             buffer for this specific tag
   * @param dataCount size of the data set being passed to add in
   *                  the config buffer
   * @return 0 if the entry was added successfully in the
   *         configuration buffer.
   */
  public native int addEntry(String tag, int[] data, int dataCount);

  /**
   * Method to add a new config entry in the config buffer
   * structure.
   *
   * @param tag selects the respective configTag to add the entry
   * @param data long array data to be added in the config buffer
   *             for this specific tag
   * @param dataCount size of the data set being passed to add in
   *                  the config buffer
   * @return 0 if the entry was added successfully in the
   *         configuration buffer.
   */
  public native int addEntry(String tag, long[] data, int dataCount);

  /**
   * Method to add a new config entry in the config buffer
   * structure.
   *
   * @param tag selects the respective configTag to add the entry
   * @param data String data to be added in the config buffer for
   *             this specific tag
   * @return 0 if the entry was added successfully in the
   *         configuration buffer.
   */
  public native int addEntry(String tag, String data);

  /**
   * Extract the entry from the config buffer for a given tag.
   *
   * @param tag Specifies the tag value whose entry is requested
   * @return object of ConfigEntry {@link
   *         MediaSecureProcessorConfig.ConfigEntry}
   */
  public native MediaSecureProcessorConfig.ConfigEntry getEntry(String tag);

  /**
   * Method to get the number of entries from config buffer.
   *
   * @return number of buffer entries
   */
  public native int getEntryCount();

  /**
   * Extract the entry from the config buffer for at a given
   * index.
   *
   * @param index specifies the indexed entry of the config buffer
   * @return object of ConfigEntry {@link
   *         MediaSecureProcessorConfig.ConfigEntry}
   */
  public native MediaSecureProcessorConfig.ConfigEntry
  getEntryByIndex(int index);

  /**
   * Method to clear the existing config entries from a config
   * buffer.
   *
   * @return 0 if the entries were cleared succesfully
   */
  public native int clear();

  /**
   * Method to verify if the config buffer is empty.
   *
   * @return true if the buffer is empty and vice versa
   */
  public native boolean isEmpty();

  /**
   * Method to check if a particular config is present in the
   * config buffer.
   *
   * @param tag specifies the tag value whose entry needs to be checked
   * @return true if entry for that tag is present or vice versa
   */
  public native boolean checkEntry(String tag);

  /**
   * Method to get the size of config buffer in bytes.
   *
   * @return size of buffer
   */
  public native int getSize();

  /**
   * Instantiate a MediaSecureProcessorConfig object for
   * configuration data marshaling.
   *
   * @param entryLimit Number of entries required to be added
   * @param dataLimit Size of config entry payload in bytes
   */
  public MediaSecureProcessorConfig(int entryLimit, int dataLimit) {
    mNativeHandle = createConfigBuffer(entryLimit, dataLimit);
  }

  /**
   * Instantiate a MediaSecureProcessorConfig object for
   * configuration data marshaling.
   *
   * @param entryLimit Number of entries required to be added
   */
  public MediaSecureProcessorConfig(int entryLimit) { this(entryLimit, 64); }

  /**
   * Instantiate a MediaSecureProcessorConfig object for
   * configuration data de-marshalling.
   *
   */
  public MediaSecureProcessorConfig() { mNativeHandle = 0; }

  /**
   * Method to set an existing marshalled buffer to
   * MediaSecureProcessorConfig object for de-marshalling.
   *
   * @param handle is a new SecureProcessorConfig object
   * @return 0 in case setting the native handle was successful
   */
  public int setNativehandle(long handle) {
    int result = -1;

    if (mNativeHandle == 0) {
      mNativeHandle = handle;
      result = 0;
    }

    return result;
  }

  /**
   * Method to return the stored native handle of
   * SecureProcessorConfig object.
   */
  public long getNativehandle() { return mNativeHandle; }

  /**
   * Method to add custom configuration tags to
   * MediaSecureProcessorConfig. These tags are expected to start
   * from CUSTOM_CONFIG_OFFSET.
   * Supported datatypes include - u8/u8[], i32/i32[], i64/i64[]
   * and string. Client is expected to set the custom config tags
   * before their usage.
   *
   * @param tagMap A map containing custom config tags and their
   *               values {@link java.util.Map<java.lang.String,
   *               java.lang.Integer>}
   * @return 0 incase the custom tags were added successfully
   */
  public int setCustomConfigs(Map<String, Integer> tagMap) {
    if (tagMap == null) {
      return -1;
    }

    Set<Map.Entry<String, Integer>> itr = tagMap.entrySet();
    for (Map.Entry<String, Integer> ptr : itr) {
      int key = ptr.getValue();
      if (key < CUSTOM_CONFIG_OFFSET) {
        Log.e(TAG, "Key value out of bounds for tag " + key);
        return -1;
      }
    }
    return insertCustomTags(tagMap);
  }

  /**
   * Private native methods
   */
  private native long createConfigBuffer(int entryLimit, int dataLimit);

  private native int insertCustomTags(Map<String, Integer> tagMap);

  private long mNativeHandle = 0;

  static { System.loadLibrary("mediasp_jni"); }
}
