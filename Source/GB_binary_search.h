//------------------------------------------------------------------------------
// GB_binary_search.h: binary search in a sorted list
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#ifndef GB_BINARY_SEARCH_H
#define GB_BINARY_SEARCH_H

//------------------------------------------------------------------------------
// GB_TRIM_BINARY_SEARCH: simple binary search
//------------------------------------------------------------------------------

// search for integer i in the list X [pleft...pright]; no zombies.
// The list X [pleft ... pright] is in ascending order.  It may have
// duplicates.

#define GB_TRIM_BINARY_SEARCH(i,X,pleft,pright)                             \
{                                                                           \
    /* binary search of X [pleft ... pright] for integer i */               \
    while (pleft < pright)                                                  \
    {                                                                       \
        int64_t pmiddle = (pleft + pright) / 2 ;                            \
        if (X [pmiddle] < i)                                                \
        {                                                                   \
            /* if in the list, it appears in [pmiddle+1..pright] */         \
            pleft = pmiddle + 1 ;                                           \
        }                                                                   \
        else                                                                \
        {                                                                   \
            /* if in the list, it appears in [pleft..pmiddle] */            \
            pright = pmiddle ;                                              \
        }                                                                   \
    }                                                                       \
    /* binary search is narrowed down to a single item */                   \
    /* or it has found the list is empty */                                 \
    ASSERT (pleft == pright || pleft == pright + 1) ;                       \
}

//------------------------------------------------------------------------------
// GB_BINARY_SEARCH: binary search and check if found
//------------------------------------------------------------------------------

// If found is true then X [pleft == pright] == i.  If duplicates appear then
// X [pleft] is any one of the entries with value i in the list.
// If found is false then
//    X [original_pleft ... pleft-1] < i and
//    X [pleft+1 ... original_pright] > i holds.
// The value X [pleft] may be either < or > i.
#define GB_BINARY_SEARCH(i,X,pleft,pright,found)                            \
{                                                                           \
    GB_TRIM_BINARY_SEARCH (i, X, pleft, pright) ;                           \
    found = (pleft == pright && X [pleft] == i) ;                           \
}

//------------------------------------------------------------------------------
// GB_SPLIT_BINARY_SEARCH: binary search, and then partition the list
//------------------------------------------------------------------------------

// If found is true then X [pleft] == i.  If duplicates appear then X [pleft]
//    is any one of the entries with value i in the list.
// If found is false then
//    X [original_pleft ... pleft-1] < i and
//    X [pleft ... original_pright] > i holds, and pleft-1 == pright
// If X has no duplicates, then whether or not i is found,
//    X [original_pleft ... pleft-1] < i and
//    X [pleft ... original_pright] >= i holds.
#define GB_SPLIT_BINARY_SEARCH(i,X,pleft,pright,found)                      \
{                                                                           \
    GB_BINARY_SEARCH (i, X, pleft, pright, found)                           \
    if (!found && (pleft == pright))                                        \
    {                                                                       \
        if (i > X [pleft])                                                  \
        {                                                                   \
            pleft++ ;                                                       \
        }                                                                   \
        else                                                                \
        {                                                                   \
            pright++ ;                                                      \
        }                                                                   \
    }                                                                       \
}

//------------------------------------------------------------------------------
// GB_TRIM_BINARY_SEARCH_ZOMBIE: binary search in the presence of zombies
//------------------------------------------------------------------------------

#define GB_TRIM_BINARY_SEARCH_ZOMBIE(i,X,pleft,pright)                      \
{                                                                           \
    /* binary search of X [pleft ... pright] for integer i */               \
    while (pleft < pright)                                                  \
    {                                                                       \
        int64_t pmiddle = (pleft + pright) / 2 ;                            \
        if (i > GB_UNFLIP (X [pmiddle]))                                    \
        {                                                                   \
            /* if in the list, it appears in [pmiddle+1..pright] */         \
            pleft = pmiddle + 1 ;                                           \
        }                                                                   \
        else                                                                \
        {                                                                   \
            /* if in the list, it appears in [pleft..pmiddle] */            \
            pright = pmiddle ;                                              \
        }                                                                   \
    }                                                                       \
    /* binary search is narrowed down to a single item */                   \
    /* or it has found the list is empty */                                 \
    ASSERT (pleft == pright || pleft == pright + 1) ;                       \
}

//------------------------------------------------------------------------------
// GB_BINARY_SEARCH_ZOMBIE: binary search with zombies; check if found
//------------------------------------------------------------------------------

