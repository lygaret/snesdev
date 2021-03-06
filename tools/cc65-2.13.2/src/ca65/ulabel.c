/*****************************************************************************/
/*                                                                           */
/*				   ulabel.c				     */
/*                                                                           */
/*		  Unnamed labels for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



/* common */
#include "check.h"
#include "coll.h"
#include "filepos.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "scanner.h"
#include "ulabel.h"



/*****************************************************************************/
/*  	    	       	      	     Data				     */
/*****************************************************************************/



/* Struct that describes an unnamed label */
typedef struct ULabel ULabel;
struct ULabel {
    FilePos 	Pos;   			/* Position of the label in the source */
    ExprNode*	Val;          	       	/* The label value - may be NULL */
    unsigned    Ref;                    /* Number of references */
};

/* List management */
static Collection ULabList	= STATIC_COLLECTION_INITIALIZER;
static unsigned ULabDefCount	= 0;	/* Number of defined labels */



/*****************************************************************************/
/*     	      	     	  	     Code		     	     	     */
/*****************************************************************************/



static ULabel* NewULabel (ExprNode* Val)
/* Create a new ULabel and insert it into the collection. The created label
 * structure is returned.
 */
{
    /* Allocate memory for the ULabel structure */
    ULabel* L = xmalloc (sizeof (ULabel));

    /* Initialize the fields */
    L->Pos = CurPos;
    L->Val = Val;
    L->Ref = 0;

    /* Insert the label into the collection */
    CollAppend (&ULabList, L);

    /* Return the created label */
    return L;
}



ExprNode* ULabRef (int Which)
/* Get an unnamed label. If Which is negative, it is a backreference (a
 * reference to an already defined label), and the function will return a
 * segment relative expression. If Which is positive, it is a forward ref,
 * and the function will return a expression node for an unnamed label that
 * must be resolved later.
 */
{
    int	    Index;
    ULabel* L;

    /* Which can never be 0 */
    PRECONDITION (Which != 0);

    /* Get the index of the referenced label */
    if (Which > 0) {
     	--Which;
    }
    Index = (int) ULabDefCount + Which;

    /* We cannot have negative label indices */
    if (Index < 0) {
	/* Label does not exist */
	Error ("Undefined label");
	/* We must return something valid */
	return GenCurrentPC();
    }

    /* Check if the label exists. If not, generate enough forward labels. */
    if (Index < (int) CollCount (&ULabList)) {
        /* The label exists, get it. */
	L = CollAtUnchecked (&ULabList, Index);
    } else {
        /* Generate new, undefined labels */
	while (Index >= (int) CollCount (&ULabList)) {
            L = NewULabel (0);
	}
    }

    /* Mark the label as referenced */
    ++L->Ref;

    /* If the label is already defined, return its value, otherwise return
     * just a reference.
     */
    if (L->Val) {
        return CloneExpr (L->Val);
    } else {
        return GenULabelExpr (Index);
    }
}



void ULabDef (void)
/* Define an unnamed label at the current PC */
{
    if (ULabDefCount < CollCount (&ULabList)) {
	/* We did already have a forward reference to this label, so has
	 * already been generated, but doesn't have a value. Use the current
	 * PC for the label value.
	 */
	ULabel* L = CollAtUnchecked (&ULabList, ULabDefCount);
	CHECK (L->Val == 0);
	L->Val = GenCurrentPC ();
	L->Pos = CurPos;
    } else {
	/* There is no such label, create it */
       	NewULabel (GenCurrentPC ());
    }

    /* We have one more defined label */
    ++ULabDefCount;
}



int ULabCanResolve (void)
/* Return true if we can resolve arbitrary ULabels. */
{
    /* We can resolve labels if we don't have any undefineds */
    return (ULabDefCount == CollCount (&ULabList));
}



ExprNode* ULabResolve (unsigned Index)
/* Return a valid expression for the unnamed label with the given index. This
 * is used to resolve unnamed labels when assembly is done, so it is an error
 * if a label is still undefined in this phase.
 */
{
    /* Get the label and check that it is defined */
    ULabel* L = CollAt (&ULabList, Index);
    CHECK (L->Val != 0);

    /* Return the label value */
    return CloneExpr (L->Val);
}



void ULabCheck (void)
/* Run through all unnamed labels and check for anomalies and errors */
{
    /* Check if there are undefined labels */
    unsigned I = ULabDefCount;
    while (I < CollCount (&ULabList)) {
	ULabel* L = CollAtUnchecked (&ULabList, I);
	PError (&L->Pos, "Undefined label");
	++I;
    }

    /* Walk over all labels and emit a warning if any unreferenced ones
     * are found.
     */
    for (I = 0; I < CollCount (&ULabList); ++I) {
        ULabel* L = CollAtUnchecked (&ULabList, I);
        if (L->Ref == 0) {
            PWarning (&L->Pos, 1, "No reference to unnamed label");
        }
    }
}



