/****************************************************************************
**
*W  set.c                       GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file contains the functions which mainly deal with proper sets.
**
**  A *proper set* is a list that has no holes, no duplicates, and is sorted.
**  For the full definition  of sets see chapter "Sets" in the {\GAP} Manual.
**  Read also section "More about Sets" about the internal flag for sets.
**
**  The second part consists  of the functions 'IsSet', 'SetList', 'SetList',
**  'IsEqualSet',  'IsSubsetSet',    'AddSet',    'RemoveSet',    'UniteSet',
**  'IntersectSet',  and 'SubtractSet'.  These  functions make it possible to
**  make sets, either  by converting a  list to  a  set, or  by computing the
**  union, intersection, or difference of two sets.
*/
#include        <assert.h>              /* assert                          */
#include        "system.h"              /* system dependent part           */

SYS_CONST char * Revision_set_c =
   "@(#)$Id$";

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */
#include        "scanner.h"             /* scanner                         */

#include        "gap.h"                 /* error handling, initialisation  */

#include        "gvars.h"               /* global variables                */

#include        "calls.h"               /* generic call mechanism          */
#include        "opers.h"               /* generic operations              */

#include        "ariths.h"              /* basic arithmetic                */

#include        "bool.h"                /* booleans                        */

#include        "records.h"             /* generic records                 */
#include        "precord.h"             /* plain records                   */

#include        "lists.h"               /* generic lists                   */
#include        "listfunc.h"            /* functions for generic lists     */
#include        "plist.h"               /* plain lists                     */
#define INCLUDE_DECLARATION_PART
#include        "set.h"                 /* plain sets                      */
#undef  INCLUDE_DECLARATION_PART
#include        "string.h"              /* strings                         */


/****************************************************************************
**

*F  IsSet( <list> ) . . . . . . . . . . . . . . . . . test if a list is a set
**
**  'IsSet' returns 1 if the list <list> is a proper set  and 0 otherwise.  A
**  proper set is a list that has no holes, no duplicates, and is sorted.  As
**  a sideeffect 'IsSet' changes the type of proper sets to 'T_SET'.
**
**  A typical call in the set functions looks like this:
**
**  |    if ( ! IsSet(list) )  list = SetList(list); |
**
**  This tests if 'list' is a proper set and the type  is changed to 'T_SET'.
**  If it is not  then 'SetList' is  called to make  a copy of 'list', remove
**  the holes, sort the copy, and remove the duplicates.
*/
#define IS_IMM_PLIST(list)  ((TNUM_OBJ(list) - T_PLIST) % 2)

Int             IsSet ( 
    Obj                 list )
{
    Int                 isSet;          /* result                          */

    /* if <list> is a plain list                                           */
    if ( T_PLIST <= TNUM_OBJ(list)
      && TNUM_OBJ(list) <= T_PLIST_CYC_SSORT+IMMUTABLE ) {

        /* if <list> is the empty list, its a set (:-)                     */
        if ( LEN_PLIST(list) == 0 ) {
            SET_FILT_LIST( list, FN_IS_EMPTY );
            isSet = 1;
        }

        /* if <list> homogeneous and strictly sorted, its a set            */
        else if ( IS_HOMOG_LIST(list) && IS_SSORT_LIST(list) ) {
            isSet = 1;
        }

        /* otherwise it is not a set                                       */
        else {
            isSet = 0;
        }

    }

    /* if it is another list                                               */
    else if ( IS_LIST(list) ) {

        /* if <list> is the empty list, its a set (:-)                     */
        if ( LEN_LIST(list) == 0 ) {
            PLAIN_LIST( list );
            SET_FILT_LIST( list, FN_IS_EMPTY );
            isSet = 1;
        }

        /* if <list> homogeneous and strictly sorted, its a set            */
        else if ( IS_HOMOG_LIST(list) && IS_SSORT_LIST(list) ) {
            PLAIN_LIST( list );
            SET_FILT_LIST( list, FN_IS_HOMOG );
            SET_FILT_LIST( list, FN_IS_SSORT );
            isSet = 1;
        }

        /* otherwise it is not a set                                       */
        else {
            isSet = 0;
        }

    }

    /* otherwise it is certainly not a set                                 */
    else {
        isSet = 0;
    }

    /* return the result                                                   */
    return isSet;
}


