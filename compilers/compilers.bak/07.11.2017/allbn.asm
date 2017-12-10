global __addbn
global __addfoo
global __compbn
global __divbase
global __divbase2
global __divbn		; делений, которое портит делитель
global __divbn2		; деление, которое не портит делитель
global __modbn		; остаток от деления
global __foo
global __mulbase
global __mulbase2
global __mulbase3
global __mulbn
global __subbn



section .data
ebx_1 dd 1 
ecx_1 dd 1
edx_1 dd 1
esi_1 dd 1
edi_1 dd 1

t dd 1
addr_del dd 1
v_al_1 dd 1
v_al_2 dd 2

section .text
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						ADDBN									//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__addbn:
	mov esi, ebp
	sub esi, 64		; tau.al
	mov ebx, [ebp + 112] 	; Вычисляем длину в базах tau

	push ebp
	mov ebp, esp
;	mov edi, [ebp + 8]	; B.al
;	mov ecx, [ebp + 12]	; Вычисляем длину в базах B

	cmp ecx, ebx
	jb .m1
	mov ecx, ebx 
	xor ebx, ebx
	inc ebx			; Гарантируем, что 2-ой цикл будет пройден хотя бы раз
	jmp .m2
.m1:
	sub ebx, ecx
.m2:
	cmp ecx, 0
	jz .cycle1_next
.cycle1:			; Цикл по длине большого числа В и tau
	mov eax, [edi]
	adc [esi], eax
	times 4 dec esi		; уменьшаем, тк tau в стеке
	times 4 inc edi
	loop .cycle1
.cycle1_next:
	mov ecx, ebx
.cycle2:			; Цикл по оставшейся длине большого числа tau
	adc [esi], dword 0
	times 4 dec esi
	loop .cycle2
.cycle2_next:
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						ADDFOO									//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__addfoo:
	push ebp
	mov ebp,esp

	mov [ebx_1], ebx
	mov [ecx_1], ecx
	mov [edx_1], edx
	mov [esi_1], esi
	mov [edi_1], edi

					; esi - начало tau
					; eax - длина tmp
					; edx - начало tmp
					; ebx - sh
					; ecx - n
	dec ebx		; Определяем границы и формируем адреса, которые затем будем использовать в цикле
	shl ebx, 2
	sub esi, ebx
	inc ecx				; проидем 2 цикл хотя бы раз
	cmp ecx, eax
	jb .next
	sub ecx, eax
	push ecx
	mov ecx, eax
	jmp .next1
.next:
	push dword 1
.next1:
	cmp ecx, 0
	jz .cycle1_next

.cycle1:				; Цикл по разности границ
	mov edi, [edx]
	adc [esi], edi
	times 4 dec esi			; уменьшаем, тк tau в стеке
	times 4 inc edx
	loop .cycle1
.cycle1_next:
	pop ecx
.cycle2:			; Цикл по оставшейся длине большого числа tau
	adc [esi], dword 0
	times 4 dec esi
	loop .cycle2
.end:
	xor eax, eax

	mov ebx, [ebx_1]
	mov ecx, [ecx_1]
	mov edx, [edx_1]
	mov esi, [esi_1]
	mov edi, [edi_1]
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						COMPBN									//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__compbn:
	push esi
	push ebx
	push ecx
	push edx
	push edi

	cmp ebx, ecx
	je .n1				; Сравнение длин чисел tau и В
	ja .one
	xor eax, eax
	dec eax
	jmp short .end
.one:
	xor eax, eax
	inc eax
	jmp short .end
.n1:
	dec ebx
	shl ebx, 2
	sub esi, ebx
	add edi, ebx
.cycle1:					; Цикл по длине числа А (или В, что тоже самое в силу равенства длин)
	cmp ecx, 0
	jz .cycle1_next
	mov edx, [esi]
	cmp edx, [edi]
	jna .next1
	xor eax, eax
	inc eax
	jmp short .end		;если найдено несовпадение, то формируем возвращаемое значение и выходим
