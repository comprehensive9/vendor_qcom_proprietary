/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qcril_db.h"
#include "qcril_config.h"
#include "config_data.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "qcril_config"

#define CONFIG_VALUE_MAX 128
#define QCRIL_PROPERTIES_TABLE_NAME "qcril_properties_table"

/* Statement to create qcril db properties table */
#define QCRIL_DB_CREATE_PROPERTIES_TABLE  \
            "create table if not exists %s"\
            "(PROPERTY TEXT,VALUE TEXT,DEF_VAL TEXT,PRIMARY KEY(PROPERTY))"

/* Query statement to query property setting values */
static const char* qcril_db_query_properties_stmt =
                      "select * from %s where PROPERTY='%s'";

/* Insert statement to insert property_name and value */
static const char* qcril_db_insert_properties_stmt =
                      "insert into %s(property, value) values('%s', '%s')";

/* Update statment to change the user settings of the property */
static const char* qcril_db_update_properties_stmt =
                      "update %s set value='%s' where property='%s'";

/* Update statement to clear all user setings */
static const char* qcril_db_clear_properties_stmt =
                      "update %s set value='%s'";


/*===========================================================================

  FUNCTION  qcril_db_query_property_callback

===========================================================================*/
/*!
    @brief
    update property.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_property_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = SQLITE_ERROR;

    char* value = NULL;
    char* def_val = NULL;
    for (int i = 0; i < argc; i++) {
        if (!strncasecmp(azColName[i], "value", 5)) {
            value = argv[i];
        } else if (!strncasecmp(azColName[i], "def_val", 7)) {
            def_val = argv[i];
        }
    }
    if (data) {
        if (value) {
            strlcpy(static_cast<char *>(data), value, CONFIG_VALUE_MAX);
            ret = SQLITE_OK;
        } else if (def_val) {
            strlcpy(static_cast<char *>(data), def_val, CONFIG_VALUE_MAX);
            ret = SQLITE_OK;
        }
    }

    return ret;
}


/*===========================================================================

  FUNCTION  qcril_db_query_properties_table

===========================================================================*/
/*!
    @brief
    Query property table.
    Caller of this function should pass sufficient buffer (value)
    for storing the information retrieved from database

    @return
    None
*/
/*=========================================================================*/
errno_enum_type qcril_db_query_properties_table
(
    const char *property_name,
    char *value
)
{
    errno_enum_type res = E_FAILURE;
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_ready())
        {
            break;
        }
        // if null pointer
        if ( !value || !property_name )
        {
            break;
        }

        /* Initialize it to default value */
        *value = '\0';
        char query[QCRIL_DB_MAX_STMT_LEN] = {0};
        snprintf(query,
                 sizeof(query),
                 qcril_db_query_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 property_name
                 );

        QCRIL_LOG_INFO(" Query %s", query);

        int ret;
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                               query, qcril_db_query_property_callback, value)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        QCRIL_LOG_INFO("value - %s", value);
        res = E_SUCCESS;

    } while (FALSE);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_insert_properties_table