/****************************************************************************
**
*F  SetList(<list>) . . . . . . . . . . . . . . . . .  make a set from a list
**
**  'SetList' returns  a new set that contains  the elements of <list>.  Note
**  that 'SetList' returns a  new list even if <list>  was already a set.  In
**  this case 'SetList' is equal to 'ShallowCopy'.
**
**  'SetList' makes a copy  of the list  <list>, removes the holes, sorts the
**  copy and finally removes duplicates, which must appear next to each other
**  now that the copy is sorted.
*/
Obj             SetList (
    Obj                 list )
{
    Obj                 set;            /* result set                      */
    Int                 lenSet;         /* length of <set>                 */
    Int                 lenList;        /* length of <list>                */
    Obj                 elm;            /* one element of the list         */
    UInt                mutable;        /* the elements are mutable        */
    UInt                i;              /* loop variable                   */

    /* make a dense copy                                                   */
    lenList = LEN_LIST( list );
    set = NEW_PLIST( T_PLIST, lenList );
    lenSet = 0;
    for ( i = 1; i <= lenList; i++ ) {
        elm = ELMV0_LIST( list, i );
        if ( elm != 0 ) {
            lenSet += 1;
            SET_ELM_PLIST( set, lenSet, elm );
        }
    }
    SET_LEN_PLIST( set, lenSet );

    /* sort the set (which is a dense plain list)                          */
    SortDensePlist( set );

    /* remove duplicates                                                   */
    mutable = RemoveDupsDensePlist( set );

    /* if possible, turn this into a set                                   */
    /*N 1997/05/12 fceller this is a HACK until set props works for lists  */
    IS_SSORT_LIST(set);

    /* return set                                                          */
    return set;
}


/****************************************************************************
**
*F  FuncLIST_SORTED_LIST( <self>, <list> )  . . . . . . . .  make a set from a list
**
**  'FuncLIST_SORTED_LIST' implements the internal function 'SetList'.
**
**  'SetList( <list> )'
**
**  'SetList' returns a new proper set, which is represented as a sorted list
**  without holes or duplicates, containing the elements of the list <list>.
**
**  'SetList' returns a new list even if the list <list> is already a  proper
**  set, in this case it is equivalent to 'ShallowCopy' (see  "ShallowCopy").
*/
Obj             FuncLIST_SORTED_LIST (
    Obj                 self,
    Obj                 list )
{
    Obj                 set;            /* result                          */

    /* check the argument                                                  */
    while ( ! IS_LIST( list ) ) {
        list = ErrorReturnObj(
            "Set: <list> must be a list (not a %s)",
            (Int)TNAM_OBJ(list), 0L,
            "you can return a list for <list>" );
    }

    /* if the list is empty create a new empty list                        */
    if ( LEN_LIST(list) == 0 ) {
        set = NEW_PLIST( T_PLIST_EMPTY, 0 );
    }

    /* if <list> is a set just shallow copy it                             */
    else if ( IS_HOMOG_LIST(list) && IS_SSORT_LIST(list) ) {
        set = SHALLOW_COPY_OBJ( list );
    }

    /* otherwise let 'SetList' do the work                                 */
    else {
        set = SetList( list );
    }

    /* return the set                                                      */
    return set;
}


