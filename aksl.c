// src/aksl/aksl.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

object::
    object::
    set_attr
    get_real_attr
    get_int_attr
    get_object_attr
    get_datum_attr
    get_real_attr
    get_int_attr
    get_object_attr
    get_datum_attr
event::
    print
event_heap::
    print
globvarlist::
    set
    set
    get
systm::
    add
    cancel_message
    newevent_abs
    newevent_abs
    newevent_abs
    newevent_abs
    newevent_abs
    broadcast
    simulate
    sgetglob
model::
    newsystem
    getsystem
    link(package*)
    link1(package* (*)())
    link2(package* (**)())
    load(package*)
    load1(package*(*)())
    load2(package*(**)())
    load(c_string&)
    missingpackages
    newobject
    print_message
    print
------------------------------------------------------------------------------*/

// AKSL header files.
#include "aksl/aksl.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files.
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

// Fast block memory for "event"s.
bmem_define(event, bmem0);

//----------------------//
//    object::object    //
//----------------------//
object::object() {
    index = -1;
    mloc2glob = 0;
    mglob2loc = 0;
    sys = 0;
    pkg = 0;
    error = 0;
    } // End of function object::object.

/*------------------------------------------------------------------------------
This is the default set_attr handler for class "object".
------------------------------------------------------------------------------*/
//----------------------//
//   object::set_attr   //
//----------------------//
int object::set_attr(attr_type& attr, value& v) {
    if (!attr.next())
        return set_attr(attr.type, v);

    // Derived classes which want to handle sub-attributes should have code
    // here to do the handling. The default is to just flag an error.
    return -1;
    } // End of function object::set_attr.

//--------------------------//
//   object::get_real_attr  //
//--------------------------//
int object::get_real_attr(object* p, mtype m, double& r) {
    value* pv = get_attr(p, m);
    if (!pv || pv->type() != vREAL)
        return eNO_VALUE;
    r = (double)*pv;
    delete pv;
    return 0;
    } // End of function object::get_real_attr.

//--------------------------//
//   object::get_int_attr   //
//--------------------------//
int object::get_int_attr(object* p, mtype m, long& i) {
    value* pv = get_attr(p, m);
    if (!pv || pv->type() != vINTEGER)
        return eNO_VALUE;
    i = (long)*pv;
    delete pv;
    return 0;
    } // End of function object::get_int_attr.

//--------------------------//
//  object::get_object_attr //
//--------------------------//
int object::get_object_attr(object* p, mtype m, object*& po) {
    value* pv = get_attr(p, m);
    if (!pv || pv->type() != vOBJECT)
        return eNO_VALUE;
    po = *pv;
    delete pv;
    return 0;
    } // End of function object::get_object_attr.

//--------------------------//
//  object::get_datum_attr  //
//--------------------------//
int object::get_datum_attr(object* p, mtype m, datum*& pd) {
    value* pv = get_attr(p, m);
    if (!pv || pv->type() != vDATUM)
        return eNO_VALUE;
    pd = *pv;
    delete pv;
    return 0;
    } // End of function object::get_datum_attr.

//--------------------------//
//   object::get_real_attr  //
//--------------------------//
int object::get_real_attr(object* p, mtype m, const value& arg, double& r) {
    value* pv = get_attr(p, m, arg);
    if (!pv || pv->type() != vREAL)
        return eNO_VALUE;
    r = (double)*pv;
    delete pv;
    return 0;
    } // End of function object::get_real_attr.

//--------------------------//
//   object::get_int_attr   //
//--------------------------//
int object::get_int_attr(object* p, mtype m, const value& arg, long& i) {
    value* pv = get_attr(p, m, arg);
    if (!pv || pv->type() != vINTEGER)
        return eNO_VALUE;
    i = (long)*pv;
    delete pv;
    return 0;
    } // End of function object::get_int_attr.

//--------------------------//
//  object::get_object_attr //
//--------------------------//
int object::get_object_attr(object* p, mtype m, const value& arg, object*& po) {
    value* pv = get_attr(p, m, arg);
    if (!pv || pv->type() != vOBJECT)
        return eNO_VALUE;
    po = *pv;
    delete pv;
    return 0;
    } // End of function object::get_object_attr.

