/*======================================================================
 *
 * DISCLAIMER
 *
 * This material was prepared as an account of work sponsored by an
 * agency of the United States Government.  Neither the United States
 * Government nor the United States Department of Energy, nor Battelle,
 * nor any of their employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY,
 * COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT,
 * SOFTWARE, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT
 * INFRINGE PRIVATELY OWNED RIGHTS.
 *
 * ACKNOWLEDGMENT
 *
 * This software and its documentation were produced with Government
 * support under Contract Number DE-AC06-76RLO-1830 awarded by the United
 * States Department of Energy.  The Government retains a paid-up
 * non-exclusive, irrevocable worldwide license to reproduce, prepare
 * derivative works, perform publicly and display publicly by or for the
 * Government, including the right to distribute to other Government
 * contractors.
 *
 *======================================================================
 *
 *  -- PEIGS  routine (version 2.1) --
 *     Pacific Northwest Laboratory
 *     July 28, 1995
 *
 *======================================================================
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <time.h>

#include "globalp.c.h"

#ifdef TIMING
#include "timing.h"
TIMINGG test_timing;
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define ffabs(a) ((a) > (0.) ? (a) : (-a))

long peigs_DEBUG=1; 


DoublePrecision psigma, psgn, peigs_shift, peigs_scale;

void pdspevx ( ivector, irange, n, vecA, mapA, lb, ub, ilb, iub, abstol,
	       meigval, vecZ, mapZ, eval, iscratch, iscsize,
	       dblptr, ibuffsize, scratch, ssize, info)
     Integer                 *ivector, *irange, *n, *mapA, *ilb, *iub, *meigval;
     Integer                 *mapZ, *iscratch, *iscsize, *ibuffsize, *ssize, *info;
     DoublePrecision         *lb, *ub, *abstol, *eval, *scratch;
     DoublePrecision         **vecA, **vecZ, **dblptr;
{
  
/*
 *
 *  Our parallel version of LAPACK's dspevx.
 *
 *
 *  Purpose
 *  =======
 *
 *  pdspevx computes some or all of the eigenvalues and, optionally,
 *  eigenvectors of a real symmetric eigenproblem, of the form
 *
 *  A*x=(lambda) * x.
 *
 *  Here A is assumed to be symmetric.
 *  A uses packed storage.
 *
 *  Arguments
 *  =========
 *
 * The current code assumes mapA and mapZ each contain
 * exactly the same set of processor id's, though not necessarily
 * in the same order.
 *
 * All arguments are POINTERS to date of the specified type unless
 * otherwise mentioned.  In particular, INTEGER = (Integer *) and
 * DOUBLE PRECISION = (DoublePrecision *)
 *
 *
 * In the following let:
 *    n      = dimension of matrix A
 *    me     = this processors id (= mxmynd_())
 *    nprocs = number of allocated processors ( = mxnprc_())
 *    nvecsA = number of entries in mapA equal to me
 *             (= count_list( me, mapA, n ))
 *    nvecsZ = number of entries in mapZ equal to me
 *             (= count_list( me, mapZ, n ))
 *
 *-----------------------------------------------------------------
 *  ivector (input) INTEGER
 *          = 0:    Compute eigenvalues only;
 *          = 1:    Compute eigenvalues and eigenvectors.
 *
 *  irange  (input) INTEGER
 *          = 1:   all eigenvalues will be found;
 *          = 2:   all eigenvalues in the half-open interval (lb, ub]
 *                 will be found;
 *          = 3:   the ilb-th through iub-th eigenvalues will be found.
 *
 *
 *  n       (input) INTEGER
 *          The number of rows and columns of the matrices A and B.
 *          N >= 0.
 *
 *  vecA    (input/workspace) array of pointers to DoublePrecision (DoublePrecision **)
 *                            dimension ( nvecsA )
 *          On entry, vecA[i], i = 0 to nvecsA-1, points to an
 *          array containing the lower triangular part of the i-th
 *          column of A which is owned by this processor.  The
 *          columns of A owned by this processer are determined by mapA
 *          (See below).
 *
 *          On exit, the contents of matrixA are destroyed.
 *
 *  mapA    (input) INTEGER array, dimension (N)
 *          mapA(i) = the id of the processor which owns column i
 *                    of the A matrix, i = 0 to n-1.
 *
 *  lb      (input) DOUBLE PRECISION
 *          If IRANGE=2,  the lower bound of the interval to be searched
 *          for eigenvalues.  Not referenced if IRANGE = 1 or 3, but
 *          must not be a pointer to NULL.
 *
 *  ub      (input) DOUBLE PRECISION
 *          If IRANGE=2,  the upper bound of the interval to be searched
 *          for eigenvalues.  Not referenced if IRANGE = 1 or 3, but
 *          must not be a pointer to NULL.
 *
 *  ilb     (input) INTEGER
 *          If IRANGE=3,  the index (from smallest to largest) of the
 *          smallest eigenvalue to be returned.  ilb >= 1.
 *          Not referenced if IRANGE = 1 or 2, but
 *          must not be a pointer to NULL.
 *
 *  iub     (input) INTEGER
 *          If RANGE=3, the index (from smallest to largest) of the
 *          largest eigenvalue to be returned.  min(ilb,N) <= iub <= N.
 *          Not referenced if IRANGE = 1 or 2, but
 *          must not be a pointer to NULL.
 *
 *  abstol  (input) DOUBLE PRECISION
 *          The absolute error tolerance for the eigenvalues.
 *          An approximate eigenvalue is accepted as converged
 *          when it is determined to lie in an interval [a,b]
 *          of width less than or equal to
 *
 *                  ABSTOL + EPS *   max( |a|,|b| ) ,
 *
 *          where EPS is the machine precision.  If ABSTOL is less than
 *          or equal to zero, then  EPS*|T|  will be used in its place,
 *          where |T| is the 1-norm of the tridiagonal matrix obtained
 *          by reducing matrix A to tridiagonal form.
 *
 *          See "Computing Small Singular Values of Bidiagonal Matrices
 *          with Guaranteed High Relative Accuracy," by Demmel and
 *          Kahan, LAPACK Working Note #3.
 *
 *  meigval (output) INTEGER
 *          The total number of eigenvalues found.  0 <= meigval <= N.
 *          If IRANGE = 1,   M = N, and if RANGE = 3,   M = IUB-ILB+1.
 *
 *  vecZ    (output) array of pointers to DoublePrecision (DoublePrecision **)
 *                   dimension ( nvecsZ )
 *          On entry, vecZ[i], i = 0 to nvecsZ-1, should point to an
 *          array of length n.
 *
 *          On exit:
 *
 *            vecZ[i], i = 0 to nvecsZ-1, points to the i-th eigenvector
 *            (as determined by the exit values in mapZ) owned by this
 *            processor.
 *
 *            The eigenvectors are normalized such that: Z'*Z = I.
 *
 *  mapZ    (input/output) INTEGER array, dimension (N)
 *          On entry:
 *
 *          mapZ(i) = the id of a processor which has room for the
 *                    i-th eigenvector, i = 0 to n-1 (current code
 *                    requires this even when computing only some eigenvalues.
 *          On exit:
 *            mapZ(i) = the id of the processor which actually owns the i-th
 *                      eigenvector, i = 0 to n-1.
 *
 *  eval    (output) DOUBLE PRECISION array, dimension (N)
 *          If INFO = 0, the eigenvalues  of the matrix
 *          in no particular order.
 *
 *  iscratch (workspace) INTEGER array, dimension
 *           Must be >= that returned from utility routine memreq_
 *
 *  iscsize  (input) INTEGER
 *           The number of usable elements in array "iscratch".
 *           Must be >= that returned from utility routine memreq_
 *
 *  dblptr   (workspace) DoublePrecision pointer to DoublePrecision (DoublePrecision **),
 *           dblprt[0] must point to the start of an array consising of
 *           pointers to DoublePrecision (DoublePrecision *).
 *           Must be >= that returned from utility routine memreq_
 *
 *  ibuffsize(input) INTEGER
 *           The number of usable elements in array "dblptr".
 *           Must be >= that returned from utility routine memreq_
 *
 *  scratch  (workspace) DOUBLE PRECISION array, dimension
 *           Must be >= that returned from utility routine memreq_
 *
 *  ssize    (input) INTEGER
 *           The number of usable elements in array "scratch".
 *           Must be >= that returned in utility routine memreq_
 *
 *  INFO    (output) INTEGER
 *
 *          = 0:  successful exit.
 *
 *          -50 <= INFO < 0, the -INFOth argument had an illegal value.
 *
 *          INFO = -51,      Input data which must be the same on all
 *                           processors is NOT the same on all processors
 *                           in mapZ.
 *
 *          = 1,             Error computing the eigenvalues of the
 *                           tridiagonal eigenproblem.  In this case
 *                           'pstebz_' returned a non-zero info, whose
 *                           value was printed to stderr.
 *
 *          = 2,             Error computing the eigenvectors of the
 *                           tridiagonal eigenproblem.  In this case
 *                           'pstein' returned a non-zero info, whose
 *                           value was printed to stderr.
 *
 *          Any processor with a negative INFO stops program execution.
 *
 *          If -50 <= INFO < 0, then bad data was passed to this routine
 *                              and no attempt is made to make sure that
 *                              INFO is the same on all processors.
 *        
 *          All other INFO      INFO should be the same on all processors in
 *                              mapA,Z.  If this routine calls a routine
 *                              which returns a negative info, then info
 *                              may not be the same on all processors.
 *                              This, however, should never happen.
 *---------------------------------------------------------------------
 */