#define GB_BINARY_SEARCH_ZOMBIE(i,X,pleft,pright,found,nzombies,is_zombie)  \
{                                                                           \
    if (nzombies > 0)                                                       \
    {                                                                       \
        GB_TRIM_BINARY_SEARCH_ZOMBIE (i, X, pleft, pright) ;                \
        found = false ;                                                     \
        is_zombie = false ;                                                 \
        if (pleft == pright)                                                \
        {                                                                   \
            int64_t i2 = X [pleft] ;                                        \
            is_zombie = GB_IS_ZOMBIE (i2) ;                                 \
            if (is_zombie)                                                  \
            {                                                               \
                i2 = GB_FLIP (i2) ;                                         \
            }                                                               \
            found = (i == i2) ;                                             \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        is_zombie = false ;                                                 \
        GB_BINARY_SEARCH(i,X,pleft,pright,found)                            \
    }                                                                       \
}

//------------------------------------------------------------------------------
// GB_SPLIT_BINARY_SEARCH_ZOMBIE: binary search with zombies; then partition
//------------------------------------------------------------------------------

#define GB_SPLIT_BINARY_SEARCH_ZOMBIE(i,X,pleft,pright,found,nzom,is_zombie) \
{                                                                           \
    if (nzom > 0)                                                           \
    {                                                                       \
        GB_TRIM_BINARY_SEARCH_ZOMBIE (i, X, pleft, pright) ;                \
        found = false ;                                                     \
        is_zombie = false ;                                                 \
        if (pleft == pright)                                                \
        {                                                                   \
            int64_t i2 = X [pleft] ;                                        \
            is_zombie = GB_IS_ZOMBIE (i2) ;                                 \
            if (is_zombie)                                                  \
            {                                                               \
                i2 = GB_FLIP (i2) ;                                         \
            }                                                               \
            found = (i == i2) ;                                             \
            if (!found)                                                     \
            {                                                               \
                if (i > i2)                                                 \
                {                                                           \
                    pleft++ ;                                               \
                }                                                           \
                else                                                        \
                {                                                           \
                    pright++ ;                                              \
                }                                                           \
            }                                                               \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        is_zombie = false ;                                                 \
        GB_SPLIT_BINARY_SEARCH(i,X,pleft,pright,found)                      \
    }                                                                       \
}

//------------------------------------------------------------------------------
// GB_lookup: find k so that j == Ah [k]
//------------------------------------------------------------------------------

// Given a sparse, hypersparse, or hyperslice matrix, find k so that j == Ah
// [k], if it appears in the list.  k is not needed by the caller, just the
// variables pstart, pend, pleft, and found.  GB_lookup cannot be used if
// A is a slice (it could be extended to handle this case).

static inline bool GB_lookup        // find j = Ah [k] in a hyperlist
(
    const bool A_is_hyper,          // true if A is hypersparse
    const int64_t *GB_RESTRICT Ah,  // A->h [0..A->nvec-1]: list of vectors
    const int64_t *GB_RESTRICT Ap,  // A->p [0..A->nvec  ]: pointers to vectors
    int64_t *GB_RESTRICT pleft,     // look only in A->h [pleft..pright]
    int64_t pright,                 // normally A->nvec-1, but can be trimmed
//  const int64_t nvec,             // A->nvec: number of vectors
    const int64_t j,                // vector to find, as j = Ah [k]
    int64_t *GB_RESTRICT pstart,    // start of vector: Ap [k]
    int64_t *GB_RESTRICT pend       // end of vector: Ap [k+1]
)
{
    if (A_is_hyper)
    {
        // to search the whole Ah list, use on input:
        // pleft = 0 ; pright = nvec-1 ;
        bool found ;
        GB_BINARY_SEARCH (j, Ah, (*pleft), pright, found) ;
        if (found)
        { 
            // j appears in the hyperlist at Ah [pleft]
            // k = (*pleft)
            (*pstart) = Ap [(*pleft)] ;
            (*pend)   = Ap [(*pleft)+1] ;
        }
        else
        { 
            // j does not appear in the hyperlist Ah
            // k = -1
            (*pstart) = -1 ;
            (*pend)   = -1 ;
        }
        return (found) ;
    }
    else
    { 
        // A is not hypersparse; j always appears
        // k = j
        (*pstart) = Ap [j] ;
        (*pend)   = Ap [j+1] ;
        return (true) ;
    }
}

#endif

