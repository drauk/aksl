// src/aksl/oral.c   2018-3-4   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
This file contains an interpreter for the ORAL language.
The functionality here is supposed to be independent of the AKSL software,
except for the use of the list classes and the error codes.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Functions in this file:

space
ts_error
ts_error
avaluelist::
    read
    print
acolonlist::
    read
    print
avalue::
    ~avalue
    read
    print
    resolvenames
    clearout
    operator double
    operator long
    IntegerColon
attrlist::
    read
    print
item::
    print
itemlist::
    findname
itemblock::
    read
    findname
    resolvenames
    print
itemptrlist::
    find
    append
    print
oralsystemlist::
    getsystem
oraldata::
    readfile
------------------------------------------------------------------------------*/

#include "aksl/oral.h"

// System header files.
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif

static const char* sysblockstr = "system";  // Keyword for system block.
static const int defttab = 4;       // Tabbing increment for avalue printing.

/*------------------------------------------------------------------------------
space() outputs a given number of copies of a string to a file.
The default ostream:            cout
The default number of copies:   1
The default string:             " "
------------------------------------------------------------------------------*/
//----------------------//
//         space        //
//----------------------//
static void space(ostream& os = cout, int n = 1, const char* s = 0) {
    static const char* deftstring = " ";
    if (!s)
        s = deftstring;
    while (--n >= 0)
        os << s;
    } // End of function space.

//----------------------//
//       ts_error       //
//----------------------//
static void ts_error(token_stream& ts) {
    cout << "Error on line " << ts.line() << endl;
    } // End of function ts_error.

//----------------------//
//       ts_error       //
//----------------------//
static void ts_error(token* pt) {
    if (pt)
        cout << "Error on line " << pt->line() << endl;
    else
        cout << "Error on line [undefined]" << endl;
    } // End of function ts_error.

/*------------------------------------------------------------------------------
avaluelist::read() attempts to read a list from a file. A negative integer is
returned if this fails.
It is assumed that the initial token "(" has been put back into the file.
------------------------------------------------------------------------------*/
//----------------------//
//   avaluelist::read   //
//----------------------//
int avaluelist::read(token_stream& ts) {
    token* pt;
    if (!(ts >> pt)) {
        cout << "End of stream at beginning of a value list.\n";
        return eEND_OF_STREAM;
        }
    if (!pt->Leftparen()) {
        cout << "Value list did not begin with '('.\n";
        delete pt;
        return eBAD_LIST_CHARACTER;
        }
    delete pt;
    for (;;) {
        if (!(ts >> pt)) {
            cout << "End of stream while reading a value list.\n";
            return eEND_OF_STREAM;
            }
        if (pt->Rightparen()) {
            delete pt;
            break;
            }
        ts.putback(pt);
        avalue* p = new avalue;
        if (p->read(ts) < 0) {
            cout << "Error while reading an element of a list.\n";
            delete p;
            return eBAD_LIST_VALUE;
            }
        append(p);
        }
    return 0;
    } // End of function avaluelist::read.

//----------------------//
//   avaluelist::print  //
//----------------------//
void avaluelist::print(ostream& os, int indent, const char* ts) {
    os << "(\n";
    Forall(register avalue, p, *this) {
        space(os, indent + defttab, ts);
        p->print(os, indent + defttab, ts);
        os << "\n";
        }
    space(os, indent + defttab, ts);
    os.put(')');
    } // End of function avaluelist::print.

/*------------------------------------------------------------------------------
acolonlist::read() attempts to read a list from a file. A negative integer is
returned if this fails.
The syntax here is "item1 : item2 : item3" rather than "(item1 item2 item3)".
Hence the first value must have been read already from the input stream by the
time this function is called. Hence it is only necessary to read the rest of the
items in the list. The list terminates with anything except a colon.
At entry to this function, the next token in the token stream is expected to be
a colon. The first item of the list is expected to be already in this list.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
.... work in progress ....
------------------------------------------------------------------------------*/
//----------------------//
//   acolonlist::read   //
//----------------------//
int acolonlist::read(token_stream& ts) {
    token* pt = 0;
    for (;;) {
        // Each item must be preceded by a colon.
        if (!(ts >> pt)) {
            // Generally EOF cannot happen in mid list. But the user
            // may in future do an "include" in the middle of a list!!!
            cout << "End of stream while reading a colon value list.\n";
            return eEND_OF_STREAM;
//            break;
            }
        // Next item should be preceded by a colon. Otherwise the list is ended.
        if (!pt->Colon()) {
            ts.putback(pt);
            break;
            }
        ts.putback(pt);
        avalue* p = new avalue;
        if (p->read(ts) < 0) {
            cout << "Error while reading an element of a list.\n";
            delete p;
            return eBAD_LIST_VALUE;
            }
        append(p);
        }
    return 0;
    } // End of function acolonlist::read.