/*
 * Local variables
 * ---------------
 */
    static Integer  IONE = 1;
    static Integer  ITWO = 2;

    Integer         iii, k, me, jndx, nn_proc, indx, msize, nsplit, neigval,
                    nproc, itmp,
                    isize, nZ2,
                    mapZ_0, nvecsQ, nvecsA, nvecsZ, nvecsZ2,
                    linfo, maxinfo, i, j, bb1, bn ;

    Integer         *i_scrat, *mapQ, *iblock,ii, *isplit;

    char            msg[ 35 ];
    char            msg2[ 30]; 

    Integer         **iptr, num_procs, *proclist;
    
    DoublePrecision vec[2], fnormA, fnormT, ulp;
    DoublePrecision *d_scrat, *ld, *lld, dummy, dummy1,
      *dd,                    /* diagonal of tridiagonal */
      *ee,                    /* lower diagonal of tridiagonal */
      *matrixQ,
      *dplus,                 /* diagonal from bidiagonal of tridiagonal */
      *lplus,                 /* lower diagonal from bidiagonal */
      *dptr;
    
    DoublePrecision **buff_ptr, **vecQ, res, syncco[1];

#ifdef TIMING
    extern DoublePrecision mxclock_();
    extern TIMINGG test_timing;
    DoublePrecision  t1, t2, tt1;