/****************************************************************************
**
*F  FuncIS_EQUAL_SET(<self>,<l1>,<l2>) test if a two lists are equal as sets
**
**  'FuncIS_EQUAL_SET' implements the internal function 'IsEqualSet'.
**
**  'IsEqualSet( <list1>, <list2> )'
**
**  'IsEqualSet'  returns  'true' if the  two  lists <list1> and  <list2> are
**  equal *when viewed as sets*, and 'false'  otherwise.  <list1> and <list2>
**  are equal if every element of  <list1> is also  an element of <list2> and
**  if every element of <list2> is also an element of <list1>.
*/
Int             EqSet (
    Obj                 listL,
    Obj                 listR )
{
    Int                 lenL;           /* length of the left operand      */
    Int                 lenR;           /* length of the right operand     */
    Obj                 elmL;           /* element of the left operand     */
    Obj                 elmR;           /* element of the right operand    */
    UInt                i;              /* loop variable                   */

    /* get the lengths of the lists and compare them                       */
    lenL = LEN_PLIST( listL );
    lenR = LEN_PLIST( listR );
    if ( lenL != lenR ) {
        return 0L;
    }

    /* loop over the elements and compare them                             */
    for ( i = 1; i <= lenL; i++ ) {
        elmL = ELM_PLIST( listL, i );
        elmR = ELM_PLIST( listR, i );
        if ( ! EQ( elmL, elmR ) ) {
            return 0L;
        }
    }

    /* no differences found, the lists are equal                           */
    return 1L;
}

Obj             FuncIS_EQUAL_SET (
    Obj                 self,
    Obj                 list1,
    Obj                 list2 )
{
    /* check the arguments, convert to sets if necessary                   */
    while ( ! IS_LIST(list1) ) {
        list1 = ErrorReturnObj(
            "IsEqualSet: <list1> must be a list (not a %s)",
            (Int)TNAM_OBJ(list1), 0L,
            "you can return a list for <list1>" );
    }
    if ( ! IsSet( list1 ) )  list1 = SetList( list1 );
    while ( ! IS_LIST(list2) ) {
        list2 = ErrorReturnObj(
            "IsEqualSet: <list2> must be a list (not a %s)",
            (Int)TNAM_OBJ(list2), 0L,
            "you can return a list for <list2>" );
    }
    if ( ! IsSet( list2 ) )  list2 = SetList( list2 );

    /* and now compare them                                                */
    return (EqSet( list1, list2 ) ? True : False );
}


