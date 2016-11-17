/*
 * lyst.c -- subroutines for managing a doubly linked list ("lyst").
 *
 *	Copyright (c) 1997, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *	Author: Jeff Biesiadecki, Jet Propulsion Laboratory
 *	Adapted by Scott Burleigh, Jet Propulsion Laboratory
 *
 * $Log: lyst.c,v $
 * Revision 1.3  2007/07/07 19:38:39  scott
 * Revamp ICI.
 *
 * Revision 1.2  2006/05/21 16:20:50  scott
 * Add llcv to ici, clean up header inclusion.
 *
 * Revision 1.1.1.1  2004/12/28 04:21:39  scott
 * Original source
 *
 * Revision 1.1.1.1  2004/12/06 16:37:31  scott
 * Original code base
 *
 * Revision 1.2  2002/12/17 16:14:25  scott
 * Added interface to sptrace, to enable tracing of DRAM and SDR space usage.
 *
 * Revision 1.1.1.1  2002/04/26 17:29:14  scott
 * CFDP as a DTN application
 *
 * Revision 1.1.1.1  2002/03/13 01:08:25  scott
 * CFDP and supporting stuff
 *
 * Revision 1.5  2001/2/19  20:30:00  scott
 * Extracted memory management abstraction into 'memmgr' library.
 *
 * Revision 1.4  1997/10/11  10:00:00  scott
 * Added symbolic naming of memory allocation/deallocation function pairs.
 *
 * Revision 1.3  1995/05/16  23:50:00  scott
 * Added ability to specify memory deallocation function as well as
 * allocation function.
 *
 * Revision 1.2  1995/03/28  22:14:11  scott
 * Add ability to select memory allocation function when creating a list.
 *
 * Revision 1.1  1994/05/12  20:56:32  chuck
 * Initial revision
 *
 * Revision 1.8  1994/04/13  20:17:17  jeffb
 * split lyst_compare_set/get into two functions
 * lyst_insert will insert either last or first when no compare function,
 * depending on how list->dir is set
 *
 * Revision 1.7  1994/04/13  20:08:16  jeffb
 * add lyst_compare_get function
 *
 * Revision 1.6  1994/03/03  00:33:09  jeffb
 * change insert/delete callbacks to refer to element that was inserted
 * or was about to be deleted
 *
 * Revision 1.5  1994/03/02  01:11:37  jeffb
 * added insert function
 *
 * Revision 1.4  1994/02/16  04:13:03  jeffb
 * changed mind - take out not too useful sorted flag
 *
 * Revision 1.3  1994/02/16  03:56:50  jeffb
 * always sort when lyst_sort called even if list->sorted is true
 *
 * Revision 1.2  1994/02/16  02:28:18  jeffb
 * replace LystPointer with void *
 * remove assert() statements from checking user provided parameters
 * add "sorted" flag and function
 *
 * Revision 1.1  1994/02/15  06:24:29  jeffb
 * Initial revision
 *
 */

#ifdef __ghs__
#define NDEBUG
#endif

#include "platform.h"
#include "memmgr.h"
#include "lystP.h"

/*
 * prototypes for private functions
 */

static void lyst__clear(Lyst);
static int lyst__inorder(Lyst,void *,void *);
static LystElt lyst__elt_create(const char *, int, Lyst, void *);
static void lyst__elt_clear(LystElt);
static char *lyst__alloc(const char *, int, int, unsigned int);
static void lyst__free(const char *, int, int, char *);

/*
 * public functions -- create and destroy list objects
 */

Lyst
Lyst_create(const char *file, int line)
{

}

Lyst
Lyst_create_using(const char *file, int line, int idx)
{

}

static void
wipe_lyst(const char *file, int line, Lyst list, int destroy)
{

}

void
Lyst_clear(const char *file, int line, Lyst list)
{

}

void
Lyst_destroy(const char *file, int line, Lyst list)
{

}

/*
 * public functions - get and set list information
 */

void
lyst_compare_set(Lyst list, LystCompareFn fn)
{

}

LystCompareFn
lyst_compare_get(Lyst list)
{

}

void
lyst_direction_set(Lyst list, LystSortDirection dir)
{

}

void
lyst_delete_set(Lyst list, LystCallback fn, void *arg)
{

}

void
lyst_delete_get(Lyst list, LystCallback *fn, void **arg)
{

}

void
lyst_insert_set(Lyst list, LystCallback fn, void *arg)
{

}

void
lyst_insert_get(Lyst list, LystCallback *fn, void **arg)
{

}

unsigned long
lyst_length(Lyst list)
{

}

/*
 * public functions -- add and delete elements
 */

LystElt
Lyst_insert(const char *file, int line, Lyst list, void *data)
{

}

LystElt
Lyst_insert_first(const char *file, int line, Lyst list, void *data)
{

}

LystElt
Lyst_insert_last(const char *file, int line, Lyst list, void *data)
{

}

LystElt
Lyst_insert_before(const char *file, int line, LystElt elt, void *data)
{

}

LystElt
Lyst_insert_after(const char *file, int line, LystElt elt, void *data)
{

}

void
Lyst_delete(const char *file, int line, LystElt elt)
{

}

/*
 * public functions -- traverse lysts
 */

LystElt
lyst_first(Lyst list)
{

}

LystElt
lyst_last(Lyst list)
{

}

LystElt
lyst_next(LystElt elt)
{

}

LystElt
lyst_prev(LystElt elt)
{

}

LystElt
lyst_search(LystElt elt, void *data)
{

}

/*
 * public functions - get and set element information
 */

Lyst
lyst_lyst(LystElt elt)
{

}

void *
lyst_data(LystElt elt)
{

}

void *
lyst_data_set(LystElt elt, void *new_)
{

}

/*
 * public functions -- miscellaneous
 */

void
lyst_sort(Lyst list)
{

}

int
lyst_sorted(Lyst list)
{

}

void
lyst_apply(Lyst list, LystCallback fn, void *user_arg)
{

}

/*
 * private functions -- zero out/initialize fields of a lyst or element
 */

static void
lyst__clear(Lyst list)
{

}

static int
lyst__inorder(Lyst list, void *data1, void *data2)
{

}

static LystElt
lyst__elt_create(const char *file, int line, Lyst list, void *data)
{

}

static void
lyst__elt_clear(LystElt elt)
{

}

static char *lyst__alloc(const char *fileName, int lineNbr, int idx,
		unsigned size)
{

}

static void lyst__free(const char *fileName, int lineNbr, int idx, char *ptr)
{

}
