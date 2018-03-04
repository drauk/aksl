// src/aksl/aksl.h   2018-3-3   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_AKSL_H
#define AKSL_AKSL_H
/*------------------------------------------------------------------------------
AKSL = Alan Kennington's Simulation Library.

Classes defined in this file:

attr_type::
object::
objectlist::
event::
event_heap::
event_heap_traversal::
globvar::
globvarlist::
systm::
systmlist::
package::
packagelist::
model::
object_friend::
------------------------------------------------------------------------------*/

// AKSL header files.
#ifndef AKSL_VALUE_H
#include "aksl/value.h"
#endif
#ifndef AKSL_DATUM_H
#include "aksl/datum.h"
#endif
#ifndef AKSL_SKI_H
#include "aksl/ski.h"
#endif
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_HEAP_H
#include "aksl/heap.h"
#endif
#ifndef AKSL_BMEM_H
#include "aksl/bmem.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif
#ifndef AKSL_OPTIONS_H
#include "aksl/options.h"
#endif

// System header files.
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif

// Forward class references.
struct event;
struct event_heap;
struct systm;
struct package;
struct model;

/*------------------------------------------------------------------------------
This class is intended for communicating attribute types from system
configuration files to simulation objects.
The linked list of attribute type indices represents the "address" of an
attribute within and object. That is, the first element of the list is the name
of an attribute. The second element is the name of a sub-attribute of this
attribute, and so forth.
------------------------------------------------------------------------------*/
//----------------------//
//      attr_type::     //
//----------------------//
struct attr_type: public slink {
    mtype type;

    attr_type* next() const { return (attr_type*)slink::next(); }

//    attr_type& operator=(const attr_type& x) {}
//    attr_type(const attr_type& x) {};
    attr_type(mtype t) { type = t; }
    attr_type() { type = -1; }
    ~attr_type() {}
    }; // End of struct attr_type.

/*------------------------------------------------------------------------------
Class "object" is the central object of the AKSL simulation concept.
------------------------------------------------------------------------------*/
//----------------------//
//       object::       //
//----------------------//
struct object: public slink {
friend struct model;
friend struct systm;
friend struct event;
friend struct object_friend;
private:
    int index;                      // Index of the object in its package.
    mtype *mloc2glob;               // Local/global message type conversions.
    mtype *mglob2loc;               // Local/global message type conversions.

    // Functions provided here for use by the "system".
    int sim(object* orig, mtype m, value* arg) {
        return recv_message(orig, mglob2loc[m], arg); }
    value* get_glob_attr(mtype m) { return get_attr(mglob2loc[m]); }
    value* get_glob_attr(mtype m, const value& arg)
        { return get_attr(mglob2loc[m], arg); }
    int set_glob_attr(mtype m, value& v)
        { return set_attr(mglob2loc[m], v); }
protected:
    // Variables for read-only access by derived classes.
    systm* sys;                 // The system in which the object exists.
    package* pkg;               // The package in which the object is packed.

    // Variable(s) and functions provided here for use by derived classes.
    int error;   // For error reporting at end of simulation.
    inline event* send_message_abs(double, object*, mtype, value*);
    inline event* send_message_abs(double, object*, mtype);
    inline event* send_message_abs(double, object*, mtype, datum*);
    inline event* send_message_abs(double, object*, mtype, double);
    inline event* send_message_abs(double, object*, mtype, long);
    inline event* send_message(double d, object* o, mtype t, value* v)
        { return send_message_abs(sysclock() + d, o, t, v); }
    inline event* send_message(double d, object* o, mtype t)
        { return send_message_abs(sysclock() + d, o, t); }
    inline event* send_message(double d, object* o, mtype t, datum* x)
        { return send_message_abs(sysclock() + d, o, t, x); }
    inline event* send_message(double d, object* o, mtype t, double x)
        { return send_message_abs(sysclock() + d, o, t, x); }
    inline event* send_message(double d, object* o, mtype t, long x)
        { return send_message_abs(sysclock() + d, o, t, x); }

