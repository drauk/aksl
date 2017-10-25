// src/aksl/oralaksl.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/oralaksl.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

avaluelist2value
acolonlist2value
attrlist2value
makeobjects
setattributes
readdatafile
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The principal export from this compilation unit is function "readdatafile".

Function "readdatafile" creates objects with the relations and attributes
indicated in the input file (in the ORAL data language), using the functions
"makeobjects" and "setattributes" which are defined in this file.

Function "makeobjects" simply creates an appropriate "object" structure for each
"item" in the input file.

Function "setattributes" sets all attributes of all objects in all systems.

The functions avaluelist2value, acolonlist2value and attrlist2value are used as
part of the process of setting all attributes of all objects.
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/oralaksl.h"
#ifndef AKSL_ORAL_H
#include "aksl/oral.h"
#endif

// System header files:
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif

// Forward function reference:
static void acolonlist2value(value& v, acolonlist* p, systm* sys);
static void attrlist2value(value& v, attrlist* p, systm* sys);

/*------------------------------------------------------------------------------
avaluelist2value enables an "avaluelist" to be assigned to a "value". This is
done by creating a "valuelist", and then making a new "value" point to it. The
result is a true copy, not just a set of pointers to the old data.
If "p" is null, then the "value" is assigned to an empty value list.
------------------------------------------------------------------------------*/
//----------------------//
//   avaluelist2value   //
//----------------------//
static void avaluelist2value(value& v, avaluelist* p, systm* sys) {
    v.clear();
    valuelist* newvlist = new valuelist;
    v = newvlist;
    if (!p)
        return;
    Forall(avalue, pa, *p) {
        value* pv = new value;
        switch(pa->type) {
        case avSTRING: // This copying causes the "value" to possess a string.
            *pv = pa->s_refname.new_strcpy();
            break;
        case avINTEGER:
            *pv = pa->i;
            break;
        case avREAL:
            *pv = pa->r;
            break;
        case avREF: {
            item* pi = pa->itm;
            object* po = pi ? (object*)pi->obj : 0;
            *pv = po;
            }
            break;
        case avLIST:
            avaluelist2value(*pv, pa->l, sys);      // Recursive call.
            break;
        case avBLOCK:
            attrlist2value(*pv, pa->blk, sys);      // Recursive call.
            break;
        case avCOLONLIST:
            acolonlist2value(*pv, pa->cl, sys);      // Recursive call.
            break;
        case avNULL:
        default:
            break;
            }
        newvlist->append(pv);
        }
    } // End of function avaluelist2value.

/*------------------------------------------------------------------------------
acolonlist2value enables an "acolonlist" to be assigned to a "value". This is
done by creating a "colonlist", and then making a new "value" point to it. The
result is a true copy, not just a set of pointers to the old data.
If "p" is null, then the "value" is assigned to an empty value list.
------------------------------------------------------------------------------*/
//----------------------//
//   acolonlist2value   //
//----------------------//
static void acolonlist2value(value& v, acolonlist* p, systm* sys) {
    v.clear();
    colonlist* newvlist = new colonlist;
    v = newvlist;
    if (!p)
        return;
    Forall(avalue, pa, *p) {
        value* pv = new value;
        switch(pa->type) {
        case avSTRING: // This copying causes the "value" to possess a string.
            *pv = pa->s_refname.new_strcpy();
            break;
        case avINTEGER:
            *pv = pa->i;
            break;
        case avREAL:
            *pv = pa->r;
            break;
        case avREF: {
            item* pi = pa->itm;
            object* po = pi ? (object*)pi->obj : 0;
            *pv = po;
            }
            break;
        case avLIST:
            avaluelist2value(*pv, pa->l, sys);      // Recursive call.
            break;
        case avBLOCK:
            attrlist2value(*pv, pa->blk, sys);      // Recursive call.
            break;
        case avCOLONLIST:
            acolonlist2value(*pv, pa->cl, sys);      // Recursive call.
            break;
        case avNULL:
        default:
            break;
            }
        newvlist->append(pv);
        }
    } // End of function avaluelist2value.

