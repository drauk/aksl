// src/aksl/objptr.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

objptrlist::
    find
    add
    append
    prepend
    remove
    copy
objcat::
    add(objptrlist&)
    add(objectlist&)
objcatlist::
    add
    print
    findlist
------------------------------------------------------------------------------*/

#include "aksl/objptr.h"

/*------------------------------------------------------------------------------
objptrlist::find() returns a objptr to the first "objptr" which points at the
given object, or else zero if there is none.
------------------------------------------------------------------------------*/
//----------------------//
//   objptrlist::find   //
//----------------------//
objptr* objptrlist::find(object* pe) {
    objptr* p = 0;
    forall(p, *this)
        if (p->dest == pe)
            break;
    return p;
    } // End of function objptrlist::find.

/*------------------------------------------------------------------------------
Function objptrlist::add() appends a objptr to an object if the object is not
already there.
------------------------------------------------------------------------------*/
//----------------------//
//    objptrlist::add   //
//----------------------//
objptr* objptrlist::add(object* pe) {
//    if (!pe)
//        return 0;
    objptr* addr = find(pe);
    if (!addr) {
        addr = new objptr;
        addr->dest = pe;
        append(addr);
        }
    return addr;
    } // End of function objptrlist::add.

/*------------------------------------------------------------------------------
Function objptrlist::append() appends a objptr to an object whether there
is a copy of the address in the list already or not.
------------------------------------------------------------------------------*/
//----------------------//
//  objptrlist::append  //
//----------------------//
objptr* objptrlist::append(object* pe) {
    objptr* addr = new objptr;
    addr->dest = pe;
    append(addr);
    return addr;
    } // End of function objptrlist::append.

/*------------------------------------------------------------------------------
Function objptrlist::prepend() prepends a objptr to an object whether there
is a copy of the address in the list already or not.
------------------------------------------------------------------------------*/
//----------------------//
// objptrlist::prepend  //
//----------------------//
objptr* objptrlist::prepend(object* pe) {
    objptr* addr = new objptr;
    addr->dest = pe;
    prepend(addr);
    return addr;
    } // End of function objptrlist::prepend.

/*------------------------------------------------------------------------------
Function objptrlist::remove1() removes the first objptr in the list which points
to a given object.
------------------------------------------------------------------------------*/
//----------------------//
//  objptrlist::remove1 //
//----------------------//
void objptrlist::remove1(object* pe) {
    objptr* p = 0;
    forall(p, *this)
        if (p->dest == pe)
            break;
    if (p)
        delremove(p);
    } // End of function objptrlist::remove1.

/*------------------------------------------------------------------------------
Function objptrlist::removeall() removes all objptrs which point to an object.
------------------------------------------------------------------------------*/
//--------------------------//
//   objptrlist::removeall  //
//--------------------------//
void objptrlist::removeall(object* pe) {
    for (objptr* p = first(); p; ) {
        objptr* q = p->next();
        if (p->dest == pe)
            delremove(p);
        p = q;
        }
    } // End of function objptrlist::removeall.

/*------------------------------------------------------------------------------
objptrlist::add() searches the given list of objects, and adds a new
object-pointer for each object of the given type "s".
------------------------------------------------------------------------------*/
//----------------------//
//    objptrlist::add   //
//----------------------//
void objptrlist::add(const objectlist& elist, const c_string& s) {
    Forall(object, e, elist)
        if (s == e->type())
            add(e);
    } // End of function objptrlist::add.

/*------------------------------------------------------------------------------
objptrlist::copy() clears the current list and then copies the elements of the
other list.
------------------------------------------------------------------------------*/
//----------------------//
//   objptrlist::copy   //
//----------------------//
void objptrlist::copy(const objptrlist& opl) {
    clear();
    Forall(objptr, p, opl) {
        objptr* po = new objptr;
        po->dest = p->dest;
        append(po);
        }
    } // End of function objptrlist::copy.

/*------------------------------------------------------------------------------
objcat::add() searches the given list for objects whose type matches the
current "type" string, and adds them to the current list "objs" if they are
not already there.
------------------------------------------------------------------------------*/
//----------------------//
//      objcat::add     //
//----------------------//
void objcat::add(objptrlist& epl) {
    Forall(objptr, p, epl)
        if (p->dest && type == p->dest->type())
            objs.add(p->dest);
    } // End of function objcat::add.

/*------------------------------------------------------------------------------
objcat::add() searches the given list for objects whose type matches the
current "type" string, and adds them to the current list "objs" if they are
not already there.
------------------------------------------------------------------------------*/
//----------------------//
//      objcat::add     //
//----------------------//
void objcat::add(objectlist& el) {
    Forall(object, p, el)
        if (type == p->type())
            objs.add(p);
    } // End of function objcat::add.

/*------------------------------------------------------------------------------
objcatlist::add() searches the given list of objects, and adds a new category
for each new type found in the list. Pointers to the objects are added to the
appropriate category lists.
------------------------------------------------------------------------------*/
//----------------------//
//    objcatlist::add   //
//----------------------//
void objcatlist::add(objectlist& elist) {
    Forall(object, e, elist) {
        objcat* c = 0;
        forall(c, *this)
            if (c->type == e->type())
                break;
        if (!c) {
            c = new objcat;
            c->settype(e->type());
            append(c);
            }
        c->add(e);
        }
    } // End of function objcatlist::add.

//----------------------//
//   objcatlist::print  //
//----------------------//
void objcatlist::print(ostream& os) {
    os << "Contents of objcatlist:\n";
    Forall(objcat, c, *this) {
        os << "In class " << c->type << ", the objects are:\n";
        Forall(objptr, e, c->objs)
            if (e->dest)
                os << "    \"" << e->dest->name << "\"\n";
        }
    } // End of function objcatlist::print.

/*------------------------------------------------------------------------------
objcatlist::findlist() returns a null pointer is there is no category with
the given name. Otherwise a pointer to the objptrlist for that category is
returned.
------------------------------------------------------------------------------*/
//----------------------//
// objcatlist::findlist //
//----------------------//
objptrlist* objcatlist::findlist(c_string& s) {
    if (s.null())
        return 0;
    objcat* p = 0;
    forall(p, *this)
        if (p->type == s)
            break;
    return p ? &p->objs : 0;
    } // End of function objcatlist::findlist.
