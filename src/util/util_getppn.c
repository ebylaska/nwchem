/* $Id$ */
/* computes the number of processes per node a.k.a ppn */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include "ga.h"
#include "typesf2c.h"
#define SIZE_GROUP 400
static short int ppn_initialized=0;
static int ppn=0;
void FATR util_getppn_(Integer *ppn_out){

  const int mxlen = 255;
  char myhostname[mxlen];
  char* recvbuf;
  int i, num_procs, me,  err, modppn;
  int size_group=SIZE_GROUP;
  MPI_Group wgroup_handle,group_handle;
  MPI_Comm group_comm;
  int ranks[SIZE_GROUP];
  
  if(ppn_initialized) {
    *ppn_out = (long) ppn;
    
  }else{
    num_procs = GA_Nnodes();
    me = GA_Nodeid();
    
    if(size_group> num_procs) size_group=num_procs;
    
    /*get world group handle to be used later */
    err=MPI_Comm_group(MPI_COMM_WORLD, &wgroup_handle);
    if (err != MPI_SUCCESS) {
      fprintf(stderr,"util_getppn: MPI_Comm_group failed\n");
      *ppn_out=0;
      return;
    }
    
    for (i=0; i< size_group; i++) ranks[i]=i;
    
    /* create new group of size size_group */
    err=MPI_Group_incl(wgroup_handle, size_group, ranks, &group_handle);
    if (err != MPI_SUCCESS) {
      fprintf(stdout,"util_getppn: MPI_Group_incl failed\n");
      *ppn_out=0;
      return;
    }
    
    /* Create new new communicator for the newly created group */
    err=MPI_Comm_create(MPI_COMM_WORLD, group_handle, &group_comm);
    if (err != MPI_SUCCESS) {
      fprintf(stdout,"util_getppn: MPI_Comm_group failed\n");
      *ppn_out=0;
      return;
    }
    
    
    if(me < size_group) {
      recvbuf=(char*)malloc(size_group*(mxlen+1)*(sizeof(char)));
      
      err=gethostname(myhostname, sizeof(myhostname) );
      if (err != 0) {
	fprintf(stdout,"util_getppn: gethostname failed\n");
	ppn=0;
	goto errlab;
      }
      
      
      err=MPI_Allgather(myhostname, mxlen, MPI_CHAR, recvbuf, mxlen, MPI_CHAR, group_comm);
      if (err != MPI_SUCCESS) {
	fprintf(stdout,"util_getppn: MPI_Allgather failed\n");
	ppn=0;
	goto errlab;
      }
      
      
      for (i=0; i< size_group; i++){
	if(strcmp(myhostname,&recvbuf[mxlen*i])==0) ppn++;
      }
      
      /*	  free malloc'ed memory */
      free(recvbuf);
      /*flush group and comm*/
      err=MPI_Group_free(&group_handle);
      if (err != MPI_SUCCESS) {
	fprintf(stdout,"util_getppn: MPI_Group_free failed\n");
	ppn=0;
	goto errlab;
      }
      
      err=MPI_Comm_free(&group_comm);
      if (err != MPI_SUCCESS) {
	fprintf(stdout,"util_getppn: MPI_Comm_free failed\n");
	ppn=0;
	goto errlab;
      }
      
    }
    /* back to world comm  -- i hope */
    /* broadcast ppn to everybody */
    err= MPI_Bcast(&ppn, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS) {
      fprintf(stdout,"util_getppn: MPI_Bcast failed\n");
      goto errlab;
    }
    

    /* check that computed ppn is a submultiple of num procs */
    
    modppn = num_procs%ppn;
    if (modppn ==0){
      ppn_initialized=1;
      *ppn_out = (long) ppn;
      return;
    }else{
      printf(" ERROR: numprocs %d  ppn %d  mod %d\n", num_procs, ppn,  modppn);
      goto errlab;
    }
  errlab:
    GA_Error(" util_getppn failure", 0);
  }
} 

/* C binding for util_ppn */
int util_cgetppn(){
  Integer* ppn_out = malloc(sizeof(Integer));
  int ppn;
  util_getppn_(ppn_out);
  ppn = (int ) *ppn_out;
  fflush(stdout);
  return ppn;
}



int util_my_smp_index(){
  int ppn= util_cgetppn();
  return GA_Nodeid()%ppn;
}

int util_my_smp_master(){
  int ppn= util_cgetppn();
  return (GA_Nodeid()/ppn)*ppn;
}