/*------------------------------------------------------------------------------
attrlist2value enables an "attrlist" to be assigned to a "value". This is done
by creating a "tagvaluelist", and then making a new "value" point to it. The
result is not just a set of pointers to the old data.
If "p" is null, then the "value" is assigned to an empty tagvalue list.
The tags in the returned attribute list are all _global_ tags.
------------------------------------------------------------------------------*/
//----------------------//
//    attrlist2value    //
//----------------------//
static void attrlist2value(value& v, attrlist* p, systm* sys) {
    v.clear();
    tagvaluelist* newtvlist = new tagvaluelist;
    v = newtvlist;
    if (!p)
        return;
    Forall(attr, pa, *p) {
        tagvalue* ptv = new tagvalue;

        // Look up the tag in the system global message table:
        ptv->tag = sys->globkey(pa->name);

        avalue* pav = pa->a;            // Should always be non-null.
        if (!pav) {
            newtvlist->append(ptv);
            continue;
            }
        switch(pav->type) {
        case avSTRING: // This copying causes the "value" to possess a string.
            *ptv = pav->s_refname.new_strcpy();
            break;
        case avINTEGER:
            *ptv = pav->i;
            break;
        case avREAL:
            *ptv = pav->r;
            break;
        case avREF: {
            item* po = pav->itm;
            object* pe = po ? (object*)po->obj : 0;
            *ptv = pe;
            }
            break;
        case avLIST:
            avaluelist2value(*ptv, pav->l, sys);    // Recursive call.
            break;
        case avBLOCK:
            attrlist2value(*ptv, pav->blk, sys);    // Recursive call.
            break;
        case avCOLONLIST:
            acolonlist2value(*ptv, pav->cl, sys);   // Recursive call.
            break;
        case avNULL:
        default:
            break;
            }
        newtvlist->append(ptv);
        }
    } // End of function attrlist2value.

/*------------------------------------------------------------------------------
This function just traverses the given "itemblock", creating objects within the
given "systm", using the given simulation "model" for class definitions for the
objects. A pointer to each created "object" is set in the corresponding
parse-tree "item", in preparation for the attribute-setting phase. This way of
doing things makes it possible for objects to reference both forwards and
backwards in system description files.
------------------------------------------------------------------------------*/
//----------------------//
//     makeobjects      //
//----------------------//
static int makeobjects(itemblock& ol, model& mdl, systm& sys) {
    Forall(item, pi, ol.items) {
        object* po = mdl.newobject(sys, pi->classname, pi->name);
        if (!po) {
            cout << "Failed to create object " << pi->name
                 << " of type " << pi->classname << DOTNL;
            return eOBJECT_NOT_CREATED;
            }
        pi->obj = po;               // Use the spare pointer left by item::.
        }
    return 0;
    } // End of function makeobjects.

/*------------------------------------------------------------------------------
This function should be called after "makeobjects" to set all attributes of all
objects in the given "itemblock", which are assumed to have been created already
in the given "systm".
First, all of the global variables indicated in the given "itemblock" are
created in the given "systm", and are set to the values indicated in the input
file. Since some of these values may involve references (pointers) to objects
in the "systm", it is essential to call "makeobjects" beforehand.
Then all of the object attributes are set.

The global variables are set with the function systm::setglob, which accepts a
string parameter for the variable name and a heap-allocated "value" for the
value of the variable.

The object attributes are set with the function object::set_attr, which takes
the same name/value parameters.

In the case of global variables, the name of the variable is stored as a copy of
the string. In the case of object attributes, the attribute name is passed to
the object straight away during processing as a local "mtype" integer, which is
located with the function package::cs_mesgkeys::key, which is looked up in the
package::cs_mesgkeys string list.
------------------------------------------------------------------------------*/
//----------------------//
//     setattributes    //
//----------------------//
static int setattributes(itemblock& ol, systm* sys) {
    if (sys) {
        Forall(attr, pa, ol.globvars) {
            if (!pa->name || !pa->a)
                continue;
            value* pv = new value;
            switch(pa->a->type) {
            case avINTEGER:
                *pv = pa->a->i;
                sys->setglob(pa->name, pv);
                break;
            case avREAL:
                *pv = pa->a->r;
                sys->setglob(pa->name, pv);
                break;
            case avSTRING: // Note that users must make their own string copies!
                *pv = pa->a->s_refname.new_strcpy();
                sys->setglob(pa->name, pv);
                break;
            case avREF: {
                item* po = pa->a->itm;
                if (!po) { // This should never happen.
                    delete pv;
                    break;
                    }
                *pv = (object*)po->obj;
                sys->setglob(pa->name, pv);
                }
                break;
            case avLIST:
                // Overloaded assignment for this is rejected by the compiler.
                avaluelist2value(*pv, pa->a->l, sys);
                sys->setglob(pa->name, pv);
                pa->a->l = 0; // Dodgy!
                break;
            case avBLOCK:
                attrlist2value(*pv, pa->a->blk, sys);
                sys->setglob(pa->name, pv);
                pa->a->blk = 0; // Dodgy!
                break;
            case avCOLONLIST:
                // Overloaded assignment for this is rejected by the compiler.
                acolonlist2value(*pv, pa->a->cl, sys);
                sys->setglob(pa->name, pv);
                pa->a->cl = 0; // Dodgy!
                break;
            case avNULL:
            default:
                break;
                }
            }
        }

    value v;
    Forall(item, p, ol.items) {
        if (!p->obj)
            continue;
        Forall(attr, pa, p->attrs) {
            if (!pa->name || !pa->a)
                continue;
            int ret = 0;
            switch(pa->a->type) {
            case avSTRING: // Note that users must make their own string copies!
                v = pa->a->s_refname.new_strcpy();
                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avINTEGER:
                v = pa->a->i;
                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avREAL:
                v = pa->a->r;
                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avREF: {
                item* po = pa->a->itm;
                if (!po) // This should never happen.
                    break;
                v = (object*)po->obj;
                ret = ((object*)p->obj)->set_attr(pa->name, v);
                }
                break;
            case avLIST:
                // Overloaded assignment for this is rejected by the compiler.
                avaluelist2value(v, pa->a->l, sys);

                // Convert global to local attribute types in "v":
                // .... ???

                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avBLOCK:
                // Overloaded assignment for this is rejected by the compiler.
                attrlist2value(v, pa->a->blk, sys);

                // Convert global to local attribute types in "v":
                // .... ???

                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avCOLONLIST:
                // Overloaded assignment for this is rejected by the compiler.
                acolonlist2value(v, pa->a->cl, sys);

                // Convert global to local attribute types in "v":
                // .... ???

                ret = ((object*)p->obj)->set_attr(pa->name, v);
                break;
            case avNULL:
            default:
                break;
                }
            if (ret < 0) {
                cout << "Error while setting attribute \"" << pa->name;
                cout << "\" of object " << ((object*)p->obj)->name << DOTNL;
                return eERROR_SETTING_ATTRIBUTE;
                }
            }
        }
    return 0;
    } // End of function setattributes.