//----------------------//
//   acolonlist::print  //
//----------------------//
void acolonlist::print(ostream& os, int indent, const char* ts) {
    os << "\n";
    bool_enum first_time = true;
    Forall(register avalue, p, *this) {
        space(os, indent + defttab, ts);
        if (first_time) {
            os << "  ";
            first_time = false;
            }
        else
            os << ": ";
        p->print(os, indent + defttab, ts);
        if (p->next())
            os << "\n";
        }
    } // End of function acolonlist::print.

//----------------------//
//   avalue::~avalue    //
//----------------------//
avalue::~avalue() {
    switch(type) {
    case avLIST:
        delete l;
        break;
    case avBLOCK:
        delete blk;
        break;
    case avCOLONLIST:
        delete cl;
        break;
        } // End of switch(type).
    } // End of function avalue::~avalue.

/*------------------------------------------------------------------------------
avalue::read() reads an attribute value from a stream. If anything goes
wrong, a negative integer is returned.
First a single value is read form the stream. Then if a colon is seen, a list of
colon-separated values is read. This is done because it is not possible to know
if the first value is an element of a list until it has been fully read in, and
a single value could be quite large -- not just one or two tokens.
------------------------------------------------------------------------------*/
//----------------------//
//     avalue::read     //
//----------------------//
int avalue::read(token_stream& ts, bool_enum ignore_colon) {
    line_number = ts.line();
    token *pt = 0, *pt2 = 0;

    // Check for immediate end of file.
    if (!(ts >> pt)) {
        cout << "End of stream at beginning of value.\n";
        return eEND_OF_STREAM;
        }

    // Read the first token to discriminate the avalue type,
    // and read a single avalue.
    if (pt->String()) {
        s_refname = pt->pc;     // Only copies the string.
        type = avSTRING;
        delete pt;
        }
    else if (pt->Integer()) {
        i = pt->i;
        type = avINTEGER;
        delete pt;
        }
    else if (pt->Real()) {
        r = pt->r;
        type = avREAL;
        delete pt;
        }
    else if (pt->Symbol()) {
        switch(pt->sym_type) {
        case symPLUS:
        case symMINUS:
            {
            int sign = ((pt->sym_type == symPLUS) ? 1 : -1);
            delete pt;
            for (;;) {
                if (!(ts >> pt)) {
                    cout << "End of stream while reading a number.\n";
                    return eEND_OF_STREAM;
                    }
                if (pt->Integer()) {
                    i = pt->i;
                    if (sign < 0)
                        i = -i;
                    type = avINTEGER;
                    delete pt;
                    break;
                    }
                if (pt->Real()) {
                    r = pt->r;
                    if (sign < 0)
                        r = -r;
                    type = avREAL;
                    delete pt;
                    break;
                    }
                if (pt->Plus())
                    delete pt;
                else if (pt->Minus()) {
                    sign *= -1;
                    delete pt;
                    }
                else {
                    cout << "Erroneous signed number.\n";
                    delete pt;
                    return eBAD_VALUE;
                    }
                }
            }
            break;
        case symHAT: // Reference.
            delete pt;
            if (!(ts >> pt2)) {
                cout << "End of stream in pointer.\n";
                return eEND_OF_STREAM;
                }
            if (!pt2->Identifier()) {
                cout << "Pointer to a non-identifier.\n";
                delete pt2;
                return eBAD_ATTRIBUTE;
                }
            s_refname = pt2->s;
            itm = 0;
            type = avREF;
            delete pt2;
            break;
        case symLEFTPAREN: { // List.
            ts.putback(pt);
            avaluelist* newlist = new avaluelist;
            if (newlist->read(ts) < 0) {
                cout << "Error in list.\n";
                delete newlist;
                return eLIST_ERROR;
                }
            l = newlist;
            type = avLIST;
            }
            break;
        case symLEFTBRACE: { // Attribute block.
            ts.putback(pt);
            attrlist* pb = new attrlist;
            if (pb->read(ts) < 0) {
                cout << "Error in block.\n";
                delete pb;
                return eATTRIBUTE_BLOCK_ERROR;
                }
            blk = pb;
            type = avBLOCK;
            }
            break;
        default: // Bad symbol type. Syntax error for production "value".
            cout << "Bad symbol at beginning of a value.\n";
            delete pt;
            return eBAD_VALUE;
            } // End of switch(pt->sym_type).
        }
    else { // Case that first token was not an integer, real, string or symbol.
        // Must be an identifier or an error.
        cout << "Bad lexical type for a value: ";
        pt->print(cout);
        cout << endl;
        delete pt;
        return eBAD_VALUE;
        }

    // If we are ignoring colons, then we're finished.
    if (ignore_colon)
        return 0;

    // Check to see if there's a colon for a colon-separated list.
    if (!(ts >> pt)) {
//        cout << "End of stream while reading a number.\n";
//        return eEND_OF_STREAM;
        return 0;
        }
    if (!pt->Symbol() || pt->sym_type != symCOLON) {
        ts.putback(pt);
        return 0;
        }

    // If there's a colon, recurse to get some more avalues.
//    cout << "Found a colon!!\n";
    ts.putback(pt);

    // Move the contents of *this out into a new avalue.
    avalue* av0 = clearout();

    // Replace *this with a colonlist with the old *this as the first element.
    cl = new acolonlist;
    cl->append(av0);
    type = avCOLONLIST;

    // Then append the rest of the colon-separated values.
    for (;;) {
        // Check to see if there's a colon for a colon-separated list.
        if (!(ts >> pt) || !pt)
            break;
        if (!pt->Symbol() || pt->sym_type != symCOLON) {
            ts.putback(pt);
            break;
            }

        // Throw away the colon.
        delete pt;

        // Read an avalue with "ignore_colon" = true.
        av0 = new avalue;
        if (av0->read(ts, true) < 0) {
            ts_error(ts);
            cout << "Error while reading attribute value in colon-list.\n";
            return eBAD_ATTRIBUTE;
            }

        // Add the new avalue to the list.
        cl->append(av0);
        }

    return 0;
    } // End of function avalue::read.

