string tauconjcomp(string nomc, int& metka) {
	string text = "";
	int SmeComp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp + 112]\n  cmp ecx, [ebp + " + NToS(SmeComp + 1020) + "]\n  jne _errend\n";
	text = text + "  mov edi, ebp\n  mov esi, [ebp + " + NToS(SmeComp + 220) + "]\n.M" + NToS(metka) + ":\n";
	text = text + "  mov eax, [esi]\n  and [edi-64], eax\n  times 4 dec edi\n  times 4 inc esi\n";
	text = text + "  loop .M" + NToS(metka) + "\n";
	metka += 1;
	return text;	
}

/*
	mov ecx, [ebp + 112]
	cmp ecx, [ebp + Comp + 1020]
	jne _errend
	mov esi, [ebp + Comp + 220]
	mov edi, ebp
	.M(metka):
	  mov eax, [esi]
	  and [edi-64], eax
	  times 4 dec edi
	  times 4 inc esi
	  loop .M(metka)
*/

string taudisjcomp(string nomc, int& metka) {
	string text = "";
	int SmeComp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp + 112]\n  cmp ecx, [ebp + " + NToS(SmeComp + 1020) + "]\n  jne _errend\n";
	text = text + "  mov edi, ebp\n  mov esi, [ebp + " + NToS(SmeComp + 220) + "]\n.M" + NToS(metka) + ":\n";
	text = text + "  mov eax, [esi]\n  or [edi-64], eax\n  times 4 dec edi\n  times 4 inc esi\n";
	text = text + "  loop .M" + NToS(metka) + "\n";
	metka += 2;
	return text;
}

/*
	mov ecx, [ebp + 112]
	cmp ecx, [ebp + Comp + 1020]
	jne _errend
	mov edi, ebp
	mov esi, [ebp + Comp + 220]
	.M(metka):
	  mov eax, [esi]
	  or [edi-64], eax
	  times 4 dec edi
	  times 4 inc esi
	  loop .M(metka)
*/

string tauaddmod2comp(string nomc, int& metka) {
	string text = "";
	int SmeComp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp + 112]\n  cmp ecx, [ebp + " + NToS(SmeComp + 1020) + "]\n  jne _errend\n";
	text = text + "  mov edi, ebp\n  mov esi, [ebp + " + NToS(SmeComp + 220) + "]\n.M" + NToS(metka) + ":\n";
	text = text + "  mov eax, [esi]\n  xor [edi-64], eax\n  times 4 dec edi\n  times 4 inc esi\n";
	text = text + "  loop .M" + NToS(metka) + "\n";
	metka += 2;
	return text;
}

/*
	mov ecx, [ebp + 112]
	cmp ecx, [ebp + Comp + 1020]
	jne _errend
	mov edi, ebp
	mov esi, [ebp + Comp + 220]
	.M(metka):
	  mov eax, [esi]
	  xor [edi-64], eax
	  times 4 dec edi
	  times 4 inc esi
	  loop .M(metka)
*/

string permutation(string nomc, int& metka) {   //Операция перестановки бит собственной переменной в соответствии с номерами в указанном комплексе
	string text = "";
	int comp = (SToN(nomc) - 1) * 4;
	text = text + "  mov edi, eax\n  xor eax, eax\n  mov esi, [ebp + " + NToS(comp + 220) + "]\n  xor ebx, ebx\n  xor ecx, ecx\n.M" + NToS(metka) + ":\n";
	text = text + "  mov edx, [_I + ebx*4]\n  and edx, edi\n  cmp edx, 0\n  je .M" + NToS(metka + 1) + "\n";
	text = text + "  mov ecx, [esi]\n  or eax, [_I + ecx*4]\n";
	text = text + ".M" + NToS(metka + 1) + ":\n  inc ebx\n  times 4 inc esi\n cmp ebx, 32\n  jnz .M" + NToS(metka) + "\n";
	metka += 2;
	return text;
}