.next1:
	cmp edx, [edi]
	jnb .next2
	xor eax, eax
	dec eax
	jmp short .end		;если найдено несовпадение, то формируем возвращаемое значение и выходим
.next2:
	times 4 inc esi
	times 4 dec edi
	dec ecx
	jmp short .cycle1

.cycle1_next:
	xor eax, eax
.end:
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop esi
ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						DIVBASE									//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__divbase:
	push ebp
	mov ebp,esp

	push ebx
	push ecx
	push edx
	push esi
	push edi

	cmp edx, 0
	jz .err
	mov ecx, ebx
	dec ecx
	shl ecx, 2
	sub esi, ecx
	mov edi, edx
	xor edx, edx
.cycle1:				;Цикл по длине большого числа А
	cmp ebx, 0
	jz .cycle1_next
	mov eax, [esi]
	div edi
	mov [esi], eax
	times 4 inc esi
	dec ebx
	jmp short .cycle1

.cycle1_next:
	mov eax, edx
.n3:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	mov esp,ebp
	pop ebp
	ret
.err:
	xor eax, eax
	dec eax
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						DIVBASE2								//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__divbase2:

					; ebx - делитель (множитель нормализации)
					; esi - начало остатка от деления (этот остаток от деления нормализованных чисел)
					; edi - начало делителя (число в памяти (в "куче"))
					; ecx - длина делителя (совпадает с длиной остатка)
	push ebp
	mov ebp,esp

	cmp ebx, 0
	jz .err
	dec ecx
	shl ecx, 2
	sub esi, ecx
	add edi, ecx
	shr ecx, 2
	inc ecx
	xor edx, edx
.cycle1:				;Цикл по длине большого числа А
	cmp ecx, 0
	jz .cycle1_next
	mov eax, [esi]
	div ebx
	mov [edi], eax
	times 4 inc esi
	times 4 dec edi
	dec ecx
	jmp short .cycle1

.cycle1_next:
	mov eax, edx
.n3:
	mov esp,ebp
	pop ebp
	ret
.err:
	xor eax, eax
	dec eax
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						DIVBN									///////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__divbn:
	mov esi, ebp
	sub esi, 64			; tau.al
	mov ebx, [ebp + 112]		; Вычисляем длину в базах tau
	push ebp
	mov ebp, esp

;	mov edi, [ebp + 8]		; B.al
;	mov ecx, [ebp + 12]		; Вычисляем длину в базах числа В

;если а < b, то частное = 0, остаток = А
;/////////////////////////////////////////////////////
	call __compbn

	cmp eax, -1
	jne .next2
	sub ecx, ebx
.cycle1:
	cmp ebx, 0
	jz .cycle3
	mov edx, [esi]
	mov [edi], edx
	mov [esi], dword 0
	times 4 dec esi
	times 4 inc edi
	dec ebx
	jmp .cycle1
	
.cycle3:
	cmp ecx, 0
	jz .end
	mov [edi], dword 0
	times 4 inc edi
	dec ecx
	jmp .cycle3
.next2:
;если b - база, то divbase
;///////////////////////////////////////////////////////
	cmp ecx, 0
	jz .err
	cmp ecx, 1
	jne .next
	mov edx, [edi]
	cmp edx, 0
	jz .err
	call __divbase
	mov [edi], eax
	jmp .end
.next:
;нормализация чисел А и В
;////////////////////////////////
;проверка необходимости нормализации
	mov edx, edi
	dec ecx
	shl ecx, 2
	add edx, ecx
	shr ecx, 2
	inc ecx
	push ecx
	mov ecx, [edx]
	cmp ecx, 0x80000000
	jnb .nonorm
	xor eax, eax
	xor edx, edx
	inc edx
	inc ecx
	div ecx
	mov [t], eax
	pop ecx
	push eax
;Нормализация большого числа tau
	call __mulbase
	mov ebx, eax
	pop eax
;Нормализация большого числа b
	call __mulbase3
	jmp .sh
.nonorm:
	pop ecx
	mov [t], dword 1