#endif



/*
 * External procedures
 * -------------------
 */
    extern Integer  mxmynd_(), mxnprc_();

    extern Integer  mapchk_(), count_list();
    extern void     memreq_();
    extern void     pdiff(), xstop_(), pgexit(), mapdif_(), reduce_maps_();
    extern void     mdiff1_(), mdiff2_(), bbcast00();
    extern void     mem_cpy();
    extern void     dshellsort_(), sorteig();

    extern DoublePrecision dnrm2_();

    extern Integer  tred2();
    extern void     pstebz_(), mxm25(), sfnorm(), pstein4(), pstein5(), pscale_();
	extern void synch_();

/*
 *  ---------------------------------------------------------------
 *                      Executable Statements
 *  ---------------------------------------------------------------
 */

   /*
    *  Get this processor's id, and the number of allocated nodes.
    */

#ifdef TIMING
    test_timing.choleski = 0.0e0;
    test_timing.inverse  = 0.0e0;
    test_timing.conjug  = 0.0e0;
    test_timing.householder  = 0.0e0;
    test_timing.pstebz  = 0.0e0;
    test_timing.pstein  = 0.0e0;
    test_timing.mxm5x  = 0.0e0;
    test_timing.mxm25  = 0.0e0;
    test_timing.pdspevx  = 0.0e0;
    test_timing.pdspgvx  = 0.0e0;
#endif


    me    = mxmynd_();
    nproc = mxnprc_();
    strcpy( msg,  "Error in pdspevx." );
