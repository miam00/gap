/****************************************************************************
**
*W  objpcgel.c                  GAP source                       Frank Celler
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
*/
#include        "system.h"              /* Ints, UInts                     */

SYS_CONST char * Revision_objpcgel_c =
   "@(#)$Id$";

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */
#include        "scanner.h"             /* scanner                         */

#include        "gvars.h"               /* global variables                */
#include        "gap.h"                 /* error handling, initialisation  */

#include        "calls.h"               /* generic call mechanism          */

#include        "records.h"             /* generic records                 */
#include        "precord.h"             /* plain records                   */

#include        "lists.h"               /* generic lists                   */
#include        "plist.h"               /* plain lists                     */
#include        "string.h"              /* strings                         */

#include        "ariths.h"              /* basic arithmetic                */
#include        "bool.h"                /* booleans                        */

#include        "objfgelm.h"            /* objects of free groups          */
#include        "objscoll.h"            /* single collector                */

#define INCLUDE_DECLARATION_PART
#include        "objpcgel.h"            /* objects of polycyclic groups    */
#undef  INCLUDE_DECLARATION_PART


/****************************************************************************
**

*F * * * * * * * * * * * * * * *  boxed objects * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  FuncLessBoxedObj( <self>, <left>, <right> )
*/
Obj FuncLessBoxedObj ( Obj self, Obj left, Obj right )
{
    return LT( ADDR_OBJ(left)[1], ADDR_OBJ(right)[1] ) ? False : True;
}


/****************************************************************************
**
*F  FuncEqualBoxedObj( <self>, <left>, <right> )
*/
Obj FuncEqualBoxedObj ( Obj self, Obj left, Obj right )
{
    return EQ( ADDR_OBJ(left)[1], ADDR_OBJ(right)[1] ) ? False : True;
}


/****************************************************************************
**

*F * * * * * * * * * * * * * * * pc word aspect * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  FuncNBitsPcWord_Comm( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_Comm ( Obj self, Obj left, Obj right )
{
    return FuncFinPowConjCol_ReducedComm(
        self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**
*F  FuncNBitsPcWord_Conjugate( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_Conjugate ( Obj self, Obj left, Obj right )
{
    left = FuncFinPowConjCol_ReducedLeftQuotient(
                self, COLLECTOR_PCWORD(left), right, left );
    return FuncFinPowConjCol_ReducedProduct(
                self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**
*F  FuncNBitsPcWord_LeftQuotient( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_LeftQuotient ( Obj self, Obj left, Obj right )
{
    return FuncFinPowConjCol_ReducedLeftQuotient(
        self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**
*F  FuncNBitsPcWord_PowerSmallInt( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_PowerSmallInt ( Obj self, Obj left, Obj right )
{
    return FuncFinPowConjCol_ReducedPowerSmallInt(
        self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**
*F  FuncNBitsPcWord_Product( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_Product ( Obj self, Obj left, Obj right )
{
    return FuncFinPowConjCol_ReducedProduct(
        self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**
*F  FuncNBitsPcWord_Quotient( <self>, <left>, <right> )
*/
Obj FuncNBitsPcWord_Quotient ( Obj self, Obj left, Obj right )
{
    return FuncFinPowConjCol_ReducedQuotient(
        self, COLLECTOR_PCWORD(left), left, right );
}


/****************************************************************************
**

*F * * * * * * * * * * * * * * free word aspect * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  Func8Bits_DepthOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func8Bits_DepthOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    Int         ebits;          /* number of bits in the exponent          */

    /* if the pc element is the identity we have to ask the pcgs           */
    if ( NPAIRS_WORD(w) == 0 )
        return INTOBJ_INT( LEN_LIST(pcgs) + 1 );

    /* otherwise it is the generators number of the first syllable         */
    else {
        ebits = EBITS_WORD(w);
        return INTOBJ_INT(((((UInt1*)DATA_WORD(w))[0]) >> ebits)+1);
    }
}


