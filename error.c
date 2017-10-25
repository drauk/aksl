// src/aksl/error.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/error.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

error_string
error_str
hashtab_strings::
    hashtab_strings
------------------------------------------------------------------------------*/

#include "aksl/error.h"

// Note that all keys must be negative!
static stringkey error_strings[] = {
    "accept failed",                    -eACCEPT_FAILED,
    "attribute block error",            -eATTRIBUTE_BLOCK_ERROR,
    "attribute setting error",          -eATTRIBUTE_SETTING_ERROR,
    "bad argument",                     -eBAD_ARGUMENT,
    "bad attribute",                    -eBAD_ATTRIBUTE,
    "bad attribute block character",    -eBAD_ATTRIBUTE_BLOCK_CHARACTER,
    "bad attribute block value",        -eBAD_ATTRIBUTE_BLOCK_VALUE,
    "bad list character",               -eBAD_LIST_CHARACTER,
    "bad list value",                   -eBAD_LIST_VALUE,
    "bad item name",                    -eBAD_ITEM_NAME,
    "bad object type name",             -eBAD_OBJECT_TYPE_NAME,
    "bad reference name",               -eBAD_REFERENCE_NAME,
    "bad system character",             -eBAD_SYSTEM_CHARACTER,
    "bad value",                        -eBAD_VALUE,
    "bind failed",                      -eBIND_FAILED,
    "command name error",               -eCOMMAND_NAME_ERROR,
    "connect failed",                   -eCONNECT_FAILED,
    "end of file",                      -eEND_OF_FILE,
    "end of stream",                    -eEND_OF_STREAM,
    "object creation error",            -eOBJECT_CREATION_ERROR,
    "object not created",               -eOBJECT_NOT_CREATED,
    "error getting system",             -eERROR_GETTING_SYSTEM,
    "error in number",                  -eERROR_IN_NUMBER,
    "error in system",                  -eERROR_IN_SYSTEM,
    "error setting attribute",          -eERROR_SETTING_ATTRIBUTE,
    "event error",                      -eEVENT_ERROR,
    "file inclusion loop",              -eFILE_INCLUSION_LOOP,
    "file name error",                  -eFILE_NAME_ERROR,
    "file open failed",                 -eFILE_OPEN_FAILED,
    "general",                          -eGENERAL,
    "impossible internal error",        -eIMPOSSIBLE_INTERNAL_ERROR,
    "initialisation failed",            -eINIT_FAILED,
    "list error",                       -eLIST_ERROR,
    "listen failed",                    -eLISTEN_FAILED,
    "message key clash",                -eMESSAGE_KEY_CLASH,
    "missing bracket",                  -eMISSING_BRACKET,
    "missing equals",                   -eMISSING_EQUALS,
    "missing semicolon",                -eMISSING_SEMICOLON,
    "no value",                         -eNO_VALUE,
    "non-blocking failed",              -eNONBLOCKING_FAILED,
    "operation in progress",            -eIN_PROGRESS,
    "package name error",               -ePACKAGE_NAME_ERROR,
    "package not found",                -ePACKAGE_NOT_FOUND,
    "name resolution error",            -eNAME_RESOLUTION_ERROR,
    "negative duration",                -eNEGATIVE_DURATION,
    "negative key",                     -eNEGATIVE_KEY,
    "new object name clash",            -eNEW_OBJECT_NAME_CLASH,
    "no objects",                       -eNO_OBJECTS,
    "no object function",               -eNO_OBJECT_FUNCTION,
    "no events",                        -eNO_EVENTS,
    "no key table",                     -eNO_KEY_TABLE,
    "no package",                       -eNO_PACKAGE,
    "no name",                          -eNO_NAME,
    "normal termination",               -eNORMAL_TERMINATION,
    "not found",                        -eNOT_FOUND,
    "null argument",                    -eNULL_ARGUMENT,
    "null file name",                   -eNULL_FILE_NAME,
    "open failed",                      -eOPEN_FAILED,
    "simulation interrupted",           -eSIMULATION_INTERRUPTED,
    "socket failed",                    -eSOCKET_FAILED,
    "unrecognised command",             -eUNRECOGNISED_COMMAND,
    (char*)0
    };

static skilist error_table(error_strings);
static hashtab_strings error_hashtab(error_strings);

/*------------------------------------------------------------------------------
Function error_string() returns a string describing the error key sent as
parameter.
------------------------------------------------------------------------------*/
//----------------------//
//     error_string     //
//----------------------//
c_string error_string(long key) {
    c_string err_str;
    error_table.string(-key, err_str);
    return err_str;     // This is tricky. See Stroustrup 1986, pages 180-181.
    } // End of function error_string.

//----------------------//
//       error_str      //
//----------------------//
const char* error_str(long key) {
    const char* s = error_hashtab.string(-key);
    return s ? s : "[unknown error message]";
    } // End of function error_str.

/*------------------------------------------------------------------------------
The array argument must be a null-terminated array of stringkeys.
That is, the string field of the last stringkey must be the null string.
------------------------------------------------------------------------------*/
//----------------------------------//
// hashtab_strings::hashtab_strings //
//----------------------------------//
hashtab_strings::hashtab_strings(stringkey* strings) {
    if (!strings)
        return;
    while (strings->s) {
        insert(strings->s, uint32(strings->i));
        ++strings;
        }
    } // End of function hashtab_strings::hashtab_strings.
