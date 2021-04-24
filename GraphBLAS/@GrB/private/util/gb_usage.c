//------------------------------------------------------------------------------
// gb_usage: check usage and make sure GrB.init has been called
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "gb_matlab.h"

void gb_usage       // check usage and make sure GrB.init has been called
(
    bool ok,                // if false, then usage is not correct
    const char *usage       // error message if usage is not correct
)
{

    //--------------------------------------------------------------------------
    // clear the debug memory table
    //--------------------------------------------------------------------------

    GB_Global_memtable_clear ( ) ;

    //--------------------------------------------------------------------------
    // make sure GrB.init has been called
    //--------------------------------------------------------------------------

    if (!GB_Global_GrB_init_called_get ( )) // TODO::: add this as GxB_get
    {

        //----------------------------------------------------------------------
        // initialize GraphBLAS
        //----------------------------------------------------------------------

        OK (GxB_init (GrB_NONBLOCKING, mxMalloc, mxCalloc, mxRealloc, mxFree,
            false)) ;

        // must use mexPrintf to print to MATLAB Command Window
        OK (GxB_Global_Option_set (GxB_PRINTF, mexPrintf)) ;
        OK (GxB_Global_Option_set (GxB_FLUSH, gb_flush)) ;

        // disable the memory pool
        for (int k = 0 ; k < 64 ; k++)
        {
            // TODO:: add this as an option for GxB_set/get
            GB_Global_free_pool_limit_set (k, 0) ;  // TODO:: use GxB_set
        }

        // MATLAB matrices are stored by column
        OK (GxB_Global_Option_set (GxB_FORMAT, GxB_BY_COL)) ;

        // print 1-based indices
        GB_Global_print_one_based_set (true) ;      // TODO:: add to GxB_set/get

        // for debug only
        GB_Global_abort_function_set (gb_abort) ;   // TODO:: add as GxB_set/get
    }

    //--------------------------------------------------------------------------
    // check usage
    //--------------------------------------------------------------------------

    if (!ok)
    {
        ERROR (usage) ;
    }

    //--------------------------------------------------------------------------
    // get test coverage
    //--------------------------------------------------------------------------

    #ifdef GBCOV
    gbcov_get ( ) ;
    #endif
}