/****************************************************************************
**
*F  Func8Bits_ExponentOfPcElement( <self>, <pcgs>, <w>, <pos> )
*/
Obj Func8Bits_ExponentOfPcElement ( Obj self, Obj pcgs, Obj w, Obj pos )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt        ebits;          /* number of exponent bits                 */
    UInt        npos;           /* the wanted generator number             */
    UInt        num;            /* number of syllables in <w>              */
    UInt1 *     ptr;            /* pointer to the syllables of <w>         */
    UInt        i;              /* loop                                    */
    UInt        gen;            /* current generator number                */

    /* all exponents are zero if the pc element if the identity            */
    num = NPAIRS_WORD(w);
    if ( num == 0 )
        return INTOBJ_INT(0);

    /* otherwise find the syllable belonging to <exp>                      */
    else {
        ebits = EBITS_WORD(w);
        exps  = 1UL << (ebits-1);
        expm  = exps - 1;
        npos  = INT_INTOBJ(pos);
        ptr   = ((UInt1*)DATA_WORD(w));
        for ( i = 1;  i <= num;  i++, ptr++ ) {
            gen = ((*ptr) >> ebits) + 1;
            if ( gen == npos ) {
                if ( (*ptr) & exps )
                    return INTOBJ_INT(((*ptr)&expm)-exps);
                else
                    return INTOBJ_INT((*ptr)&expm);
            }
            if ( npos < gen )
                return INTOBJ_INT(0);
        }
        return INTOBJ_INT(0);
    }
}


/****************************************************************************
**
*F  Func8Bits_LeadingExponentOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func8Bits_LeadingExponentOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt1       p;              /* first syllable                          */

    /* the leading exponent is zero iff the pc element if the identity     */
    if ( NPAIRS_WORD(w) == 0 )
        return Fail;

    /* otherwise it is the exponent of the first syllable                  */
    else {
        exps = 1UL << (EBITS_WORD(w)-1);
        expm = exps - 1;
        p = ((UInt1*)DATA_WORD(w))[0];
        if ( p & exps )
            return INTOBJ_INT((p&expm)-exps);
        else
            return INTOBJ_INT(p&expm);
    }
}


/****************************************************************************
**

*F  Func16Bits_DepthOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func16Bits_DepthOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    Int         ebits;          /* number of bits in the exponent          */

    /* if the pc element is the identity we have to ask the pcgs           */
    if ( NPAIRS_WORD(w) == 0 )
        return INTOBJ_INT( LEN_LIST(pcgs) + 1 );

    /* otherwise it is the generators number of the first syllable         */
    else {
        ebits = EBITS_WORD(w);
        return INTOBJ_INT(((((UInt2*)DATA_WORD(w))[0]) >> ebits)+1);
    }
}


/****************************************************************************
**
*F  Func16Bits_ExponentOfPcElement( <self>, <pcgs>, <w>, <pos> )
*/
Obj Func16Bits_ExponentOfPcElement ( Obj self, Obj pcgs, Obj w, Obj pos )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt        ebits;          /* number of exponent bits                 */
    UInt        npos;           /* the wanted generator number             */
    UInt        num;            /* number of syllables in <w>              */
    UInt2 *     ptr;            /* pointer to the syllables of <w>         */
    UInt        i;              /* loop                                    */
    UInt        gen;            /* current generator number                */

    /* all exponents are zero if the pc element if the identity            */
    num = NPAIRS_WORD(w);
    if ( num == 0 )
        return INTOBJ_INT(0);

    /* otherwise find the syllable belonging to <exp>                      */
    else {
        ebits = EBITS_WORD(w);
        exps  = 1UL << (ebits-1);
        expm  = exps - 1;
        npos  = INT_INTOBJ(pos);
        ptr   = ((UInt2*)DATA_WORD(w));
        for ( i = 1;  i <= num;  i++, ptr++ ) {
            gen = ((*ptr) >> ebits) + 1;
            if ( gen == npos ) {
                if ( (*ptr) & exps )
                    return INTOBJ_INT(((*ptr)&expm)-exps);
                else
                    return INTOBJ_INT((*ptr)&expm);
            }
            if ( npos < gen )
                return INTOBJ_INT(0);
        }
        return INTOBJ_INT(0);
    }
}


/****************************************************************************
**
*F  Func16Bits_LeadingExponentOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func16Bits_LeadingExponentOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt2       p;              /* first syllable                          */

    /* the leading exponent is zero iff the pc element if the identity     */
    if ( NPAIRS_WORD(w) == 0 )
        return Fail;

    /* otherwise it is the exponent of the first syllable                  */
    else {
        exps = 1UL << (EBITS_WORD(w)-1);
        expm = exps - 1;
        p = ((UInt2*)DATA_WORD(w))[0];
        if ( p & exps )
            return INTOBJ_INT((p&expm)-exps);
        else
            return INTOBJ_INT(p&expm);
    }
}