;сам цикл 
;///////////////////////////////
;вычисляем sh
.sh:
	
	mov edx, esi
	shl ebx, 2
	sub edx, ebx
	times 4 dec edx
	shr ebx, 2
	
	push edx
	push esi
	push ebx
.cycle_cp:
	cmp ebx, 0
	jz .cycle_cp_next
	mov eax, [esi]
	mov [edx], eax
	mov [esi], dword 0
	times 4 dec esi
	times 4 dec edx
	dec ebx
	jmp .cycle_cp
.cycle_cp_next:
	mov [edx], dword 0
	pop ebx
	pop esi
	pop edx

	sub ebx, ecx
	inc ebx			; sh - (разница длин tau и b) + 1

	push edi
	dec ecx
	shl ecx, 2
	add edi, ecx
	mov eax, [edi]
	mov [v_al_1], eax
	times 4 dec edi
	mov eax, [edi]
	mov [v_al_2], eax
	shr ecx, 2
	inc ecx
	pop edi
.cycle2:
	cmp ebx, 0
	jz .cycle2_next
	push ebx
	push edx
	add ebx, ecx
	times 2 dec ebx
	shl ebx, 2
	sub edx, ebx
	mov eax, [edx]		; (u.al + sh + n - 2)
	times 4 dec edx
	push edx
	mov edx, [edx]		; (u.al + sh + n - 1)
	

	mov ebx, [v_al_1]
	cmp edx, ebx
	jz .ffff
	div ebx 			; qApp - eax
					; r - edx
	push eax
	push edx			; r - [esp] 

	xor edx, edx
	mov ebx, [v_al_2]
	mul ebx
	pop ebx
	cmp edx, ebx			; проверка необходимисти корректировки qApp
	ja .qappmin
	cmp edx, ebx
	jb .qapp
	mov edx, [esp + 4]
	times 8 inc edx			; (u.al + sh + n - 3)
	mov edx, [edx]
	cmp eax, edx
	ja .qappmin
	jmp short .qapp
.qappmin:				; необходимо уменьшать qApp
	sub [esp], dword 1
	jmp short .qapp
.ffff:
	xor eax, eax
	dec eax
	push eax
.qapp:					; Вычисляем tmp = MulBASE2(v, q)
					; [esp] = q, т. е. в вершине стека расположено q
	mov edx, [esp + 4]
	times 4 dec edx
	call __mulbase2			; Вычисляем большое число tmp = B * q
					; В eax - длина tmp
					; В edx - начало tmp
	push esi
	mov esi, [esp + 12]		; Начало tau
	mov ebx, [esp + 16]		; sh
					; В ecx - n
	
	call __foo			; Отнимаем в заданных пределах число tmp от числа tau

	cmp eax, 1			; Проверяем был ли займ
	jnz .n2
	sub [esp + 4], dword 1
	
	mov esi, [esp + 12]
	mov edx, edi
	mov eax, ecx
	call __addfoo			; Если займ был, то корректируем значение q и большого числа tau: q = q - 1, 
.n2:					; Займа не было
	mov esi, [esp]

	dec ebx
	shl ebx, 2
	sub esi, ebx
	mov edx, [esp + 4]
	mov [esi], edx			; Запись результата
	shr ebx, 2
	pop esi

	mov edx, [esp + 8]
	add esp, 16
	jmp .cycle2
.cycle2_next:

;/////////////////////////////////////////////////////////////////////////////////////////////////////////
; Здесь делитель перезаписывается на отаток от деления. Делитель при этом портится!!!
	mov ebx, [t]			; ebx - делитель (множитель нормализации)
	mov esi, edx			; esi - начало остатка от деления (этот остаток от деления нормализованных чисел)
					; edi - начало делителя (число в памяти (в "куче"))
					; ecx - длина делителя (совпадает с длиной остатка)
	call __divbase2 			; Вычисление остатка от деления
.end:
	xor eax, eax
	mov esp,ebp
	pop ebp
	ret
.err:
	xor eax, eax
	dec eax
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						DIVBN2									///////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__divbn2:
	mov esi, ebp
	sub esi, 64			; tau.al
	mov ebx, [ebp + 112]		; Вычисляем длину в базах tau
	push ebp
	mov ebp, esp