/****************************************************************************
**
*F  FuncIS_SUBSET_SET(<self>,<s1>,<s2>) test if a set is a subset of another
**
**  'FuncIS_SUBSET_SET' implements the internal function 'IsSubsetSet'.
**
**  'IsSubsetSet( <set1>, <set2> )'
**
**  'IsSubsetSet' returns 'true'  if the set  <set2> is a  subset of the  set
**  <set1>, that is if every element of <set2>  is also an element of <set1>.
**  Either  argument may also  be a list that is  not a proper  set, in which
**  case 'IsSubsetSet' silently applies 'Set' (see "Set") to it first.
*/
Obj             FuncIS_SUBSET_SET (
    Obj                 self,
    Obj                 set1,
    Obj                 set2 )
{
    UInt                len1;           /* length of  the left  set        */
    UInt                len2;           /* length of  the right set        */
    UInt                i1;             /* index into the left  set        */
    UInt                i2;             /* index into the right set        */
    Obj                 e1;             /* element of left  set            */
    Obj                 e2;             /* element of right set            */
    UInt                pos;            /* position                        */

    /* check the arguments, convert to sets if necessary                   */
    while ( ! IS_LIST(set1) ) {
        set1 = ErrorReturnObj(
            "IsSubsetSet: <set1> must be a list (not a %s)",
            (Int)TNAM_OBJ(set1), 0L,
            "you can return a list for <set1>" );
    }
    while ( ! IS_LIST(set2) ) {
        set2 = ErrorReturnObj(
            "IsSubsetSet: <set2> must be a list (not a %s)",
            (Int)TNAM_OBJ(set2), 0L,
            "you can return a list for <set2>" );
    }
    if ( ! IsSet( set1 ) )  set1 = SetList( set1 );

    /* special case if the second argument is a set                        */
    if ( IsSet( set2 ) ) {

        /* get the logical lengths and get the pointer                     */
        len1 = LEN_PLIST( set1 );
        len2 = LEN_PLIST( set2 );
        i1 = 1;
        i2 = 1;

        /* now compare the two sets                                        */
        while ( i1 <= len1 && i2 <= len2 && len2 - i2 <= len1 - i1 ) {
            e1 = ELM_PLIST( set1, i1 );
            e2 = ELM_PLIST( set2, i2 );
            if ( EQ( e1, e2 ) ) {
                i1++;  i2++;
            }
            else if ( LT( e1, e2 ) ) {
                i1++;
            }
            else {
                break;
            }
        }

    }

    /* general case                                                        */
    else {

        /* first convert the other argument into a proper list             */
        PLAIN_LIST( set2 );

        /* get the logical lengths                                         */
        len1 = LEN_PLIST( set1 );
        len2 = LEN_PLIST( set2 );

        /* loop over the second list and look for every element            */
        for ( i2 = 1; i2 <= len2; i2++ ) {

            /* ignore holes                                                */
            if ( ELM_PLIST(set2,i2) == 0 )
                continue;

            /* perform the binary search to find the position              */
            pos = PositionSortedDensePlist( set1, ELM_PLIST(set2,i2) );

            /* test if the element was found at position k                 */
            if ( len1<pos || ! EQ(ELM_PLIST(set1,pos),ELM_PLIST(set2,i2)) ) {
                break;
            }

        }

    }

    /* return 'true' if every element of <set2> appeared in <set1>         */
    return ((i2 == len2 + 1) ? True : False);
}


/****************************************************************************
**

*F * * * * * * * * * * * * * * GAP level functions  * * * * * * * * * * * * *
*/

/****************************************************************************
**


*F  FuncADD_SET( <self>, <set>, <obj> ) . . . . . . . add an element to a set
**
**  'FuncADD_SET' implements the internal function 'AddSet'.
**
**  'AddSet( <set>, <obj> )'
**
**  'AddSet' adds <obj>, which may be an object  of an arbitrary type, to the
**  set <set>, which must be a proper set.  If <obj> is already an element of
**  the set <set>, then <set> is not changed.  Otherwise <obj> is inserted at
**  the correct position such that <set> is again a set afterwards.
**
**  'AddSet' does not return  anything, it is only  called for the sideeffect
**  of changing <set>.
*/
Obj FuncADD_SET (
    Obj                 self,
    Obj                 set,
    Obj                 obj )
{
    UInt                len;            /* logical length of the list      */
    UInt                pos;            /* position                        */
    UInt                i;              /* loop variable                   */

    /* check the arguments                                                 */
    while ( ! IsSet(set) || ! IS_MUTABLE_OBJ(set) ) {
        set = ErrorReturnObj(
            "AddSet: <set> must be a mutable proper set (not a %s)",
            (Int)TNAM_OBJ(set), 0L,
            "you can return a set for <set>" );
    }
    len = LEN_LIST(set);

    /* perform the binary search to find the position                      */
    pos = PositionSortedDensePlist( set, obj );

    /* add the element to the set if it is not already there               */
    if ( len < pos || ! EQ( ELM_PLIST(set,pos), obj ) ) {
        GROW_PLIST( set, len+1 );
        SET_LEN_PLIST( set, len+1 );
        for ( i = len+1; pos < i; i-- ) {
            SET_ELM_PLIST( set, i, ELM_PLIST(set,i-1) );
        }
        SET_ELM_PLIST( set, pos, obj );
        CHANGED_BAG( set );

        /* fix up the type of the result                                   */
        if ( HAS_FILT_LIST( set, FN_IS_SSORT ) ) {
            CLEAR_FILTS_LIST(set);
            if ( IS_MUTABLE_OBJ(obj) ) {
                SET_FILT_LIST( set, FN_IS_DENSE );
            }
            else {
                SET_FILT_LIST( set, FN_IS_SSORT );
            }
        }
        else {
            CLEAR_FILTS_LIST(set);
            SET_FILT_LIST( set, FN_IS_DENSE );
        }
    }

    /* return void, this is a procedure                                    */
    return (Obj)0;
}