//--------------------------//
//  object::get_datum_attr  //
//--------------------------//
int object::get_datum_attr(object* p, mtype m, const value& arg, datum*& pd) {
    value* pv = get_attr(p, m, arg);
    if (!pv || pv->type() != vDATUM)
        return eNO_VALUE;
    pd = *pv;
    delete pv;
    return 0;
    } // End of function object::get_datum_attr.

//--------------------------//
//       event::print       //
//--------------------------//
void event::print(ostream& os, systm* ps) const {
    os << "t = " << t << ",\n    ";
    if (ps) {
        os << "\"";
        if (!ps->print_message(mty, os))
            os << "Illegal message";
        os << "\", ";
        }
    os << (orig ? (const char*)orig->name : "Cancelled event");
    os << " -> " << (dest ? (const char*)dest->name : "BROADCAST") << DOTNL;
    datum* pd;
    if (orig && arg && (pd = *arg) != 0)
        pd->print(os);
    } // End of function event::print.

//----------------------//
//   event_heap::print  //
//----------------------//
void event_heap::print(ostream& os, systm* ps) {
    os << "Contents of event_heap 0x" << hex8((long)this) << ":\n";
    os << "    Size of heap = " << length() << DOTNL;
    if (!empty()) {
        os << "    First element is:\n    ";
        first()->print(os, ps);
        }
    } // End of function event_heap::print.

/*------------------------------------------------------------------------------
globvarlist::set(c_string&, value*) sets a global variable in the list.
Note that copies are NOT made of values given as parameters. The given value
must therefore be allocated on the heap and never used again!
------------------------------------------------------------------------------*/
//------------------------------------------//
//    globvarlist::set(c_string&, value*)   //
//------------------------------------------//
int globvarlist::set(const c_string& name, value* pv) {
    if (name.null())
        return 0;
    globvar* pg = 0;
    forall(pg, *this)
        if (pg->name == name)
            break;
    if (pg) { // If the globvar name exists, re-set it.
        delete pg->val;
        pg->val = pv;
        return 0;
        }

    // If the globvar name is not found, add a new name to the list.
    pg = new globvar;
    pg->name = (const char*)name;   // Workaround because of C++ const problem.
    pg->val = pv;

    append(pg);
    return 0;
    } // End of function globvarlist::set(c_string&, value*).

/*------------------------------------------------------------------------------
globvarlist::set(c_string&, c_string&) sets a global variable in the list.
A copy is made of the string value of the global variable.
------------------------------------------------------------------------------*/
//----------------------------------------------//
//    globvarlist::set(c_string&, c_string&)    //
//----------------------------------------------//
int globvarlist::set(const c_string& name, const c_string& sval) {
    if (name.null())
        return 0;

    value* pv = new value;
    *pv = sval.new_strcpy();
    set(name, pv);

    return 0;
    } // End of function globvarlist::set(c_string&, c_string&).

/*------------------------------------------------------------------------------
globvarlist::get() returns the value of a global variable.
A new copy of the value is not created. The user may only read the value!
------------------------------------------------------------------------------*/
//----------------------//
//   globvarlist::get   //
//----------------------//
value* globvarlist::get(const c_string& name) {
    if (name.null())
        return 0;
    globvar* pg = 0;
    forall(pg, *this)
        if (pg->name == name)
            break;
    return pg ? pg->val : 0;
    } // End of function globvarlist::get.

//----------------------//
//      systm::add      //
//----------------------//
int systm::add(object* po) {
    if (!po)
        return eBAD_ARGUMENT;
    objects.append(po);
    po->sys = this;
    return 0;
    } // End of function systm::add.

/*------------------------------------------------------------------------------
Safe version of cancel_message(), which checks to make sure that the event is
still in the FEL. The given event is not even read until it is established that
it is actually in the FEL. Unfortunately, the same patch of memory may have been
re-used by another event, which will now get clobbered!!!! The object should
really take note of when events arrive, so as not to clobber re-used space.
One way to prevent this sort of problem would be to put a sequence number in all
events, but this would give a nasty space and time overhead. However, if there
is demand for this sort of thing, it could be an option some day.
------------------------------------------------------------------------------*/
//--------------------------//
//  systm::cancel_message   //
//--------------------------//
void systm::cancel_message(event* pe, object* po) {
    if (!pe || !po)
        return;
    event_heap_traversal eh(events);
    for (event* pe1 = eh.next(); pe1; pe1 = eh.next()) {
        if (pe1 == pe) {
            if (pe->origin() == po)
                pe->cancel();
            break;
            }
        }
    } // End of function systm::cancel_message.