    // [Functions which send messages on behalf of other objects.]
    inline event* send_message_abs(object*, double, object*, mtype, value*);
    inline event* send_message_abs(object*, double, object*, mtype);
    inline event* send_message_abs(object*, double, object*, mtype, datum*);
    inline event* send_message_abs(object*, double, object*, mtype, double);
    inline event* send_message_abs(object*, double, object*, mtype, long);
    inline event* send_message(object* p, double d, object* o,mtype t,value* v)
        { return send_message_abs(p, sysclock() + d, o, t, v); }
    inline event* send_message(object* p, double d, object* o, mtype t)
        { return send_message_abs(p, sysclock() + d, o, t); }
    inline event* send_message(object* p, double d, object* o,mtype t,datum* x)
        { return send_message_abs(p, sysclock() + d, o, t, x); }
    inline event* send_message(object* p, double d, object* o,mtype t,double x)
        { return send_message_abs(p, sysclock() + d, o, t, x); }
    inline event* send_message(object* p, double d, object* o, mtype t, long x)
        { return send_message_abs(p, sysclock() + d, o, t, x); }

    inline void cancel_message_no_check(event*);    // Fast no-check version.
    inline void cancel_message(event*);             // Safer version.
    inline object* newobject(c_string&, c_string&);
    inline double sysclock();
    inline int setglob(const c_string&, value* = 0);
    inline int setglob(const c_string&, const c_string&);
    inline const value* getglob(const c_string&);
    inline const value* getglob(const char*);
    inline const char* sgetglob(const c_string&);
    inline const char* sgetglob(const char*);
    inline const char* message_string(mtype); // Convert local mtype to string.

    // Handler-functions to be provided in derived classes for "call-back"
    // by the simulation kernel.
    virtual int construct() { return 0; }
    virtual int set_attr(mtype /*attr*/, value& /*newvalue*/) { return 0; }
    virtual int set_attr(attr_type& /*attr*/, value& /*newvalue*/);
    virtual value* get_attr(mtype /*attr*/) { return 0; }
    virtual value* get_attr(mtype /*attr*/, const value& /*arg*/) { return 0; }
    virtual int init() { return 0; }
    virtual int recv_message(object* /*caller*/, mtype /*msg*/, value* /*arg*/)
        { return 0; }
    virtual int term() { return 0; }
    virtual void del() {}
public:
    // Variable functions provided here for general use.
    c_string name;           // Read-only. Set at construction time.
    object* next() { return (object*)slink::next(); }
    inline int set_attr(const c_string&, value&);
    inline value* get_attr(const c_string&);
    inline value* get_attr(const c_string&, const value&);
    value* get_attr(object* p, mtype m) {
        return p ? p->get_glob_attr(mloc2glob[m]) : 0; }
    value* get_attr(object* p, mtype m, const value& arg) {
        return p ? p->get_glob_attr(mloc2glob[m], arg) : 0; }
    int set_attr(object* p, mtype m, value& v) {
        return p ? p->set_glob_attr(mloc2glob[m], v) : 0; }
    int set_attr(object* p, mtype m, object* po) {
        value v; v = po; return p ? p->set_glob_attr(mloc2glob[m], v) : 0; }
    int set_attr(object* p, mtype m, double d) {
        value v; v = d; return p ? p->set_glob_attr(mloc2glob[m], v) : 0; }

    int get_real_attr(object*, mtype, double&);
    int get_int_attr(object*, mtype, long&);
    int get_object_attr(object*, mtype, object*&);
    int get_datum_attr(object*, mtype, datum*&);
    int get_real_attr(object*, mtype, const value& arg, double&);
    int get_int_attr(object*, mtype, const value& arg, long&);
    int get_object_attr(object*, mtype, const value& arg, object*&);
    int get_datum_attr(object*, mtype, const value& arg, datum*&);

    // Unsafe message type conversion functions.
    // Could get bus error if "t" is out of range.
    mtype loc2glob(mtype t) { return mloc2glob[t]; }
    mtype glob2loc(mtype t) { return mglob2loc[t]; }

