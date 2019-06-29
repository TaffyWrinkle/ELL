////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ell_py.i (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// we want a different "ell_py" module name for python so we can wrap it in a more python friendly 
// package named "ell"

%module(moduleimport="import $module") "ell_py"

%include "ell.i"
