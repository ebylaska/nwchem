      subroutine dplot_dump(lgaussian,out_unit,iproc,luout,
     ,     where,what,title,
     ,     natom,xyz,charge,volume,tol_rho,
     ,     coofrom,cooto,no_of_spacings,ngrid,values,atag)
      implicit none
      logical lgaussian
      character *(*) title
      character*8 where,what                ! [input]
      integer out_unit ! [input]
      double precision xyz(3,*),charge(*)
      integer natom
      integer no_of_spacings(3),ngrid
      integer iproc,luout
      double precision values(*),volume,coofrom(3),cooto(3)
      character*16 atag(*)
c
      double precision spread(3),step(3),angle(3),tol_rho
      integer const_m(3),const_p(3)
c
      integer i,ii,ifast,atomicno
      double precision appch,sum
      character*16 element
      character*2 symbol
      logical geom_tag_to_element
      external geom_tag_to_element
c
      appch=0d0
      call  dplot_defgrid(volume,angle,
     ,     spread,step,const_m,const_p,
     ,     cooto,coofrom,no_of_spacings)
c
      If (iProc.eq.0 . and . Where.ne.'NUCLEI') Then
         if(lgaussian) then
            Write(Out_Unit,*)"Cube file generated by NWChem"
            Write(Out_Unit,*) Title
         else
            Write(Out_Unit,*)Title
            Write(Out_Unit,115)
 115        format('(1F15.10)')
         endif
      End If
      If (iProc.eq.0 . and . Where.ne.'NUCLEI') Then
         if(lgaussian) then
 9498       format(I5,3F12.6)
            Write(Out_Unit,9498)
     &           natom,(coofrom(i),i=1,3)
            Write(Out_Unit,9498)
     &           no_of_spacings(1)+1,step(1),0d0,0d0
            Write(Out_Unit,9498)
     &           no_of_spacings(2)+1,0d0,step(2),0d0
            Write(Out_Unit,9498)
     &           no_of_spacings(3)+1,0d0,0d0,step(3)
            do i=1,natom
            if (.not. geom_tag_to_element(atag(i), symbol, 
     &         element, atomicno)) then
               if (symbol .ne. 'bq') call errquit
     &              ('dplotdump: center is neither atom nor bq',
     &              0,0)
            endif
            Write(Out_Unit,9499)
     .            atomicno,charge(i),xyz(1,i),xyz(2,i),xyz(3,i)
 9499       format(I5,4F12.6)
c
c     dump MOlist (if any)
c
         enddo
         else
            Write(Out_Unit,'(3(1x,f10.5),3(3x,f5.2))')
     &           (Spread(i),i=1,3),(Angle(i),i=1,3)
            Write(Out_Unit,'(3(2x,i4))')(no_of_spacings(i),i=1,3)
            iFast = 1
            Write(Out_Unit,'(5x,i1,6(2x,i4))')
     &           iFast,(Const_M(i),Const_P(i),i=1,3)
         endif
      End If
c
c     dump grid values
c
      If (iProc.eq.0 . and . Where.ne.'NUCLEI') Then
c
         if(lgaussian) then ! for cube files
c
c     truncation
c
            Do i = 1, nGrid
               if(abs(values(i)).lt.tol_rho) values(i)=0d0
            enddo
            Do i = 1, nGrid,No_Of_Spacings(3)+1
               Write(Out_Unit,99498)(values(i+ii),ii=0,
     .              No_Of_Spacings(3))
99498          format(6E13.5)
            enddo
            else
            Do i = 1, nGrid
               Write(Out_Unit,'(f15.10)')values(i)
            End Do
         endif
         If (What.eq.'VIEW') Then
            write(Out_unit,19498)
19498       format(10I5)
         endif
c
         If (What.eq.'DENSITY') Then
            Sum = 0.d0
            Do i = 1, nGrid
               Sum = Sum + values(i)
            End Do
            AppCh = Sum*Volume
            Write(LuOut,*)
            Write(LuOut,'(a,f30.5)')'  Sum of elements      = ',sum
            Write(LuOut,'(a,f30.5)')'  Integration volume   = ',volume
            Write(LuOut,'(a,f30.5)')'  Integrated Charge    = ',AppCh
         End If
      End If
      return
      end
c $Id$