    // Function to be provided in derived classes for general use.
    virtual const char* type() { return "object"; } // Class name of object.

    object();
    ~object() { del(); }
    }; // End of struct object.

//----------------------//
//      objectlist::    //
//----------------------//
struct objectlist: private s2list {
friend struct systm;
friend struct model;
private:
    model* mdl;
public:
    using s2list::empty;
    using s2list::length;
    object* first() const { return (object*)s2list::first(); }
    object* last() const { return (object*)s2list::last(); }
private:
    // These functions are made private because objectlists are always
    // "owned" by their system and/or model.
    void append(object* p) { s2list::append(p); }
    void prepend(object* p) { s2list::prepend(p); }
    object* popfirst() { return (object*)s2list::popfirst(); }
    object* poplast() { return (object*)s2list::poplast(); }
    object* remove(object* p) { return (object*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(object* p) { delete remove(p); }
    void clear() { for (object* p = first(); p; )
        { object* q = p->next(); delete p; p = q; } clearptrs(); }
public:
    objectlist(model& m) { mdl = &m; }
//    objectlist(model* m) { mdl = m; }
    ~objectlist() { clear(); }
    }; // End of struct objectlist.

/*------------------------------------------------------------------------------
Class "event" is one of the fundamental 3 classes of any event-driven simulation
framework:
-   system (including the FEL [Future Event List])
-   object
-   event
During a simulation, the system dequeues events from the FEL.
Each event structure instructs the system to call an object with the indicated
parameters.
When the object receives the dequeued message, it may emit any number of
messages to the system, which enqueues them in the FEL.
Thus there is a circle of invocations as follows.

event  <----------  system
   \                  ^
    \                /
     \              /
      \            /
       \          /
        \        /
         \      /
          v    /
          object

The object invokes the system to enqueue an event.
The system dequeues an event.
The event indirectly invokes an object (via the system).

This is why all three classes are in a single header file.
Each of these three classes must invoke the methods of the other in a cyclical
(non-hierarchical) fashion.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The "index" member will be used for determining the dequeuing order of
events which have the same execution time.
------------------------------------------------------------------------------*/
//----------------------//
//        event::       //
//----------------------//
#if AKSL_SYSTM_FEL_STRICT_ORDER
struct event: private tim2 {
#else
struct event: private tim {
#endif

friend struct object;          // For event cancelling.
friend struct event_heap;
#ifdef WIN32
friend struct event_heap_traversal;
#endif
friend struct systm;
private:
    object* orig;       // Source of the event. (Only null if cancelled.)
    object* dest;       // Destination of the event. (Null for broadcast.)
    mtype   mty;        // (Global) type of event.
    value*  arg;        // The event argument(s).

    void cancel() { orig = 0; }
public:
    double time() const { return t; }
    object* origin() const { return orig; }
    void print(ostream&, systm* = 0) const;

    // simulate() returns 0 if okay, or the erroneous object if not okay.
    object* simulate();
    void setarg(datum* d) { delete arg; arg = new value(d); }
    void setarg(double r) { delete arg; arg = new value(r); }
    void setarg(long r) { delete arg; arg = new value(r); }

    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }

    event(double tt, object* oo, object* dd, mtype mm) {
        t = tt;
        orig = oo;
        dest = dd;
        mty = mm;
        arg = 0;
        }
    ~event() {}
    }; // End of struct event.

//----------------------//
//      event_heap::    //
//----------------------//
#if AKSL_SYSTM_FEL_STRICT_ORDER
struct event_heap: private min_tim2_heap {
#else
struct event_heap: private min_tim_heap {
#endif
friend struct event_heap_traversal;
public:
    // The routine members:
#if AKSL_SYSTM_FEL_STRICT_ORDER
    const event* first() const { return (const event*)min_tim2_heap::first(); }
    const event* last() const { return (const event*)min_tim2_heap::last(); }
    using min_tim2_heap::empty;
    using min_tim2_heap::length;
#else
    const event* first() const { return (const event*)min_tim_heap::first(); }
    const event* last() const { return (const event*)min_tim_heap::last(); }
    using min_tim_heap::empty;
    using min_tim_heap::length;
#endif

