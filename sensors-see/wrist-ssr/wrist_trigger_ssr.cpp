/*===========================================================================

 Copyright (c) 2021, Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ===========================================================================*/

#ifdef SNS_WEARABLES_TARGET_BG

#include <iostream>
#include <cutils/properties.h>
#include <cutils/log.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define LOG_TAG "sns_wrist_trigger_ssr"
#define SSR_BUS_ROOT "/sys/bus/msm_subsys/devices"
#define SSR_SETUP_PROP "vendor.sensors.wrist_ssr"
#define MAX_NUM_SUBSYS 2
#define SUBSYS_NAME_LEN 30
#define SUBSYS_PATH_LEN 100
#define ADSP_LEN 4
#define BG_LEN 7
#define RELATED_LEN 7

static char sns_subsys_list[MAX_NUM_SUBSYS][SUBSYS_NAME_LEN];

struct sns_ssr_node_map {
  char subsys_name[SUBSYS_NAME_LEN];
  char ssr_node[SUBSYS_PATH_LEN];
  bool write_back;
  char restart_level[SUBSYS_NAME_LEN];
};

struct sns_ssr_data {
  int count;
  sns_ssr_node_map subsys_map[MAX_NUM_SUBSYS];
};
static sns_ssr_data sns_ssr_info;

static const char *ADSP_SSR_SYSFS_PATH
            = "/sys/kernel/boot_adsp/ssr";
static const char *BG_SSR_SYSFS_PATH
            = "/sys/kernel/boot_bg/ssr";


typedef enum sns_wrist_rc
{
  RET_SUCCESS = 0,
  RET_FAILED,
}sns_wrist_rc;

/*
   Helper function to read and clean up data from the ssr sysfs
   nodes
*/
static int get_subsys_string(char *path, char *buf,int buf_size)
{
  int fd = -1;
  int str_size = 0;
  sns_wrist_rc rc = RET_SUCCESS;

  if (!path)
  {
    ALOGE("Invalid path string");
    rc = RET_FAILED;
  }
  else if (!buf || buf_size <= 0)
  {
    ALOGE("Invalid buf/buf size passed");
    rc = RET_FAILED;
  }
  else
  {
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
      ALOGE("Failed to open %s: %s", path, strerror(errno));
      rc = RET_FAILED;
    }
    else
    {
      memset(buf, '\0', buf_size);
      if (read(fd, buf, buf_size - 1) < 0)
      {
        ALOGE("Failed to read %s: %s", path, strerror(errno));
        rc = RET_FAILED;
      }
      else
      {
        str_size = strlen(buf);
        if (str_size == 0)
        {
          ALOGE("Read 0 length string from : %s", path);
          rc = RET_FAILED;
        }
        else
        {
          if (buf[str_size -1] == '\n')
          {
            buf[str_size -1] = '\0';
          }
        }
      }

      close(fd);
    }
  }

  return rc;
}

/*  Goes through the ssr devices directory and
    fills up the ssr_info structure with the
    subsys name/node mappings for adsp and bg-wear
*/
static int populate_subsys_map()
{
  DIR *dir_ssr_bus = NULL;
  struct dirent *de = NULL;
  char subsys_name_path[SUBSYS_PATH_LEN];
  char subsys_name[SUBSYS_NAME_LEN];
  sns_wrist_rc rc = RET_SUCCESS;

  memset((void*)&sns_ssr_info, 0, sizeof(sns_ssr_info));

  dir_ssr_bus = opendir(SSR_BUS_ROOT);
  if (!dir_ssr_bus)
  {
    ALOGE("Failed to open ssr root dir: %s", strerror(errno));
    rc = RET_FAILED;
  }
  else
  {
    // Go through /sys/bus/msm_subsys/devices dir
    while((de = readdir(dir_ssr_bus)))
    {
      if (de->d_name[0] == '.')
      {
        continue;
      }

      if (sns_ssr_info.count == MAX_NUM_SUBSYS)
      {
        break;
      }

      /*  Get the subsys name  */
      snprintf(subsys_name_path, sizeof(subsys_name_path),
               "%s/%s/name", SSR_BUS_ROOT, de->d_name);
      if (get_subsys_string(subsys_name_path, subsys_name,
                           sizeof(subsys_name)) != RET_SUCCESS)
      {
        ALOGE("failed to get_subsys_string");
        rc = RET_FAILED;
      }
      else
      {
        if (strlen(subsys_name) > 0 &&
            subsys_name[strlen(subsys_name) - 1] == '\n')
        {
          subsys_name[strlen(subsys_name) - 1] = '\0';
        }

        if((!strncmp(subsys_name, "adsp", ADSP_LEN)) ||
           (!strncmp(subsys_name, "bg-wear", BG_LEN)))
        {
          strlcpy(sns_ssr_info.subsys_map[sns_ssr_info.count].subsys_name, subsys_name,
                  sizeof(sns_ssr_info.subsys_map[sns_ssr_info.count].subsys_name));
          strlcpy(sns_ssr_info.subsys_map[sns_ssr_info.count].ssr_node, de->d_name,
                  sizeof(sns_ssr_info.subsys_map[sns_ssr_info.count].ssr_node));
          sns_ssr_info.count++;
        }
      }
    }

    closedir(dir_ssr_bus);
    dir_ssr_bus = NULL;
  }

  return rc;
}