/*
	mov edi, eax
	xor eax, eax
	mov esi, [ebp+comp+220]
	xor ebx, ebx
	xor ecx, ecx
	.M1:
	  mov edx, [_I + ebx*4]
	  and edx, edi
	  cmp edx, 0
	  je .M2
	  mov ecx, [esi]
	  or eax, [_I + ecx*4]
	.M2:
	  inc ebx
	  times 4 inc esi
	  cmp ebx, 32
	  jnz .M1
*/

string permutationcomp(string nomc, int& metka) {
	string text = "";
	int comp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp+112]\n  mov esi, ebp\n  sub esi, 64\n  mov ebx, ecx\n  shl ebx, byte 2\n  mov edi, esi\n";
	text = text + "  sub edi, ebx\n  std\n  rep movsd\n  mov ecx, [ebp+112]\n  xor eax, eax\n  add esi, ebx\n  mov edi, esi\n  std\n  rep stosd\n";
	text = text + "  mov ecx, [ebp+112]\n  mov esi, [ebp+" + NToS(comp + 220) + "]\n  xor edx, edx\n.M" + NToS(metka) + ":\n  mov eax, [edi]\n";
	text = text + "  xor ebx, ebx\n.M" + NToS(metka + 1) + ":\n  push edx\n  mov edx, [_I+ebx*4]\n  and edx, eax\n  cmp edx, 0\n  je .M" + NToS(metka + 2) + "\n";
	text = text + "  push eax\n  push ebx\n  push edi\n  mov eax, [esi]\n  mov ebx, 32\n  xor edx, edx\n  div ebx\n  mov edi, ebp\n";
	text = text + "  shl eax, byte 2\n  sub edi, eax\n  mov ebx, [edi-64]\n  or ebx, [_I+edx*4]\n  mov [edi-64], ebx\n  pop edi\n  pop ebx\n  pop eax\n";
	text = text + ".M" + NToS(metka + 2) + ":\n  inc ebx\n  pop edx\n  inc edx\n  times 4 inc esi\n  cmp ebx, 32\n  jl .M" + NToS(metka + 1) + "\n";
	text = text + "  times 4 dec edi\n  dec ecx\n  cmp ecx, 0\n  jnz .M" + NToS(metka) + "\n";
	metka += 3;
	return text;
}

/*
	mov ecx, [ebp+112]
	mov esi, ebp
	sub esi, 64        ; esi = (ebp - 64) 
	mov ebx, ecx
	shl ebx, byte 2
	mov edi, esi
	sub edi, ebx       ; edi = (ebp - 64 - Q*4)
	std                ; DF = 1 уменьшение
	rep movsd          ; повторяем ecx раз

	mov ecx, [ebp+112]	
	xor eax, eax
	add esi, ebx
	mov edi, esi
	std
	rep stosd

	mov ecx, [ebp+112]	
	mov esi, [ebp+comp+220]
	xor edx, edx
.M1:
	mov eax, [edi]
	xor ebx, ebx
.M2:
	push edx
	mov edx, [_I+ebx*4]
	and edx, eax
	cmp edx, 0
	je .M3

	push eax
	push ebx
	push edi

	mov eax, [esi]
	mov ebx, 32
	xor edx, edx
	div ebx
	mov edi, ebp
	shl eax, byte 2
	sub edi, eax
	mov ebx, [edi-64]
	or ebx, [_I+edx*4]
	mov [edi-64], ebx

	pop edi
	pop ebx
	pop eax
	
	
.M3:
	inc ebx
	pop edx
	inc edx
	times 4 inc esi
	cmp ebx, 32
	jl .M2

	times 4 dec edi
	dec ecx
	cmp ecx, 0
	jnz .M1
	
*/

