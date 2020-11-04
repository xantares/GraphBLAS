//------------------------------------------------------------------------------
// GB_mx_put_global: put the GraphBLAS status in MATLAB workspace
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GB_mex.h"

void GB_mx_put_global
(
    bool cover
)
{

    //--------------------------------------------------------------------------
    // free the complex type and operators
    //--------------------------------------------------------------------------

    Complex_finalize ( ) ;

    //--------------------------------------------------------------------------
    // return the time to MATLAB, if it was computed
    //--------------------------------------------------------------------------

    GB_mx_put_time ( ) ;

    //--------------------------------------------------------------------------
    // log the statement coverage
    //--------------------------------------------------------------------------

    #ifdef GBCOVER
    if (cover) GB_cover_put ( ) ;
    #endif

    //--------------------------------------------------------------------------
    // finalize GraphBLAS
    //--------------------------------------------------------------------------

    GrB_finalize ( ) ;

    //--------------------------------------------------------------------------
    // check nmalloc
    //--------------------------------------------------------------------------

    int64_t nmalloc = GB_Global_nmalloc_get ( ) ;
    if (nmalloc != 0)
    {
        printf ("in GB_mx_put_global: GraphBLAS nmalloc "GBd"!\n", nmalloc) ;
        mexErrMsgTxt ("memory leak in test!") ;
    }
}

