#############################################################################
##
#W  semigrp.gd                  GAP library                     Thomas Breuer
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains the declaration of operations for semigroups.
##
Revision.semigrp_gd :=
    "@(#)$Id$";


#############################################################################
##
#P  IsSemigroup( <D> )
##
##  A semigroup is a magma with associative multiplication.
##
IsSemigroup := IsMagma and IsAssociative;
SetIsSemigroup := Setter( IsSemigroup );
HasIsSemigroup := Tester( IsSemigroup );


#############################################################################
##
#A  AsSemigroup( <D> )  . . . . . . . . . . . . domain <D> viewed as semigroup
##
AsSemigroup := NewAttribute( "AsSemigroup", IsCollection );
SetAsSemigroup := Setter( AsSemigroup );
HasAsSemigroup := Tester( AsSemigroup );


#############################################################################
##
#A  GeneratorsOfSemigroup( <D> ) . . . . . semigroup generators of domain <D>
##
##  Semigroup generators of a domain are the same as magma generators.
##
GeneratorsOfSemigroup    := GeneratorsOfMagma;
SetGeneratorsOfSemigroup := SetGeneratorsOfMagma;
HasGeneratorsOfSemigroup := HasGeneratorsOfMagma;


#############################################################################
##
#O  SemigroupByGenerators( <gens> ) . . . . . . semigroup generated by <gens>
##
SemigroupByGenerators := NewOperation( "SemigroupByGenerators",
    [ IsCollection ] );
#T 1997/01/16 fceller was old 'NewConstructor'


#############################################################################
##
#F  Semigroup( <gen>, ... ) . . . . . . . . semigroup generated by collection
#F  Semigroup( <gens> ) . . . . . . . . . . semigroup generated by collection
##
##  'Semigroup( <gen>, ... )' is the semigroup generated by the arguments
##  <gen>, ...
##
##  If the only argument <obj> is a list that is not a matrix then
##  'Semigroup( <obj> )' is the semigroup generated by the elements
##  of that list.
##
##  It is *not* checked whether the underlying multiplication is associative.
##
Semigroup := NewOperationArgs( "Semigroup" );


#############################################################################
##
#F  Subsemigroup( <S>, <gens> ) . . .  subsemigroup of <S> generated by <gens>
#F  SubsemigroupNC( <S>, <gens> ) . .  subsemigroup of <S> generated by <gens>
##
Subsemigroup := Submagma;

SubsemigroupNC := SubmagmaNC;


#############################################################################
##
#F  FreeSemigroup( <rank> )
#F  FreeSemigroup( <rank>, <name> )
#F  FreeSemigroup( <name1>, <name2>, ... )
#F  FreeSemigroup( <names> )
##
##  Called in the first form, 'FreeSemigroup' returns a free semigroup on
##  <rank> generators.
##  Called in the second form, 'FreeSemigroup' returns a free semigroup on
##  <rank> generators, printed as '<name>1', '<name>2' etc.
##  Called in the third form, 'FreeSemigroup' returns a free semigroup on
##  as many generators as arguments, printed as <name1>, <name2> etc.
##  Called in the fourth form, 'FreeSemigroup' returns a free semigroup on
##  as many generators as the length of the list <names>, the $i$-th
##  generator being printed as '<names>[$i$]'.
##
FreeSemigroup := NewOperationArgs( "FreeSemigroup" );


#############################################################################
##
#E  semigrp.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here



