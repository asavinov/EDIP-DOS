
CPU_88 = 1

            PUBLIC  _pastl_88
            PUBLIC  _cutout_sec_88

cseg        SEGMENT PARA PUBLIC 'CODE'
            ASSUME  cs:cseg

;____________________________________________________________________________
;   prototype:   unsigned int pastl_88( int huge *matr_ptr,
;                                       int huge *last_ptr,
;                                       int a_num,
;                                       unsigned int m_num );
;   synopsis     matr_ptr - pointer to the matrix
;                last_ptr - pointer to the last matrix line
;                a_num    - the attributes number
;                m_num    - the lines number in matrix
;                return   - 0 if there is no result, else 1
;   name         _pastl_88 - pastes last line with one of the previous lines
;   description  This procedure takes 4 parameters from stack
;                sp+14- m_num       string number in matrix
;                sp+12- a_num       attributes (sections) number
;                sp+10- last_ptr    (segment) huge last line
;                sp+8 - last_ptr    (offset)  address
;                sp+6 - matr_ptr    (segment) huge starting address
;                sp+4 - matr_ptr    (offset)  of the matrix
;                sp+2 -             (segment) far return
;                sp+0 -             (offset)  address
;
;                This procedure takes last line in the matrix pointed to by
;                last_ptr and tries to paste it with one of the previous
;                lines starting from the begining of the matrix (line number
;                1 in manrix). The result is tested on the absorbtion.
;                If the result is absorbed, then the process is continued,
;                else 1 is returned. If there is no more lines to paste with
;                the last line (that is the end of the matrix was
;                encountered), then 0 is returned.
;____________________________________________________________________________
_pastl_88   PROC    far                ;entry point
            jmp     start              ;skip through data
m_num       DW      0                  ;local variables and parameters
a_num       DW      0                    ;
l_len       DW      0                    ;
matr_ptr    DD      0                    ;
last_ptr    DD      0

start:
           ;save registers
            push    bp
					  push    si
						push    di
						push    ds

           ;copy all parameters in local area
            mov     bp, sp             ;
            add     bp, 8              ;skip through 0 saved registers

           ;load matr_ptr in es:bp and save it
            mov     ax, [bp+4]                        ;offset matr_ptr
            mov     WORD PTR matr_ptr,ax
            mov     ax, [bp+6]                        ;segment matr_ptr
            mov     WORD PTR matr_ptr+2, ax

           ;load last_ptr in ds:bx and save it
            mov     ax, [bp+8]                        ;offset last_ptr
            mov     WORD PTR last_ptr,ax
            mov     ax, [bp+10]                       ;segment last_ptr
            mov     WORD PTR last_ptr+2, ax

           ;a_num parameter
            mov     ax, [bp+12]                       
            mov     a_num, ax
            shl     ax, 1                             ;2 * a_num
            mov     l_len, ax                         ;initialize l_len

           ;m_num parameter
            mov     ax, [bp+14]                       
            mov     m_num, ax
            mov     cx, ax
            dec     cx
            les     bp, matr_ptr
            lds     bx, last_ptr
line_loop:
           ;go to the next line
            add     bp, l_len		       ;add with line length
           ;convert es:bp to huge pointer
            mov     dx, bp
            and     bp, 0fh
IF CPU_88   
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
ELSE        
            shr     dx, 4
ENDIF
            mov     ax, es
            add     ax, dx
            mov     es, ax

            push    cx
            mov     cx, a_num
            sub     si, si
	section_loop:
	           ;try to paste two sections pointed to by si
  	          mov     dx, [bx+si]        ;take last line
    	        mov     di, es:[bp+si]     ;take current line section
      	      mov     ax, dx
        	    and     ax, di
          	  cmp     ax, dx
            	je      not_pastl1
	            cmp     ax, di
  	          je      not_pastl1

              mov     di, l_len
      	     ;save pasted section
              add     bx, di
	            mov     [bx+si], ax
              sub     bx, di

             ;begin pasting the resting sections
        	    push    cx
              mov     dx, si
          	  mov     cx, di
              shr     cx, 1                           ; cx = a_num
	            sub     si, si
		pastl_loop:
		            cmp     si, dx                        ;is it AND sec?
    		        je      continue           ;yes, skip over it
        		    mov     ax, [bx+si]        ;no, take last line section
            		or      ax, es:[bp+si]
		           ;compare result in ax with mask in zero line
    		        push    ds
        		    push    bx
            		lds     bx, matr_ptr
		            cmp     ax, [bx+si]
    		        pop     bx
        		    pop     ds
            		je      not_pastl          ;if not pasted (zf=1)
		           ;save this section
                add     bx, di
  		          mov     [bx+si], ax
                sub     bx, di
		        continue:
    		        add     si, 2              ;go to the next section
        		    loop    pastl_loop

              call absorb_proc

    not_pastl:
              mov     si, dx
              pop     cx
    not_pastl1:
              add     si, 2
              loop    section_loop

            pop     cx
            loop    line_loop

         ;restore registers
          pop     ds
          pop     di
          pop     si
          pop     bp
          sub     ax, ax
          ret
  success:
  			 ;correct stack pointer
				  add     sp, 12
         ;restore registers
          pop     ds
          pop     di
          pop     si
          pop     bp
          mov     ax, 1
          ret

_pastl_88   ENDP

;____________________________________________________________________________
; absorb_proc this procedure saves registers es:bp and dx in stack,
;             initializes ds:bx to point on the resulting line,
;             es:bp points on current line to test.
;             if the resulting line is absorbed, then nothing is done,
;             else jump to success is done (and 1 returned by pastl_88).
;             Register di must contain l_len.
;____________________________________________________________________________
absorb_proc PROC    near
            push    es
            push    bp
            push    dx

           ;initialize pointer to resulting line in ds:bx
            add     bx, di
           ;initialize pointer to the zero line in es:bp
            les     bp, matr_ptr
           ;initialize lines counter in cx
            mov     cx, m_num