;	mov edi, [ebp + 8]		; B.al
;	mov ecx, [ebp + 12]		; Вычисляем длину в базах числа В

;если а < b, то частное = 0, остаток = А
;/////////////////////////////////////////////////////
	call __compbn

	cmp eax, -1
	jne .next2
.cycle1:
	cmp ebx, 0
	jz .end
	mov [esi], dword 0
	times 4 dec esi
	dec ebx
	jmp .cycle1

.next2:
;если b - база, то divbase
;///////////////////////////////////////////////////////
	cmp ecx, 0
	jz .err
	cmp ecx, 1
	jne .next
	mov edx, [edi]
	cmp edx, 0
	jz .err
	call __divbase
	jmp .end
.next:
;нормализация чисел А и В
;////////////////////////////////
;проверка необходимости нормализации
	mov edx, edi
	dec ecx
	shl ecx, 2
	add edx, ecx
	shr ecx, 2
	inc ecx
	push ecx
	mov ecx, [edx]
	cmp ecx, 0x80000000
	jnb .nonorm

					; записываем делитель

	push ecx
	mov ecx, [esp + 4]
	push edi
	push ebx
	shl ebx, 5
	mov eax, esi
	sub eax, ebx
	mov [addr_del], eax
	pop ebx
.c5:
	cmp ecx, 0
	jz .c5_next
	mov edx, [edi]
	mov [eax], edx
	times 4 inc edi
	times 4 dec eax
	dec ecx
	jmp .c5
.c5_next:
	pop edi
	pop ecx


	xor eax, eax
	xor edx, edx
	inc edx
	inc ecx
	div ecx
	mov [t], eax
	pop ecx
	push eax
;Нормализация большого числа tau
	call __mulbase
	mov ebx, eax
	pop eax
;Нормализация большого числа b
	call __mulbase3
	jmp .sh
.nonorm:
	pop ecx
	mov [t], dword 1
;сам цикл 
;///////////////////////////////
;вычисляем sh
.sh:
	
	mov edx, esi
	shl ebx, 2
	sub edx, ebx
	times 4 dec edx
	shr ebx, 2
	
	push edx
	push esi
	push ebx
.cycle_cp:
	cmp ebx, 0
	jz .cycle_cp_next
	mov eax, [esi]
	mov [edx], eax
	mov [esi], dword 0
	times 4 dec esi
	times 4 dec edx
	dec ebx
	jmp .cycle_cp
.cycle_cp_next:
	mov [edx], dword 0
	pop ebx
	pop esi
	pop edx

	sub ebx, ecx
	inc ebx			; sh - (разница длин tau и b) + 1

	push edi
	dec ecx
	shl ecx, 2
	add edi, ecx
	mov eax, [edi]
	mov [v_al_1], eax
	times 4 dec edi
	mov eax, [edi]
	mov [v_al_2], eax
	shr ecx, 2
	inc ecx
	pop edi
.cycle2:
	cmp ebx, 0
	jz .cycle2_next
	push ebx
	push edx
	add ebx, ecx
	times 2 dec ebx
	shl ebx, 2
	sub edx, ebx
	mov eax, [edx]		; (u.al + sh + n - 2)
	times 4 dec edx
	push edx
	mov edx, [edx]		; (u.al + sh + n - 1)
	

	mov ebx, [v_al_1]
	cmp edx, ebx
	jz .ffff
	div ebx 			; qApp - eax
					; r - edx
	push eax
	push edx			; r - [esp] 

	xor edx, edx
	mov ebx, [v_al_2]
	mul ebx
	pop ebx
	cmp edx, ebx			; проверка необходимисти корректировки qApp
	ja .qappmin
	cmp edx, ebx
	jb .qapp
	mov edx, [esp + 4]
	times 8 inc edx			; (u.al + sh + n - 3)
	mov edx, [edx]
	cmp eax, edx
	ja .qappmin
	jmp short .qapp
