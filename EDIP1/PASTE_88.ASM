
CPU_88 = 1

            PUBLIC  _paste_88
            PUBLIC  _cutout_sec_88
            EXTRN   _output_info : far

cseg        SEGMENT PARA PUBLIC 'CODE'
            ASSUME  cs:cseg

;____________________________________________________________________________
;   prototype:   unsigned int paste_88( int huge* matr_ptr,
;                                       unsigned int a_num,
;                                       unsigned int n_num,
;                                       unsigned int m_num );
;   synopsis     matr_ptr - pointer to the matrix
;                a_num    - the attributes number
;                n_num    - the lines number already pasted
;                m_num    - the lines number in matrix
;                return   - the lines number in resulting matrix
;   name         _paste_88 - pastes all disjuncts in matrix for one pass
;   description  This procedure takes 4 parameters from stack
;                sp+12- m_num       string number in matrix
;                sp+10- n_num       string number that already are pasted
;                sp+8 - a_num       attributes (sections) number
;                sp+6 - matr_ptr    (segment) far starting address
;                sp+4 - matr_ptr    (offset)  of the matrix
;                sp+2 -             (segment) far return
;                sp+0 -             (offset)  address
;                This function makes one pass through the matrix pointed to
;                by matr_ptr and pastes all m_num disjuncts excepting the 
;                first n_num with each other. Every new line is tested to 
;                know if it is absorbed by the all previous lines.
;____________________________________________________________________________
_paste_88   PROC    far                ;entry point
            jmp     start              ;skip through data
m_num       DW      0                  ;local variables and parameters
n_num       DW      0
r_num       DW      0                    ;
a_num       DW      0                    ;
l_len       DW      0                    ;
slow        DW      0                    ;
matr_ptr    DD      0                    ;
res_ptr     DD      0                    ;pointer to current resulting line
slow_ptr    DD      0
reg_si      DW      0
reg_di      DW      0
reg_ds      DW      0
reg_cx      DW      0

start:
           ;save registers 
            mov     reg_si, si         ;
            mov     reg_di, di         ;
            mov     ax, ds             ;
            mov     reg_ds, ax         ;

           ;copy all parameters in local area
            mov     bp, sp             ;
            add     bp, 0              ;skip through 0 saved registers
            mov     ax, [bp+4]         ;take offset matr_ptr
            mov     WORD PTR matr_ptr,ax  ;and save it
            mov     ax, [bp+6]         ;take segment matr_ptr
            mov     WORD PTR matr_ptr+2, ax      ;and save it
            mov     ax, [bp+8]         ;take a_num parameter
            mov     a_num, ax          ;and save it
            shl     ax, 1              ;2 * a_num
            mov     l_len, ax          ;initialize l_len
            mov     ax, [bp+10]        ;take n_num parameter
            mov     n_num, ax          ;and save it
            inc     ax
            mov     slow, ax           ;initialize slow n_num + 1
            mov     ax, [bp+12]        ;take m_num parameter
            mov     m_num, ax          ;and save it
            mov     r_num, ax          ;initialize r_num

           ;initialize res_ptr
            mov     ax, m_num
            inc     ax                 ;m_num + 1
            mov     di, l_len
            mul     di                 ;dx:ax=di*ax=2*a_num*(m_num+1)
            add     ax, WORD PTR matr_ptr ;add low words
            adc     dx, 0              ;if c=1, then dx=dx+1
IF CPU_88   
            shl     dx, 1
            shl     dx, 1						 
            shl     dx, 1
            shl     dx, 1
            mov     dh, dl
            mov     dl, 0
ELSE        
            shl     dx, 12
ENDIF
            add     dx, WORD PTR matr_ptr+2;add high words
           ;convert dx:ax to huge pointer
            mov     bx, ax
            and     ax, 0fh
IF CPU_88   
            shr     bx, 1
            shr     bx, 1
            shr     bx, 1
            shr     bx, 1
ELSE        
            shr     bx, 4