//----------------------//
//  systm::newevent_abs //
//----------------------//
event* systm::newevent_abs(
        double t, object* orig, object* dest, mtype msg, value* arg) {
    if (t < clck) {
        if (!orig)
            cout << "Warning: negative delay event attempted by null object.\n";
        else
            cout << "Warning: negative delay event attempted by "
                 << orig->name << DOTNL;
        return 0;
        }
    if (!orig)
        return 0;
    event* pe = new event(t, orig, dest, msg);
    pe->arg = arg;
    events.insert(pe);
    return pe;
    } // End of function systm::newevent_abs.

//----------------------//
//  systm::newevent_abs //
//----------------------//
event* systm::newevent_abs(double t, object* orig, object* dest, mtype msg) {
    if (t < clck) {
        if (!orig)
            cout << "Warning: negative delay event attempted by null object.\n";
        else
            cout << "Warning: negative delay event attempted by "
                  << orig->name << DOTNL;
        return 0;
        }
    if (!orig)
        return 0;
    event* pe = new event(t, orig, dest, msg);
    events.insert(pe);
    return pe;
    } // End of function systm::newevent_abs.

//----------------------//
//  systm::newevent_abs //
//----------------------//
event* systm::newevent_abs(
        double t, object* orig, object* dest, mtype message, datum* x) {
    if (t < clck) {
        if (!orig)
            cout << "Warning: negative delay event attempted by null object.\n";
        else
            cout << "Warning: negative delay event attempted by "
                  << orig->name << DOTNL;
        return 0;
        }
    if (!orig)
        return 0;
    event* pevt = new event(t, orig, dest, message);
    pevt->setarg(x);
    events.insert(pevt);

    return pevt;
    } // End of function systm::newevent_abs.

//----------------------//
//  systm::newevent_abs //
//----------------------//
event* systm::newevent_abs(
        double t, object* orig, object* dest, mtype message, double x) {
    if (t < clck) {
        if (!orig)
            cout << "Warning: negative delay event attempted by null object.\n";
        else
            cout << "Warning: negative delay event attempted by "
                  << orig->name << DOTNL;
        return 0;
        }
    if (!orig)
        return 0;
    event* pevt = new event(t, orig, dest, message);
    pevt->setarg(x);
    events.insert(pevt);

    return pevt;
    } // End of function systm::newevent_abs.

//----------------------//
//  systm::newevent_abs //
//----------------------//
event* systm::newevent_abs(
        double t, object* orig, object* dest, mtype message, long x) {
    if (t < clck) {
        if (!orig)
            cout << "Warning: negative delay event attempted by null object.\n";
        else
            cout << "Warning: negative delay event attempted by "
                  << orig->name << DOTNL;
        return 0;
        }
    if (!orig)
        return 0;
    event* pevt = new event(t, orig, dest, message);
    pevt->setarg(x);
    events.insert(pevt);

    return pevt;
    } // End of function systm::newevent_abs.

/*------------------------------------------------------------------------------
Function systm::broadcast() sends a copy of a given message to all objects
in the system, except for the caller. This is called by event::simulate()
when the destination pointer of an event is null.
The null object pointer is returned if no objects complain. Otherwise a
pointer to the complainant is returned.
The order of sending messages is the order of creation of the
objects at system-creation time.
Note: If any object tampers with "arg", then all later recipients of the
argument are affected.
------------------------------------------------------------------------------*/
//----------------------//
//   systm::broadcast   //
//----------------------//
object* systm::broadcast(object* orig, mtype mty, value* arg) {
    object* dest = 0;
    forall(dest, objects)
        if (dest != orig && dest->sim(orig, mty, arg) < 0)
            break;
    return dest;
    } // End of function systm::broadcast.