.qappmin:				; необходимо уменьшать qApp
	sub [esp], dword 1
	jmp short .qapp
.ffff:
	xor eax, eax
	dec eax
	push eax

.qapp:					; Вычисляем tmp = MulBASE2(v, q)
					; [esp] = q, т. е. в вершине стека расположено q
	mov edx, [esp + 4]
	times 4 dec edx
	call __mulbase2			; Вычисляем большое число tmp = B * q
					; В eax - длина tmp
					; В edx - начало tmp
	push esi
	mov esi, [esp + 12]		; Начало tau
	mov ebx, [esp + 16]		; sh
					; В ecx - n
	
	call __foo			; Отнимаем в заданных пределах число tmp от числа tau

	cmp eax, 1			; Проверяем был ли займ
	jnz .n2
	sub [esp + 4], dword 1
	
	mov esi, [esp + 12]
	mov edx, edi
	mov eax, ecx
	call __addfoo			; Если займ был, то корректируем значение q и большого числа tau: q = q - 1, 
.n2:					; Займа не было
	mov esi, [esp]

	dec ebx
	shl ebx, 2
	sub esi, ebx
	mov edx, [esp + 4]
	mov [esi], edx			; Запись результата
	shr ebx, 2
	pop esi

	mov edx, [esp + 8]
	add esp, 16
	jmp .cycle2
.cycle2_next:

;/////////////////////////////////////////////////////////////////////////////////////////////////////////
; Здесь делителю присваевается исходное значение
	mov ebx, [t]
	cmp ebx, 1
	je .end

	mov eax, [addr_del] 
.c6:
	cmp ecx, 0
	jz .end
	mov edx, [eax]
	mov [edi], edx
	times 4 inc edi
	times 4 dec eax
	dec ecx
	jmp .c6
.end:
	xor eax, eax
	mov esp,ebp
	pop ebp
	ret
.err:
	xor eax, eax
	dec eax
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						MODBN									///////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__modbn:
	mov esi, ebp
	sub esi, 64			; tau.al
	mov ebx, [ebp + 112]		; Вычисляем длину в базах tau
	push ebp
	mov ebp, esp

;	mov edi, [ebp + 8]		; B.al
;	mov ecx, [ebp + 12]		; Вычисляем длину в базах числа В

;если а < b, то частное = 0, остаток = А
;/////////////////////////////////////////////////////
	call __compbn

	cmp eax, -1
	je .end2
.next2:
;если b - база, то divbase
;///////////////////////////////////////////////////////
	cmp ecx, 0
	jz .err
	cmp ecx, 1
	jne .next
	mov edx, [edi]
	cmp edx, 0
	jz .err
	call __divbase
	mov [esi], eax
	dec ebx
	times 4 dec esi
.c5:
	cmp ebx, 0
	jz .end2
	mov [esi], dword 0
	dec ebx
	times 4 dec esi
	jmp .c5
.next:
;нормализация чисел А и В
;////////////////////////////////
;проверка необходимости нормализации
	mov edx, edi
	dec ecx
	shl ecx, 2
	add edx, ecx
	shr ecx, 2
	inc ecx
	push ecx
	mov ecx, [edx]
	cmp ecx, 0x80000000
	jnb .nonorm

					; записываем делитель

	push ecx
	mov ecx, [esp + 4]
	push edi
	push ebx
	shl ebx, 5
	mov eax, esi
	sub eax, ebx
	mov [addr_del], eax
	pop ebx
.c9:
	cmp ecx, 0
	jz .c9_next
	mov edx, [edi]
	mov [eax], edx
	times 4 inc edi
	times 4 dec eax
	dec ecx
	jmp .c9
.c9_next:
	pop edi
	pop ecx


	xor eax, eax
	xor edx, edx
	inc edx
	inc ecx
	div ecx
	mov [t], eax
	pop ecx
	push eax
;Нормализация большого числа tau
	call __mulbase
	mov ebx, eax
	pop eax
;Нормализация большого числа b
	call __mulbase3
	jmp .sh