/*
setdbg_(&peigs_DEBUG);
*/

    
   /*
    *     Test the input parameters.
    */

   linfo = 0;

   if ( ivector == NULL )
      linfo = -1;
   else if ( irange == NULL )
      linfo = -2;
   else if ( n == NULL )
     linfo = -3;
   else if ( vecA == NULL )
     linfo = -4;
   else if ( mapA == NULL )
     linfo = -5;
   else if ( lb == NULL )
     linfo = -6;
   else if ( ub == NULL )
     linfo = -7;
   else if ( ilb == NULL )
      linfo = -8;
   else if ( iub == NULL )
      linfo = -9;
   else if ( abstol == NULL )
      linfo = -10;
   else if ( meigval == NULL )
      linfo = -11;
   else if ( vecZ == NULL )
      linfo = -12;
   else if ( mapZ == NULL )
      linfo = -13;
   else if ( eval == NULL )
      linfo = -14;
   else if ( iscratch == NULL )
      linfo = -15;
   else if ( iscsize == NULL )
      linfo = -16;
   else if ( dblptr == NULL )
      linfo = -17;
   else if ( ibuffsize == NULL )
      linfo = -18;
   else if ( scratch == NULL )
      linfo = -19;
   else if ( ssize == NULL )
      linfo = -20;
   else if ( info == NULL )
      linfo = -21;

   if ( linfo != 0 ) {
     if ( info != NULL )
        *info = linfo;

     fprintf( stderr, " %s me = %d argument %d is a pointer to NULL. \n",
              msg, me, -linfo );
     xstop_( &linfo );
     return;
   }

    msize   = *n;
   *info    = 0;
   *meigval = 0;


   /*
    *  Quick return if possible.
    */

   if ( *n == 0 )
      return;

   /*
    *  Continue error checking.
    */

   if ( *ivector < 0  || *ivector > 1 )
      *info = -1;
   else if ( *irange < 1  || *irange > 3 )
      *info = -2;
   else if ( *n < 0 )
      *info = -3;
   else if ( *irange == 2  && *lb >= *ub )
      *info = -7;
   else if ( *irange == 3  && *ilb < 1 )
      *info = -8;
   else if ( *irange == 3  && ( *iub < *ilb  ||  *iub > *n ) )
      *info = -9;
   else if( mapchk_( n, mapA ) != 0 )
     *info = -5;
   else if( mapchk_( n, mapZ ) != 0 )
     *info = -13;
    
   if ( *info != 0 ) {
       linfo = *info;
       fprintf( stderr, " %s me = %d argument %d has an illegal value. \n",
                msg, me, -linfo);
       xstop_( info );
       return;
   }

   /*
    * Count the number of columns of A and Z owned by this processor.
    * Must own something.
    */

    nvecsA = count_list( me, mapA, &msize );
    nvecsZ = count_list( me, mapZ, &msize );

    if ( nvecsA + nvecsZ <= 0 )
      return;

    for ( k = 0; k < nvecsZ; k++ )
      if ( vecZ[ k ] == NULL )
        *info = -12;
	 
    for ( k = 0; k < nvecsA; k++ )
      if ( vecA[ k ] == NULL )
        *info = -4;
    
   if ( *info != 0 ) {
       linfo = *info;
       fprintf( stderr, " %s me = %d argument %d contains a pointer to NULL. \n",
                msg, me, -linfo);
       xstop_( info );
       return;
   }
    
   /*
    *  REQUIRE mapA, mapZ to contain exactly the same
    *  set of processors, but not necessarily in the same order.
    */
    
   mapdif_( n, mapA, mapZ, iscratch, &linfo );
   if ( linfo != 0 )
     *info = -13;
    
   if ( *info != 0 ) {
       fprintf( stderr, " %s me = %d mapA,Z differ \n", msg, me );
       xstop_( info );
       return;
   }
    
   /*
    * check memory allocation
    * i is the integer scratch size
    * j is the DoublePrecision precision scratch size
    * k is the pointer scratch size
    */
    
   itmp = 1;
   memreq_( &itmp, n, mapA, mapZ, mapZ, &i, &j, &k, iscratch );
  
   linfo = 0;
   if ( *iscsize < i )
     linfo = -16;
   else if ( *ibuffsize < k )
     linfo = -18;
   else if ( *ssize < j )
     linfo = -20;
   
   if ( linfo != 0 ) {
     *info = linfo;
     fprintf( stderr, " %s me = %d Insufficient workspace provided. \n", msg, me );
     xstop_( info );
     return;
   }
    
    /*
     *  ------------------------------------------------
     *  No local errors, compare data across processors.
     *  ------------------------------------------------
     */
    
    /*
     *  Reduce mapA and mapZ to a single sorted list of processors.
     */
    
#ifdef DEBUG7
    printf(" in pdspevx me = %d \n", mxmynd_());