absb_loop:
           ;go to the next line
            add     bp, di             
           ;convert es:bp to huge pointer
            mov     dx, bp
            and     bp, 0fh
IF CPU_88   
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
ELSE        
            shr     dx, 4
ENDIF
            mov     ax, es
            add     ax, dx
            mov     es, ax

            mov     dx, cx             ;save lines counter
            mov     cx, di             ;initialize cx = a_num
            shr     cx, 1
            mov     si, 0-2
sec_loop:
            add     si, 2              ;go to the next section
            mov     ax, [bx+si]        ;take last section
            not     ax
            test    ax, es:[bp+si]
            loopz   sec_loop           ;zf=1 - continue, zf=0 - break loop
            mov     cx, dx             ;restore lines counter
            loopnz  absb_loop          ;zf=1 - break, zf=0 - continue loop

           ;if new line is not absorbed, then return 1
            jnz     success

            sub     bx, di
            pop     dx
            pop     bp
            pop     es

            ret
absorb_proc ENDP

;____________________________________________________________________________
;   prototype:   unsigned int cutout_end_88( int huge* line_ptr,
;                                            unsigned int l_num,
;                                            unsigned int a_num,
;                                            unsigned int m_num );
;   synopsis     line_ptr - pointer to the line number l_num in matrix;
;                l_num    - the number of the line to cut out;
;                a_num    - the attributes number;
;                m_num    - the lines number in matrix;
;                return   - the new lines number in matrix if the line l_num
;                           is absorbed and cut out (that is m_num - 1) and
;                           0 if it is not absorbed;
;   name         _cutout_end_88 - tries to absorb the line number l_num
;                           pointed to by line_ptr considering only the
;                           from the higher part of the matrix (that is
;                           with the numbers from l_num+1 till m_num);
;                           if the line is absorbed it is cut out from
;                           the matrix; if the absorbtion fails nothing
;                           is done and 0 returned;
;   description  This procedure takes 4 parameters from stack
;                sp+12- m_num       lines number in matrix
;                sp+10- a_num       attributes (sections) number
;                sp+8 - l_num       the number of the line to cut out;
;                sp+6 - line_ptr    (segment) far line address
;                sp+4 - line_ptr    (offset)  to cut out
;                sp+2 -             (segment) far return
;                sp+0 -             (offset)  address
;                At the begining the procedure tries to absorb the line
;                l_num by the lines with the numbers from l_num+1 till
;                m_num. If the line is absorbed then it is cut out from
;                the matrix, that is the higher part of the matrix is
;                shifted on one line below without changing lines order.
;                In this case the new lines number in matrix is returned
;                (that is m_num - 1). If the line is not absorbed then
;                0 is returned.
;____________________________________________________________________________
_cutout_sec_88   PROC    far           ;entry point
            jmp     begin              ;skip through data

loop_num    DW      0
m_lines     DW      0

begin:
           ;save registers
					  push    si
            push    di
            push    ds
            push    bp
           ;take parameters from stack
            mov     bp, sp
            add     bp, 8
            mov     ax, [bp+4]         ;take offset line_ptr
						mov     di, ax
						mov     si, ax
						mov     ax, [bp+6]         ;take segment line_ptr
            mov     es, ax
            mov     ds, ax
            mov     cx, [bp+12]        ;take m_num
            mov     m_lines, cx          ;save m_num
            sub     cx, [bp+8]         ;cx = m_num - l_num
            jz      not_cutout
            mov     loop_num, cx
            mov     bp, [bp+10]        ;take a_num
            shl     bp, 1              ;l_len = 2 * a_num

absorb_loop:
               ;go to the next source line ds:si
							  add si, bp
							 ;convert ds:si to huge pointer
							  mov dx, si
								and si, 0fh
IF CPU_88
                shr dx, 1
                shr dx, 1
                shr dx, 1
                shr dx, 1
ELSE
                shr dx, 4
ENDIF
                mov ax, ds
                add ax, dx
                mov ds, ax
               ;initialize section pointer
                mov bx, 0-2
               ;save lines counter and initialize section counter
                mov dx, cx
                mov cx, bp             ;cx = l_len
                shr cx, 1              ;cx = a_num
sect_loop:
                    add  bx, 2
                    mov  ax, es:[di+bx]
                    not  ax
                    test ax, [si+bx]
                    loopz sect_loop    ;zf=1 - continue, zf=0 - break loop
                mov cx, dx
                loopnz  absorb_loop    ;zf=1 - break, zf=0 - continue loop
            jnz not_cutout

           ;load in ds:si line_ptr
            mov     si, di
            mov     ax, es
            mov     ds, ax

            add     si, bp
					 ;convert ds:si to huge pointer
					  mov     dx, si
						and     si, 0fh
IF CPU_88
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
            shr     dx, 1
ELSE
            shr     dx, 4
ENDIF
            mov     ax, ds
            add     ax, dx
            mov     ds, ax
           ;initialize bp = a_num
            shr     bp, 1
           ;initialize lines counter to copy
            mov     cx, loop_num
copy_loop:
                mov   dx, cx
                mov   cx, bp
                cld
            rep movsw
                mov   cx, dx
								loop copy_loop

           ;restore registers 
            pop     bp
            pop     ds
            pop     di
					  pop     si
            mov     ax, m_lines
            dec     ax
            ret
not_cutout:
           ;restore registers
            pop     bp
            pop     ds
            pop     di
					  pop     si
            sub     ax, ax
            ret

_cutout_sec_88  ENDP

cseg        ENDS
						END