.nonorm:
	pop ecx
	mov [t], dword 1
;сам цикл 
;///////////////////////////////
;вычисляем sh
.sh:
	
	mov edx, esi
	shl ebx, 2
	sub edx, ebx
	times 4 dec edx
	shr ebx, 2
	
	push edx
	push esi
	push ebx
.cycle_cp:
	cmp ebx, 0
	jz .cycle_cp_next
	mov eax, [esi]
	mov [edx], eax
	mov [esi], dword 0
	times 4 dec esi
	times 4 dec edx
	dec ebx
	jmp .cycle_cp
.cycle_cp_next:
	mov [edx], dword 0
	pop ebx
	pop esi
	pop edx

	sub ebx, ecx
	inc ebx			; sh - (разница длин tau и b) + 1

	push edi
	dec ecx
	shl ecx, 2
	add edi, ecx
	mov eax, [edi]
	mov [v_al_1], eax
	times 4 dec edi
	mov eax, [edi]
	mov [v_al_2], eax
	shr ecx, 2
	inc ecx
	pop edi
.cycle2:
	cmp ebx, 0
	jz .cycle2_next
	push ebx
	push edx
	add ebx, ecx
	times 2 dec ebx
	shl ebx, 2
	sub edx, ebx
	mov eax, [edx]		; (u.al + sh + n - 2)
	times 4 dec edx
	push edx
	mov edx, [edx]		; (u.al + sh + n - 1)
	

	mov ebx, [v_al_1]
	cmp edx, ebx
	jz .ffff
	div ebx 			; qApp - eax
					; r - edx
	push eax
	push edx			; r - [esp] 

	xor edx, edx
	mov ebx, [v_al_2]
	mul ebx
	pop ebx
	cmp edx, ebx			; проверка необходимисти корректировки qApp
	ja .qappmin
	cmp edx, ebx
	jb .qapp
	mov edx, [esp + 4]
	times 8 inc edx			; (u.al + sh + n - 3)
	mov edx, [edx]
	cmp eax, edx
	ja .qappmin
	jmp short .qapp
.qappmin:				; необходимо уменьшать qApp
	sub [esp], dword 1
	jmp short .qapp
.ffff:
	xor eax, eax
	dec eax
	push eax
.qapp:					; Вычисляем tmp = MulBASE2(v, q)
					; [esp] = q, т. е. в вершине стека расположено q
	mov edx, [esp + 4]
	times 4 dec edx
	call __mulbase2			; Вычисляем большое число tmp = B * q
					; В eax - длина tmp
					; В edx - начало tmp
	push esi
	mov esi, [esp + 12]		; Начало tau
	mov ebx, [esp + 16]		; sh
					; В ecx - n
	
	call __foo			; Отнимаем в заданных пределах число tmp от числа tau

	cmp eax, 1			; Проверяем был ли займ
	jnz .n2
	sub [esp + 4], dword 1
	
	mov esi, [esp + 12]
	mov edx, edi
	mov eax, ecx
	call __addfoo			; Если займ был, то корректируем значение q и большого числа tau: q = q - 1, 
.n2:					; Займа не было
	mov esi, [esp]

	dec ebx
	pop esi

	mov edx, [esp + 8]
	add esp, 16
	jmp .cycle2
.cycle2_next:

;/////////////////////////////////////////////////////////////////////////////////////////////////////////
; Здесь tau перезаписывается на отаток от деления.
	push edx
	push esi 			; esi - начало tau
	mov esi, edx			; esi - начало остатка от деления (этот остаток от деления нормализованных чисел)
	mov edx, [t]			; edx - делитель (множитель нормализации)
	mov ebx, ecx			; ebx - длина делителя (совпадает с длиной остатка)
	call __divbase 			; Вычисление остатка от деления
					; В результате выполнения - остаток от деления, но по адресу esi=edx(т.е. не в tau)
	pop esi
	pop edx
	mov esp,ebp
	pop ebp
	mov ebx, [ebp + 112]		; Вычисляем длину в базах tau
	sub ebx, ecx
	push ecx