string LeftShiftMod(int& metka) {
	string text = "";
	text = text + "  mov ecx, [ebp+112]\n  mov ebx, ecx\n  shl ebx, byte 5\n  cmp eax, ebx\n  jl .M" + NToS(metka) + "\n";
	text = text + "  xor eax, eax\n  mov edi, ebp\n  sub edi, 64\n  std\n  rep stosd\n  jmp .M" + NToS(metka + 3) + "\n";
	text = text + ".M" + NToS(metka) + ":\n  mov ebx, 32\n  xor edx, edx\n  div ebx\n  cmp edx, 0\n  jz .M" + NToS(metka + 2) + "\n";
	text = text + "  sub ecx, eax\n  push ecx\n  mov edi, ebp\n  sub edi, 64\n  mov ebx, ecx\n  dec ebx\n  shl ebx, byte 2\n";
	text = text + "  sub edi, ebx\n  mov esi, [edi]\n  mov cl, dl\n  shl esi, cl\n  mov [edi], esi\n  times 4 inc edi\n";
	text = text + "  mov ebx, 32\n  sub ebx, edx\n  pop ecx\n  dec ecx\n  cmp ecx, 0\n  jz .M" + NToS(metka + 2) + "\n";
	text = text + "  push eax\n.M" + NToS(metka + 1) + ":\n  push ecx\n  mov esi, [edi]\n  mov cl, bl\n  shr esi, cl\n";
	text = text + "  mov eax, [edi-4]\n  or eax, esi\n  mov [edi-4], eax\n  mov esi, [edi]\n  mov cl, dl\n  shl esi, cl\n  mov [edi], esi\n";
	text = text + "  times 4 inc edi\n  pop ecx\n  loop .M" + NToS(metka + 1) + "\n  pop eax\n.M" + NToS(metka + 2) + ":\n  cmp eax, 0\n";
	text = text + "  jz .M" + NToS(metka + 3) + "\n  mov ecx, [ebp+112]\n  mov ebx, ecx\n  sub ecx, eax\n  mov edi, ebp\n  sub edi, 64\n";
	text = text + "  dec ebx\n  shl ebx, byte 2\n  sub edi, ebx\n  mov ebx, eax\n  shl ebx, byte 2\n  mov esi, edi\n  add esi, ebx\n";
	text = text + "  cld\n  rep movsd\n  mov ecx, eax\n  xor eax, eax\n  rep stosd\n.M" + NToS(metka + 3) + ":\n";
	metka += 4;
	return text;	
}

/*
	mov eax, <value_shift>
	mov ecx, [ebp+112]
	mov ebx, ecx
	shl ebx, byte 5
	cmp eax, ebx
	jl .M1             ;если велечина сдвига больше общей мощности комплекса, то мы наш комплекс обнуляем и больше ничего не делаем
	
	xor eax, eax       
	mov edi, ebp
	sub edi, 64
	std               ; DF = 1, чтобы уменьшался
	rep stosd
	jmp .M4
	
	.M1:
	mov ebx, 32
	xor edx, edx
	div ebx
	cmp edx, 0
	jz .M3
           			; eax =  <value_shift>/32
	   			; edx =  <value_shift>%32
	sub ecx, eax
	push ecx   ; сохраним значение в стек

	mov edi, ebp
	sub edi, 64
	mov ebx, ecx
	dec ebx
	shl ebx, byte 2
	sub edi, ebx

	mov esi, [edi]
	mov cl, dl
	shl esi, cl
	mov [edi], esi

	times 4 inc edi

	mov ebx, 32
	sub ebx, edx

	pop ecx
	dec ecx
	cmp ecx, 0
	jz .M3

	push eax
	.M2:
	push ecx
	mov esi, [edi]
	mov cl, bl
	shr esi, cl
	mov eax, [edi-4]
	or eax, esi
	mov [edi-4], eax
	mov esi, [edi]
	mov cl, dl
	shl esi, cl
	mov [edi], esi
	times 4 inc edi
	pop ecx
	loop .M2
	pop eax

	.M3:
	
	cmp eax, 0
	jz .M4

	mov ecx, [ebp+112]
	mov ebx, ecx
	sub ecx, eax     ;  eax = целая часть от деления велечины сдвига на 32, ecx = разность между мощностью и велечиной eax
	mov edi, ebp
	sub edi, 64
	dec ebx
	shl ebx, byte 2
	sub edi, ebx    ; edi = (ebp - 64 - (Q - 1)*4), где Q - мощность собственного комплекса   (приёмник)
	
	mov ebx, eax    
	shl ebx, byte 2
	mov esi, edi
	add esi, ebx    ; esi = (edi + eax*4)    источник
	cld             ; DF = 0, чтобы увеличивался
	rep movsd       ; повторяю ecx раз

	mov ecx, eax	; теперь надо обнулить с 1 до eax-й базы
	xor eax, eax     
	rep stosd

	.M4:
	
*/