#endif
    
    proclist = iscratch;
    reduce_maps( *n, mapA, *n, mapZ, 0, mapZ, &nn_proc, proclist );

    i_scrat = iscratch + nn_proc;

    /*
     *  Check scaler Integer inputs.
     */
    
    i_scrat[ 0 ] = *ivector;
    i_scrat[ 1 ] = *irange;
    i_scrat[ 2 ] = *n;
    i_scrat[ 3 ] = *ilb;
    i_scrat[ 4 ] = *iub;
    
    isize = 5 * sizeof( Integer );
    strcpy(msg2, "ivector,irange,n,ilb,or iub\n");
    pdiff( &isize, (char *) i_scrat, proclist, &nn_proc, i_scrat+5, msg, msg2, &linfo );
    
    pgexit( &linfo, msg, proclist, &nn_proc, scratch );
    
    if ( linfo != 0 ) {
      *info = -51;
      return;
    }

    /*
     *  Check other inputs.
     */
    
    maxinfo = 0;

    isize   = msize * sizeof( Integer );
    strcpy(msg2, "mapA ");
    pdiff( &isize, (char *) mapA, proclist, &nn_proc, i_scrat, msg, msg2, &linfo );
    

    maxinfo = max( maxinfo, linfo );
    
    strcpy(msg2, "mapZ ");
    pdiff( &isize, (char *) mapZ, proclist, &nn_proc, i_scrat, msg, msg2, &linfo );


    maxinfo = max( maxinfo, linfo );


    
    *scratch       = *lb;
    *(scratch + 1) = *ub;
    *(scratch + 2) = *abstol;
    
    isize = 3 * sizeof( DoublePrecision );
    strcpy(msg2, "lb,ub,or abstol ");
    pdiff( &isize, (char *) scratch, proclist, &nn_proc, (Integer *) (scratch+3), msg, msg2, &linfo );


    maxinfo = max( maxinfo, linfo );
    
    linfo = maxinfo;
    
    pgexit( &linfo, msg, proclist, &nn_proc, scratch );

    
    if ( linfo != 0 ) {
      *info = -51;
      return;
   }

   /* ----------------------------------------
    * All input data is good. Start computing. 
    * ----------------------------------------
    */

    /*
     * Initialize workspace.
     */

    i_scrat = iscratch;
    d_scrat = scratch;
    buff_ptr = dblptr;

    /*
     * Assume mapA and mapQ are the same.
     */

    nvecsQ = nvecsA;

    mapQ = i_scrat;
    i_scrat += msize;

    iblock = i_scrat;
    i_scrat += msize;

    isplit = i_scrat;
    i_scrat += msize;

    matrixQ = d_scrat;
    d_scrat += nvecsQ * msize;

    dd = d_scrat;
    d_scrat += msize;

    ee = d_scrat;
    d_scrat += msize;

    dplus = d_scrat;
    d_scrat += msize;
    
    lplus = d_scrat;
    d_scrat += msize;
    
    vecQ = buff_ptr;
    buff_ptr += nvecsQ;
    
    ld = d_scrat;
    d_scrat += msize;
    lld = d_scrat;
    d_scrat += msize;
    

    iptr = (Integer **) buff_ptr;
    
    /*
     * Copy mapA to mapQ.
     */

    mem_cpy(mapA, mapQ, msize);

    /*
     * Set DoublePrecision pointers to actual data.
     */


    dptr = matrixQ;
    for (indx = 0; indx < nvecsQ; indx++){
        vecQ[indx] = dptr;
	dptr += msize;
    }

    /*
     * Reduce A to tridiagonal form.
     */

    
    
    if (nvecsA + nvecsQ > 0) {
      fnormA = 0.0;
      /*
	 if( nvecsA > 0 ){
	 sfnorm( &msize, vecA, mapA, &fnormA, i_scrat, d_scrat, &linfo);
	 }
	 */
      
#ifdef TIMING
      tt1 = t1 = mxclock_();
#endif
#ifdef DEBUG7
   printf(" in pdspevx tred2 me = %d \n", mxmynd_());
#endif

   tred2( &msize, vecA, mapA, vecQ, mapQ, dd, ee, i_scrat, d_scrat);

#ifdef DEBUG7
   printf(" in pdspevx out tred2 me = %d \n", mxmynd_());
#endif
   
   
#ifdef TIMING
   mxsync_();
   t2 = mxclock_();
   test_timing.householder = t2 - t1;
#endif
   
    }
    
    mapZ_0 = *mapZ;
    
    mdiff1_( n, mapZ, n, mapA, i_scrat, &num_procs ); 
    
    
    /*
     * Compute eigenvalues.
     */
    
    if (nvecsZ > 0){

#ifdef TIMING
      t1 = mxclock_();
#endif
#ifdef DEBUG7
   printf(" in pdspevx pstebz10 me = %d \n", mxmynd_());
#endif
      
      peigs_shift = 0.0e0;
      peigs_scale = 0.0e0;
      ee[0] = 0.0e0;
      
#ifdef PSCALE
      pscale_( irange, &msize, lb, ub, ilb, iub, abstol,
	      dd, ee, dplus, lplus,
	      mapZ, &neigval, &nsplit, eval, iblock, isplit,
	      d_scrat, i_scrat, &linfo);
#endif
      
   pstebz10_( irange, &msize, lb, ub, ilb, iub, abstol,
	      dd, ee, dplus, lplus,
	      mapZ, &neigval, &nsplit, eval, iblock, isplit,
	      d_scrat, i_scrat, &linfo);

   if ( linfo != 0 ) {
     pstebz9_( irange, &msize, lb, ub, ilb, iub, abstol, dd, ee,
	       dplus, lplus, mapZ, &neigval, &nsplit, eval, iblock, isplit,
	       d_scrat, i_scrat, info);
     
     if ( *info != 0 ) {
       for ( iii = 0 ; iii < msize; iii++ )
	 d_scrat[iii] = dd[iii];
       for (iii = 0; iii < msize; iii++)
	 d_scrat[iii+msize] = ee[iii];
       dsterf_(&msize, d_scrat, &d_scrat[msize+1], &linfo);
       for ( iii = 0; iii < msize; iii++ )
	 eval[iii] = dd[iii];
       if ( linfo != 0 ) {
	 printf(" error in peigs...dsterf me = %d \n", me);
	 printf(" error in peigs...fails all eval solver me = %d \n", me);
	 exit(-1);
       }
     }
   }
     
     
   
#ifdef DEBUG7
   printf(" out pdspevx pstebz10 me = %d \n", mxmynd_());
#endif
   
#ifdef TIMING
   mxsync_();
   t2 = mxclock_();
   test_timing.pstebz = t2 - t1;
#endif
   
   
   if ( NO_EVEC ) 
     goto END;
   
   for(indx = 0;indx < msize;indx++){
     dummy1 = lplus[indx];
     dummy = dplus[indx]*dummy1;
     ld[indx] = dummy;
     lld[indx] = dummy*dummy1;
   }
   
   
   if( *info != 0 ) {
     fprintf(stderr, " %s me = %d pstebz_ returned info = %d \n", msg, me, *info );
     *info = 1;
   }
   
    }
    
    /*
     * Send info, neigval and eval to all processors in {mapA} - {mapZ}
     */
    
    mdiff1_( n, mapA, n, mapZ, i_scrat, &num_procs ); 
    if( num_procs > 0 ){
      i_scrat[num_procs] = mapZ_0;
      num_procs++;
      ibcast00( info,     sizeof(Integer),                 2, mapZ_0, num_procs, i_scrat );
      ibcast00( (char *) &neigval, sizeof(Integer),                 3, mapZ_0, num_procs, i_scrat );
      bbcast00( (char *) eval,     neigval*sizeof(DoublePrecision), 4, mapZ_0, num_procs, i_scrat );
    }
    
    *meigval = neigval;
    
    if ( *info != 0 )
      return;
    
    /*
     * Exit if there aren't any eigenvectors to be computed.
     */
    
    if ( neigval == 0 )
      return;
    
    if ( *ivector == 0 ) {
      dshellsort_( &neigval, eval );
      return;
    }
    
    /*
     * Compute eigenvectors.
     */

    nvecsZ2 = count_list( me, mapZ, meigval );
    