static void trigger_adsp_ssr()
{
  int fd = -1;
  fd = open(ADSP_SSR_SYSFS_PATH, O_WRONLY);

  if (fd < 0)
  {
    ALOGE("failed to open sys/kernel/boot_adsp/ssr");
  }
  else
  {
    if (write(fd, "1", 1) > 0)
    {
      ALOGI("adsp ssr triggered successfully");
    }
    else
    {
      ALOGE("failed to write sys/kernel/boot_adsp/ssr");
      perror("Error: ");
    }

    close(fd);
  }
}

static void trigger_bg_ssr()
{
  int fd = -1;
  fd = open(BG_SSR_SYSFS_PATH, O_WRONLY);

  if (fd < 0)
  {
    ALOGE("failed to open sys/kernel/boot_bg/ssr");
  }
  else
  {
    if (write(fd, "1", 1) > 0)
    {
      ALOGI("bg ssr triggered successfully");
    }
    else
    {
      ALOGE("failed to write sys/kernel/boot_bg/ssr");
      perror("Error: ");
    }

    close(fd);
  }
}


static void setup_and_trigger_ssr(int num_subsys)
{
  char ssr_toggle_path[SUBSYS_PATH_LEN];
  int fd = -1;
  int i, j;

  for (i = 0; i < num_subsys; i++)
  {
    for(j = 0; j < sns_ssr_info.count; j++)
    {
      if(strncmp(sns_ssr_info.subsys_map[j].subsys_name, sns_subsys_list[i], strlen(sns_subsys_list[i])) != 0)
      {
        continue;
      }

      char subsys_path_name[SUBSYS_PATH_LEN];
      snprintf(subsys_path_name, sizeof(subsys_path_name),
               "%s/%s", SSR_BUS_ROOT, sns_ssr_info.subsys_map[j].ssr_node);
      ALOGD("subsys_path_name = %s", subsys_path_name);

      /*  Set ADSP and BG restart_level to related
          if it is not related and trigger SSR
      */
      struct stat stat_path;
      if(!stat(subsys_path_name, &stat_path))
      {
        snprintf(ssr_toggle_path, sizeof(ssr_toggle_path),
                 "%s/restart_level", subsys_path_name);

        fd = open(ssr_toggle_path, O_RDWR);
        if (fd < 0)
        {
          ALOGE("Failed to open %s : %s, continuing", ssr_toggle_path,
                            strerror(errno));
          continue;
        }

        if (read(fd, sns_ssr_info.subsys_map[j].restart_level, SUBSYS_NAME_LEN) < 0)
        {
          ALOGE("Failed to read from ssr node: %s", strerror(errno));
        }
        else
        {
          if(strncmp(sns_ssr_info.subsys_map[j].restart_level, "related", RELATED_LEN))
          {
            if (write(fd, "related", RELATED_LEN) < 0)
            {
              ALOGE("Failed to write to ssr node: %s", strerror(errno));
              close(fd);
              fd = -1;
              continue;
            }
            else
            {
              ALOGI("setting restart_level of %s to related successful", sns_ssr_info.subsys_map[i].ssr_node);
              sns_ssr_info.subsys_map[j].write_back = true;
            }
          }
          if(!strncmp(sns_ssr_info.subsys_map[j].subsys_name, "adsp", ADSP_LEN))
          {
            ALOGI("Trigering ADSP SSR");
            trigger_adsp_ssr();
            /*  After triggering ADSP SSR, wait for few ms
                so that BG is notified of ADSP SSR
            */
            usleep(200 * 1000);
          }
          else if(!strncmp(sns_ssr_info.subsys_map[j].subsys_name, "bg-wear", BG_LEN))
          {
            ALOGI("Triggering BG SSR");
            trigger_bg_ssr();
          }
        }

        close(fd);
        fd = -1;
      }
    }
  }

  /*  Need to wait for 1 sec after BG SSR is triggered  */
  sleep(1);
}

