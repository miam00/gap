/****************************************************************************
**
*W  bool.c                      GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file contains the functions for the boolean package.
**
**  Note that boolean objects actually contain no data. The three of them
**  are distinguished by their addresses, kept in the C globals False, 
**  True and Fail.
*/
#include        "system.h"              /* system dependent part           */

SYS_CONST char * Revision_bool_c =
   "@(#)$Id$";

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */
#include        "scanner.h"             /* scanner                         */

#include        "gap.h"                 /* error handling, initialisation  */

#include        "gvars.h"               /* global variables                */
#include        "calls.h"               /* generic call mechanism          */
#include        "opers.h"               /* generic operations              */

#include        "ariths.h"              /* basic arithmetic                */

#define INCLUDE_DECLARATION_PART
#include        "bool.h"                /* booleans                        */
#undef  INCLUDE_DECLARATION_PART

#include        "records.h"             /* generic records                 */
#include        "precord.h"             /* plain records                   */

#include        "lists.h"               /* generic lists                   */
#include        "string.h"              /* strings                         */


/****************************************************************************
**

*V  True  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  true value
**
**   'True' is the value 'true'.
*/
Obj True;


/****************************************************************************
**
*V  False . . . . . . . . . . . . . . . . . . . . . . . . . . . . false value
**
**  'False' is the value 'false'.
*/
Obj False;


/****************************************************************************
**
*V  Fail  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  fail value
**
**  'Fail' is the value 'fail'.
*/
Obj Fail;


/****************************************************************************
**

*F  TypeBool( <bool> )  . . . . . . . . . . . . . . . kind of a boolean value
**
**  'TypeBool' returns the kind of boolean values.
**
**  'TypeBool' is the function in 'TypeObjFuncs' for boolean values.
*/
Obj TYPE_BOOL;

Obj TypeBool (
    Obj                 val )
{
    return TYPE_BOOL;
}


/****************************************************************************
**
*F  PrintBool( <bool> ) . . . . . . . . . . . . . . . . print a boolean value
**
**  'PrintBool' prints the boolean value <bool>.
*/
void PrintBool (
    Obj                 bool )
{
    if ( bool == True ) {
        Pr( "true", 0L, 0L );
    }
    else if ( bool == False ) {
        Pr( "false", 0L, 0L );
    }
    else if ( bool == Fail ) {
        Pr( "fail", 0L, 0L );
    }
    else {
        Pr( "<<very strange boolean value>>", 0L, 0L );
    }
}


/****************************************************************************
**
*F  EqBool( <boolL>, <boolR> )  . . . . . . . . .  test if <boolL> =  <boolR>
**
**  'EqBool' returns 'True' if the two boolean values <boolL> and <boolR> are
**  equal, and 'False' otherwise.
*/
Int EqBool (
    Obj                 boolL,
    Obj                 boolR )
{
    if ( boolL == boolR ) {
        return 1L;
    }
    else {
        return 0L;
    }
}


/****************************************************************************
**
*F  LtBool( <boolL>, <boolR> )  . . . . . . . . .  test if <boolL> <  <boolR>
**
**  'LtBool' return  'True'  if the boolean   value <boolL> is less  than the
**  boolean value <boolR> and 'False' otherwise.
*/
Int LtBool (
    Obj                 boolL,
    Obj                 boolR )
{
    if ( boolL == True && boolR == False ) {
        return 1L;
    }
    else {
        return 0L;
    }
}


/****************************************************************************
**
*F  IsBoolFilt( <self>, <obj> ) . . . . . . . . . .  test for a boolean value
**
**  'IsBoolFilt' implements the internal filter 'IsBool'.
**
**  'IsBool( <obj> )'
**
**  'IsBool'  returns  'true'  if  <obj>  is   a boolean  value  and  'false'
**  otherwise.
*/
Obj IsBoolFilt;

