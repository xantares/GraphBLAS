//------------------------------------------------------------------------------
// GB_AxB_saxpy3_coarseHash_phase1:
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

{

    //--------------------------------------------------------------------------
    // phase1: coarse hash task, C=A*B
    //--------------------------------------------------------------------------

    // Initially, Hf [...] < mark for all of Hf.
    // Let f = Hf [hash] and h = Hi [hash]

    // f < mark          : unoccupied.
    // h == i, f == mark : occupied with C(i,j)

    // The mask M can be optionally checked, if it is dense (full, bitmpa, or
    // sparse/hyper with all entries present) and checked in place.  This
    // method is not used if M is present and sparse.

    for (int64_t kk = kfirst ; kk <= klast ; kk++)
    {
        GB_GET_B_j ;            // get B(:,j)
        Cp [kk] = 0 ;           // ok: C is sparse
        if (bjnz == 0) continue ;

        #ifdef GB_CHECK_MASK_ij

            // The mask M is dense (full, bitmap, or sparse/hyper with all
            // entries present in the entire matrix).  Get pointers Mjb and
            // Mjx into the M(:,j) vector.
            GB_GET_M_j
            #ifndef M_SIZE
            #define M_SIZE 1
            #endif
            const M_TYPE *GB_RESTRICT Mjx = Mask_struct ? NULL :
                ((M_TYPE *) Mx) + (M_SIZE * pM_start) ;
            const int8_t *GB_RESTRICT Mjb = M_is_bitmap ? (Mb+pM_start) : NULL ;

        #else

            // M is not present
            if (bjnz == 1)
            { 
                if (!GBB (Bb, pB)) continue ;
                int64_t k = GBI (Bi, pB, bvlen) ;   // get B(k,j)
                GB_GET_A_k ;            // get A(:,k)
                Cp [kk] = aknz ;        // nnz(C(:,j)) = nnz(A(:,k))
                continue ;
            }

        #endif

        mark++ ;
        int64_t cjnz = 0 ;
        for ( ; pB < pB_end ; pB++)     // scan B(:,j)
        {
            if (!GBB (Bb, pB)) continue ;
            int64_t k = GBI (Bi, pB, bvlen) ;   // get B(k,j)
            GB_GET_A_k ;                // get A(:,k)
            // scan A(:,k)
            for (int64_t pA = pA_start ; pA < pA_end ; pA++)
            {
                if (!GBB (Ab, pA)) continue ;
                int64_t i = GBI (Ai, pA, avlen) ; // get A(i,k)
                #ifdef GB_CHECK_MASK_ij
                // check mask condition and skip if C(i,j) is protected by
                // the mask
                GB_CHECK_MASK_ij ;
                #endif
                int64_t hash ;
                bool marked = false ;
                bool done = false ;
                for (hash = GB_HASHF (i) ; ; GB_REHASH (hash, i))
                {
                    // if the hash entry is marked then it is occuppied with
                    // some row index in the current C(:,j).
                    marked = (Hf [hash] == mark) ;
                    // if found, then the hash entry holds the row index i.
                    bool found = marked && (Hi [hash] == i) ;
                    // if the hash entry is unmarked, then it is empty, and i
                    // is not in the hash table.  In this case, C(i,j) is a new
                    // entry.  The search terminates if either i is found, or
                    // if an empty (unmarked) slot is found.
                    if (found || !marked) break ;
                }
                if (!marked)
                { 
                    // empty slot found, insert C(i,j)
                    Hf [hash] = mark ;
                    Hi [hash] = i ;
                    cjnz++ ;            // C(i,j) is a new entry.
                }
            }
        }
        // count the entries in C(:,j)
        Cp [kk] = cjnz ;        // ok: C is sparse
    }

    // this task is done; go to the next one
    continue ;
}

#undef M_TYPE
#undef M_SIZE