static void restore_restart_level()
{
  char ssr_toggle_path[SUBSYS_PATH_LEN];
  int fd = -1;
  int i;

  for (i = 0; i < sns_ssr_info.count; i++)
  {
    if(sns_ssr_info.subsys_map[i].write_back == false)
    {
      continue;
    }

    char subsys_path_name[SUBSYS_PATH_LEN];
    snprintf(subsys_path_name, sizeof(subsys_path_name),
             "%s/%s", SSR_BUS_ROOT, sns_ssr_info.subsys_map[i].ssr_node);

    /*  Set ADSP and BG restart_level to its original value  */
    struct stat stat_path;
    if(!stat(subsys_path_name, &stat_path))
    {
      snprintf(ssr_toggle_path, sizeof(ssr_toggle_path),
               "%s/restart_level", subsys_path_name);

      fd = open(ssr_toggle_path, O_WRONLY);
      if (fd < 0)
      {
        ALOGE("Failed to open %s : %s, continuing", ssr_toggle_path,
                     strerror(errno));
        continue;
      }

      if (write(fd, sns_ssr_info.subsys_map[i].restart_level,
                sizeof(sns_ssr_info.subsys_map[i].restart_level)) < 0)
      {
        ALOGE("Failed to write to ssr node: %s", strerror(errno));
      }
      else
      {
        ALOGI("writing %s to %s", sns_ssr_info.subsys_map[i].restart_level, sns_ssr_info.subsys_map[i].subsys_name);
        sns_ssr_info.subsys_map[i].write_back = false;
      }

      close(fd);
      fd = -1;
    }
  }
}

int main(int argc, char *argv[])
{
  int i = 0;
  char *tok_ptr = NULL;
  int use_prop = 0;
  char *subsys_name = NULL;
  int num_subsys = 0;
  char subsys_buf[PROPERTY_VALUE_MAX] = {0};
  sns_wrist_rc rc = RET_SUCCESS;

  if (argc < 2)
  {
    ALOGI("Using vendor.sensors.wrist_ssr for sns_wrist_ssr");
    use_prop = 1;
  }

  if (argc > MAX_NUM_SUBSYS + 1)
  {
    ALOGE("Invalid arguments: Too many subsys names provided");
    rc = RET_FAILED;
  }
  else if (populate_subsys_map() != RET_SUCCESS)
  {
    ALOGE("Failed to get subsys list");
    rc = RET_FAILED;
  }
  else if (!use_prop)
  {
    for (i = 1; i < argc; i++)
    {
      strlcpy(sns_subsys_list[i-1], argv[i], SUBSYS_NAME_LEN * sizeof(char));
      num_subsys++;
    }
  }
  else
  {
    property_get(SSR_SETUP_PROP, subsys_buf, "N/A");
    if (!strncmp(subsys_buf, "N/A", sizeof(subsys_buf)))
    {
      ALOGI("ssr prop empty");
    }
    else
    {
      subsys_name = strtok_r(subsys_buf, " ", &tok_ptr);
      while (subsys_name && num_subsys < MAX_NUM_SUBSYS)
      {
        strlcpy(sns_subsys_list[num_subsys], subsys_name,
                SUBSYS_NAME_LEN * sizeof(char));
        num_subsys++;
        subsys_name = strtok_r(NULL, " ", &tok_ptr);
      }
    }

    if(rc != RET_FAILED)
    {
      setup_and_trigger_ssr(num_subsys);
      restore_restart_level();
    }
  }

  return rc;
}

#endif