/*------------------------------------------------------------------------------
avalue::print() just prints out an avalue.
------------------------------------------------------------------------------*/
//----------------------//
//     avalue::print    //
//----------------------//
void avalue::print(ostream& os, int indent, const char* ts) {
    switch(type) {
    case avNULL:
        os << "NULL";
        break;
    case avSTRING:
        os << "STRING: \"" << s_refname << "\"";
        break;
    case avINTEGER:
        os << "INTEGER: " << i;
        break;
    case avREAL:
        os << "REAL: " << r;
        break;
    case avREF:
        os << "REF: ^" << s_refname;
        break;
    case avLIST:
        os << "LIST: ";
        if (l)
            l->print(os, indent, ts);
        break;
    case avBLOCK:
        os << "BLOCK: {\n";
        if (blk)
            blk->print(os, indent, ts);
        space(os, indent + defttab, ts);
        os << "}";
        break;
    case avCOLONLIST:
        os << "COLONLIST: ";
        if (cl)
            cl->print(os, indent, ts);
        break;
    default:
        os << "UNKNOWN TYPE";
        break;
        }
    } // End of function avalue::print.

/*------------------------------------------------------------------------------
avalue::resolvenames() resolves all references in an avalue, and recursively
within the avalue. Since item-pointers cannot point to global variables, it is
not necessary to have access to the entire itemblock in order to resolve the
item-pointers.
------------------------------------------------------------------------------*/
//----------------------//
// avalue::resolvenames //
//----------------------//
bool_enum avalue::resolvenames(itemlist& items) {
    bool_enum allfound = true;

    switch(type) {
    case avREF: {
        item* p = items.findname(s_refname);
        if (!p) {
            cout << "Name resolution error. Pointer ^" << s_refname;
            cout << " is undefined.\n";
            allfound = false;
            }
        itm = p;
        }
        break;
    case avLIST: {
        if (l) {
            Forall(avalue, pv, *l)
                allfound = (bool_enum)(allfound && pv->resolvenames(items));
            }
        }
        break;
    case avBLOCK: {
        if (blk) {
            Forall(attr, pa, *blk)
                allfound = (bool_enum)(allfound && pa->resolvenames(items));
            }
        }
        break;
    case avCOLONLIST: {
        if (cl) {
            Forall(avalue, pv, *cl)
                allfound = (bool_enum)(allfound && pv->resolvenames(items));
            }
        }
        break;
        } // End of switch(type).

    return allfound;
    } // End of function avalue::resolvenames.