/*------------------------------------------------------------------------------
This function reads the contents of a file with the name "filename" into the
"model" structure "mdl". The file is first parsed, and then structures of class
"object" are created to represent the system described in the file.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
First, a parse-tree object of class "oraldata" is created to hold the results of
parsing the file.
Then the file is parsed with the function oraldata::readfile.
Then the simulation packages requested in the input file are loaded into the
system model "mdl" to provide the "object"s to simulate the system described.
Then for each system (typically on system only) in the input file:
-   create "object" structures according to the file's descriptions
-   set the indicated attribute lists for each each object created.
If all goes well, 0 is returned. Otherwise a negative integer is returned,
indicating the category of failure.
The second to last step invokes function "makeobjects", which is defined in this
file.
The last step (attribute setting) is performed by calling function
"setattributes" which is also defined in this file.
------------------------------------------------------------------------------*/
//----------------------//
//     readdatafile     //
//----------------------//
int readdatafile(model& mdl, c_string& filename, int trace) {
    oraldata od;
    oralsystem* os;
    int err;

    // Use the ORAL parser to read the file:
    if ((err = od.readfile(filename, trace)) < 0) {
        cout << "Error while parsing file \"" << filename << "\": ";
        cout << error_string(err) << DOTNL;
        return err;
        }
    if (trace >= 1) {
        long nsys = od.oslist.length();
        if (nsys == 0)
            cout << "No systems found in file \"" << filename << "\".\n";
        else if (nsys == 1 && od.oslist.first()->name.nullstr()) {
            cout << "Have just read the system.\n";
            }
        else {
            cout << "Have just read the following system";
            if (nsys > 1)
                cout << "s";
            cout << " from file \"" << filename << "\":\n   ";
            forall(os, od.oslist)
                os->name.print_with_quotes(cout);
            cout << DOTNL;
            }
        }

    // Load all of the referenced simulation packages.
    Forall(strng, sp, od.packagenames) {
        if (notnullstr(sp->s)) {
            if ((err = mdl.load(sp->s)) < 0) {
                cout << "Error while loading module \"" << sp->s << "\".\n";
                return err;
                }
            }
        }

    // Create objects for each object description in the input file.
    forall(os, od.oslist) {
        if (!os->name)
            continue;
        if (trace >= 1) {
            cout << "Creating objects in system";
            os->name.print_with_quotes(cout);
            cout << "...\n";
            }

        // Find or create system with requested name.
        systm* sys = mdl.getsystem(os->name);
        if (!sys) {
            cout << "Error while getting a system called \"";
            cout << os->name << "\".\n";
            return eERROR_GETTING_SYSTEM;
            }

        // Make objects for each object desription in the input file:
        if (makeobjects(os->items, mdl, *sys) < 0) {
            cout << "Error encountered while creating objects.\n";
            return eOBJECT_CREATION_ERROR;
            }

        // Set the attributes of all objects in the system, according to
        // the attribute lists in the input file:
        if (trace >= 1)
            cout << "Setting attributes...\n";
        if (setattributes(os->items, sys) < 0) {
            cout << "Error encountered while setting attributes.\n";
            return eATTRIBUTE_SETTING_ERROR;
            }
        }

    return 0;
    } // End of function readdatafile.
