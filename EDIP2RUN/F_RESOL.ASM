
PUBLIC  _f_resolution
cseg    SEGMENT PARA PUBLIC 'CODE'
ASSUME  cs:cseg

;____________________________________________________________________________
;   prototype:   COMP f_resolution( COMP huge *matr_ptr,
;                                   COMP huge *fst_ptr,
;                                   COMP huge *sec_ptr,
;                                   COMP huge *res_ptr,
;                                   SECT_N and_sect_num,
;                                   COMP_OFS and_sect_ofs );
;   synopsis     matr_ptr - pointer to the matrix
;                fst_ptr  - pointer to the first line to paste
;                sec_ptr  - pointer to the second line to paste
;                res_ptr  - pointer to the resulting line
;                and_sect_num - the number of &-section
;                and_sect_ofs - offset of the &-section 
;                return   - 0xff if there is no result, else constant of
;                           the resulting disjunct
;   name         f_resolution - pastes two lines from the matrix
;   description  This procedure takes 6 parameters from stack
;                sp+22- and_sect_ofs offset of the &-section
;                sp+20- and_sect_num the number of &-section
;                sp+18- res_ptr      (segment) huge resulting line
;                sp+16- res_ptr      (offset)  address
;                sp+14- sec_ptr      (segment) huge second line
;                sp+12- sec_ptr      (offset)  address
;                sp+10- fst_ptr      (segment) huge first line
;                sp+8 - fst_ptr      (offset)  address
;                sp+6 - matr_ptr     (segment) huge starting address
;                sp+4 - matr_ptr     (offset)  of the matrix
;                sp+2 -              (segment) far return
;                sp+0 -              (offset)  address
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ ah - sec_component ³ al - fst_component ³
; ³ bh - sec_critical  ³ bl - fst_critical  ³
; ³ ch -               ³ cl - comp_counter  ³
; ³ dh - sec_constant  ³ dl - fst_constant  ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ ah - sec_component ³ al - fst_component ³
; ³ bh - comp_min      ³ bl - critical      ³
; ³ ch -               ³ cl - sec(comp)count³
; ³ dh - to save count ³ dl - constant      ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;                The function returns constant of the resulting disjunct.
;                If disjuncts are not pasted, then 255 (0xff) is returned.
;
;____________________________________________________________________________
_f_resolution  PROC    far
	jmp		short	start                    ; skip through data
	matr_ptr    DD      0                ; local variables and parameters
	res_ptr     DD      0
  fst_ptr     DD      0
  and_sec_num DB      0

start:
	push	si                             ; save registers
	push	di
	push	ds
  push	bp

	mov		bp, sp                         ; copy all parameters in local area
	add		bp, 8                          ; skip through 4 saved registers

	mov		ax, [bp+4]                     ; save matr_ptr
	mov		WORD PTR matr_ptr, ax
	mov		ax, [bp+6] 
	mov		WORD PTR matr_ptr + 2, ax
  lds		si, [bp+8]                     ; save fst_ptr and load it in ds:si
	mov		WORD PTR fst_ptr, si
  mov		ax, ds
	mov		WORD PTR fst_ptr + 2, ax
	les   di, [bp+12]                    ; load sec_ptr in es:di
  mov   ax, [bp+16]                    ; save res_ptr
  mov		WORD PTR res_ptr, ax
  mov   ax, [bp+18]
  mov   WORD PTR res_ptr + 2, ax
  mov   al, [bp+20]                    ; save and_sec_num
  mov   BYTE PTR and_sec_num, al
  mov   bp, [bp+22]                    ; load and_sec_ofs in bp

  mov   dl, ds:[si]                    ; dl = fst_constant
  mov   dh, es:[di]                    ; dh = sec_constant
  sub   bx, bx                         ; fst_critical = sec_critical = 0
  lds   si, matr_ptr                   ; initialize cx by the and_comp_num
  mov   cl, ds:[si+bp]
  sub   ch, ch
  lds   si, fst_ptr

	and_comp_loop:                       ; -------------S-T-A-R-T---L-O-O-P-

		mov		al, ds:[si+bp]               ; al = fst_component
    cmp		al, 0                        ; if fst_component = 0
    jnz		skip1                        ; no, nothing to do
    mov		al, dl                       ; yes, fst_component = fst_constant
	skip1:
  	mov		ah, es:[di+bp]               ; ah = sec_component
    cmp		ah, 0                        ; if sec_component = 0
    jnz		skip2                        ; no, nothing to do
    mov		ah, dh                       ; yes, sec_component = sec_constant
  skip2:
  	
  	lds		si, res_ptr
  	cmp		ah, al
    je		sec_eq_fst
    ja		sec_gt_fst

    mov 	ds:[si+bp], ah               ; res_component = sec_component
    cmp		bl, al                       ; if fst_critical >= fst_component
    jae		skip3                        ; yes, nothing to do
    mov		bl, al                       ; no, fst_critical = fst_component
  skip3:
  	jmp		short	skip4

  sec_gt_fst:
    mov		ds:[si+bp], al               ; res_component = fst_component
    cmp		bh, ah                       ; if sec_critical >= sec_component
    jae		skip5                        ; yes, nothing to do
    mov		bh, ah                       ; no, sec_critical = sec_component
  skip5:
  	jmp		short	skip4

  sec_eq_fst:
    mov		ds:[si+bp], al               ; res_component = fst(sec)_component
    
  skip4:
  	lds		si, fst_ptr

    inc		bp
    loop	and_comp_loop                ; -------------------E-N-D--L-O-O-P-

	cmp		bh, bl                         ; bl = min(fst_critical,sec_critical)
  jae		skip6
  mov		bl, bh