/*------------------------------------------------------------------------------
This is the core of the AKSL simulation scheduler.
The efficiency of simulations depends primarily on this function.
------------------------------------------------------------------------------*/
//----------------------//
//    systm::simulate   //
//----------------------//
int systm::simulate(double duration, double start) {
    int err = 0;

    if (duration <= 0) {
        cout << "Terminating simulation due to non-positive duration.\n";
        return eNEGATIVE_DURATION;
        }
    if (objects.empty()) {
        cout << "Terminating simulation due to lack of objects.\n";
        return eNO_OBJECTS;
        }
    events.clear();
    clck = start;
    if (trace >= 4 && mdl)
        mdl->print(cout);

    // Initialise all packages.
    package* pp = 0;
    forall(pp, mdl->packages)
        if (pp->init)
            (*pp->init)(pp);

    // Initialise all objects.
    object* po = 0;
    forall(po, objects) {
        if (trace >= 2) {
            cout << "Initialising object at 0x"
                 << hex8(long(po)) << NL;
            cout << "  Name is: "
                 << (!po->name.null() ? (const char*)po->name : "NONE")
                 << DOTNL;
            cout << "  Type is: "
                 << (po->type() ? po->type() : "NONE") << DOTNL;
            }
        if ((err = po->init()) < 0)
//            return eINIT_FAILED;
            return err;
        }
    double finish = start + duration;
    if (trace >= 3)
        events.print(cout, this);

    // This is the "main loop" of AKSL simulations.
    // Note: Signals could be used to set finish to 0 or clck, so as to
    // terminate a simulation upon instruction from the user.
    while (clck < finish) {
        // Check to see if any signals occurred since the last event?
        // ....

        event* evt = events.popfirst();
        if (!evt) {
            cout << "Simulation ending with exhaustion of events.\n";
            return eNO_EVENTS;
            }
        if (trace >= 1) {
            cout << "Simulating event: ";
            evt->print(cout, this);
            }
        if (evt->origin()) { // Ignore cancelled events.
            clck = evt->time();
            object* err = evt->simulate();
            if (err) {
                cout << "Termination condition received from the "
                     << err->type();
                cout << " called " << err->name << DOTNL;
                return (err->error < 0) ? err->error : eEVENT_ERROR;
                }
            }
        delete(evt);
        }

    // Terminate all objects and packages.
    forall(po, objects)
        po->term();
    forall(pp, mdl->packages)
        if (pp->term)
            (*pp->term)(pp);

    // Delete all objects and packages.
    forall(po, objects)
        po->del();
    forall(pp, mdl->packages)
        if (pp->del)
            (*pp->del)(pp);

    return 0;
    } // End of function systm::simulate.

/*------------------------------------------------------------------------------
systm::sgetglob() gets the string value of an global variable, or
the null pointer, if it is not a string. The return value is read-only.
------------------------------------------------------------------------------*/
//----------------------//
//    systm::sgetglob   //
//----------------------//
const char* systm::sgetglob(const c_string& n) {
    value* pv = globvars.get(n);
    if (!pv || pv->type() != vSTRING)
        return 0;
    return *pv;
    } // End of function systm::sgetglob.

/*------------------------------------------------------------------------------
model::newsystem() creates a new system, with a given name.
------------------------------------------------------------------------------*/
//----------------------//
//   model::newsystem   //
//----------------------//
systm* model::newsystem(c_string name) {
    systm* p = new systm(*this);
    p->name = name;
    systems.append(p);
    return p;
    } // End of function model::newsystem.

/*------------------------------------------------------------------------------
model::getsystem() finds else creates a systm with a given name.
------------------------------------------------------------------------------*/
//----------------------//
//   model::getsystem   //
//----------------------//
systm* model::getsystem(c_string name) {
    // Try to find the system.
    systm* p = 0;
    forall(p, systems)
        if (p->name == name)
            break;

    // If not found, create it.
    if (!p)
        p = newsystem(name);
    return p;
    } // End of function model::getsystem.

/*------------------------------------------------------------------------------
model::link(package*) just makes a note of a package to be loaded later.
------------------------------------------------------------------------------*/
//----------------------//
//     model::link      //
//----------------------//
int model::link(package* pp) {
    if (!pp)
        return eBAD_ARGUMENT;
    if (pp->name.null())
        return eNO_NAME;
    if (!pp->new_object)
        return eNO_OBJECT_FUNCTION;
    linkedpackages.append(pp);
    return 0;
    } // End of function model::link.

/*------------------------------------------------------------------------------
model::link1(package*(*)()) just makes a note of a package to be loaded later.
------------------------------------------------------------------------------*/
//----------------------//
//     model::link1     //
//----------------------//
int model::link1(package*(*init)()) {
    if (!init)
        return eBAD_ARGUMENT;
    package* pp = init();
    if (!pp)
        return eNO_PACKAGE;
    return link(pp);
    } // End of function model::link1.