/****************************************************************************
**
*F  FuncREM_SET( <self>, <set>, <obj> ) . . . .  remove an element from a set
**
**  'FuncREM_SET' implements the internal function 'RemoveSet'.
**
**  'RemoveSet( <set>, <obj> )'
**
**  'RemoveSet' removes <obj>, which may be an object of arbitrary type, from
**  the set <set>, which must be a  proper set.  If  <obj> is in  <set> it is
**  removed and all  entries of <set>  are shifted one position leftwards, so
**  that <set> has no  holes.  If <obj>  is not in  <set>, then <set>  is not
**  changed.  No error is signalled in this case.
**
**  'RemoveSet'   does   not return anything,  it   is  only called  for  the
**  sideeffect of changing <set>.
*/
Obj FuncREM_SET (
    Obj                 self,
    Obj                 set,
    Obj                 obj )
{
    UInt                len;            /* logical length of the list      */
    UInt                pos;            /* position                        */
    UInt                i;              /* loop variable                   */

    /* check the arguments                                                 */
    while ( ! IsSet(set) || ! IS_MUTABLE_OBJ(set) ) {
        set = ErrorReturnObj(
            "RemoveSet: <set> must be a mutable proper set (not a %s)",
            (Int)TNAM_OBJ(set), 0L,
            "you can return a set for <set>" );
    }
    len = LEN_LIST(set);

    /* perform the binary search to find the position                      */
    pos = PositionSortedDensePlist( set, obj );

    /* remove the element from the set if it is there                      */
    if ( pos <= len && EQ( ELM_PLIST(set,pos), obj ) ) {
        for ( i = pos; i < len; i++ ) {
            SET_ELM_PLIST( set, i, ELM_PLIST(set,i+1) );
        }
        SET_ELM_PLIST( set, len, 0 );
        SET_LEN_PLIST( set, len-1 );

        /* fix up the type of the result                                   */
        if ( len-1 == 0 ) {
            CLEAR_FILTS_LIST(set);
            SET_FILT_LIST( set, FN_IS_EMPTY );
        }
    }

    /* return void, this is a procedure                                    */
    return (Obj)0;
}


/****************************************************************************
**
*V  TmpUnion  . . . . . . . . . . . . . . . . . . buffer for the union, local
*F  FuncUNITE_SET( <self>, <set1>, <set2> ) . . .  unite one set with another
**
**  'FuncUNITE_SET' implements the internal function 'UniteSet'.
**
**  'UniteSet( <set1>, <set2> )'
**
**  'UniteSet' changes the set <set1> so that it becomes the  union of <set1>
**  and <set2>.  The union is the set of those elements  that are elements of
**  either set.  So 'UniteSet'  adds (see  "AddSet")  all elements to  <set1>
**  that are in <set2>.  <set2> may be a list that  is  not  a proper set, in
**  which case 'Set' is silently applied to it.
**
**  'FuncUNITE_SET' merges <set1> and <set2> into a  buffer that is allocated
**  at initialization time.
**
**  'TmpUnion' is the global  bag that serves as  temporary bag for the union.
**  It is created in 'InitSet' and is resized when necessary.
*/
Obj TmpUnion;