ENDIF
            add     dx, bx
           ;save res_ptr
            mov     WORD PTR res_ptr, ax
            mov     WORD PTR res_ptr + 2, dx

           ;find first slow line offset
            mov     ax, slow
            mov     di, l_len			
            mul     di                 ;dx:ax=di*ax=2*a_num*(n_num+1)
           ;add long offset in dx:ax with huge pointer in matr_ptr
            add     ax, WORD PTR matr_ptr ;add low words
            adc     dx, 0              ;if c=1, then dx=dx+1
IF CPU_88   
            shl     dx, 1
            shl     dx, 1						 
            shl     dx, 1
            shl     dx, 1
            mov     dh, dl
            mov     dl, 0
ELSE        
            shl     dx, 12
ENDIF
            add     dx, WORD PTR matr_ptr+2 ;add high words
           ;convert dx:ax to huge pointer
            mov     bx, ax
            and     ax, 0fh
IF CPU_88   
            shr     bx, 1
            shr     bx, 1
            shr     bx, 1
            shr     bx, 1
ELSE        
            shr     bx, 4
ENDIF
            add     dx, bx
           ;initialize huge pointer to first slow line in ds:bx and slow_ptr  
            mov     ds, dx
            mov     bx, ax
            mov     WORD PTR slow_ptr, ax
            mov     WORD PTR slow_ptr + 2, dx

           ;initialize cx - slow lines counter
            mov     ax, n_num
            mov     cx, m_num
            sub     cx, ax             ;cx = m_num - n_num
            jz      break_slow_loop    ;if cx=0, nothing to do
slow_loop:

           ;save ds:bx in slow_ptr and cs in reg_cx
            mov     WORD PTR slow_ptr, bx
            mov     ax, ds
            mov     WORD PTR slow_ptr + 2, ax
            mov     reg_cx, cx
           ;restore ds
            mov     ax, reg_ds
            mov     ds, ax
           ;push parameters r_num and slow in stack
            mov     ax, r_num 
            push    ax
            mov     ax, slow
            push    ax
           ;output information
            call    _output_info
           ;pop parameters from stack
            add     sp, 4
           ;save ds
            mov     ax, ds
            mov     reg_ds, ax
           ;restore ds:bx from slow_ptr and cx from reg_cx
            lds     bx, slow_ptr 
            mov     cx, reg_cx
            
           ;paste current slow line with all fast lines
            call    slow_proc
           ;go to the next slow line
            add     bx, l_len
           ;convert ds:bx to huge pointer
            mov     dx, bx
            and     bx, 0fh
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
           ;increment slow to output
            inc     slow
            loop    slow_loop
break_slow_loop:

           ;restore registers 
            mov     si, reg_si         ;
            mov     di, reg_di         ;
            mov     ax, reg_ds         ;
            mov     ds, ax             ;

            mov     ax, r_num            
            ret                        ;return to DOS or debuger
_paste_88   ENDP

;____________________________________________________________________________
; slow_proc   this procedure takes two parameters:
;	               ds:bx - huge pointer to the slow line,
;                cx    - current slow lines counter value.
;             calculates the number of fast lines to operate using cx
;             initializes es:bp to point on first matrix line
;             pastes current slow line (ds:bx) with all fast lines
;____________________________________________________________________________
slow_proc   PROC    near
           ;initialize fast lines counter in cx 
            push    cx                 ;save slow lines counter in stack
            mov     ax, cx
            mov     cx, m_num
            sub     cx, ax
            jz      break_fast_loop    ;if cx=0, nothing to do

           ;initialize pointer to first line in es:bp
            les     bp, matr_ptr       ;load matrix pointer in es:bp
            add     bp, l_len     		 ;add with line length
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
fast_loop:
           ;paste current slow line (ds:bx) with current fast line (es:bp)
            call    fast_proc
           ;go to the next fast line
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
            loop    fast_loop
break_fast_loop:
            pop     cx                 ;restore slow lines counter
            ret
slow_proc   ENDP

;____________________________________________________________________________
; fast_proc   this procedure takes parameters:
;                ds:bx - huge pointer to the slow line
;                es:bp - huge pointer to the fast line
;             initialize cx - attributes counter to paste on
;             initialize si - current section pointer 
;             pastes current slow (ds:bx) and fast (es:bp) lines
;                on all sections
;____________________________________________________________________________
fast_proc   PROC    near
            push    cx                 ;save fast lines counter
            mov     cx, a_num   			 ;initialize paste section counter
            sub     si, si             ;initialize section pointer by 0