#ifdef TIMING
    mxsync_();
    t1 = mxclock_();
#endif
    
    if (nvecsZ2 > 0) {
      
      /*
	do fine cluster and mgs
	*/
      
      
#ifdef DEBUG7
      printf(" me = %d just before pstein5 %d \n", me, *info );
#endif
      
      /*
	tight cluster
	*/
      
      
      /*      fprintf(stderr, "me = %d pdspevx 11 \n", me );
	      fflush(stderr);*/
      
      syncco[0] = 0.0e0;
      gsum00( (char *) syncco, 1, 5, 10, mapA[0], nn_proc, proclist, d_scrat);
      
      pstein5 ( &msize, dd, ee, dplus, lplus, ld, lld,
		&neigval, eval, iblock, &nsplit, isplit,
		mapZ, vecZ, d_scrat,i_scrat, iptr, info);

	linfo = 26001;
/*      synch_(&linfo );
*/
      syncco[0] = 0.0e0;
      gsum00( (char *) syncco, 1, 5, 11, mapA[0], nn_proc, proclist, d_scrat);

      
#ifdef DEBUG7
      printf(" me = %d just after pstein5 %d \n", me, *info );
#endif
      
      /*
	 mgs loose cluster
	 */

#ifdef DEBUG7
      printf(" me = %d just before pstein4 %d \n", me, *info );
#endif

	pstein4 ( &msize, dd, ee, dplus, lplus, ld, lld,
	&neigval, eval, iblock, &nsplit, isplit,
		  mapZ, vecZ, d_scrat,i_scrat, iptr, info);
	
	/*
	  synch_(&linfo);
	  */
	syncco[0] = 0.0e0;
	gsum00( (char *) syncco, 1, 5, 12, mapA[0], nn_proc, proclist, d_scrat);
	
	
#ifdef DEBUG7
      printf(" me = %d just after pstein4 %d \n", me, *info );
      fflush(stderr);
#endif
      
      
      if( *info != 0 ) {
        fprintf(stderr, " %s me = %d pstein returned info = %d \n", msg, me, *info );
        *info = 2;
      }
      
    }
      