/****************************************************************************
**

*F  Func32Bits_DepthOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func32Bits_DepthOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    Int         ebits;          /* number of bits in the exponent          */

    /* if the pc element is the identity we have to ask the pcgs           */
    if ( NPAIRS_WORD(w) == 0 )
        return INTOBJ_INT( LEN_LIST(pcgs) + 1 );

    /* otherwise it is the generators number of the first syllable         */
    else {
        ebits = EBITS_WORD(w);
        return INTOBJ_INT(((((UInt4*)DATA_WORD(w))[0]) >> ebits)+1);
    }
}


/****************************************************************************
**
*F  Func32Bits_ExponentOfPcElement( <self>, <pcgs>, <w>, <pos> )
*/
Obj Func32Bits_ExponentOfPcElement ( Obj self, Obj pcgs, Obj w, Obj pos )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt        ebits;          /* number of exponent bits                 */
    UInt        npos;           /* the wanted generator number             */
    UInt        num;            /* number of syllables in <w>              */
    UInt4 *     ptr;            /* pointer to the syllables of <w>         */
    UInt        i;              /* loop                                    */
    UInt        gen;            /* current generator number                */

    /* all exponents are zero if the pc element if the identity            */
    num = NPAIRS_WORD(w);
    if ( num == 0 )
        return INTOBJ_INT(0);

    /* otherwise find the syllable belonging to <exp>                      */
    else {
        ebits = EBITS_WORD(w);
        exps  = 1UL << (ebits-1);
        expm  = exps - 1;
        npos  = INT_INTOBJ(pos);
        ptr   = ((UInt4*)DATA_WORD(w));
        for ( i = 1;  i <= num;  i++, ptr++ ) {
            gen = ((*ptr) >> ebits) + 1;
            if ( gen == npos ) {
                if ( (*ptr) & exps )
                    return INTOBJ_INT(((*ptr)&expm)-exps);
                else
                    return INTOBJ_INT((*ptr)&expm);
            }
            if ( npos < gen )
                return INTOBJ_INT(0);
        }
        return INTOBJ_INT(0);
    }
}


/****************************************************************************
**
*F  Func32Bits_LeadingExponentOfPcElement( <self>, <pcgs>, <w> )
*/
Obj Func32Bits_LeadingExponentOfPcElement ( Obj self, Obj pcgs, Obj w )
{
    UInt        expm;           /* signed exponent mask                    */
    UInt        exps;           /* sign exponent mask                      */
    UInt4       p;              /* first syllable                          */

    /* the leading exponent is zero iff the pc element if the identity     */
    if ( NPAIRS_WORD(w) == 0 )
        return Fail;

    /* otherwise it is the exponent of the first syllable                  */
    else {
        exps = 1UL << (EBITS_WORD(w)-1);
        expm = exps - 1;
        p = ((UInt4*)DATA_WORD(w))[0];
        if ( p & exps )
            return INTOBJ_INT((p&expm)-exps);
        else
            return INTOBJ_INT(p&expm);
    }
}


/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/


/****************************************************************************
**


*F  SetupPcElements() . . . . . . .  initialize the pc group elements package
*/
void SetupPcElements ( void )
{
}


