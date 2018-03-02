// src/aksl/error.h   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/include/aksl/error.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_ERROR_H
#define AKSL_ERROR_H
/*------------------------------------------------------------------------------
Classes in this file:

hashtab_strings::
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The error keys are chosen negative so that they can be distinguished from
the usually non-negative normal return values of functions which return
an integer. This is also essential for the look-up table implementation.
------------------------------------------------------------------------------*/

#ifndef AKSL_SKI_H
#include "aksl/ski.h"
#endif
#ifndef AKSL_HASHFN_H
#include "aksl/hashfn.h"
#endif

enum aksl_error_t {
    eERRORMIN = -1000,   // Chosen to make all errors negative.
    eACCEPT_FAILED,
    eATTRIBUTE_BLOCK_ERROR,
    eATTRIBUTE_SETTING_ERROR,
    eBAD_ARGUMENT,
    eBAD_ATTRIBUTE,
    eBAD_ATTRIBUTE_BLOCK_CHARACTER,
    eBAD_ATTRIBUTE_BLOCK_VALUE,
    eBAD_LIST_CHARACTER,
    eBAD_LIST_VALUE,
    eBAD_ITEM_NAME,
    eBAD_OBJECT_TYPE_NAME,
    eBAD_REFERENCE_NAME,
    eBAD_SYSTEM_CHARACTER,
    eBAD_VALUE,
    eBIND_FAILED,
    eCOMMAND_NAME_ERROR,
    eCONNECT_FAILED,
    eEND_OF_FILE,
    eEND_OF_STREAM,
    eOBJECT_CREATION_ERROR,
    eOBJECT_NOT_CREATED,
    eERROR_GETTING_SYSTEM,
    eERROR_IN_NUMBER,
    eERROR_IN_SYSTEM,
    eERROR_SETTING_ATTRIBUTE,
    eEVENT_ERROR,
    eFILE_INCLUSION_LOOP,
    eFILE_NAME_ERROR,
    eFILE_OPEN_FAILED,
    eGENERAL,
    eIMPOSSIBLE_INTERNAL_ERROR,
    eIN_PROGRESS,
    eINIT_FAILED,
    eLIST_ERROR,
    eLISTEN_FAILED,
    eMESSAGE_KEY_CLASH,
    eMISSING_BRACKET,
    eMISSING_EQUALS,
    eMISSING_SEMICOLON,
    eNO_VALUE,
    eNONBLOCKING_FAILED,
    eOPEN_FAILED,
    ePACKAGE_NAME_ERROR,
    ePACKAGE_NOT_FOUND,
    eNAME_RESOLUTION_ERROR,
    eNEGATIVE_DURATION,
    eNEGATIVE_KEY,
    eNEW_OBJECT_NAME_CLASH,
    eNOT_FOUND,
    eNO_OBJECT_FUNCTION,
    eNO_OBJECTS,
    eNO_EVENTS,
    eNO_KEY_TABLE,
    eNO_PACKAGE,
    eNO_NAME,
    eNORMAL_TERMINATION,
    eNULL_ARGUMENT,
    eNULL_FILE_NAME,
    eSIMULATION_INTERRUPTED,
    eSOCKET_FAILED,
    eUNRECOGNISED_COMMAND,

    eERRORMAX                       // eERRORMAX must be negative!!!
    }; // End of enum aksl_error_t.

/*------------------------------------------------------------------------------
A hash table for a given array of strings.
------------------------------------------------------------------------------*/
//----------------------//
//   hashtab_strings::  //
//----------------------//
struct hashtab_strings : public hashtab_32_8 {

public:
    const char* string(long key)
        { void* p = 0; return find(p, uint32(key)) ? (const char*)p : 0; }

//    hashtab_strings& operator=(const hashtab_strings& x) {}
//    hashtab_strings(const hashtab_strings& x) {}
    // Construct from null-terminated array:
//    hashtab_strings(const stringkey* strings);
    hashtab_strings(stringkey* strings);
    ~hashtab_strings() {}
    }; // End of struct hashtab_strings.

extern c_string error_string(long);
extern const char* error_str(long);

#endif /* AKSL_ERROR_H */