    // The cast in the next line should not be necessary!
#if AKSL_SYSTM_FEL_STRICT_ORDER
    void insert(event* p) { min_tim2_heap::insert((tim2*)p); }
    event* popfirst() { return (event*)min_tim2_heap::popfirst(); }
#else
    void insert(event* p) { min_tim_heap::insert((tim*)p); }
    event* popfirst() { return (event*)min_tim_heap::popfirst(); }
#endif
    void delfirst() { delete popfirst(); }
    void del_events() {
        event* pe;
        while ((pe = popfirst()) != 0)
            delete(pe);
        }
    void print(ostream&, systm* = 0);
    void clear() { del_events(); }

    event_heap() {}
    ~event_heap() {}
    }; // End of struct event_heap.

//--------------------------//
//  event_heap_traversal::  //
//--------------------------//
#if AKSL_SYSTM_FEL_STRICT_ORDER
struct event_heap_traversal: private tim2_heap_traversal {
public:
    event* next() { return (event*)tim2_heap_traversal::next(); }
    using tim2_heap_traversal::init;

//    event_heap_traversal& operator=(const event_heap_traversal& x) {}
//    event_heap_traversal(const event_heap_traversal& x) {};
    event_heap_traversal(event_heap& h): tim2_heap_traversal(h) {}
    ~event_heap_traversal() {}

#else
struct event_heap_traversal: private heap_traversal {
public:
    event* next() { return (event*)heap_traversal::next(); }
    using heap_traversal::init;

//    event_heap_traversal& operator=(const event_heap_traversal& x) {}
//    event_heap_traversal(const event_heap_traversal& x) {};
    event_heap_traversal(event_heap& h): heap_traversal(h) {}
    ~event_heap_traversal() {}
#endif
    }; // End of struct event_heap_traversal.

/*------------------------------------------------------------------------------
The global variables should really be in some sort of hash table.
------------------------------------------------------------------------------*/
//----------------------//
//       globvar::      //
//----------------------//
struct globvar: public slink {
friend struct globvarlist;
private:
    c_string name;      // Name of the global variable.
    value* val;         // Value of the global variable.
public:
    globvar* next() { return (globvar*)slink::next(); }