/*------------------------------------------------------------------------------
Move the entire contents of this avalue into a new avalue.
------------------------------------------------------------------------------*/
//----------------------//
//   avalue::clearout   //
//----------------------//
avalue* avalue::clearout() {
    avalue* av0 = new avalue;
    av0->type = type;

    switch(type) {
    case avSTRING:
        av0->s_refname = s_refname;
        break;
    case avINTEGER:
        av0->i = i;
        break;
    case avREAL:
        av0->r = r;
        break;
    case avREF:
        av0->s_refname = s_refname;
        av0->itm = itm;
        break;
    case avLIST:
        av0->l = l;
        break;
    case avBLOCK:
        av0->blk = blk;
        break;
    case avCOLONLIST:
        av0->cl = cl;
        break;
    case avNULL:
    default:
        av0->type = avNULL;
        break;
        }

    // Make this avalue a null avalue.
    s_refname.clear();
    type = avNULL;

    return av0;
    } // End of function avalue::clearout.

//--------------------------//
//  avalue::operator double //
//--------------------------//
avalue::operator double() {
    switch(type) {
    case avINTEGER:
        return double(i);
    case avREAL:
        return r;
    case avCOLONLIST: {
        double x = 0;
        Forall(avalue, p, *cl) {
            switch(p->type) {
            case avINTEGER:
                x *= 60;
                x += double(p->i);
                break;
            case avREAL:
                x *= 60;
                x += p->r;
                break;
            default:
                return 0;
                } // End of switch(p->type).
            } // End of Forall(p).
        return x;
        }
    default:
        return 0;
        }
    } // End of function avalue::operator double.

//--------------------------//
//   avalue::operator long  //
//--------------------------//
avalue::operator long() {
    switch(type) {
    case avINTEGER:
        return i;
    case avREAL:
        return (long)r;
    case avCOLONLIST: {
        long x = 0;
        Forall(avalue, p, *cl) {
            switch(p->type) {
            case avINTEGER:
                x *= 60;
                x += p->i;
                break;
            case avREAL:
                x *= 60;
                x += long(p->r);
                break;
            default:
                return 0;
                } // End of switch(p->type).
            } // End of Forall(p).
        return x;
        }
    default:
        return 0;
        }
    } // End of function avalue::operator long.

/*------------------------------------------------------------------------------
This function is true iff the avalue is an integer or a colon-separated list of
integers.
------------------------------------------------------------------------------*/
//----------------------//
// avalue::IntegerColon //
//----------------------//
bool_enum avalue::IntegerColon() const {
    // Either an integer or a colon-list of integers.
    if (type == avINTEGER)
        return true;
    if (type == avCOLONLIST) {
        Forall(avalue, p, *cl)
            if (p->type != avINTEGER)
                return false;
        return true;
        }
    return false;
    } // End of function avalue::IntegerColon.

/*------------------------------------------------------------------------------
attrlist::read() attempts to read an attribute block from a file.
A negative integer is returned if this fails.
It is assumed that the initial token "{" has been put back into the file.
------------------------------------------------------------------------------*/
//----------------------//
//    attrlist::read    //
//----------------------//
int attrlist::read(token_stream& ts) {
    token* pt1;
    if (!(ts >> pt1)) {
        ts_error(ts);
        cout << "End of stream at beginning of an attribute block.\n";
        return eEND_OF_STREAM;
        }
    if (!pt1->Leftbrace()) {
        cout << "Attribute block did not begin with '{'.\n";
        delete pt1;
        return eBAD_ATTRIBUTE_BLOCK_CHARACTER;
        }
    delete pt1;
    for (;;) {
        if (!(ts >> pt1)) {
            ts_error(ts);
            cout << "End of stream while reading an attribute block.\n";
            return eEND_OF_STREAM;
            }
        if (pt1->Rightbrace()) {
            delete pt1;
            break;
            }

        if (!pt1->Identifier()) {
            ts_error(ts);
            cout << "Error while reading an attribute name.\n";
            delete pt1;
            return eBAD_ATTRIBUTE;
            }
        c_string attrname = pt1->s;
        delete pt1;

        if (!(ts >> pt1)) { // Gobble the '='.
            ts_error(ts);
            cout << "Error while reading \'=\' after attribute name \""
                 << attrname << "\"\n";
            delete pt1;
            return eEND_OF_STREAM;
            }
        if (!pt1->Equals()) { // Gobble the '='.
            ts_error(ts);
            cout << "Didn't find \'=\' after attribute name \""
                 << attrname << "\"\n";
            delete pt1;
            return eMISSING_EQUALS;
            }
        delete pt1;

        avalue* attrvalue = new avalue;
        if (attrvalue->read(ts) < 0) {
            ts_error(ts);
            cout << "Error while reading an attribute value.\n";
            return eBAD_ATTRIBUTE;
            }

        // Gobble the ';'.
        if (!attrvalue->Block()) {
            if (!(ts >> pt1) || !pt1->Semicolon()) {
                ts_error(ts);
                cout << "Didn't find \';\' after value of attribute \""
                     << attrname << "\".\n";
                delete pt1;
                return eMISSING_SEMICOLON;
                }
            delete pt1;
            }

        // Append the new attribute.
        attr* pa = new attr;
        pa->name = attrname;
        pa->a = attrvalue;
        append(pa);
        }
    return 0;
    } // End of function attrlist::read.