string RightShiftMod(int& metka) {
	string text = "";
	text = text + "  mov ecx, [ebp+112]\n  mov ebx, ecx\n  shl ebx, byte 5\n  cmp eax, ebx\n  jl .M" + NToS(metka) + "\n";
	text = text + "  xor eax, eax\n  mov edi, ebp\n  sub edi, 64\n  std\n  rep stosd\n  jmp .M" + NToS(metka + 3) + "\n";
	text = text + ".M" + NToS(metka) + ":\n  mov ebx, 32\n  xor edx, edx\n  div ebx\n  cmp edx, 0\n  jz .M" + NToS(metka + 2) + "\n";
	text = text + "  sub ecx, eax\n  push ecx\n  mov edi, ebp\n  sub edi, 64\n  mov ebx, eax\n  shl ebx, byte 2\n  sub edi, ebx\n";
	text = text + "  mov esi, [edi]\n  mov cl, dl\n  shr esi, cl\n  mov [edi], esi\n  times 4 dec edi\n  mov ebx, 32\n  sub ebx, edx\n";
	text = text + "  pop ecx\n  dec ecx\n  cmp ecx, 0\n  jz .M" + NToS(metka + 2) + "\n  push eax\n.M" + NToS(metka + 1) + ":\n";
	text = text + "  push ecx\n  mov esi, [edi]\n  mov cl, bl\n  shl esi, cl\n  mov eax, [edi+4]\n  or eax, esi\n  mov [edi+4], eax\n";
	text = text + "  mov esi, [edi]\n  mov cl, dl\n  shr esi, cl\n  mov [edi], esi\n  times 4 dec edi\n  pop ecx\n  loop .M" + NToS(metka + 1) + "\n";
	text = text + "  pop eax\n.M" + NToS(metka + 2) + ":\n  cmp eax, 0\n  jz .M" + NToS(metka + 3) + "\n  mov ecx, [ebp+112]\n";
	text = text + "  sub ecx, eax\n  mov edi, ebp\n  sub edi, 64\n  mov ebx, eax\n  shl ebx, byte 2\n  mov esi, edi\n  sub esi, ebx\n";
	text = text + "  std\n  rep movsd\n  mov ecx, eax\n  xor eax, eax\n  rep stosd\n.M" + NToS(metka + 3) + ":\n";
	metka += 4;
	return text;
}

