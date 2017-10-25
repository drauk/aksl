// src/aksl/value.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

value::
    copyto
    print
    operator long
    operator double
    operator const char*
    operator object*
    operator datum*
    operator valuelist*
    operator tagvaluelist*
    operator colonlist*
    operator=(long)
    operator=(double)
    operator=(char*)
    operator=(object*)
    operator=(datum*)
    operator=(valuelist*)
    operator=(tagvaluelist*)
    operator=(colonlist*)
valuelist::
    copy
tagvaluelist::
    operator=(long)
    operator=(double)
    operator=(char*)
    operator=(object*)
    operator=(datum*)
    operator=(valuelist*)
    operator=(tagvaluelist*)
    operator=(colonlist*)
    copy
colonlist::
    copy
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/value.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files:
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

// Fast block memory allocation.
bmem_define(value, bmem0);
bmem_define(tagvalue, bmem0);

/*------------------------------------------------------------------------------
Copy a "value" to a given pre-existing "value".
------------------------------------------------------------------------------*/
//----------------------//
//    value::copyto     //
//----------------------//
void value::copyto(value& x) const {
    switch(ty) {
    case vINTEGER:
        x = i;
        break;
    case vREAL:
        x = r;
        break;
    case vSTRING:   // This copying causes the "value" to possess a string.
        x = s;
        break;
    case vOBJECT:
        x = p;
        break;
    case vDATUM:
        x = p;
        break;
    case vLIST: {
        valuelist* vl = l->copy();
        x = vl;
        }
        break;
    case vTVLIST: {
        tagvaluelist* z = tvl->copy();
        x = z;
        }
        break;
    case vCOLONLIST: {
        colonlist* vl = cl->copy();
        x = vl;
        }
        break;
    case vNONE:
    default:
        break;
        }
    } // End of function value::copyto.

/*------------------------------------------------------------------------------
value::print() is an ad hoc printing routine for a "value".
------------------------------------------------------------------------------*/
//----------------------//
//     value::print     //
//----------------------//
void value::print(ostream& os) const {
    switch(ty) {
    case vINTEGER:
        os << "INTEGER: " << i;
        break;
    case vREAL:
        os << "REAL: " << r;
        break;
    case vSTRING:
        os << "STRING: \"" << s << "\"";
        break;
    case vOBJECT:
        os << "OBJECT: 0x" << hex8(long(p));
        break;
    case vDATUM:
        os << "DATUM: 0x" << hex8(long(d));
        break;
    case vLIST:
        os << "LIST: 0x" << hex8(long(l));
        break;
    case vTVLIST:
        os << "TVLIST: 0x" << hex8(long(tvl));
        break;
    case vCOLONLIST:
        os << "COLONLIST: 0x" << hex8(long(cl));
        break;
    case vNONE:
    default:
        break;
        }
    } // End of function value::print.