.c6:
	cmp ecx, 0
	jz .c6_next
	mov eax, [edx]
	mov [esi], eax
	times 4 dec edx
	times 4 dec esi
	dec ecx
	jmp .c6
.c6_next:
	pop ecx
.c7:
	cmp ebx, 0
	jz .c7_next
	mov [esi], dword 0
	times 4 dec esi
	dec ebx
	jmp .c7
.c7_next:
	mov eax, [t]
	cmp eax, 1
	je .end
	
	mov eax, [addr_del] 
.c8:
	cmp ecx, 0
	jz .end
	mov edx, [eax]
	mov [edi], edx
	times 4 inc edi
	times 4 dec eax
	dec ecx
	jmp .c8

.end:
	xor eax, eax
	ret
.end2:
	xor eax, eax
	mov esp,ebp
	pop ebp
	ret
.err:
	xor eax, eax
	dec eax
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						FOO									///////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__foo:
	push ebp
	mov ebp,esp

	mov [ebx_1], ebx
	mov [ecx_1], ecx
	mov [edx_1], edx
	mov [esi_1], esi
	mov [edi_1], edi
					; esi - начало tau
					; eax - длина tmp
					; edx - начало tmp
					; ebx - sh
					; ecx - n
	dec ebx
	shl ebx, 2
	sub esi, ebx			; Формируем адреса, использующиеся в цикле
	inc ecx
	xor edi, edi

.next1:
	cmp ecx, 0
	jz .cycle1_next
	xor ebx, ebx
.cycle1:				; Цикл по разности границ
	xor edi, edi
	cmp eax, 0
	jz .m1
	mov edi, [edx]
	dec eax
.m1:
	sub esp, 4
	mov [esp], dword 0
	sub [esi], ebx
	adc [esp], dword 0
	pop ebx
	sub [esi], edi
	adc ebx, 0
	times 4 dec esi
	times 4 dec edx
	loop .cycle1
.cycle1_next:
	xor ecx, ecx
.end:
	cmp ebx, 1
	jz .one
	xor eax, eax
	jmp .end2
.one:
	xor eax, eax
	inc eax				;Формируем возвращаемое значение
.end2:
	mov ebx, [ebx_1]
	mov ecx, [ecx_1]
	mov edx, [edx_1]
	mov esi, [esi_1]
	mov edi, [edi_1]
	mov esp,ebp
	pop ebp
	ret

;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						MULBASE									//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__mulbase:
	push ebp
	mov ebp,esp
	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov ecx, eax
	xor edi, edi
	push ebx
.cycle1: 			;Цикл по длине большого числа tau
	cmp ebx, 0
	jz .cycle1_next
	xor edx, edx
	mov eax, [esi]
	mul ecx
	add eax, edi
	adc edx, 0
	mov [esi], eax
	mov edi, edx
	times 4 dec esi
	dec ebx
	jmp short .cycle1
.cycle1_next:
	mov [esi], edi
	pop eax
	cmp edi, 0
	jz .end
	inc eax			; Длина tau в базах
.end:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	mov esp,ebp
	pop ebp
	ret


;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						MULBASE2								//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__mulbase2:
	push ebp
	mov ebp,esp

	mov [ebx_1], ebx
	mov [ecx_1], ecx
	mov [edx_1], edx
	mov [esi_1], esi
	mov [edi_1], edi

	push ecx
	mov esi, edx
	inc ecx
.cycle2:
	cmp ecx, 0
	jz .cycle2_next
	mov [esi], dword 0
	times 4 dec esi
	dec ecx
	jmp .cycle2

.cycle2_next:
	mov ecx, [esp]
	mov ebx, [ebp + 8]
	mov esi, edx
	xor edx, edx
	push edx
.cycle1: 			;Цикл по длине большого числа А
	cmp ecx, 0
	jz .cycle1_next
	mov edx, [esp]
	add [esi - 4], edx
	mov [esp], dword 0
	adc [esp], dword 0
	mov eax, [edi]
	mul ebx
	add [esi], eax
	adc [esi - 4], edx
	adc [esp], dword 0
	times 4 inc edi
	times 4 dec esi
	dec ecx
	jmp short .cycle1