Obj IsBoolHandler (
    Obj                 self,
    Obj                 obj )
{
    /* return 'true' if <obj> is a boolean and 'false' otherwise           */
    if ( TNUM_OBJ(obj) == T_BOOL ) {
        return True;
    }
    else if ( TNUM_OBJ(obj) < FIRST_EXTERNAL_TNUM ) {
        return False;
    }
    else {
        return DoFilter( self, obj );
    }
}


/****************************************************************************
**

*F  ReturnTrue1( <val1> ) . . . . . . . . . . . . . . . . . .  return  'True'
**
**  'ReturnTrue?'  simply return  'True'  independent of  the values of   the
**  arguments.
**
**  Those  functions are  useful for  dispatcher  tables if the types already
**  determine the outcome.
*/
Obj ReturnTrue1 (
    Obj                 self,
    Obj                 val1 )
{
    return True;
}


/****************************************************************************
**
*F  ReturnTrue2( <val1>, <val2> ) . . . . . . . . . . . . . .  return  'True'
*/
Obj ReturnTrue2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return True;
}


/****************************************************************************
**
*F  ReturnTrue3( <val1>, <val2>, <val3> ) . . . . . . . . . .  return  'True'
*/
Obj ReturnTrue3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return True;
}


/****************************************************************************
**
*F  ReturnFalse1( <val1> )  . . . . . . . . . . . . . . . . .  return 'False'
**
**  'ReturnFalse?' likewise return 'False'.
*/
Obj ReturnFalse1 (
    Obj                 self,
    Obj                 val1 )
{
    return False;
}


/****************************************************************************
**
*F  ReturnFalse2( <val1>, <val2> )  . . . . . . . . . . . . .  return 'False'
*/
Obj ReturnFalse2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return False;
}


/****************************************************************************
**
*F  ReturnFalse3( <val1>, <val2>, <val3> )  . . . . . . . . .  return 'False'
*/
Obj ReturnFalse3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return False;
}


/****************************************************************************
**
*F  ReturnFail1( <val1> ) . . . . . . . . . . . . . . . . . .  return  'Fail'
**
**  'ReturnFail?' likewise return 'Fail'.
*/
Obj ReturnFail1 (
    Obj                 self,
    Obj                 val1 )
{
    return Fail;
}


/****************************************************************************
**
*F  ReturnFail2( <val1>, <val2> ) . . . . . . . . . . . . . .  return  'Fail'
*/
Obj ReturnFail2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return Fail;
}


/****************************************************************************
**
*F  ReturnFail3( <val1>, <val2>, <val3> ) . . . . . . . . . .  return  'Fail'
*/
Obj ReturnFail3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return Fail;
}


/****************************************************************************
**

*F  SaveBool( <bool> ) . . . . . . . . . . . . . . . . . . . . save a Boolean 
**
**  Actually, there is nothing to do
*/

void SaveBool( Obj obj )
{
  return;
}

/****************************************************************************
**
*F  LoadBool( <bool> ) . . . . . . . . . . . . . . . . . . . . save a Boolean 
**
**  Actually, there is nothing to do
*/

void LoadBool( Obj obj )
{
  return;
}

/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*E  SetupBool() . . . . . . . . . . . . . . . initialize the booleans package
*/
void SetupBool ( void )
{
    /* install the marking functions for boolean values                    */
    InfoBags[ T_BOOL ].name = "boolean";
    InitMarkFuncBags( T_BOOL, MarkNoSubBags );


    /* install the saving functions                                       */
    SaveObjFuncs[ T_BOOL ] = SaveBool;

    /* install the loading functions                                       */
    LoadObjFuncs[ T_BOOL ] = LoadBool;


    /* install the printer for boolean values                              */
    PrintObjFuncs[ T_BOOL ] = PrintBool;


    /* install the comparison functions                                    */
    EqFuncs[ T_BOOL ][ T_BOOL ] = EqBool;
    LtFuncs[ T_BOOL ][ T_BOOL ] = LtBool;
}