//----------------------//
//    attrlist::print   //
//----------------------//
void attrlist::print(ostream& os, int indent, const char* ts) {
    Forall(attr, p, *this) {
        space(os, indent + defttab, ts);
        os << (p->name.nullstr() ? "null attribute name"
                                 : (const char*)p->name);
        os << " = ";
        if (p->a)
            p->a->print(os, indent + defttab, ts);
        else
            os << "no value";
        os << "\n";
        }
    } // End of function attrlist::print.

/*------------------------------------------------------------------------------
item::print() prints an "item" to stdout.
------------------------------------------------------------------------------*/
//----------------------//
//      item::print     //
//----------------------//
void item::print(ostream& os, int indent, const char* ts) {
    space(os, indent, ts);
    os << "Class: \""
       << (classname.nullstr() ? "none" : (const char*)classname);
    os << "\", name: \""
       << (name.nullstr() ? "none" : (const char*)name);
    os << "\", attributes:\n";
    attrs.print(os, indent, ts);
    } // End of function item::print.

/*------------------------------------------------------------------------------
itemlist::findname() returns a pointer to an item which has a given name.
A null pointer is returned if something goes wrong.
------------------------------------------------------------------------------*/
//--------------------------//
//    itemlist::findname    //
//--------------------------//
item* itemlist::findname(c_string& s) {
    item* p = 0;
    forall(p, *this)
        if (p->name == s)
            break;
    return p;
    } // End of function itemlist::findname.