Obj FuncUNITE_SET (
    Obj                 self,
    Obj                 set1,
    Obj                 set2 )
{
    UInt                lenr;           /* length  of result set           */
    UInt                len1;           /* length  of left  set            */
    UInt                len2;           /* length  of right set            */
    UInt                i1;             /* index into left  set            */
    UInt                i2;             /* index into right set            */
    Obj                 e1;             /* element of left  set            */
    Obj                 e2;             /* element of right set            */

    /* check the arguments                                                 */
    while ( ! IsSet(set1) || ! IS_MUTABLE_OBJ(set1) ) {
        set1 = ErrorReturnObj(
            "UniteSet: <set1> must be a mutable proper set (not a %s)",
            (Int)TNAM_OBJ(set1), 0L,
            "you can return a set for <set1>" );
    }
    while ( ! IS_LIST(set2) ) {
        set2 = ErrorReturnObj(
            "UniteSet: <set2> must be a list (not a %s)",
            (Int)TNAM_OBJ(set2), 0L,
            "you can return a list for <set2>" );
    }
    if ( ! IsSet(set2) )  set2 = SetList(set2);

    /* get the logical lengths and the pointer                             */
    len1 = LEN_PLIST( set1 );
    len2 = LEN_PLIST( set2 );
    GROW_PLIST( TmpUnion, len1 + len2 );
    lenr = 0;
    i1 = 1;
    i2 = 1;

    /* now merge the two sets into the union                               */
    while ( i1 <= len1 && i2 <= len2 ) {
        e1 = ELM_PLIST( set1, i1 );
        e2 = ELM_PLIST( set2, i2 );
        if ( EQ( e1, e2 ) ) {
            lenr++;
            SET_ELM_PLIST( TmpUnion, lenr, e1 );
            CHANGED_BAG( TmpUnion );
            i1++;  i2++;
        }
        else if ( LT( e1, e2 ) ) {
            lenr++;
            SET_ELM_PLIST( TmpUnion, lenr, e1 );
            CHANGED_BAG( TmpUnion );
            i1++;
        }
        else {
            lenr++;
            SET_ELM_PLIST( TmpUnion, lenr, e2 );
            CHANGED_BAG( TmpUnion );
            i2++;
        }
    }
    while ( i1 <= len1 ) {
        e1 = ELM_PLIST( set1, i1 );
        lenr++;
        SET_ELM_PLIST( TmpUnion, lenr, e1 );
        CHANGED_BAG( TmpUnion );
        i1++;
    }
    while ( i2 <= len2 ) {
        e2 = ELM_PLIST( set2, i2 );
        lenr++;
        SET_ELM_PLIST( TmpUnion, lenr, e2 );
        CHANGED_BAG( TmpUnion );
        i2++;
    }

    /* fix up the type of the result                                       */
    CLEAR_FILTS_LIST(set1);
    if ( 0 == LEN_PLIST(set1) ) {
        RetypeBag( set1, MUTABLE_TNUM(TNUM_OBJ(set2)) );
    }

    /* resize the result and copy back from the union                      */
    GROW_PLIST(    set1, lenr );
    SET_LEN_PLIST( set1, lenr );
    for ( i1 = 1;  i1 <= lenr;  i1++ ) {
        SET_ELM_PLIST( set1, i1, ELM_PLIST( TmpUnion, i1 ) );
        CHANGED_BAG( set1 );
        SET_ELM_PLIST( TmpUnion, i1, (Obj)0 );
    }

    /* return void, this is a procedure                                    */
    return (Obj)0;
}