/****************************************************************************
**
*E  InitBool()  . . . . . . . . . . . . . . . initialize the booleans package
**
**  'InitBool' initializes the boolean package.
*/
void InitBool ( void )
{
    UInt            gvar;
    Obj             tmp;

    /* install the kind function                                           */
    ImportGVarFromLibrary( "TYPE_BOOL", &TYPE_BOOL );
    TypeObjFuncs[ T_BOOL ] = TypeBool;


    /* make the three boolean bags                                         */
    InitGlobalBag( &True,  "src/bool.c:TRUE"  );
    InitGlobalBag( &False, "src/bool.c:FALSE" );
    InitGlobalBag( &Fail,  "src/bool.c:FAIL"  );

    if ( ! SyRestoring ) {
        True  = NewBag( T_BOOL, 0L );
        False = NewBag( T_BOOL, 0L );
        Fail  = NewBag( T_BOOL, 0L );
    }


    /* `fail' is a variable not a language construct                       */
    if ( ! SyRestoring ) {
        gvar = GVarName( "fail" );
        AssGVar( gvar, Fail );
        MakeReadOnlyGVar(gvar);
    }


    /* make and install the 'IS_BOOL' filter*/
    C_NEW_GVAR_FILT( "IS_BOOL", "obj", IsBoolFilt, IsBoolHandler,
          "src/bool.c:IS_BOOL" );


    /* make and install the 'RETURN_TRUE' function                         */
    InitHandlerFunc( ReturnTrue1, "src/bool.c:ReturnTrue1" );
    InitHandlerFunc( ReturnTrue2, "src/bool.c:ReturnTrue2" );
    InitHandlerFunc( ReturnTrue3, "src/bool.c:ReturnTrue3" );

    if ( ! SyRestoring ) {
        tmp = NewFunctionC( "RETURN_TRUE", -1L, "args", ReturnTrue1 );
        HDLR_FUNC( tmp, 1 ) = ReturnTrue1;
        HDLR_FUNC( tmp, 2 ) = ReturnTrue2;
        HDLR_FUNC( tmp, 3 ) = ReturnTrue3;
        AssGVar( GVarName("RETURN_TRUE"), tmp );
    }


    /* make and install the 'RETURN_FALSE' function                        */
    InitHandlerFunc( ReturnFalse1, "src/bool.c:ReturnFalse1" );
    InitHandlerFunc( ReturnFalse2, "src/bool.c:ReturnFalse2" );
    InitHandlerFunc( ReturnFalse3, "src/bool.c:ReturnFalse3" );

    if ( ! SyRestoring ) {
        tmp = NewFunctionC("RETURN_FALSE",-1L,"args",ReturnFalse1);
        HDLR_FUNC( tmp, 1 ) = ReturnFalse1;
        HDLR_FUNC( tmp, 2 ) = ReturnFalse2;
        HDLR_FUNC( tmp, 3 ) = ReturnFalse3;
        AssGVar( GVarName( "RETURN_FALSE" ), tmp );
    }


    /* make and install the 'RETURN_FAIL' function                        */
    InitHandlerFunc( ReturnFail1, "src/bool.c:ReturnFail1" );
    InitHandlerFunc( ReturnFail2, "src/bool.c:ReturnFail2" );
    InitHandlerFunc( ReturnFail3, "src/bool.c:ReturnFail3" );

    if ( ! SyRestoring ) {
        tmp = NewFunctionC("RETURN_FAIL", -1L, "args", ReturnFail1);
        HDLR_FUNC( tmp, 1 ) = ReturnFail1;
        HDLR_FUNC( tmp, 2 ) = ReturnFail2;
        HDLR_FUNC( tmp, 3 ) = ReturnFail3;
        AssGVar( GVarName( "RETURN_FAIL" ), tmp );
    }
}


/****************************************************************************
**
*E  CheckBool() . check the initialisation of initialize the booleans package
**
**  'InitBool' initializes the boolean package.
*/
void CheckBool ( void )
{
    SET_REVISION( "bool_c",     Revision_bool_c );
    SET_REVISION( "bool_h",     Revision_bool_h );
}


/****************************************************************************
**

*E  bool.c  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