skip6:

	cmp		bl, 0                          ; if critical = 0
  jnz		skip17                         ; no, nothing to do
	pop		bp
  pop		ds
  pop		di
  pop		si
  mov		al, 255
  ret
skip17:

	cmp		dh, dl                         ; dl = min(fst_constant,sec_constant)
	jae		skip7
	mov		dl, dh
skip7:

	sub		cx, cx                         ; cx = the attribute number
  lds		si, matr_ptr
  mov		al, ds:[si]
  lds		si, fst_ptr
  mov		cl, al
	sub		al, and_sec_num                ; and_sec_num = attr_num - and_sec_num
  mov		and_sec_num, al

	mov		bp, 1
	
	or_sec_loop:                         ; -------------S_T_A_R_T_-_L_O_O_P---
		
		mov		dh, cl                       ; save section counter

    lds		si, matr_ptr                 ; load component counter
    mov		cl, ds:[si+bp]
    lds		si, fst_ptr

    cmp		dh, and_sec_num              ; is it &-section
    je		skip8

    mov		bh, 255                      ; comp_min = 255 (0xff)

		or_comp_loop:                      ; -------------S_T_A_R_T_-_L_O_O_P---
			mov		al, ds:[si+bp]
			mov		ah, es:[di+bp]
			lds		si, res_ptr
			cmp		ah, al
			ja		skip9

			mov		ds:[si+bp], al             ; res_component = fst_component
      cmp		al, bh                     ; if res_component >= comp_min
      jae		skip10                     ; yes, nothing to do
      mov		bh, al                     ; no, comp_min = res_component
		skip10:
			jmp		short	skip11

		skip9:			  	
			mov		ds:[si+bp], ah             ; res_component = sec_component
			cmp		ah, bh                     ; if res_component >= comp_min
			jae		skip12                     ; yes, nothing to do
			mov		bh, ah                     ; no, comp_min = res_component
		skip12:
		
		skip11:
			lds		si, fst_ptr		  		
			inc		bp
			loop	or_comp_loop  		         ; ----------------E_N_D_-_L_O_O_P---
		
    cmp		bl, bh                       ; if critical <= comp_min
    jbe		skip13                       ; yes, not pasted

  	cmp		dl, bh                       ; constant > comp_min
    ja		skip14                       ; yes, nothing to do
    mov		dl, bh                       ; no, constant = comp_min
  skip14:
		
		jmp		short	skip15
	skip8:
  	add		bp, cx
	skip15:
  	mov		cl, dh                       ; restore counter
    loop	or_sec_loop                  ; ----------------E_N_D_-_L_O_O_P---

	lds		si, matr_ptr
  les		di, res_ptr
  mov		es:[di], dl                    ; res_constant = dl
  mov		cl, ds:[si]                    ; cl = attr_num
  mov		bp, 1
  sub		ax, ax                         ; al = 0

	sec_loop:
		mov		dh, cl                       ; save section counter in dh
    mov		cl, ds:[si+bp]               ; load component counter in cl

		comp_loop:
  		cmp		es:[di+bp], dl             ; if res_component > res_constant
      ja		skip16                     ; yes, nothing to do
      mov		es:[di+bp], al             ; no, res_component = 0
    skip16:
    	inc		bp
      loop	comp_loop

    mov		cl, dh                       ; restore section counter
    loop	sec_loop

  pop		bp
  pop		ds
  pop		di
  pop		si
  mov		al, dl
  ret

skip13:
	pop		bp
  pop		ds
  pop		di
  pop		si
  mov		al, 255
  ret

_f_resolution		ENDP
cseg	ENDS
END