/****************************************************************************
**
*F  InitPcElements()  . . . . . . .  initialize the pc group elements package
*/
void InitPcElements ( void )
{

    /* export position numbers 'PCWP_SOMETHING'                            */
    if ( ! SyRestoring ) {
        AssGVar( GVarName( "PCWP_FIRST_ENTRY" ),
                 INTOBJ_INT(PCWP_FIRST_ENTRY) );
        AssGVar( GVarName( "PCWP_NAMES" ),
                 INTOBJ_INT(PCWP_NAMES) );
        AssGVar( GVarName( "PCWP_COLLECTOR" ),
                 INTOBJ_INT(PCWP_COLLECTOR) );
        AssGVar( GVarName( "PCWP_FIRST_FREE" ),
                 INTOBJ_INT(PCWP_FIRST_FREE) );
    }

    /* methods for boxed objs                                              */
    C_NEW_GVAR_FUNC( "LessBoxedObj", 2, "lobj, lobj",
                  FuncLessBoxedObj,
      "src/objpcgel.c:LessBoxedObj" );

    C_NEW_GVAR_FUNC( "EqualBoxedObj", 2, "lobj, lobj",
                  FuncEqualBoxedObj,
      "src/objpcgel.c:EqualBoxedObj" );


    /* finite power conjugate collector words                              */
    C_NEW_GVAR_FUNC( "NBitsPcWord_Comm", 2,
                       "n_bits_pcword, n_bits_pcword",
                  FuncNBitsPcWord_Comm,
      "src/objpcgel.c:NBitsPcWord_Comm" );

    C_NEW_GVAR_FUNC( "NBitsPcWord_Conjugate", 2, "n_bits_pcword, n_bits_pcword",
                  FuncNBitsPcWord_Conjugate,
      "src/objpcgel.c:NBitsPcWord_Conjugate" );

    C_NEW_GVAR_FUNC( "NBitsPcWord_LeftQuotient", 2, "n_bits_pcword, n_bits_pcword",
                  FuncNBitsPcWord_LeftQuotient,
      "src/objpcgel.c:NBitsPcWord_LeftQuotient" );

    C_NEW_GVAR_FUNC( "NBitsPcWord_PowerSmallInt", 2, "n_bits_pcword, small_integer",
                  FuncNBitsPcWord_PowerSmallInt,
      "src/objpcgel.c:NBitsPcWord_PowerSmallInt" );

    C_NEW_GVAR_FUNC( "NBitsPcWord_Product", 2, "n_bits_pcword, n_bits_pcword",
                  FuncNBitsPcWord_Product,
      "src/objpcgel.c:NBitsPcWord_Product" );

    C_NEW_GVAR_FUNC( "NBitsPcWord_Quotient", 2, "n_bits_pcword, n_bits_pcword",
                  FuncNBitsPcWord_Quotient,
      "src/objpcgel.c:NBitsPcWord_Quotient" );


    /* 8 bits word                                                         */
    C_NEW_GVAR_FUNC( "8Bits_DepthOfPcElement", 2, "8_bits_pcgs, 8_bits_pcword",
                  Func8Bits_DepthOfPcElement,
      "src/objpcgel.c:8Bits_DepthOfPcElement" );

    C_NEW_GVAR_FUNC( "8Bits_ExponentOfPcElement", 3, "8_bits_pcgs, 8_bits_pcword, int",
                  Func8Bits_ExponentOfPcElement,
      "src/objpcgel.c:8Bits_ExponentOfPcElement" );

    C_NEW_GVAR_FUNC( "8Bits_LeadingExponentOfPcElement", 2, "8_bits_pcgs, 8_bits_word",
                  Func8Bits_LeadingExponentOfPcElement,
      "src/objpcgel.c:8Bits_LeadingExponentOfPcElement" );

    /* 16 bits word                                                        */
    C_NEW_GVAR_FUNC( "16Bits_DepthOfPcElement", 2, "16_bits_pcgs, 16_bits_pcword",
                  Func16Bits_DepthOfPcElement,
      "src/objpcgel.c:16Bits_DepthOfPcElement" );

    C_NEW_GVAR_FUNC( "16Bits_ExponentOfPcElement", 3, "16_bits_pcgs, 16_bits_pcword, int",
                  Func16Bits_ExponentOfPcElement,
      "src/objpcgel.c:16Bits_ExponentOfPcElement" );

    C_NEW_GVAR_FUNC( "16Bits_LeadingExponentOfPcElement", 2, "16_bits_pcgs, 16_bits_word",
                  Func16Bits_LeadingExponentOfPcElement,
      "src/objpcgel.c:16Bits_LeadingExponentOfPcElement" );

    /* 32 bits word                                                        */
    C_NEW_GVAR_FUNC( "32Bits_DepthOfPcElement", 2, "32_bits_pcgs, 32_bits_pcword",
                  Func32Bits_DepthOfPcElement,
      "src/objpcgel.c:32Bits_DepthOfPcElement" );

    C_NEW_GVAR_FUNC( "32Bits_ExponentOfPcElement", 3, "32_bits_pcgs, 32_bits_pcword, int",
                  Func32Bits_ExponentOfPcElement,
      "src/objpcgel.c:32Bits_ExponentOfPcElement" );

    C_NEW_GVAR_FUNC( "32Bits_LeadingExponentOfPcElement", 2, "32_bits_pcgs, 32_bits_word",
                  Func32Bits_LeadingExponentOfPcElement,
      "src/objpcgel.c:32Bits_LeadingExponentOfPcElement" );
}


/****************************************************************************
**
*F  CheckPcElements() . check initialisation of the pc group elements package
*/
void CheckPcElements ( void )
{
    SET_REVISION( "objpcgel_c", Revision_objpcgel_c );
    SET_REVISION( "objpcgel_h", Revision_objpcgel_h );
}


/****************************************************************************
**

*E  objpcgel.c  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