    globvar() { val = 0; }
    ~globvar() { delete val; }
    }; // End of struct globvar.

//----------------------//
//     globvarlist::    //
//----------------------//
struct globvarlist: private s2list {
public:
    // The routine members.
    using s2list::empty;
    using s2list::length;
    globvar* first() const { return (globvar*)s2list::first(); }
    globvar* last() const { return (globvar*)s2list::last(); }
    void append(globvar* p) { s2list::append(p); }
    void prepend(globvar* p) { s2list::prepend(p); }
    globvar* popfirst() { return (globvar*)s2list::popfirst(); }
    globvar* poplast() { return (globvar*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    using s2list::insertafter;
    void clear() { for (globvar* p = first(); p; )
        { globvar* q = p->next(); delete p; p = q; } clearptrs(); }

    int set(const c_string&, value* = 0);
    int set(const c_string&, const c_string&);
    value* get(const c_string&);

    globvarlist() {}
    ~globvarlist() { clear(); }
    }; // End of struct globvarlist.

//----------------------//
//        systm::       //
//----------------------//
struct systm: public slink {
private:
    model* mdl;             // The model in which the system is defined.
    globvarlist globvars;   // The global variables.
    event_heap events;      // The event heap.
    double clck;            // The system clock (in seconds).
    short trace;            // The trace level of the system.
public:
    c_string name;          // The name of the system.
    objectlist objects;     // The objects in the system.

    systm* next() { return (systm*)slink::next(); }

    double sysclock() { return clck; }
    unsigned long event_count() { return events.length(); }
    int add(object*);
    void set_trace_level(short t) { trace = t; }

    int simulate(double = 1, double = 0);
    inline object* broadcast(object*, mtype, value* = 0);
    int dump(FILE* = stdout) { return 0; }
    const char* sgetglob(const c_string& n);
    int setglob(const c_string& n, value* v = 0)
        { return globvars.set(n, v); }
    int setglob(const c_string& n, const c_string& v)
        { return globvars.set(n, v); }
    const value* getglob(const c_string& n) { return globvars.get(n); }
    void cancel_message(event*, object*);  // Intended for use by objects.
    inline bool_enum print_message(int index, ostream&);
    inline mtype globkey(c_string&);

    // NOTE: These functions should be made inline!
    event* newevent_abs(double, object*, object*, mtype, value*);
    event* newevent_abs(double, object*, object*, mtype);
    event* newevent_abs(double, object*, object*, mtype, datum*);
    event* newevent_abs(double, object*, object*, mtype, double);
    event* newevent_abs(double, object*, object*, mtype, long);

    systm(model& m): objects(m) {
        mdl = &m;
        clck = 0;
        trace = 0;
        }
    ~systm() {} // Should the object list be deleted here?
    }; // End of struct systm.

//----------------------//
//      systmlist::     //
//----------------------//
struct systmlist: private s2list {
public:
    // The routine members.
    systm* first() const { return (systm*)s2list::first(); }
    systm* last() const { return (systm*)s2list::last(); }
    using s2list::empty;
    using s2list::length;
    void append(systm* p) { s2list::append(p); }
    void prepend(systm* p) { s2list::prepend(p); }
    systm* popfirst() { return (systm*)s2list::popfirst(); }
    systm* poplast() { return (systm*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    using s2list::insertafter;
    void clear() { for (systm* p = first(); p; )
        { systm* q = p->next(); delete p; p = q; } clearptrs(); }

    systmlist() {}
    ~systmlist() { clear(); }
    }; // End of struct systmlist.

/*------------------------------------------------------------------------------
The package initialiser function is called once only, just before all objects in
the simulation are called with their init functions.
The construct, term and del functions may later be similarly used.
------------------------------------------------------------------------------*/
//----------------------//
//       package::      //
//----------------------//
struct package: public slink {
friend struct model;
private:
    mtype *mloc2glob;               // Local/global message conversion table.
    mtype *mglob2loc;               // Global/local message conversion table.
    model* mdl;                     // The model using the package.
public:
    // Provided here for general use.
    package* next() { return (package*)slink::next(); }
    inline object* newobject(systm&, c_string&, c_string&);
    inline const char* message_string(mtype); // String name for local integer.

    // Unsafe message type conversion functions.
    // Could get bus error if "t" is out of range or the array is null.
    mtype loc2glob(mtype t) { return mloc2glob[t]; }
    mtype glob2loc(mtype t) { return mglob2loc[t]; }

    // To be provided by each simulation package to the "model".
    c_string name;                          // Name of the package.
    skilist cs_mesgkeys;                    // Table of message types.
    object* (*new_object)(int);             // Object constructor.
                                            // Optional handlers.
    int (*construct)(package*);             // Package constructor.
    int (*init)(package*);                  // Package initialiser.
    int (*term)(package*);                  // Package terminator.
    int (*del)(package*);                   // Package destructor.

    package() {
        mloc2glob = 0;
        mglob2loc = 0;
        mdl = 0;
        new_object = 0;
        construct = 0;
        init = 0;
        term = 0;
        del = 0;
        }
    ~package() { delete[] mloc2glob; delete[] mglob2loc; }
    }; // End of struct package.

//----------------------//
//     packagelist::    //
//----------------------//
struct packagelist: private s2list {
public:
    // The routine members.
    package* first() const { return (package*)s2list::first(); }
    package* last() const { return (package*)s2list::last(); }
    using s2list::empty;
    using s2list::length;
    void append(package* p) { s2list::append(p); }
    void prepend(package* p) { s2list::prepend(p); }
    package* popfirst() { return (package*)s2list::popfirst(); }
    package* poplast() { return (package*)s2list::poplast(); }
    package* remove(package* p) { return (package*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(package* p) { delete remove(p); }
    using s2list::insertafter;
    void clear() { for (package* p = first(); p; )
        { package* q = p->next(); delete p; p = q; } clearptrs(); }

    packagelist() {}
    ~packagelist() { clear(); }
    }; // End of struct packagelist.

//----------------------//
//        model::       //
//----------------------//
struct model {
friend struct systm;
friend struct package;
private:
    systmlist systems;          // The systems using this model.
    strnglist pkgsneeded;       // The packages required for the simulation.
    bool_enum pkgnotfound;      // True if a package was not found.
    packagelist linkedpackages; // List of packages linked but not loaded.
    packagelist packages;       // List of loaded packages.
    objectlist reps;            // Representatives of each class of object.

    // The global message name table.
    const char **mtypenames;
    c_stringlist cs_mtypenames;     // The list of message types in the model.
public:
    // To be called by applications programs.
    systm* newsystem(c_string);     // Create a system.
    systm* getsystem(c_string);     // Find or else create a system.
    int link(package*);             // Link a package.

    // Sun C++ 2.0 can't distinguish the two following functions.
    int link1(package* (*)());      // Link package via a function.
    int link2(package* (**)());     // Link packages via a function table.
    int load(package*);             // Load a package.
    int load1(package* (*)());      // Load package via a function.
    int load2(package* (**)());     // Load packages via a function table.
    int load(const c_string&);      // Load by name. (Must be pre-linked.)
    strnglist* missingpackages();   // Packages that need to be linked.

    // For general use, and via system interfaces.
    object* newobject(systm&, c_string&, c_string&);
    bool_enum print_message(int index, ostream& = cout);
    mtype globkey(c_string& cs) { return (mtype)cs_mtypenames.index(cs); }

    void print(ostream& = cout);

    model(): reps(*this) {
        pkgnotfound = false;
        mtypenames = 0;
        }
    ~model() {}
    }; // End of struct model.

/*------------------------------------------------------------------------------
This class is intended so that classes _not_ derived from "object" can send
messages on behalf of objects, and also ready the system clock.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Warning:  the message type parameter "m" is translated using the loc2glob table
of the object on whose behalf the message is sent. Hence the object_friend
derived class must be in the _same_ package as the object class, because "m"
must come from the message table of the object class.
------------------------------------------------------------------------------*/
//----------------------//
//    object_friend::   //
//----------------------//
struct object_friend {

public:
    double sysclock(object* p) { return p->sysclock(); }
    event* send_message(object* p, double t, object* d, mtype m, value* v)
        { return p->send_message(t, d, m, v); }
    event* send_message(object* p, double t, object* d, mtype m)
        { return p->send_message(t, d, m); }
    event* send_message(object* p, double t, object* d, mtype m, datum* x)
        { return p->send_message(t, d, m, x); }
    event* send_message(object* p, double t, object* d, mtype m, double r)
        { return p->send_message(t, d, m, r); }
    event* send_message(object* p, double t, object* d, mtype m, long i)
        { return p->send_message(t, d, m, i); }

//    object_friend& operator=(const object_friend& x) {}
//    object_friend(const object_friend& x) {};
    object_friend() {}
    ~object_friend() {}
    }; // End of struct object_friend.

/*------------------------------------------------------------------------------
Inline member functions.
------------------------------------------------------------------------------*/
//----------------------//
//       object::       //
//----------------------//
inline event* object::send_message_abs(double time, object* dest, mtype message,
        value* arg) {
    return sys->newevent_abs(time, this, dest, mloc2glob[message], arg); }
inline event* object::send_message_abs(double time, object* dest,
        mtype message) {
    return sys->newevent_abs(time, this, dest, mloc2glob[message]); }
inline event* object::send_message_abs(double time, object* dest, mtype message,
        datum* t) {
    return sys->newevent_abs(time, this, dest, mloc2glob[message], t); }
inline event* object::send_message_abs(double time, object* dest, mtype message,
        double t) {
    return sys->newevent_abs(time, this, dest, mloc2glob[message], t); }
inline event* object::send_message_abs(double time, object* dest,
        mtype message, long t) {
    return sys->newevent_abs(time, this, dest, mloc2glob[message], t); }

/*------------------------------------------------------------------------------
These functions send messages on behalf of another object, but using the
message table of the real sending object (which calls these functions).
------------------------------------------------------------------------------*/
inline event* object::send_message_abs(object* p, double time, object* dest,
        mtype message, value* arg) {
    return sys->newevent_abs(time, p, dest, mloc2glob[message], arg); }
inline event* object::send_message_abs(object* p, double time, object* dest,
        mtype message) {
    return sys->newevent_abs(time, p, dest, mloc2glob[message]); }
inline event* object::send_message_abs(object* p, double time, object* dest,
        mtype message, datum* t) {
    return sys->newevent_abs(time, p, dest, mloc2glob[message], t); }
inline event* object::send_message_abs(object* p, double time, object* dest,
        mtype message, double t) {
    return sys->newevent_abs(time, p, dest, mloc2glob[message], t); }
inline event* object::send_message_abs(object* p, double time, object* dest,
        mtype message, long t) {
    return sys->newevent_abs(time, p, dest, mloc2glob[message], t); }

inline void object::cancel_message_no_check(event* p) // Unsafe version.
    { if (p && p->origin() == this) p->cancel(); }
inline void object::cancel_message(event* p)
    { if (sys) sys->cancel_message(p, this); }
inline object* object::newobject(c_string& type, c_string& newname) {
    return (pkg && sys) ? pkg->newobject(*sys, type, newname) : 0; }
inline double object::sysclock() {
    return sys->sysclock(); }
inline int object::setglob(const c_string& a, value* b) {
    return sys->setglob(a, b); }
inline int object::setglob(const c_string& a, const c_string& b) {
    return sys->setglob(a, b); }
inline const value* object::getglob(const c_string& s) {
    return sys->getglob(s); }
inline const value* object::getglob(const char* s) {
    c_string cs(s); return sys->getglob(cs); }
inline const char* object::sgetglob(const c_string& s) {
    return sys->sgetglob(s); }
inline const char* object::sgetglob(const char* s) {
    c_string cs(s); return sys->sgetglob(s); }
inline int object::set_attr(const c_string& attr, value& newvalue) {
    return set_attr((mtype)pkg->cs_mesgkeys.key(attr), newvalue); }
inline value* object::get_attr(const c_string& attr) {
    return get_attr((mtype)pkg->cs_mesgkeys.key(attr)); }
inline value* object::get_attr(const c_string& attr, const value& arg) {
    return get_attr((mtype)pkg->cs_mesgkeys.key(attr), arg); }
inline const char* object::message_string(mtype m) {
    return pkg ? pkg->message_string(m) : 0; }

/*------------------------------------------------------------------------------
Function event::simulate() returns 0 if okay, or the erroneous object
if not okay. If the "dest" pointer is null, then a copy of the message
is sent to all objects in the system which contains "orig".
Order of delivery is essentially unpredictable.
Special NOTE: If any object tampers with "arg", then all later
recipients of the argument are affected!!!!!
------------------------------------------------------------------------------*/
//----------------------//
//        event::       //
//----------------------//
inline object* event::simulate() {
    return (dest) ? ((dest->sim(orig, mty, arg) < 0) ? dest : 0) :
        orig->sys->broadcast(orig, mty, arg);
    }

//----------------------//
//        systm::       //
//----------------------//
inline bool_enum systm::print_message(int index, ostream& os)
    { return mdl ? mdl->print_message(index, os) : false; }
inline mtype systm::globkey(c_string& cs)
    { return mdl ? mdl->globkey(cs) : -1; }

//----------------------//
//       package::      //
//----------------------//
inline object* package::newobject(systm& sys, c_string& type, c_string& nm) {
    return mdl ? mdl->newobject(sys, type, nm) : 0; }
inline const char* package::message_string(mtype m) { // m must be local.
    return (m >= 0) ? mdl->mtypenames[mloc2glob[m]] : 0; }

#endif /* AKSL_AKSL_H */