/*
	mov eax, <value_shift>
	mov ecx, [ebp+112]
	mob ebx, ecx
	shl ebx, byte 5
	cmp eax, ebx
	jl .M1             ; если велечина сдвига больше общей мощности комплекса, то мы наш комплекс обнуляем и больше ничего не делаем

	xor eax, eax        
	mov edi, ebp
	sub edi, 64
	std                ; DF = 1, чтобы уменьшался
	rep stosd
	jmp .M4

	.M1:
	mov ebx, 32
	xor edx, edx
	div ebx
	cmp edx, 0
	jz .M3             ; Если ноль, то никакого сдвига у нас нет, просто переписывание и обнуление ненужных(за это и отвечает .M3)
           		   ; eax =  <value_shift>/32
	   		   ; edx =  <value_shift>%32
	sub ecx, eax
	push ecx           ; сохраним значение в стек
	mov edi, ebp       ; вычисляем адрес той базы, которая при сдвиге на eax вправо станет первой(с неё будем начинать сдвиг на edx)
	sub edi, 64
	mov ebx, eax
	shl ebx, byte 2
	sub edi, ebx

	mov esi, [edi]     ; сразу сдвигаем ту базу которая станет первой на edx вправо     
	mov cl, dl
	shr esi, cl
	mov [edi], esi

	times 4 dec edi    ; берём следующую базу
	mov ebx, 32        ; вычисляем разность (32 - edx)
	sub ebx, edx
	
	pop ecx
	dec ecx
	cmp ecx, 0         ; если нам надо сделать это для одной базы, то выполняем сл. процедуры: переписывание и обнуление
	jz .M3

	push eax
	.M2:
	push ecx
	mov esi, [edi]
	mov cl, bl
	shl esi, cl        ; сдвигаем очередную базу влево на (32 - edx)
	mov eax, [edi+4]
	or eax, esi        ; и делаем дизъюнкцию с базой, лежащей выше
	mov [edi+4], eax
	mov esi, [edi]
	mov cl, dl
	shr esi, cl        ; сдвигаем эту базу вправо на edx
	mov [edi], esi
	times 4 dec edi
	pop ecx
	loop .M2
	pop eax

	.M3:               ; Эта метка отвечает за переписывание на велечину eax, и обнуление тех баз которые нам не нужны

	cmp eax, 0
	jz .M4	
	
	mov ecx, [ebp+112]	
	sub ecx, eax       ; eax = целая часть от деления велечины сдвига на 32, ecx = разность между мощностью и велечиной eax
	mov edi, ebp
	sub edi, 64        ; edi = (ebp - 64)   (приёмник)
    
	mov ebx, eax    
	shl ebx, byte 2
	mov esi, edi
	sub esi, ebx       ; esi = (edi - eax*4)    (источник)
	std                ; DF = 1, чтобы уменьшался
	rep movsd          ; повторяю ecx раз        (здесь остановился, надо проверить для обнуления куда встанет edi послепереписывания?)

	mov ecx, eax	   ; теперь надо обнулить с eax-й базы до последней
	xor eax, eax    
	rep stosd

	.M4:
*/

string minvaluecomp(string nomc) {
	string text = "";
	int comp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp+" + NToS(comp + 1020) + "]\n  mov edi, [ebp+" + NToS(comp + 220) + "]\n";
	text = text + "  xor eax, eax\n  cld\n  rep stosd\n";
	return text;
}

/*
	mov ecx, [ebp + comp + 1020]
	mov edi, [ebp + comp + 220]
	xor eax, eax
	cld
	rep stosd
*/

string maxvaluecomp(string nomc) {
	string text = "";
	int comp = (SToN(nomc) - 1) * 4;
	text = text + "  mov ecx, [ebp+" + NToS(comp + 1020) + "]\n  mov edi, [ebp+" + NToS(comp + 220) + "]\n";
	text = text + "  mov eax, 0xffffffff\n  cld\n  rep stosd\n";
	return text;
}

/*
	mov ecx, [ebp + comp + 1020]
	mov edi, [ebp + comp + 220]
	mov eax, 0xffffffff
	cld
	rep stosd
*/

string Inversiontau(int& metka) {
	string text = "";
	text = text + "  mov ecx, [ebp+112]\n  mov edi, ebp\n  sub edi, 64\n.M" + NToS(metka) + ":\n  mov esi, [edi]\n  not esi\n";
	text = text + "  mov [edi], esi\n  times 4 dec edi\n  loop .M" + NToS(metka) + "\n";
	metka += 1;
	return text;
}

/*
	mov ecx, [ebp+112]
	mov edi, ebp
	sub edi, 64
	.M1:
	mov esi, [edi]
	not esi
	mov [edi], esi
	times 4 dec edi
	loop .M1
*/