/*------------------------------------------------------------------------------
model::link2(package*(**)()) expects a parameter which is a null-terminated
array of pointers to functions which return pointers to "package" structures.
The packages returned are then linked into the model.
Don't forget the null pointer (package*(*)())0 at the end of the array!!
------------------------------------------------------------------------------*/
//----------------------//
//     model::link2     //
//----------------------//
int model::link2(package* (**pkgs)()) {
    int ret = 0;
    for (package*(**p)() = pkgs; *p; ++p)
        if ((ret = link1(*p)) < 0)
            return ret;
    return 0;
    } // End of function model::link2.

/*------------------------------------------------------------------------------
model::load(package*) is the function which loads packages into the
model at run time, and sorts out relations between the packages.
------------------------------------------------------------------------------*/
//----------------------//
//     model::load      //
//----------------------//
int model::load(package* pkg) {
    mtype i, count;
    package* pkg2;
    object *p0, *p1;

    if (!pkg)
        return eNULL_ARGUMENT;
    if (pkg->name.null())
        return eNO_NAME;
    if (!pkg->new_object)
        return eNO_OBJECT_FUNCTION;

    // Make one representative of each class from the new_object() function.
    for (i = 0; (p0 = pkg->new_object(i)) != 0; ++i) {
        if (nullstr(p0->type()))
            return eBAD_OBJECT_TYPE_NAME;

        const char* pc = p0->type();
        forall(p1, reps)
            if (strcmp(pc, p1->type()) == 0)
                break;
        if (p1)
            return eNEW_OBJECT_NAME_CLASH;

        p0->index = i;
        p0->pkg = pkg;
        reps.append(p0);
        }

    // Check the message keys of the package for repeats.
    if (pkg->cs_mesgkeys.repeats())
        return eMESSAGE_KEY_CLASH;

    // Merge in the set of message types.
    merge(cs_mtypenames, pkg->cs_mesgkeys);

    // Construct the local-to-global message type conversion tables.
    if (pkg->cs_mesgkeys.min_i() < 0)
        return eNEGATIVE_KEY;

    // Need entries for all integers from 0 to max.
    count = (mtype)pkg->cs_mesgkeys.max_i() + 1;
    pkg->mloc2glob = new mtype[count];

    // Put in catches in case of non-existent messages.
    for (i = 0; i < count; ++i)
        pkg->mloc2glob[i] = (mtype)-1;

    Forall(ski, pski, pkg->cs_mesgkeys)
        pkg->mloc2glob[pski->i] = (mtype)cs_mtypenames.index(pski->s);

    // Construct the global-to-local message type conversion tables.
    count = (mtype)cs_mtypenames.length();
    pkg->mglob2loc = new mtype[count];
    delete[] mtypenames;
    mtypenames = new const char*[count];
    i = 0;
    Forall(c_stringlink, pcsl, cs_mtypenames) {
        pkg->mglob2loc[i] = (mtype)pkg->cs_mesgkeys.key(*pcsl);
        // Stick a convenience copy of the type-name list in array mtypenames.
        mtypenames[i] = pcsl->read();   // NOTE: only cross-reference!!!!!!!!!
        ++i;
        }

    // Bring glob2loc tables of all other packages up to date also.
    forall(pkg2, packages) {
        delete[] pkg2->mglob2loc;
        pkg2->mglob2loc = new mtype[count];
        i = 0;
        Forall(c_stringlink, pcsl, cs_mtypenames) {
            pkg2->mglob2loc[i] = (mtype)pkg2->cs_mesgkeys.key(*pcsl);
            ++i;
            }
        }

    // Add the package descriptor to the current list of packages.
    pkg->mdl = this;
    packages.append(pkg);

    return 0;
    } // End of function model::load.

/*------------------------------------------------------------------------------
Function model::load1(package*(*)()) loads in a packages from a package
creating function.
------------------------------------------------------------------------------*/
//----------------------//
//     model::load1     //
//----------------------//
int model::load1(package*(*init)()) {
    if (!init)
        return eNULL_ARGUMENT;

    // Fetch a new package-descriptor structure from a package.
    package* pp = init();
    if (!pp)
        return eNO_PACKAGE;
    return load(pp);
    } // End of function model::load1.