#ifdef TIMING
      mxsync_();
      t2 = mxclock_();
      test_timing.pstein = t2 - t1;
#endif
    
    
      /*
       * Send info, mapZ to any processors in 
     * ({mapA} U {mapZ[neigval:*n-1]}) - {mapZ[0:neigval-1]}
     */
    
    
    nZ2 = *n - neigval;
    
    
    /*
     * Compute product of Q from tred2 and Z, the eigenvectors of the
     * tridiagonal matrix.  This gives the eigenvectors of A.
     */

    nvecsZ = count_list( me, mapZ, &neigval );

#ifdef TIMING
      t1 = mxclock_();
#endif
    
      if (nvecsQ + nvecsZ > 0) {
	mxm25 ( &msize, &msize, vecQ, mapQ, &neigval,
		vecZ, mapZ, vecZ, i_scrat, d_scrat);
      }
      syncco[0] = 0.0e0;
      gsum00( (char *) syncco, 1, 5, 14, mapA[0], nn_proc, proclist, d_scrat);
      
#ifdef TIMING
      mxsync_();
    t2 = mxclock_();
    test_timing.mxm25 = t2 - t1;
#endif
    
    
    /*
     * Sort eigenvalues, mapZ and eigenvectors
     */
    
END:
    
    for ( iii = 0; iii < neigval; iii++){
      eval[iii] += psgn*psigma;
    }
    
    sorteig(&msize, &neigval, vecZ, mapZ, eval, i_scrat, d_scrat);
    
    syncco[0] = 0.0e0;
    gsum00( (char *) syncco, 1, 5, 117, mapA[0], nn_proc, proclist, d_scrat);
    
    
#ifdef PSCALE
    dummy = peigs_scale;
    for ( iii=0; iii < neigval; iii++ )
      eval[iii] = peigs_shift + eval[iii]*dummy;
#endif
    
    
#ifdef DEBUG7
    fprintf(stderr, "me = %d Exiting pdspevx \n", me );
#endif
    
#ifdef TIMING
    t2 = mxclock_();
    test_timing.pdspevx = t2 - tt1;
    if (me == 0 ){
      fprintf(stderr, " n = %d nprocs = %d \n", msize, nproc);
      fprintf(stderr, " pdspevx = %f \n", test_timing.pdspevx);
      fprintf(stderr, " choleski = %f \n", test_timing.choleski);
      fprintf(stderr, " inverse = %f \n", test_timing.inverse);
      fprintf(stderr, " conjug = %f \n", test_timing.conjug);
      fprintf(stderr, " householder = %f \n", test_timing.householder);
      fprintf(stderr, " mxm5x = %f \n", test_timing.mxm5x);
      fprintf(stderr, " mxm25 = %f \n", test_timing.mxm25);
      fprintf(stderr, " pstein = %f \n", test_timing.pstein);
      fprintf(stderr, " pstebz = %f \n", test_timing.pstebz);
    }
#endif
    return;
  }