===========================================================================*/
/*!
    @brief
    Insert property table.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
errno_enum_type qcril_db_insert_properties_table
(
    const char *property_name,
    const char *value
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    create_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret     = SQLITE_OK;
    int     table_exists = FALSE;
    errno_enum_type res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" property_name: %s, value: %s",
                     property_name? property_name : "null",
                     value? value : "null");

    do
    {
        // db not ready
        if (!qcril_db_ready())
        {
            break;
        }
        // if null pointer
        if ( !value || !property_name )
        {
            break;
        }

        table_exists =
            qcril_db_check_if_table_exists(QCRIL_PROPERTIES_TABLE_NAME);

        if ( !table_exists )
        {
            snprintf(create_stmt,
                     sizeof(create_stmt),
                     QCRIL_DB_CREATE_PROPERTIES_TABLE,
                     QCRIL_PROPERTIES_TABLE_NAME);

            if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                      create_stmt, NULL, NULL)))
            {
                QCRIL_LOG_ERROR("stmt %s", create_stmt);
                QCRIL_LOG_ERROR("Could not create table %d", res);
                break;
            }
        }


        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 property_name,
                 value
                 );

        QCRIL_LOG_INFO("insert_stmt %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}
/*===========================================================================

  FUNCTION  qcril_db_properties_table_user_set

===========================================================================*/
/*!
    @brief
    Insert property table.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
errno_enum_type qcril_db_properties_table_user_set
(
    const char *property_name,
    const char *value
)
{
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret     = SQLITE_OK;
    errno_enum_type res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" property_name: %s, value: %s",
                     property_name? property_name : "null",
                     value? value : "null");

    do
    {
        // db not ready
        if (!qcril_db_ready())
        {
            break;
        }
        // if null pointer
        if ( !value || !property_name )
        {
            break;
        }

        snprintf(update_stmt,
                 sizeof(update_stmt),
                 qcril_db_update_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 value,
                 property_name
                 );

        QCRIL_LOG_INFO("update_stmt %s", update_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not update %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

errno_enum_type qcril_config_clear_all_user_settings() {
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret     = SQLITE_OK;
    errno_enum_type res = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO("Clear all user settings");

    do
    {
        // db not ready
        if (!qcril_db_ready())
        {
            break;
        }

        snprintf(update_stmt,
                 sizeof(update_stmt),
                 qcril_db_clear_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 ""
                 );

        QCRIL_LOG_INFO("update_stmt %s", update_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not update %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

errno_enum_type qcril_config_set(property_id_type id, const std::string& config_value) {
    auto key = ConfigData::getInstance().getPropertyKey(id);
    if (key == "") return E_FAILURE;
    // XXX: Could RIL only rely on SQLITE_CORRUPT returned by sqlite3_exec and remove
    // the explicit CHECK here?
    qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);
    auto res = qcril_db_properties_table_user_set(key.c_str(), config_value.c_str());
    if (res == E_SUCCESS)
        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
    return res;
}

errno_enum_type qcril_config_get(property_id_type id, std::string& config_value) {
    auto key = ConfigData::getInstance().getPropertyKey(id);
    if (key == "") return E_FAILURE;

    char args[CONFIG_VALUE_MAX] = {'\0'};
    auto res = qcril_db_query_properties_table(key.c_str(), args);
    if (args[0] == '\0') res = E_NOT_FOUND;
    if (res == E_SUCCESS)
        config_value = args;
    return res;
}

// variants
errno_enum_type qcril_config_get(property_id_type id, int& config_value) {
    std::string config;
    auto res = qcril_config_get(id, config);
    if (res != E_SUCCESS) return res;

    auto c_str = config.c_str();
    char* end_ptr;
    auto ret_val = strtol(c_str, &end_ptr, 0);
    if ((errno != EINVAL) && (errno != ERANGE)
            && (ret_val != LONG_MAX) && (end_ptr != c_str)) {
        config_value = static_cast<int>(ret_val);
        return E_SUCCESS;
    } else {
        return E_FAILURE;
    }
}

errno_enum_type qcril_config_get(property_id_type id, bool& config_value) {
    std::string config;
    auto res = qcril_config_get(id, config);
    if (res != E_SUCCESS) return res;

    if (!config.compare("true") || !config.compare("1")) {
        config_value = true;
        return E_SUCCESS;
    } else if (!config.compare("false") || !config.compare("0")) {
        config_value = false;
        return E_SUCCESS;
    } else {
        return E_FAILURE;
    }
}

errno_enum_type qcril_config_set(property_id_type id, int config_value) {
    return qcril_config_set(id, std::to_string(config_value));
}

errno_enum_type qcril_config_set(property_id_type id, bool config_value) {
    return qcril_config_set(id, config_value ? "true" : "false");
}

// C compatible interface (will be deprecated)
extern "C" {
errno_enum_type qcril_config_set(property_id_type id, const char* config_value) {
    return qcril_config_set(id, (const std::string&)config_value);
}

errno_enum_type qcril_config_get(property_id_type id, char** p_config_value) {
    std::string val;
    *p_config_value = nullptr;
    if (qcril_config_get(id, val) == E_SUCCESS) {
        *p_config_value = strdup(val.c_str());
    }
    return (*p_config_value == nullptr) ? E_FAILURE : E_SUCCESS;
}
} // extern "C"