/*------------------------------------------------------------------------------
model::load2(package*(**)()) expects a parameter which is a null-terminated
array of pointers to functions which return pointers to "package" structures.
The packages returned are then loaded into the model.
Don't forget the null pointer (package*(*)())0 at the end of the array!!
------------------------------------------------------------------------------*/
//----------------------//
//     model::load2     //
//----------------------//
int model::load2(package* (**pkgs)()) {
    int ret;
    for (package*(**p)() = pkgs; *p; ++p)
        if ((ret = load1(*p)) < 0)
            return ret;
    return 0;
    } // End of function model::load2.

/*------------------------------------------------------------------------------
model::load(c_string&) finds a package with the given name in the
"linkedpackages" list.
------------------------------------------------------------------------------*/
//----------------------//
//     model::load      //
//----------------------//
int model::load(const c_string& packagename) {
    if (packagename.null())
        return eBAD_ARGUMENT;
    pkgsneeded.append(packagename);
    package* pp1 = 0;
    forall(pp1, linkedpackages)
        if (packagename == pp1->name)
            break;
    if (!pp1) {
        // Check if it has already been loaded.
        package* pp2 = 0;
        forall(pp2, packages)
            if (packagename == pp2->name)
                break;
        if (pp2)
            return 0;
        cout << "Could not find package with name "
             << (const char*)packagename << DOTNL;
        pkgnotfound = true;
        return ePACKAGE_NOT_FOUND;
        }
    linkedpackages.remove(pp1);
    int err = load(pp1);
    if (err < 0) {
        cout << "Failed to load package " << (const char*)packagename << DOTNL;
        linkedpackages.append(pp1);
        return err;
        }
    return 0;
    } // End of function model::load.

/*------------------------------------------------------------------------------
model::missingpackages() returns a null pointer if all packages needed were
found. Otherwise a list of names of packages required is returned.
------------------------------------------------------------------------------*/
//--------------------------//
//  model::missingpackages  //
//--------------------------//
strnglist* model::missingpackages() {
    if (!pkgnotfound)
        return 0;
    return pkgsneeded.copy();
    } // End of function model::missingpackages.

/*------------------------------------------------------------------------------
model::newobject() searches each package to find an appropriate object
constructor. A null pointer is returned if not successful.

This function must not be called until all packages have been loaded. Otherwise
the message ID translation tables will be wrong!

Because of the object::construct() call in this routine, recursion often happens
through this function.
------------------------------------------------------------------------------*/
//----------------------//
//   model::newobject   //
//----------------------//
object* model::newobject(systm& sys, c_string& type, c_string& name) {
    if (type.null())
        return 0;

    // Locate a representative of the given class.
    object* x0 = 0;
    forall(x0, reps)
        if (type == x0->type())
            break;

    if (!x0) {
        cout << "No object of type " << type << " is currently registered.\n";
        cout << "Current contents of this model:\n";
        print(cout);
        return 0;
        }

    // Make a new member of the requested class.
    package* p0 = x0->pkg;
    int i0 = x0->index;
    object* x1 = (*p0->new_object)(i0);
    if (!x1) {
        cout << "Failed to make duplicate object of type " << type << DOTNL;
        return 0;
        }

    // Initialise certain fields of the new object.
    x1->pkg = p0;
    x1->index = i0;
    x1->name = name;
    x1->mloc2glob = p0->mloc2glob;
    x1->mglob2loc = p0->mglob2loc;
    sys.add(x1);

    // Give the object a chance to create some more objects.
    int err;
    if ((err = x1->construct()) < 0) {
        cout << "Object " << name << " of type " << type;
        cout << " failed to construct itself.\n";
        c_string err_str = error_string(err);
        if (!err_str.null())
            cout << "Reason: " << err_str << DOTNL;
        return 0;
        }
    return x1;
    } // End of function model::newobject.

//----------------------//
// model::print_message //
//----------------------//
bool_enum model::print_message(int index, ostream& os) {
    c_stringlink* p = cs_mtypenames.first();
    while (--index >= 0 && p)
        p = p->next();
    if (!p)
        return false;
    os << *p;
    return true;
    } // End of function model::print_message.

//----------------------//
//     model::print     //
//----------------------//
void model::print(ostream& os) {
    os << "\nThe contents of \"reps\":\n";
    Forall(object, p1, reps)
        os << p1->type() << NL;
    os << NL;

    // Print global message table.
    os << "\nGlobal message table is:\n";
    cs_mtypenames.print(os);
    } // End of function model::print.