/****************************************************************************
**
*F  FuncINTER_SET( <self>, <set1>, <set2> ) .  intersect one set with another
**
**  'FuncINTER_SET' implements the internal function 'IntersectSet'.
**
**  'IntersectSet( <set1>, <set2> )'
**
**  'IntersectSet' changes the set <set1> so that it becomes the intersection
**  of <set1> and <set2>.  The intersection is the set of those elements that
**  are  elements in both sets.   So 'IntersectSet' removes (see "RemoveSet")
**  all elements from <set1> that are not  in  <set2>.  <set2> may be a  list
**  that is not a proper set, in which case 'Set' is silently applied to it.
*/
Obj FuncINTER_SET (
    Obj                 self,
    Obj                 set1,
    Obj                 set2 )
{
    UInt                len1;           /* length  of left  set            */
    UInt                len2;           /* length  of right set            */
    UInt                lenr;           /* length  of result set           */
    UInt                i1;             /* index into left  set            */
    UInt                i2;             /* index into right set            */
    Obj                 e1;             /* element of left  set            */
    Obj                 e2;             /* element of right set            */

    /* check the arguments                                                 */
    while ( ! IsSet(set1) || ! IS_MUTABLE_OBJ(set1) ) {
        set1 = ErrorReturnObj(
            "IntersectSet: <set1> must be a mutable proper set (not a %s)",
            (Int)TNAM_OBJ(set1), 0L,
            "you can return a set for <set1>" );
    }
    while ( ! IS_LIST(set2) ) {
        set2 = ErrorReturnObj(
            "IntersectSet: <set2> must be a list (not a %s)",
            (Int)TNAM_OBJ(set2), 0L,
            "you can return a list for <set2>" );
    }
    if ( ! IsSet(set2) )  set2 = SetList(set2);

    /* get the logical lengths and the pointer                             */
    len1 = LEN_PLIST( set1 );
    len2 = LEN_PLIST( set2 );
    lenr = 0;
    i1 = 1;
    i2 = 1;

    /* now merge the two sets into the intersection                        */
    while ( i1 <= len1 && i2 <= len2 ) {
        e1 = ELM_PLIST( set1, i1 );
        e2 = ELM_PLIST( set2, i2 );
        if ( EQ( e1, e2 ) ) {
            lenr++;
            SET_ELM_PLIST( set1, lenr, e1 );
            i1++;  i2++;
        }
        else if ( LT( e1, e2 ) ) {
            i1++;
        }
        else {
            i2++;
        }
    }

    /* resize the result or clear the rest of the bag                      */
    SET_LEN_PLIST( set1, lenr );
    SHRINK_PLIST(  set1, lenr );

    /* fix up the type of the result                                       */
    if ( lenr == 0 ) {
        CLEAR_FILTS_LIST(set1);
        SET_FILT_LIST( set1, FN_IS_EMPTY );
    }

    /* return void, this is a procedure                                    */
    return (Obj)0;
}


/****************************************************************************
**
*F  FuncSUBTR_SET( <self>, <set1>, <set2> ) . . subtract one set from another
**
**  'FuncSUBTR_SET' implements the internal function 'SubstractSet'.
**
**  'SubstractSet( <set1>, <set2> )'
**
**  'SubstractSet' changes the  set <set1> so  that it becomes the difference
**  of <set1> and <set2>.  The difference is the set of the elements that are
**  in <set1> but not in <set2>.  So 'SubtractSet' removes  (see "RemoveSet")
**  all elements from <set1> that are in <set2>.   <set2> may  be a list that
**  is not a proper set, in which case 'Set' is silently applied to it.
*/
Obj FuncSUBTR_SET (
    Obj                 self,
    Obj                 set1,
    Obj                 set2 )
{
    UInt                len1;           /* length  of left  set            */
    UInt                len2;           /* length  of right set            */
    UInt                lenr;           /* length  of result set           */
    UInt                i1;             /* index into left  set            */
    UInt                i2;             /* index into right set            */
    Obj                 e1;             /* element of left  set            */
    Obj                 e2;             /* element of right set            */

    /* check the arguments                                                 */
    while ( ! IsSet(set1) || ! IS_MUTABLE_OBJ(set1) ) {
        set1 = ErrorReturnObj(
            "SubtractSet: <set1> must be a mutable proper set (not a %s)",
            (Int)TNAM_OBJ(set1), 0L,
            "you can return a set for <set1>" );
    }
    while ( ! IS_LIST(set2) ) {
        set2 = ErrorReturnObj(
            "SubtractSet: <set2> must be a list (not a %s)",
            (Int)TNAM_OBJ(set2), 0L,
            "you can return a list for <set2>" );
    }
    if ( ! IsSet(set2) )  set2 = SetList(set2);

    /* get the logical lengths and the pointer                             */
    len1 = LEN_PLIST( set1 );
    len2 = LEN_PLIST( set2 );
    lenr = 0;
    i1 = 1;
    i2 = 1;

    /* now merge the two sets into the difference                          */
    while ( i1 <= len1 && i2 <= len2 ) {
        e1 = ELM_PLIST( set1, i1 );
        e2 = ELM_PLIST( set2, i2 );
        if ( EQ( e1, e2 ) ) {
            i1++;  i2++;
        }
        else if ( LT( e1, e2 ) ) {
            lenr++;
            SET_ELM_PLIST( set1, lenr, e1 );
            i1++;
        }
        else {
            i2++;
        }
    }
    while ( i1 <= len1 ) {
        e1 = ELM_PLIST( set1, i1 );
        lenr++;
        SET_ELM_PLIST( set1, lenr, e1 );
        i1++;
    }

    /* resize the result or clear the rest of the bag                      */
    SET_LEN_PLIST( set1, lenr );
    SHRINK_PLIST(  set1, lenr );

    /* fix up the type of the result                                       */
    if ( lenr == 0 ) {
        CLEAR_FILTS_LIST(set1);
        SET_FILT_LIST( set1, FN_IS_EMPTY );
    }

    /* return void, this is a procedure                                    */
    return (Obj)0;
}


