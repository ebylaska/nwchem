       INTEGER FUNCTION LNBLNK (STRING)
C:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
C Purpose:   Returns the position of the last non-blank character
C
C Arguments: STRING   character string (input only)
C
C Remarks:   All FORTRAN 77 character variables are blank padded on the
C            right.  The intrinsic function LEN returns the dimension
C            of the character object, not the length of the contents.
C            
C:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
C $Header: /tmp/mss/nwchem/src/rimp2/lnblnk.f,v 1.2 1994-09-01 21:07:41 d3e129 Exp $
C
C    Revision 0.0  87/07/24  bernholdt (VAX)
C $Log: not supported by cvs2svn $
c Revision 1.1  1994/06/14  21:54:19  gg502
c First cut at RIMP2.
c
c Revision 1.1  91/08/26  23:11:19  bernhold
c Initial revision
c 
C    Revision 1.1  88/01/11  22:08:15  bernhold
C    Initial revision
C    
C
C System:     Standard FORTRAN 77
C
C Copyright 1987 David E. Bernholdt
C:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
       INTEGER I
       CHARACTER*(*) STRING
       CHARACTER*1 BLANK, NULL
       PARAMETER (BLANK = ' ')
C
C      Start at the end and work backwards
C
       NULL=CHAR(0)
       DO 100 I = LEN(STRING), 1, -1
C         Look for first non-whitespace character
          IF (STRING(I:I) .NE. BLANK .AND. String(I:I) .ne. NULL) THEN
             LNBLNK = I
             RETURN
          ENDIF
  100  CONTINUE
C
C      If we get this far, the string is empty
       LNBLNK = 0
       RETURN
       END