//--------------------------//
//   value::operator long   //
//--------------------------//
value::operator long() const {
    switch(ty) {
    case vINTEGER:
        return i;
    case vREAL:
        return (long)r;
    case vSTRING:
    case vOBJECT:
    case vDATUM:
    case vLIST:
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator long.

//--------------------------//
//  value::operator double  //
//--------------------------//
value::operator double() const {
    switch(ty) {
    case vINTEGER:
        return i;
    case vREAL:
        return r;
    case vSTRING:
    case vOBJECT:
    case vDATUM:
    case vLIST:
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator double.

/*------------------------------------------------------------------------------
WARNING: This function does not return a new copy of s. The receiver must
somehow determine whether the string was supposed to be deleted on arrival or
else left alone because it was static.
------------------------------------------------------------------------------*/
//------------------------------//
// value::operator const char*  //
//------------------------------//
value::operator const char*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
        return 0;
    case vSTRING:
        return s;
    case vOBJECT:
    case vDATUM:
    case vLIST:
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator const char*.

//--------------------------//
//  value::operator object* //
//--------------------------//
value::operator object*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
    case vSTRING:
        return 0;
    case vOBJECT:
        return p;
    case vDATUM:
    case vLIST:
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator object*.

//--------------------------//
//  value::operator datum*  //
//--------------------------//
value::operator datum*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
    case vSTRING:
    case vOBJECT:
        return 0;
    case vDATUM:
        return d;
    case vLIST:
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator datum*.

//------------------------------//
//  value::operator valuelist*  //
//------------------------------//
value::operator valuelist*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
    case vSTRING:
    case vOBJECT:
    case vDATUM:
        return 0;
    case vLIST:
        return l;
    case vTVLIST:
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator valuelist*.

//------------------------------//
// value::operator tagvaluelist*//
//------------------------------//
value::operator tagvaluelist*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
    case vSTRING:
    case vOBJECT:
    case vDATUM:
    case vLIST:
        return 0;
    case vTVLIST:
        return tvl;
    case vCOLONLIST:
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator tagvaluelist*.

//------------------------------//
//  value::operator colonlist*  //
//------------------------------//
value::operator colonlist*() const {
    switch(ty) {
    case vINTEGER:
    case vREAL:
    case vSTRING:
    case vOBJECT:
    case vDATUM:
    case vLIST:
    case vTVLIST:
        return 0;
    case vCOLONLIST:
        return cl;
    case vNONE:
    default:
        return 0;
        }
    } // End of function value::operator colonlist*.

//--------------------------//
//  value::operator=(long)  //
//--------------------------//
value& value::operator=(long ii) {
    clear();
    ty = vINTEGER;
    i = ii;
    return *this;
    } // End of function value::operator=(long).

//--------------------------//
// value::operator=(double) //
//--------------------------//
value& value::operator=(double rr) {
    clear();
    ty = vREAL;
    r = rr;
    return *this;
    } // End of function value::operator=(double).

/*------------------------------------------------------------------------------
Warning: No copy is made of the string. The sender must somehow inform the
recipient whether the string is allocated static or on the heap.
------------------------------------------------------------------------------*/
//----------------------------------//
//      value::operator=(char*)     //
//----------------------------------//
value& value::operator=(char* ss) {
    clear();
    ty = vSTRING;
    s = ss;
    return *this;
    } // End of function value::operator=(char*).

//------------------------------//
//   value::operator=(object*)  //
//------------------------------//
value& value::operator=(object* pp) {
    clear();
    ty = vOBJECT;
    p = pp;
    return *this;
    } // End of function value::operator=(object*).

//------------------------------//
//   value::operator=(datum*)   //
//------------------------------//
value& value::operator=(datum* dd) {
    clear();
    ty = vDATUM;
    d = dd;
    return *this;
    } // End of function value::operator=(datum*).

//------------------------------//
// value::operator=(valuelist*) //
//------------------------------//
value& value::operator=(valuelist* ll) {
    clear();
    ty = vLIST;
    l = ll;
    return *this;
    } // End of function value::operator=(valuelist*).

//----------------------------------//
//  value::operator=(tagvaluelist*) //
//----------------------------------//
value& value::operator=(tagvaluelist* tt) {
    clear();
    ty = vTVLIST;
    tvl = tt;
    return *this;
    } // End of function value::operator=(tagvaluelist*).

//------------------------------//
// value::operator=(colonlist*) //
//------------------------------//
value& value::operator=(colonlist* ll) {
    clear();
    ty = vCOLONLIST;
    cl = ll;
    return *this;
    } // End of function value::operator=(colonlist*).

//----------------------//
//    valuelist::copy   //
//----------------------//
valuelist* valuelist::copy() {
    valuelist* vl = new valuelist;
    Forall(value, pv, *this)
        vl->append(pv->copy());
    return vl;
    } // End of function valuelist::copy.

//--------------------------//
// tagvalue::operator=(long)//
//--------------------------//
tagvalue& tagvalue::operator=(long ii) {
    clear();
    ty = vINTEGER;
    i = ii;
    return *this;
    } // End of function tagvalue::operator=(long).

//------------------------------//
//  tagvalue::operator=(double) //
//------------------------------//
tagvalue& tagvalue::operator=(double rr) {
    clear();
    ty = vREAL;
    r = rr;
    return *this;
    } // End of function tagvalue::operator=(double).

/*------------------------------------------------------------------------------
Warning: No copy is made of the string. The sender must somehow inform the
recipient whether the string is allocated static or on the heap.
------------------------------------------------------------------------------*/
//------------------------------//
//  tagvalue::operator=(char*)  //
//------------------------------//
tagvalue& tagvalue::operator=(char* ss) {
    clear();
    ty = vSTRING;
    s = ss;
    return *this;
    } // End of function tagvalue::operator=(char*).

//------------------------------//
// tagvalue::operator=(object*) //
//------------------------------//
tagvalue& tagvalue::operator=(object* pp) {
    clear();
    ty = vOBJECT;
    p = pp;
    return *this;
    } // End of function tagvalue::operator=(object*).

//------------------------------//
//  tagvalue::operator=(datum*) //
//------------------------------//
tagvalue& tagvalue::operator=(datum* dd) {
    clear();
    ty = vDATUM;
    d = dd;
    return *this;
    } // End of function tagvalue::operator=(datum*).

//----------------------------------//
//  tagvalue::operator=(valuelist*) //
//----------------------------------//
tagvalue& tagvalue::operator=(valuelist* ll) {
    clear();
    ty = vLIST;
    l = ll;
    return *this;
    } // End of function tagvalue::operator=(valuelist*).

//--------------------------------------//
//   tagvalue::operator=(tagvaluelist*) //
//--------------------------------------//
tagvalue& tagvalue::operator=(tagvaluelist* tt) {
    clear();
    ty = vTVLIST;
    tvl = tt;
    return *this;
    } // End of function tagvalue::operator=(tagvaluelist*).

//----------------------------------//
//  tagvalue::operator=(colonlist*) //
//----------------------------------//
tagvalue& tagvalue::operator=(colonlist* ll) {
    clear();
    ty = vCOLONLIST;
    cl = ll;
    return *this;
    } // End of function tagvalue::operator=(colonlist*).

//----------------------//
//  tagvaluelist::copy  //
//----------------------//
tagvaluelist* tagvaluelist::copy() {
    tagvaluelist* tvl = new tagvaluelist;
    Forall(tagvalue, ptv, *this)
        tvl->append(ptv->copy());
    return tvl;
    } // End of function valuelist::copy.

//----------------------//
//    colonlist::copy   //
//----------------------//
colonlist* colonlist::copy() {
    colonlist* vl = new colonlist;
    Forall(value, pv, *this)
        vl->append(pv->copy());
    return vl;
    } // End of function colonlist::copy.