/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  SetupSet()  . . . . . . . . . . . . . . . . .  initialize the set package
*/
void SetupSet ( void )
{
}


/****************************************************************************
**
*F  InitSet() . . . . . . . . . . . . . . . . . .  initialize the set package
**
**  'InitSet' initializes the set package.
*/
void InitSet ( void )
{
    /* install internal functions                                          */
    C_NEW_GVAR_FUNC( "LIST_SORTED_LIST", 1, "list",
                  FuncLIST_SORTED_LIST,
           "src/set.c:LIST_SORTED_LIST" );

    C_NEW_GVAR_FUNC( "IS_EQUAL_SET", 2, "set1, set2",
                  FuncIS_EQUAL_SET,
           "src/set.c:IS_EQUAL_SET" );

    C_NEW_GVAR_FUNC( "IS_SUBSET_SET", 2, "set1, set2",
                  FuncIS_SUBSET_SET,
           "src/set.c:IS_SUBSET_SET" );


    C_NEW_GVAR_FUNC( "ADD_SET", 2, "set, val",
                  FuncADD_SET,
           "src/set.c:ADD_SET" );

    C_NEW_GVAR_FUNC( "REM_SET", 2, "set, val",
                  FuncREM_SET,
           "src/set.c:REM_SET" );

    C_NEW_GVAR_FUNC( "UNITE_SET", 2, "set1, set2",
                  FuncUNITE_SET,
           "src/set.c:UNITE_SET" );

    C_NEW_GVAR_FUNC( "INTER_SET", 2, "set1, set2",
                  FuncINTER_SET,
           "src/set.c:INTER_SET" );

    C_NEW_GVAR_FUNC( "SUBTR_SET", 2, "set1, set2",
                  FuncSUBTR_SET,
           "src/set.c:SUBTR_SET" );


    /* create the temporary union bag                                      */
    InitGlobalBag( &TmpUnion, "src/set.c:TmpUnion" );
    if ( ! SyRestoring ) {
        TmpUnion = NEW_PLIST( T_PLIST, 1024 );
        SET_LEN_PLIST( TmpUnion, 1024 );
    }
}


/****************************************************************************
**
*F  CheckSet()  . . . . . . . . . check the initialisation of the set package
*/
void CheckSet ( void )
{
    SET_REVISION( "set_c",      Revision_set_c );
    SET_REVISION( "set_h",      Revision_set_h );
}


/****************************************************************************
**

*E  set.c . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