.cycle1_next:
	add esp, 4
	pop eax			;Формирование длины числа С, получившегося в результате
	cmp edx, 0
	jz .end
	inc eax
.end:
	mov ebx, [ebx_1]
	mov ecx, [ecx_1]
	mov edx, [edx_1]
	mov esi, [esi_1]
	mov edi, [edi_1]
	mov esp,ebp
	pop ebp
	ret


;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
;////////////////////////						MULBASE3								//////////////////////////
;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__mulbase3:
	push ebp
	mov ebp,esp
	pusha

	mov ebx, eax
	xor esi, esi
.cycle1: 			;Цикл по длине большого числа А
	cmp ecx, 0
	jz .cycle1_next
	xor edx, edx
	mov eax, [edi]
	mul ebx
	add eax, esi
	adc edx, 0
	mov [edi], eax
	mov esi, edx
	times 4 inc edi
	dec ecx
	jmp short .cycle1
.cycle1_next:
	mov [edi], esi

	popa
	mov esp,ebp
	pop ebp
	ret





__mulbn:
	mov esi, ebp
	sub esi, 64			; tau.al
	mov ebx, [ebp + 112]		; Вычисляем длину в базах tau

	push ebp
	mov ebp,esp

;	mov edi, [ebp + 8]		; B.al
;	mov ecx, [ebp + 12]		; Вычисляем длину в базах B

	mov edx, esi
	shl ebx, 2
	sub edx, ebx
	times 4 dec edx
	shr ebx, 2
	push edx
	push esi
	push ebx
cycle_cp:
	cmp ebx, 0
	jz cycle_cp_next
	mov eax, [esi]
	mov [edx], eax
	mov [esi], dword 0
	times 4 dec esi
	times 4 dec edx
	dec ebx
	jmp cycle_cp
cycle_cp_next:
	pop edx				; Длина tau
	pop esi
	pop ebx
cycle1:					;Внешний цикл по длине tau
	cmp edx, 0
	jz cycle1_next
	push edx
	push esi
	push ecx
	push edi
	sub esp, 4
	mov [esp], dword 0
	cycle2:				;Внутренний цикл по длине В
		cmp ecx, 0
		jz cycle2_next
		mov edx, [esp]
		add [esi - 4], edx
		mov [esp], dword 0
		adc [esp], dword 0
		mov eax, [ebx]
		mov edx, [edi]
		mul edx
		add [esi], eax
		adc [esi - 4], edx
		adc [esp], dword 0
		times 4 inc edi
		times 4 dec esi
		dec ecx
		jmp short cycle2
	cycle2_next:
	add esp, 4
	pop edi
	pop ecx
	pop esi
	pop edx
	times 4 dec esi
	times 4 dec ebx
	dec edx

	jmp short cycle1
cycle1_next:
	
end:
	mov eax, edx
	mov esp,ebp
	pop ebp
	ret

__subbn:
	mov esi, ebp
	sub esi, 64			; tau.al
	mov ebx, [ebp + 112] 		; Вычисляем длину в базах tau

	push ebp
	mov ebp,esp

;	mov edi, [ebp + 8]		; B.al
;	mov ecx, [ebp + 12]		; Вычисляем длину в базах B

	cmp ebx, ecx
	jb .m1
	sub ebx, ecx
	jmp .m2
.m1:
	mov ecx, ebx
	xor ebx, ebx
	inc ebx
.m2:
	cmp ecx, 0
	jz .cycle1_next
.cycle1:					;Цикл по длине В
	mov edx, [edi]
	sbb [esi], edx
	times 4 dec esi
	times 4 inc edi
	loop .cycle1
.cycle1_next:
	mov ecx, ebx
	cmp ecx, 0
	jz .end
.cycle2:					;Цикл по длине, оставшейся от А
	sbb [esi], dword 0
	times 4 dec esi
	loop .cycle2
.end:
	mov esp,ebp
	pop ebp
	ret