/*------------------------------------------------------------------------------
itemblock::read() reads a system block from a data file.
The stream is expected to be aligned so that the next token is
the introductory left brace '{' of the system block.
If something reasonably nasty happens, then a negative integer is returned.
------------------------------------------------------------------------------*/
//----------------------//
//   itemblock::read    //
//----------------------//
int itemblock::read(token_stream& ts) {
    long linecount = 0;  // Keep count of number of '\n' chars read.
    token *pt1 = 0, *pt2 = 0, *pt3 = 0;

    if (!(ts >> pt1)) {
        ts_error(ts);
        cout << "End of file immediately after \"" << sysblockstr;
        cout << "\" keyword.\n";
        return eEND_OF_STREAM;
        }
    if (!pt1->Leftbrace()) {
        ts_error(ts);
        cout << "A \"" << sysblockstr << "\" keyword not followed by '{'.\n";
        delete pt1;
        return eBAD_SYSTEM_CHARACTER;
        }
    for (;;) {
        if (!(ts >> pt1)) {
            ts_error(ts);
            cout << "File ended before close of a \"";
            cout << sysblockstr << "\" block.\n";
            cout << "A \"" << sysblockstr
                 << "\" block must be terminated by '}'.\n";
            return eEND_OF_STREAM;
            }
        if (pt1->Rightbrace()) { // End of system block.
            delete pt1;
            break;
            }
        if (!pt1->Identifier()) {
            ts_error(pt1);
            delete pt1;
            return eCOMMAND_NAME_ERROR;
            }
        c_string classname = pt1->s;
        delete pt1;
        if (!(ts >> pt2)) {
            ts_error(ts);
            cout << "Something went wrong after reading an item class.\n";
            return eEND_OF_STREAM;
            }
        if (pt2->Dot()) {
            ts_error(ts);
            cout << "Global sub-variables not yet implemented.\n";
            return eBAD_ATTRIBUTE;
            }
        if (pt2->Equals()) { // The classname is a global variable name.
            delete pt2;
            avalue* attrvalue = new avalue;
            if (attrvalue->read(ts) < 0) {
                ts_error(ts);
                cout << "Error while reading a global variable value.\n";
                return eBAD_ATTRIBUTE;
                }
            // Gobble the ';'.
            if (!attrvalue->Block()) {
                if (!(ts >> pt3) || !pt3->Semicolon()) {
                    ts_error(ts);
                    cout << "Didn't find \';\' after value"
                            " of global variable \"" << classname << "\".\n";
                    delete pt3;
                    return eMISSING_SEMICOLON;
                    }
                delete pt3;
                }
            attr* pa = new attr;
            pa->name = classname;
            pa->a = attrvalue;
            globvars.append(pa); // Note that the last defn is always used.
            continue;            // See setattributes() in oralaksl.c.
            }
        c_string itemname;
        if (pt2->Leftbrace()) {
//            ts_error(ts);
//            cout << "Anonymous items/objects not yet implemented.\n";
            ts.putback(pt2);
            itemname = classname;
            itemname += "$";
            char* s = new_str(anon_item_count);
            if (s) {
                itemname += s;
                delete[] s;
                anon_item_count += 1;
                }
            }
        else if (!pt2->Identifier()) {
            ts_error(pt2);
            cout << "Something went wrong while reading an item name.\n";
            delete pt2;
            return eBAD_ITEM_NAME;
            }
        else {
            itemname = pt2->s;
            delete pt2;
            }

        // Check to see if the object has already been defined.
        if (findname(itemname)) {
            ts_error(ts);
            cout << "Multiply defined object \"" << itemname << "\"\n";
            return eNEW_OBJECT_NAME_CLASH;
            }

        item* itm = new item;
        itm->classname = classname;
        itm->name = itemname;

        if (!(ts >> pt3) || !pt3->Leftbrace()) { // Gobble the '{'.
            ts_error(ts);
            cout << "Didn't find \'{\' after object name \""
                 << itemname << "\"\n";
            delete pt3;
            return eMISSING_BRACKET;
            }
        delete pt3;
        for (;;) { // Read the attributes.
            if (!(ts >> pt2)) {
                ts_error(ts);
                cout << "Error in data file, reading an attribute name.\n";
                return eEND_OF_STREAM;
                }
            if (pt2->Rightbrace()) {
                delete pt2;
                break;
                }
            if (!pt2->Identifier()) {
                ts_error(pt2);
                cout << "Error while reading an attribute name.\n";
                delete pt2;
                return eBAD_ATTRIBUTE;
                }
            c_string attrname = pt2->s;
            delete pt2;
            if (!(ts >> pt3)) { // Gobble the '='.
                ts_error(ts);
                cout << "Error while reading \'=\' after attribute name \""
                     << attrname << "\"\n";
                delete pt3;
                return eEND_OF_STREAM;
                }
            if (pt3->Dot()) {
                ts_error(ts);
                cout << "Object sub-attributes not yet implemented.\n";
                return eBAD_ATTRIBUTE;
                }
            if (!pt3->Equals()) { // Gobble the '='.
                ts_error(ts);
                cout << "Didn't find \'=\' after attribute name \""
                     << attrname << "\"\n";
                delete pt3;
                return eMISSING_EQUALS;
                }
            delete pt3;
            avalue* attrvalue = new avalue;
            if (attrvalue->read(ts) < 0) {
                ts_error(ts);
                cout << "Error while reading an attribute value.\n";
                return eBAD_ATTRIBUTE;
                }
            // Gobble the ';'.
            if (!attrvalue->Block()) {
                if (!(ts >> pt3) || !pt3->Semicolon()) {
                    ts_error(ts);
                    cout << "Didn't find \';\' after value of attribute \""
                         << attrname << "\".\n";
                    delete pt3;
                    return eMISSING_SEMICOLON;
                    }
                delete pt3;
                }
            attr* pa = new attr;
            pa->name = attrname;
            pa->a = attrvalue;
            itm->attrs.append(pa);
            }

        items.append(itm);
        if (!ts)
            break;
        }
    return 0;
    } // End of function itemblock::read.

/*------------------------------------------------------------------------------
itemblock::findname() returns a pointer to an item which has a given name.
A null pointer is returned if something goes wrong.
------------------------------------------------------------------------------*/
//----------------------//
//  itemblock::findname //
//----------------------//
item* itemblock::findname(c_string& s) {
    item* p = 0;
    forall(p, items)
        if (p->name == s)
            break;
    return p;
    } // End of function itemblock::findname.