section_loop:
           ;paste two lines (ds:bx and es:bp) on section pointed to by si
            call    section_proc	
            add     si, 2              ;go to the next section to paste
            loop    section_loop
            pop     cx                 ;restore fast lines counter
            ret
fast_proc   ENDP

;____________________________________________________________________________
; section_proc this procedure takes parameters:
;                 ds:bx - pointer to slow line,
;                 es:bp - pointer to fast line,
;                 si    - pointer to section,
;              and pastes two lines on one section.
;____________________________________________________________________________ 
section_proc PROC   near
           ;try to paste two sections pointed to by si
            mov     dx, [bx+si]        ;take slow line section
            mov     di, es:[bp+si]     ;take fast line section
            mov     ax, dx
            and     ax, di
            cmp     ax, dx             ;compare with slow line section
            je      not_paste1         ;if not pasted (zf=1), then return
            cmp     ax, di             ;compare with fast line section
            je      not_paste1         ;if not pasted (zf=1), then return

           ;save pasted section
            push    ds
            push    bx
            lds     bx, res_ptr
            mov     [bx+si], ax
            pop     bx
            pop     ds

            push    cx
            mov     cx, a_num
            mov     dx, si             ;save pointer on pasted section in dx
            sub     si, si             ;begin pasting from zero section
paste_loop:
            cmp     si, dx             ;is it pasted section (AND sec) ?
            je      continue           ;yes, skip through it to the next
            mov     ax, [bx+si]        ;no, take slow line section
            or      ax, es:[bp+si]     ;OR with fast line section
           ;compare result in ax with mask in zero line
            push    ds
            push    bx
            lds     bx, matr_ptr
            cmp     ax, [bx+si]
            pop     bx
            pop     ds
            je      not_paste          ;if not pasted (zf=1), then return
           ;save this section
            push    ds
            push    bx
            lds     bx, res_ptr
            mov     [bx+si], ax
            pop     bx
            pop     ds
        continue:
            add     si, 2              ;go to the next section
            loop    paste_loop

           ;test if the resulting line is absorbed
            call    absorb_proc
not_paste:
            pop     cx
            mov     si, dx             ;restore paste section pointer
not_paste1:
            ret
section_proc ENDP
;____________________________________________________________________________
; absorb_proc this procedure saves registers ds:bx and es:bp in stack,
;             initializes ds:bx to point on the resulting line,
;             es:bp points on current line to test.
;             if the resulting line is absorbed, then nothing is done,
;             else r_num and res_ptr are incremented
;____________________________________________________________________________  
absorb_proc PROC    near
            push    ds
            push    bx
            push    es
            push    bp
            push    dx

           ;initialize pointer to resulting line in ds:bx
            lds     bx, res_ptr
           ;initialize pointer to the first line in es:bp
            les     bp, matr_ptr			 ;it's already corrected value
           ;initialize lines counter in cx
            mov     cx, r_num
           ;initialize di
            mov     di, l_len
line_loop:
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
            mov     cx, di             ;initialize cx by l_len / 2
            shr     cx, 1
            sub     si, si
            sub     si, 2              ;correct section pointer
sec_loop:   
            add     si, 2              ;go to the next section
            mov     ax, [bx+si]        ;take resulting section
            not     ax
            test    ax, es:[bp+si]
            loopz   sec_loop           ;zf=1 - continue, zf=0 - break loop
            mov     cx, dx             ;restore lines counter
            loopnz  line_loop          ;zf=1 - break, zf=0 - continue loop

            jz      not_paste2         ;if new line is absorbed, then return
            inc     r_num              ;if no, add new line
            add     bx, di             ;and increment resulting line pointer
           ;convert ds:bx to huge pointer
            mov     dx, bx
            and     bx, 0fh
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
            mov     WORD PTR res_ptr, bx
            mov     WORD PTR res_ptr+2, ax
not_paste2:
            pop     dx
            pop     bp
            pop     es
            pop     bx
            pop     ds
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