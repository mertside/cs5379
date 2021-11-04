/*** This is a parallelization of floyd algirthm. ***
 *** The code is not guaranteed bug-free.         ***/


/*  D[][], D0[][] are arrays of size (n/sqrt(np)) x (n/sqrt(np))  *
 *  pid is process id, and np is number of processes              */

void pfloyd_2D(int n, int **D0, int **D, int pid, int np) 
{
    int  i, j, k, sqrtp, count, pidx, pidy, *hprocs, *vprocs,
    MPI_Group MPI_GROUP_WORLD, hgroup, vgroup ;
    MPI_Comm hcomm, vcomm ;

    sqrtp = (int) sqrt(np) ;
    count = n/sqrtp ;
    pidx = pid%sqrtp ;
    pidy = pid/sqrtp ;

/*** Creat horizontal and vertical communicators - hcomm, vcomm ***/
    hprocs = (int *) calloc(sqrtp, sizeof(int) ) ;
    vprocs = (int *) calloc(sqrtp, sizeof(int) ) ;
    for(i=0; i<sqrtp; i++) {
        hprocs[i] = i + pidy*sqrtp ;
        vprocs[i] = pidx + i*sqrtp ;
    }

    MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
    MPI_Group_incl(MPI_GROUP_WORLD, sqrtp, hprocs, &hgroup);
    MPI_Group_incl(MPI_GROUP_WORLD, sqrtp, vprocs, &vgroup);
    MPI_Comm_create(MPI_COMM_WORLD, hgroup, &hcomm);
    MPI_Comm_create(MPI_COMM_WORLD, vgroup, &vcomm);

    MPI_Group_free(&MPI_GROUP_WORLD);
    MPI_Group_free(&hgroup);
    MPI_Group_free(&vgroup);
    free(hprocs); 
    free(vprocs);
/*** End of creating horizontal & vertical communicators ***/


    int  k_local, hroot, vroot ;
    int  *hbuf, *vbuf ;

    hbuf = (int *)calloc(count, sizeof(int));
    vbuf = (int *)calloc(count, sizeof(int));

    for(k=0; k<n; k++) {
       h_root= k/count ;
       v_root = h_root;
       k_local = k%count ;

       if(pidx==h_root) {
           for(i=0; i<count; i++) hbuf[i] = D0[i][k_local] ;
       }
       MPI_Bcast(hbuf, count, MPI_INT, h_root, hcomm) ;

       if(pidy==v_root) {
           for(j=0; j<count; j++) vbuf[j] = D0[k_local][j] ;
       }
       MPI_Bcast(vbuf, count, MPI_INT, v_root, vcomm) ;

       for(i=0; i<count; i++)
           for(j=0; j<count; j++) {
                D[i][j] = min(D0[i][j], hbuf[i]+vbuf[j]) ;
                D0[i][j] = D[i][j] ;
           }
    }

    free(hbuf); free(vbuf);
} /**************** End of function pfloyd_2D() ******************/