/*------------------------------------------------------------------------------
itemblock::resolvenames() resloves the names of all objects referred to
in attribute values of items in the list. If something goes wrong, a
negative value is returned.
------------------------------------------------------------------------------*/
//--------------------------//
//  itemblock::resolvenames //
//--------------------------//
int itemblock::resolvenames() {
    bool_enum allfound = true;

    // All items.
    attr* pa;
    Forall(item, p, items) {
        forall(pa, p->attrs)
            if (pa->a) {
                if (!pa->a->resolvenames(items)) {
                    cout << "Error while resolving attribute " << pa->name;
                    cout << " of item " << p->name << ".\n";
                    allfound = false;
                    }
                }
        }

    // All global variables.
    forall(pa, globvars)
        if (pa->a) {
            if (!pa->a->resolvenames(items)) {
                cout << "Error while resolving global variable ";
                cout << pa->name << ".\n";
                allfound = false;
                }
            }
    return (allfound ? 0 : eNOT_FOUND);
    } // End of function itemblock::resolvenames.

/*------------------------------------------------------------------------------
itemblock::print() prints out a list of items to a given stream.
------------------------------------------------------------------------------*/
//----------------------//
//   itemblock::print   //
//----------------------//
void itemblock::print(ostream& os, int indent, const char* ts) {
    os << "Global variables:\n\n";
    globvars.print(os, indent, ts);

    os << "\nItems:\n\n";
    Forall(item, p, items) {
        p->print(os, indent, ts);
        os << "\n";
        }
    } // End of function itemblock::print.

//----------------------//
//   itemptrlist::find  //
//----------------------//
itemptr* itemptrlist::find(item* pi) {
    itemptr* p = 0;
    forall(p, *this)
        if (p->p == pi)
            break;
    return p;
    } // End of function itemptrlist::find.

//----------------------//
//  itemptrlist::append //
//----------------------//
itemptr* itemptrlist::append(item* pi) {
    itemptr* pip = new itemptr;
    pip->p = pi;
    append(pip);
    return pip;
    } // End of function itemptrlist::append.

/*------------------------------------------------------------------------------
This routine prints the item list with arrows to show which members of the
the list point to which others. On the left, the basic "next" linkage is
shown. On the right, an arrow is shown to any occurrence of the item "pi"
is shown.
------------------------------------------------------------------------------*/
//----------------------//
//  itemptrlist::print  //
//----------------------//
void itemptrlist::print(item* pi, ostream& os) {
    bool_enum pi_found = false;
    Forall(itemptr, p, *this) {
        if (p == first())
            os << "    ";
        else
            os << " -> ";
        if (p->p)
            os << p->p->classname << " " << p->p->name;
        else
            os << "0";
        if (!p->next() && pi_found)
            os << " ->";
        if (p->p == pi) {
            os << " <-";
            pi_found = true;
            }
        os << "\n";
        }
    } // End of function itemptrlist::print.

/*------------------------------------------------------------------------------
oralsystemlist::getsystem() returns a pointer to an oralsystem which has a
given name. A null pointer is returned if something goes wrong.
------------------------------------------------------------------------------*/
//------------------------------//
//   oralsystemlist::getsystem  //
//------------------------------//
oralsystem* oralsystemlist::getsystem(c_string& s) {
    oralsystem* p = 0;
    forall(p, *this)
        if (p->name == s)
            break;
    if (!p) {
        p = new oralsystem;
        p->name = s;
        append(p);
        }
    return p;
    } // End of function oralsystemlist::getsystem.

/*------------------------------------------------------------------------------
oraldata::readfile() reads a data file, and merges the contents into the
already existing contents of the oraldata structure. A negative error code is
returned if something goes wrong. If the filename parameter is the null
pointer, then standard input is used for input.
------------------------------------------------------------------------------*/
//----------------------//
//  oraldata::readfile  //
//----------------------//
int oraldata::readfile(c_string& filename, int trace, c_stringlist* prev) {
    int err = 0;
    c_stringlist filenamelist;  // For detection of file inclusion loops.
                                // Used if and only if prev == 0.
    if (prev) {
        if (prev->find(filename)) {
            cout << "oraldata::readfile: detected file inclusion loop.\n";
            return eFILE_INCLUSION_LOOP;
            }
        }
    else
        prev = &filenamelist;
    prev->append(filename);

    token_stream ts;
    if (!ts.open((const char*)filename)) {
        cout << "Couldn't open file \"" << (const char*)filename << "\".\n";
        return eFILE_OPEN_FAILED;
        }

    token *pt1 = 0, *pt2 = 0, *pt3 = 0;
    for (;;) { // For all commands in the file...
        if (!(ts >> pt1))
            break;
        if (!pt1->Identifier()) {
            ts_error(pt1);
            delete pt1;
            return eCOMMAND_NAME_ERROR;
            }
        if (pt1->s == sysblockstr) {
            delete pt1;
            if (!(ts >> pt2)) {
                ts_error(ts);
                return eEND_OF_FILE;
                }
            c_string systemname;
            if (pt2->Identifier()) {
                systemname = pt2->s;
                delete pt2;
                if (!(ts >> pt3)) {
                    ts_error(ts);
                    return eEND_OF_FILE;
                    }
                if (!pt3->Leftbrace()) {
                    ts_error(pt3);
                    delete pt3;
                    return eCOMMAND_NAME_ERROR;
                    }
                ts.putback(pt3);
                }
            else if (pt2->Leftbrace()) {
                systemname = "";
                ts.putback(pt2);
                }
            else {
                ts_error(pt2);
                delete pt2;
                return eCOMMAND_NAME_ERROR;
                }

            if (trace >= 1) {
                cout << "Reading system";
                systemname.print_with_quotes(cout);
                cout << "...\n";
                }

            // Find or create system.
            oralsystem* sys = oslist.getsystem(systemname);
            if (!sys) {
                cout << "Error while getting a system called \"";
                cout << systemname << "\".\n";
                return eERROR_GETTING_SYSTEM;
                }
            if (sys->items.read(ts) < 0) { // Read contents of the system.
                cout << "Error encountered while reading system block.\n";
                return eERROR_IN_SYSTEM;
                }
            if (trace >= 2) {
                cout << "Contents of file " << filename << ":\n";
                sys->items.print(cout);
                }
            if (trace >= 1)
                cout << "Resolving pointers...\n";
            if (sys->items.resolvenames() != 0) {
                cout << "Error encountered while resolving names.\n";
                return eNAME_RESOLUTION_ERROR;
                }
            }
        else if (pt1->s == (const char*)"package") {
            delete pt1;
            if (!(ts >> pt2) || !pt2->Identifier()) {
                ts_error(ts);
                cout << "Expected a package name after \"package\".\n";
                delete pt2;
                return ePACKAGE_NAME_ERROR;
                }
            c_string packagename = pt2->s;
            delete pt2;
            if (!(ts >> pt2) || !pt2->Semicolon()) { // Gobble the ';'.
                ts_error(ts);
                cout << "Didn't find \';\' after package declaration: \""
                     << packagename << "\".\n";
                delete pt2;
                return eMISSING_SEMICOLON;
                }
            delete pt2;
            if (!packagenames.find(packagename))
                packagenames.append(packagename);
            }
        else if (pt1->s == (const char*)"include") {
            // Note: It is important when "readfile" is called that the
            // current oraldata structure be in a valid state.
            delete pt1;
            pt1 = 0;
            if (!(ts >> pt1) || !pt1->String()) {
                ts_error(ts);
                cout << "Error in file name after \"include\".\n";
                if (!pt1)
                    cout << "---- error in tokenising.\n";
                else if (!pt1->String()) {
                    cout << "---- not a string: ";
                    pt1->print(cout);
                    cout << endl;
                    }
                delete pt1;
                return eFILE_NAME_ERROR;
                }
            c_string incfilename = pt1->pc;
            delete pt1;
            if (prev->find(incfilename)) {
                ts_error(ts);
                cout << "File inclusion loop. Inclusion sequence was:\n";
                Forall(c_stringlink, pcsl, *prev)
                    cout << "    " << *pcsl << "\n";
                cout << "    " << incfilename << "\n";
                return eFILE_INCLUSION_LOOP;
                }
            if ((err = readfile(incfilename, trace, prev)) < 0) {
                ts_error(ts);
                cout << "Error while parsing file " << incfilename << ": ";
                cout << error_string(err) << ".\n";
                return err;
                }
            }
        else { // Unrecognised data file command.
            ts_error(pt1);
            cout << "Unrecognised command \"" << pt1->s;
            cout << "\" in file \"" << filename << "\".\n";
            delete pt1;
            return eUNRECOGNISED_COMMAND;
            }
        if (!ts)
            break;
        }
    return 0;
    } // End of function oraldata::readfile.
