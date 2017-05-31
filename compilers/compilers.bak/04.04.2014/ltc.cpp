#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <regex.h>
#include <vector>
#include <string.h>

using namespace std;

char FileName[500];

//******************************************************************************** 
void Debug(string s)
{	cout << "[DEBUG]: " << s << endl;
}
//******************************************************************************** 
void Internal_Error(string fail, string message)
{	cout << "[INTERNAL_ERROR]: " << message << endl;
	ofstream file;
	char fn[500];
	strncpy(fn,FileName,400);
	strcat(fn,".err"); printf("имя файла с техтом: %s \n",fn); 
	file.open(fn);
	file << fail;
	file.close();
	exit(1);
}
//******************************************************************************** 
string SubStr(string s, int k)
{	string tmp;
	tmp.resize(s.size() - k);
	for(int i = k, j = 0; i < s.size(); i++, j++)
		tmp[j] = s[i];
	return tmp;
}

//******************************************************************************** 
string NToS(unsigned long number)
{	char *tmp = new char[sizeof(number)-1];
	sprintf(tmp, "%lu", number);
	string s;
	s.resize(sizeof(number)-1);
	s = tmp;
	return s;
}

//******************************************************************************** 
int SToN(string s)
{
	const char *tmp = s.c_str();
	return atoi(tmp);
}

//******************************************************************************** 
vector<string> GetStr(const char *pattern, string str)
{	regex_t regex;
	regmatch_t pm[100];
	vector<string> tmp;
	if(regcomp(&regex, pattern, REG_EXTENDED)) {
	  fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}
	if(regexec(&regex, str.c_str(), 90, pm, 0))
	  tmp.push_back("$$$");
	else{ 
	  tmp.push_back(str.substr(0,pm[0].rm_so));//Debug(str.substr(0,pm[0].rm_so));
	  tmp.push_back(str.substr(pm[0].rm_so,pm[0].rm_eo - pm[0].rm_so));//Debug(str.substr(pm[0].rm_so,pm[0].rm_eo - pm[0].rm_so));
	  int l=str.size();
	  tmp.push_back(str.substr(pm[0].rm_eo,l));//Debug(str.substr(pm[0].rm_eo,l));
	  for(int i=1;i<90;i++){
	    if(pm[i].rm_so<0) break;
	    tmp.push_back(str.substr(pm[i].rm_so,pm[i].rm_eo - pm[i].rm_so));//Debug(str.substr(pm[i].rm_so,pm[i].rm_eo - pm[i].rm_so));
	  }
	}
  return tmp;
}

//******************************************************************************** 
void ExecF(string stext)
{ ofstream f;
	char buf[500];
	int rer;
	strcat(strcpy(buf,FileName),".s");
	f.open(buf);
	f << stext;
	f.close();
	strcat(strcat(strcpy(buf,"nasm -felf -g "),FileName),".s");
  printf("*** %s\n",buf);
  if((rer=system(buf)) != 0) return;
//  Debug("rer="+NToS(rer));
	strcat(strcat(strcat(strcat(strcpy(buf,"ld -melf_i386 -o "),FileName)," "),FileName),".o");
  printf("*** %s\n",buf);
  if(system(buf) != 0) return;
  strcat(strcat(strcpy(buf,"rm "),FileName),".o");
  system(buf);
  printf("*** %s\n",buf);
}

//******************************************************************************** 
string ReadFile(const char *filename)
{	ifstream file;
	string s, tmp;
	file.open(filename);
	if(file.is_open())
		while(getline(file, tmp))
			s = s + tmp + "\n";
	else
		s = "";
	return s;
	file.close();
}

//******************************************************************************** 
vector<string> Split_comma(string s)
{	string tmp;
	vector<string> res, tmp2;
	tmp = s;//Debug(s);
	while(1) {
	  tmp2 = GetStr("[[:space:],]+", tmp);//Debug(tmp2[0]);
	  if(tmp2[0]=="$$$") break;
	  tmp = tmp2[2];
	  res.push_back(tmp2[0]);
	}
	res.push_back(tmp);
	return res;
}

//******************************************************************************** 
vector<string> Split(string str)
{	vector<string> results, tmp1, tmp2, utr;
	string ex = str; //Debug(ex);
//	const char *space = "^[[:space:]]*";
	while(1)
	{ tmp1 = GetStr("(\n|^)[[:space:]]*([a-zA-Z0-9_]+)\\(([a-zA-Z0-9,[:space:]/]+)\\)", ex);//Debug(tmp1[0]);
	  if(tmp1[0] == "$$$") break;
	  results.push_back(tmp1[0]); 
 		results.push_back(tmp1[4]);
		results.push_back(tmp1[5]);
		ex = tmp1[2];
	}
	results.push_back(ex);
	return results;
}

//******************************************************************************** 
string Replace(string s, string str2, string str1)
{	string::size_type pos = 0;
//	Debug(str1);Debug(s);
	while((pos=str1.find(s,pos)) != string::npos ) { 
		str1.replace(pos, s.size(),str2);
    pos++;
  } //Debug(str1);
  return str1;
}

//******************************************************************************** 
int GetAddr(string name)
{ int addr, index;
	vector<string> tmp;
	tmp = GetStr("^[a-z]$", name);
	if(tmp[0] != "$$$")
		addr = ((int)name[0] - (int)'a' + 1)*4;
	else {
		tmp = GetStr("^[LFQS][0-9]+$", name);
		if(tmp[0] != "$$$") {
			index = SToN(SubStr(name, 1));
			if(index == 0 || index > 99)
				Internal_Error(name, "Номер комплекса не 1 - 99");
			if(name[0] == 'Q')
				addr = (index - 1)*4 + 1020;
			else if(name[0] == 'S')
				addr = (index - 1)*4 + 620;
			else
			addr = (index - 1)*4 + 220;
		}
		else {
			tmp = GetStr("^[Z]$", name);
			if(tmp[0] != "$$$")	addr = 108;
			else
				Internal_Error(name, "Неверное имя " + name);
		}
	}
	return addr; 
}
 
//******************************************************************************** 
string SetReg(string name, string op, string reg)
{ vector<string> tmp;
	string stext = ""; 
//Debug("name = " + name);	
	tmp = GetStr("^([a-zZ])|([QS][0-9]+$)", name);
	if(tmp[0] != "$$$") // a-z, Q1, S2
		{ stext = stext + "  " + op + " " + reg + ",[ebp+" + NToS(GetAddr(name)) + "]\n";return stext;}
	tmp = GetStr("^([LF])([0-9]+)([a-z])$", name);
	if(tmp[0] != "$$$") { //L1a, L5b, F12c
		stext = stext + "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext =  stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "F")
			stext = stext + "  mov ebx,[ebx]\n  and ebx,0x000000ff\n  " + op + " " + reg + ",ebx\n";
		else
			stext = stext + "  " + op + " " + reg + ",[ebx]\n";
			return stext;
	} // проверка по ёмкости?!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tmp = GetStr("^([LF])([0-9]+)\\(([a-z])\\([+-])(\\d+)\\)$", name);
	if(tmp[0] != "$$$") { //L1(a+5), L5(b-4), F12(c+1)
		stext = stext + "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[6] == "+")
			stext = stext + "  add ebx,"+tmp[7] + "]\n";
		else
			stext = stext + "  sub ebx,"+tmp[7] + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext =  stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "F")
			stext = stext + "  mov ebx,[ebx]\n  and ebx,0x000000ff\n  " + op + " " + reg + ",ebx\n";
		else
			stext = stext + "  " + op + " " + reg + ",[ebx]\n";
			return stext;
	} // проверка по ёмкости?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tmp = GetStr("^I([a-z])$", name);
	if(tmp[0] != "$$$")	{ // Ia
		stext = stext + "  mov ebx,[ebp+" + NToS(GetAddr(tmp[3])) + "]\n  and ebx,0x1f\n";
		stext = stext + "  " + op + " " + reg + ",[_I+ebx*4]\n";
		return stext;
	}
	tmp = GetStr("^I([0-9]+)", name);
	if(tmp[0] != "$$$")	{ // I8
		int i = SToN(tmp[3]);
		if(i>31) Internal_Error(stext,"номер > 31 : " + name);
		stext = stext + "  " + op + " " + reg + ",[_I+" + NToS(i*4) + "]\n";
		return stext;
	}
	tmp = GetStr("^([LF])([0-9]+)\\.([0-9]+)$", name);
	if(tmp[0] != "$$$")	{ // L1.0, L5.2, F12.3
		stext = stext + "  mov ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "F") {
			stext = stext + "  " + op + " " + reg + ",[ebx+" + tmp[5] + "]\n";
			stext = stext + "  and " + reg + ",0x000000ff\n";
		}
		else
			stext = stext + "  " + op + " " + reg + ",[ebx+" + NToS((SToN(tmp[5]))*4) + "]\n";
			return stext;
	} // проверка по ёмкости?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tmp = GetStr("^[0-9]+$", name);
	if(tmp[0] != "$$$")	// десятичная константа
		{ stext = stext + "  " + op + " " + reg + "," + name + "\n";return stext;}
	tmp = GetStr("^'.'$", name);
	if(tmp[0] != "$$$") {	// символьная константа
//		tmp = Split_Bracket("'", "'", name);
		stext = stext + "  " + op + " " + reg + "," + NToS((int)tmp[1][1]) + "\n";
		return stext;
	} 
//Debug(stext);
	Internal_Error(name,"неизвестная конструкция:"+name); 
}	//SetReg

//******************************************************************************** 
string StoreTau(string name, string op)
{ vector<string> tmp;
	string stext;
//	Debug("name="+name);
	tmp = GetStr("^[a-zZ]", name); // в переменную
	if(tmp[0] != "$$$")	stext = "  " + op + " [ebp+" + NToS(GetAddr(name)) + "],eax\n";
	tmp = GetStr("^Q([0-9]+)$", name);
	if(tmp[0] != "$$$") // в мощность комплекса
		if((SToN(tmp[3]) > 0) && (SToN(tmp[3]) < 100))
			stext = "  mov [ebp+" + NToS(GetAddr(name)) + "],eax\n";
		else Internal_Error(name,"неправильный номер комплекса");
	tmp = GetStr("^([LF][0-9]+)\\.([0-9]+)$", name);
	if(tmp[0] != "$$$") { // L1.6 F4.56
		stext = "  mov ebx,[ebp+" + NToS((SToN(SubStr(tmp[3], 1)) - 1)*4 + 220) + "]\n";
		if(tmp[3][0] == 'F')
			stext = stext + "  " + op + " [ebx+" + tmp[4] + "],al\n";
		else
			stext = stext + "  " + op + " [ebx+" + NToS((SToN(tmp[4]))*4) + "],eax\n";
	}
	tmp = GetStr("^([LF])([0-9]+)([a-z])$", name);
	if(tmp[0] != "$$$") { // L3a F1v
		stext = "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext = stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  " + op + " [ebx],eax\n";
		else
			stext = stext + "  " + op + " [ebx],al\n";
	}
	tmp = GetStr("^([LF])([0-9]+)\\(([a-z])([+-])([0-9]+)\\)$", name);
	if(tmp[0] != "$$$") { //L1(a+5), L5(b-4), F12(c+1)
		stext = "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[6] == "+")
			stext = stext + "  add ebx,"+tmp[7] + "\n";
		else
			stext = stext + "  sub ebx,"+tmp[7] + "\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext =  stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "F")
			stext = stext + "  " + op + " [ebx],al\n";
		else
			stext = stext + "  " + op + " [ebx],eax\n";
	} // проверка по ёмкости?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
return stext; 
} //StoreTau

//******************************************************************************** 
string StoreReg(string name, string op, string reg)
{ vector<string> tmp;
	string stext;
//	Debug("name="+name);
	tmp = GetStr("^[a-zZ]", name); // в переменную
	if(tmp[0] != "$$$")	stext = "  " + op + " [ebp+" + NToS(GetAddr(name)) + "]," + reg + "\n";
	tmp = GetStr("^Q([0-9]+)$", name);
	if(tmp[0] != "$$$") // в мощность комплекса
		if((SToN(tmp[3]) > 0) && (SToN(tmp[3]) < 100))
			stext = "  mov [ebp+" + NToS(GetAddr(name)) + "]," + reg + "\n";
		else Internal_Error(name,"неправильный номер комплекса");
	tmp = GetStr("^([LF][0-9]+)\\.([0-9]+)$", name);
	if(tmp[0] != "$$$") { // L1.6 F4.56
		stext = "  mov ebx,[ebp+" + NToS((SToN(SubStr(tmp[3], 1)) - 1)*4 + 220) + "]\n";
		if(tmp[3][0] == 'F')
			stext = stext + "  " + op + " [ebx+" + tmp[4] + "],al\n";
		else
			stext = stext + "  " + op + " [ebx+" + NToS((SToN(tmp[4]))*4) + "]," + reg + "\n";
	}
	tmp = GetStr("^([LF])([0-9]+)([a-z])$", name);
	if(tmp[0] != "$$$") { // L3a F1v
		stext = "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext = stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "L")
			stext = stext + "  " + op + " [ebx]," + reg + "\n";
		else
			stext = stext + "  " + op + " [ebx],al\n";
	}
	tmp = GetStr("^([LF])([0-9]+)\\(([a-z])([+-])([0-9]+)\\)$", name);
	if(tmp[0] != "$$$") { //L1(a+5), L5(b-4), F12(c+1)
		stext = "  mov ebx,[ebp+" + NToS(GetAddr(tmp[5])) + "]\n";
		if(tmp[6] == "+")
			stext = stext + "  add ebx,"+tmp[7] + "\n";
		else
			stext = stext + "  sub ebx,"+tmp[7] + "\n";
		if(tmp[3] == "L")
			stext = stext + "  shl ebx,byte 2\n";
		stext =  stext + "  add ebx,[ebp+" + NToS((SToN(tmp[4]) - 1)*4 + 220) + "]\n";
		if(tmp[3] == "F")
			stext = stext + "  " + op + " [ebx]," + reg[1] + "l\n";
		else
			stext = stext + "  " + op + " [ebx]," + reg + "\n";
	} // проверка по ёмкости?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  return stext; 
} //StoreReg

//******************************************************************************** 
string CmpJmp(string arg1, string cmpop, string arg2, string mark)
{ string stext, op;
	stext = SetReg(arg1, "mov", "eax");
	stext = stext + "  mov edx,eax\n";
	stext = stext + SetReg(arg2, "mov", "eax");
	stext = stext + "  cmp edx,eax\n";
	if(cmpop == "<")
		op = "  jb";
	else if(cmpop == ">")
		op = "  ja";
	else if(cmpop == "≤")
		op = "  jbe";
	else if(cmpop == "≥")
		op = "  jae";
	else if(cmpop == "=")
		op = "  je";
	else if(cmpop == "≠")
		op = "  jne";
	stext = stext + op + " .P" + mark + "\n";
	return stext; 
}
//******************************************************************************** 
string AppendArray(string array_number, string size)
{ int perem=2, index;
	string addr, stext;
	vector<string> tmp1 = GetStr("^[0-9]+$", size);
	if(tmp1[0]!="$$$")
		perem = 0;
	else {
		tmp1 = GetStr("^[a-z]$", size);
		if(tmp1[0]!="$$$")
			perem = 1;
	}
	if(perem == 2)
		Internal_Error(stext, "Ошибка в увелечении ёмкости комплекса: \"S" + array_number + "(" + size + ")\" ёмкость число или переменная");
	index = SToN(array_number);
	if(index < 100 && index > 0) {
		addr = NToS((index-1)*4 + 620);
//		stext = "  mov ebx,[ebp+1416]\n  mov [ebp+" + addr + "],ebx\n";
		if(perem == 1) 
			stext = "  mov eax,[ebp+" + NToS(((int)size[0]-(int)'a'+1)*4) + "]\n  add [ebp+" + addr + "],eax\n  add [ebp+1416],eax\n";
		else 
			stext = "  add [ebp+" + addr + "],dword " + size + "\n  add [ebp+1416],dword " + size + "\n";
		stext = stext + "  call _addmem\n";
	}
	else
		Internal_Error(stext, "Ошибка в увелечении ёмкости комплекса: \"" + array_number + "\" номер комплекса должен быть >0 и <100");
	return stext; 
}
//******************************************************************************** 
string Call(string name, string callsign, map<string, string> functions)
{ string sign, tmp, v_from, v_to;
	int h, i, k;
	vector<string> tmp1, tmp2, invp, outvp, inv, outv;
	sign = functions[name];
	string stext = "  sub esp,1420\n  push ebp\n  mov edx,esp\n  mov ebx,[ebp+1416]\n  mov [edx+1416],ebx\n";
	tmp1 = GetStr("/", callsign);
	invp = Split_comma(tmp1[0]);
	outvp = Split_comma(tmp1[2]);
	tmp1 = GetStr("/", sign);
	inv = Split_comma(tmp1[0]);
	outv = Split_comma(tmp1[2]);
	if((inv.size() != invp.size()) || (outv.size() != outvp.size())) {
		Debug("Invalid sign for " + name + "(" + sign + "): " + callsign);
		exit(1);
	}
	for(i = 0; i < invp.size(); i++) { // копирование входных значений
		v_from = invp[i];
		v_to = inv[i];
		tmp1 = GetStr("^[a-z]$", v_to);
		if(tmp1[0]!="$$$")	{ // переменная
			tmp1 = GetStr("^[a-z]$", v_from);
			if(tmp1[0]!="$$$") { // переменная
			  stext = stext + "  mov eax,[ebp+" + NToS(((int)v_from[0] - (int)'a' + 1)*4) + "]\n  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],eax\n";
				continue;
			}
			tmp1 = GetStr("^Q([0-9]+)$", v_from); 
			if(tmp1[0]!="$$$") { // мощность комплекса
				stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp1[3]) - 1)*4 + 1020) + "]\n  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],eax\n";
				continue;
			}
			tmp1 = GetStr("^[a-fA-F0-9]+h$", v_from); 
			if(tmp1[0]!="$$$") { // шестнадцатиричная константа
				stext = stext + "  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],dword " + v_from + "\n";
				continue;
			}
			tmp1 = GetStr("^[0-9]+$", v_from); 
			if(tmp1[0]!="$$$") { // десятичная константа
				stext = stext + "  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],dword " + v_from + "\n";
				continue;
			}
			tmp1 = GetStr("^'.'$", v_from); 
			if(tmp1[0]!="$$$") { // символьная константа
					stext = stext + "  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],dword " + v_from + "\n";
				continue;
			}
			tmp1 = GetStr("^([LF])([0-9]+)\\.([0-9]+$)", v_from); 
			if(tmp1[0]!="$$$") { // L1.1 F4.8
				int nk = SToN(tmp1[4]);;
				string type=tmp1[3];
				stext = stext + "  mov ebx,[ebp+" + NToS((nk - 1)*4 + 220) + "]\n";
				if(type == "L")
					stext = stext + "  mov eax,[ebx+" + NToS(SToN(tmp1[5])*4) + "]\n";
				else
					stext = stext + "  mov eax,[ebx+" + tmp1[5] + "]\n  and eax,0xff\n";
				stext = stext + "  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],eax\n";
				continue;
			}
			tmp1 = GetStr("^([LF])([0-9]+)([a-z])$", v_from); 
			if(tmp1[0]!="$$$") { // L1a F4b
				int nk = SToN(tmp1[4]);
				string type = tmp1[3];
				stext = stext + "  mov ebx,[ebp+" + NToS(((int)tmp1[5][0] - (int)'a' + 1)*4) + "]\n";
				if(type == "L")
					stext = stext + "  shl ebx,byte 2\n";
				stext = stext + "  add ebx,[ebp+" + NToS((nk - 1)*4 + 220) + "]\n  mov eax,[ebx]\n";
				if(type == "F")
					stext = stext + "  and eax,0xff\n";
				stext = stext + "  mov [edx+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],eax\n";
				continue;
			}
			Internal_Error(stext, "\"" + v_from + "\" не является значением переменной");
		}
		tmp1 = GetStr("^[LF]([0-9]+)$", v_to);
		if(tmp1[0]!="$$$")	{ // комплекс
			tmp2 = GetStr("^[LF]([0-9]+)$", v_from);
			if(tmp2[0]!="$$$")	{ // комплекс
				if(v_to[0] == v_from[0]) {
					stext = stext + "  mov al,[ebp+" + NToS(SToN(tmp2[3]) + 120) + "]\n  mov [edx+" + NToS(SToN(tmp1[3]) + 120) + "],al\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 220) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 220) + "],eax\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 620) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 620) + "],eax\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 1020) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 1020) + "],eax\n";
					continue;
				}
				else
					Internal_Error(stext, "\"" + v_from + "\" разные типы комплексов");
			}
			Internal_Error(stext, "\"" + v_from + "\" не является комплексом");
		}
	}
	for(i = 0; i < outv.size(); i++) { // выходные комплексы являются и входными
		v_from = outvp[i];
		v_to = outv[i];
		tmp1 = GetStr("^[LF]([0-9]+)$", v_to);
		if(tmp1[0]!="$$$")	{ // комплекс
			tmp2 = GetStr("^[LF]([0-9]+)$", v_from);
			if(tmp2[0]!="$$$")	{ // комплекс
				if(v_to[0] == v_from[0]) {
					stext = stext + "  mov al,[ebp+" + NToS(SToN(tmp2[3]) + 120) + "]\n  mov [edx+" + NToS(SToN(tmp1[3]) + 120) + "],al\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 220) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 220) + "],eax\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 620) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 620) + "],eax\n";
					stext = stext + "  mov eax,[ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 1020) + "]\n  mov [edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 1020) + "],eax\n";
					continue;
				}
				else
					Internal_Error(stext, "\"" + v_from + "\" разные типы комплексов");
			}
			Internal_Error(stext, "\"" + v_from + "\" не является комплексом");
		}
	}
	stext = stext + "  mov ebp,edx\n  call " + name + "\n  mov edx,esp\n  pop ebp\n  add esp,1420\n";
	for(i = 0; i < outv.size(); i++)	{ // копирование выходных значений
		v_to = outvp[i];
		v_from = outv[i];
		tmp1 = GetStr("^[a-z]$", v_from);
		if(tmp1[0]!="$$$")	{ // переменная
			stext = stext + "  mov eax,[edx+" + NToS(((int)v_from[0] - (int)'a' + 1)*4) + "]\n";
			tmp2 = GetStr("^[a-z]$", v_to); 
			if(tmp2[0]!="$$$") { // переменная
				stext = stext + "  mov [ebp+" + NToS(((int)v_to[0] - (int)'a' + 1)*4) + "],eax\n";
				continue;
			}
			tmp2 = GetStr("^([LF])([0-9]+)\\.([0-9]+)$", v_to); 
			if(tmp2[0]!="$$$") { // элемент комплекса L1.1
				int nk = SToN(tmp2[4]);
				string type = tmp2[3];
				stext = stext + "  mov ebx,[ebp+" + NToS((nk - 1)*4 + 220) + "]\n";
				if(type == "F")
					stext = stext + "  mov [ebx+" + tmp2[5] + "],al\n";
				else
					stext = stext + "  mov [ebx+" + NToS(SToN(tmp2[5])*4) + "],eax\n";
				continue;
			}
			tmp2 = GetStr("^([LF])([0-9]+)([a-z])$", v_to); 
			if(tmp2[0]!="$$$") { // элемент комплекса L1a
				int nk = SToN(tmp2[4]);
				string type = tmp2[3];
				stext = stext + "  mov ebx,[ebp+" + NToS((nk - 1)*4 + 220) + "]\n";
				stext = stext + "  mov edx,[ebp+" + NToS(((int)tmp2[5][0] - (int)'a' + 1)*4) + "]\n";
				if(type == "F")
					stext = stext + "  mov [edx+ebx],al\n";
				else
					stext = stext + "  mov [ebx+edx*4],eax\n";
				continue;
			}
			Internal_Error(stext, "\"" + v_from + "\" не является значением переменной");
		}
		tmp1 = GetStr("^[LF]([0-9]+)$", v_from);
		if(tmp1[0]!="$$$")	{ // комплекс
			tmp2 = GetStr("^[LF]([0-9]+)$", v_to);
			if(tmp2[0]!="$$$")	{ // комплекс
				if(v_to[0] == v_from[0]) {
					stext = stext + "  mov eax,[edx+" + NToS((SToN(tmp1[3]) - 1)*4 + 1020) + "]\n  mov [ebp+" + NToS((SToN(tmp2[3]) - 1)*4 + 1020) + "],eax\n";
					continue;
				}
			}
			Internal_Error(stext, "\"" + v_from + "\" разные типы комплексов");
		}
	}
	return stext; 
}
 
//******************************************************************************** 
string CreateArray(string array_name, string size)
{ int perem=2, index, len;
	string emc_byte, bprotect, addr, stext;
	vector<string> tmp1 = GetStr("^[0-9]+$", size);
	if(tmp1[0]!="$$$")
		perem = 0;
	else {
		tmp1 = GetStr("^[a-z]$", size);
		if(tmp1[0]!="$$$")
			perem = 1;
	}
	if(perem == 2)
		Internal_Error(stext, "Ошибка в создании комплекса: \"S" + array_name + "(" + size + ")\" ёмкость число или переменная");
	index = SToN(SubStr(array_name, 1));
	if(array_name[0] == 'L')
		bprotect = NToS(0);
	else if(array_name[0] == 'F')
		bprotect = NToS(1);
	else
		Internal_Error(stext, "Ошибка в создании комплекса: \"" + array_name + "\" неизвестный тип комплекса");
	if(index < 100 && index > 0) {
		addr = NToS(index+120);
		stext = stext + "  mov byte[ebp+" + addr + "]," + bprotect + "\n";
		addr = NToS((index-1)*4 + 220);
		stext = stext + "  mov ebx,[ebp+1416]\n  mov [ebp+" + addr + "],ebx\n";
		if(perem == 1) {
			stext = stext + "  mov eax,[ebp+" + NToS(((int)size[0]-(int)'a'+1)*4) + "]\n";
			stext = stext + "  mov [ebp+" + NToS(SToN(addr)+400) + "],eax\n";
			if(array_name[0] == 'L')
				stext = stext + "  shl eax,2\n";
			else
				stext = stext + "  and eax,0xfffffff0\n  add eax,16\n";
			stext = stext + "  add [ebp+1416],eax\n";
		}
		else {
			len = SToN(size);
			if(array_name[0] == 'L')
				emc_byte = NToS(len*4);
			else
				emc_byte = NToS((len+3)/4*4);
			stext = stext + "  mov [ebp+" + NToS(SToN(addr)+400) + "],dword " + emc_byte + "\n";
			stext = stext + "  add [ebp+1416],dword " + emc_byte + "\n";
		}
		stext = stext + "  mov [ebp+" + NToS(SToN(addr)+800) + "],dword 0\n";
		stext = stext + "  call _addmem\n";
	}
	else
		Internal_Error(stext, "Ошибка в создании комплекса: \"" + NToS(index) + "\" номер комплекса должен быть >0 и <100");
	return stext; 
}

#include "CompOper.cpp"

//******************************************************************************** 
vector<string> SubProgram(string name, vector<string> functions, map<string, string> lmas, map<string, string> cmas, int mstring)
{	string sign, code, lfunc, stext, sdata="";
	vector<string> tmp, tmp1, tmp2, tmp3;
	int p, metka=1, tau=0; 
					// metka - начальный номер метки для операции повышенной размерности
					// tau - номер текущего комплекса
	sign = lmas[name];
	code = cmas[name];
	stext = name + ":\n"; Debug("подпрограмма "+name);
	while(1){ //if(name=="genecomp") Debug(code);
		tmp1 = GetStr("^[[:space:]]*$", code);
		if(tmp1[0] != "$$$") break;
		tmp1 = GetStr("^[[:space:]]*\\*\\*\\*[^\n]*\n", code);
		if(tmp1[0] != "$$$") { // Комментарий
		  code = tmp1[2];
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\/\\*.*\\*\\/", code);
		if(tmp1[0] != "$$$") { // Комментарий многострочный
		  code = tmp1[2];
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\{([^\\{\\}]+)\\}", code);
		if(tmp1[0] != "$$$") {// Ассемблерная вставка
			code = tmp1[2]; //Debug(tmp1[1]); Debug(tmp1[3]);
			stext = stext + "  " + tmp1[3] + "\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\*([a-zA-Z0-9_]+)\\(([a-zZ,[:space:]LFQ0-9. ]*\\/[a-zZ,[:space:]LF0-9. ]*)\\)", code);
		if(tmp1[0] != "$$$") {// Вызов подпрограммы
			int ff = 0, j = 0;
			string func;
			code = tmp1[2];
			lfunc = tmp1[3];
			stext = stext + Call(lfunc, tmp1[4], lmas);
			while(j < functions.size())	{
				func = functions[j];
				if(func == lfunc)	{
					ff = 1;
					break;
				}
				j++;
			}
			if(ff == 0) functions.push_back(lfunc);
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\@('[^']+')>F([0-9]+)", code); 
		if(tmp1[0] != "$$$") { // добавление строки к символьному комплексу
			int ncomp = SToN(tmp1[4]);
			code = tmp1[2];
			tmp1[3] = Replace("\\n", "',10,'", tmp1[3]);
			sdata = sdata + "_M" + NToS(mstring) + ": db " + tmp1[3] + "\n_LM"+NToS(mstring) + " equ $-_M" + NToS(mstring) + "\n";
			stext = stext + "  add [ebp+" + NToS((ncomp-1)*4+1020) + "],dword _LM" + NToS(mstring) + "\n  mov ebx,[ebp+" + NToS((ncomp-1)*4+620) + "]\n  cmp [ebp+" + NToS((ncomp-1)*4+1020) + "],ebx\n  ja _errend\n  mov ecx,_LM" + NToS(mstring) + "\n  mov esi,_M" + NToS(mstring) + "\n  mov edi,[ebp+" + NToS((ncomp-1)*4+220) + "]\n";
			stext = stext + "  add edi,[ebp+" + NToS((ncomp-1)*4+1020) + "]\n";
			stext = stext + "  sub edi,_LM" + NToS(mstring) + "\n  cld\n  rep movsb\n";
			mstring += 1;
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\/('[^']+')>C", code); 
		if(tmp1[0] != "$$$") { // Вывод строки на экран
			code = tmp1[2];
			tmp1[3] = Replace("\\n", "',10,'", tmp1[3]);
			sdata = sdata + "_M" + NToS(mstring) + ": db  " + tmp1[3] + "\n_LM"+NToS(mstring) + " equ $-_M" + NToS(mstring) + "\n";
			stext = stext + "  mov eax,4\n  mov ebx,1\n  mov ecx,_M" + NToS(mstring) + "\n  mov edx,_LM" + NToS(mstring) +"\n  int 0x80\n";
			mstring += 1;
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\/F([0-9]+)([<>])C", code);
		if(tmp1[0] != "$$$") { // Ввод/Вывод символьного комплекса
			code = tmp1[2];
			if(tmp1[4] == "<") {
				stext = stext + "  mov eax,3\n  mov ebx,0\n";
				stext = stext + "  mov ecx,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n  add ecx, [ebp+" + NToS((SToN(tmp1[3])-1)*4 + 1020) + "]\n";
			}
			else {
				stext = stext + "  mov eax,4\n  mov ebx,1\n";
				stext = stext + "  mov ecx,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n";
			}
			if(tmp1[4] == "<")
				stext = stext + "  mov edx,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 620) + "]\n";
			else
				stext = stext + "  mov edx,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 1020) + "]\n";
			stext = stext + "  int 0x80\n";
			if(tmp1[4] == "<")
				stext = stext + "  dec eax\n  add [ebp+" + NToS((SToN(tmp1[3])-1)*4 + 1020) + "],eax\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\@\\+S([0-9]+)\\(([^)]+)\\)", code);		
		if(tmp1[0] != "$$$") { // Увеличение ёмкости комплекса
			code = tmp1[2];
			stext = stext + AppendArray(tmp1[3], tmp1[4]);
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\@\\+([LF][0-9]+)\\(([0-9]+|[a-z])\\)", code); 
		if(tmp1[0] != "$$$") {  // Создание комплекса
			code = tmp1[2]; //Debug("Create Array: name = " +	tmp1[3] + " size = " + tmp1[4]);	
			stext = stext + CreateArray(tmp1[3], tmp1[4]);
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\@([-%])([LF][0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Удаление/Сокращение комплекса
			string oper;
			int ncomp = SToN(SubStr(tmp1[4],1));
			code = tmp1[2];
			if(tmp1[3] == "-")
				stext = stext + "  mov [ebp+" + NToS((ncomp-1)*4 + 220) + "],dword 0\n";
			else if(tmp1[3] == "%")
				stext = stext + "  mov ebx,[ebp+" + NToS((ncomp-1)*4 + 1020) + "]\n  mov [ebp+" + NToS((ncomp-1)*4 + 620) + "],ebx\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*\\@([<>])([LF])([0-9]+)(\\.[0-9]+|[a-z])?", code); 
		if(tmp1[0] != "$$$") { // Вставка/Извлечение элемента комплекса
			string adrnach, adrmoch, nomvstav, adremk;
//Debug("dir = "+tmp1[3]);
//Debug("type = "+tmp1[4]);
//Debug("nomer = "+tmp1[5]);
//if(name == "pramor") Debug(tmp1[1]);
			code = tmp1[2];
			adrnach = NToS((SToN(tmp1[5]) - 1)*4 + 220);
			adrmoch = NToS((SToN(tmp1[5]) - 1)*4 + 1020);
			adremk = NToS((SToN(tmp1[5]) - 1)*4 + 620);
			nomvstav = "[ebp+" + adrmoch + "]\n";
			tmp2 = GetStr("\\.[0-9]+|[a-z]", tmp1[1]);
			if(tmp2[0] != "$$$") {
//Debug("elem = "+tmp2[1]);
				if(tmp2[1][0] == '.')
					nomvstav = SubStr(tmp2[1],1)+"\n";
				else
					nomvstav = "[ebp+" + NToS(((int)tmp2[1][0] - (int)'a' + 1)*4)+"]\n";
			}
			stext = stext + "  mov ebx," + nomvstav;
			if(tmp1[3] == ">") {
				if(tmp1[4] == "L") {											// ebx - номер вносимого элемента (eax)
					stext = stext + "  shl ebx,2\n  mov ecx,[ebp+" + adrmoch + "]\n  shl ecx,2\n  cmp ecx,[ebp+" + adremk + "]\n  jae _errend\n"; 	
					stext = stext + "  mov edi,ecx\n  add edi,[ebp+" + adrnach + "]\n  mov esi,edi\n  sub esi,4\n  sub ecx,ebx\n  std\n  rep movsb\n";
					stext = stext + "  add ebx,[ebp+" + adrnach + "]\n  mov [ebx],eax\n  inc dword [ebp+" + adrmoch + "]\n";
				}
				else {
					stext = stext + "  mov ecx,[ebp+" + adrmoch + "]\n  cmp ecx,[ebp+" + adremk + "]\n  jae _errend\n"; 	
					stext = stext + "  mov edi,ecx\n  add edi,[ebp+" + adrnach + "]\n  mov esi,edi\n  dec esi\n  sub ecx,ebx\n  std\n  rep movsb\n";
					stext = stext + "  add ebx,[ebp+" + adrnach + "]\n  mov [ebx],al\n  inc dword [ebp+" + adrmoch + "]\n";
				}
			}
			else	{			
				if(tmp1[4] == "L") { 										// ebx - номер извлекаемого элемента (в eax)
					stext = stext + "  shl ebx,2\n  mov edi,ebx\n  add edi,[ebp+" + adrnach + "]\n  mov eax,[edi]\n  mov ecx,[ebp+" + adrmoch + "]\n  shl ecx,2\n"; 
					stext = stext + "  mov esi,edi\n  add esi,4\n  sub ecx,ebx\n  cld\n  rep movsb\n  dec dword [ebp+" + adrmoch + "]\n";
				}			
				else {
					stext = stext + "  mov edi,ebx\n  add edi,[ebp+" + adrnach + "]\n  xor eax,eax\n  mov al,[edi]\n  mov ecx,[ebp+" + adrmoch + "]\n"; 	
					stext = stext + "  mov esi,edi\n  inc esi\n  sub ecx,ebx\n  cld\n  rep movsb\n  dec dword [ebp+" + adrmoch + "]\n";
				}		
			}
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*§([0-9]+)", code);  
		if(tmp1[0] != "$$$") { // Параграф
			code = tmp1[2];
			stext = stext + ".P" + tmp1[3] + ":\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*→([0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Безусловный переход
			code = tmp1[2];
			stext = stext + "  jmp .P" + tmp1[3] + "\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*↪([0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Переход по нулю
			code = tmp1[2];
			stext = stext + "  and eax,eax\n  jz .P" + tmp1[3] + "\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*↦([0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Переход по единице
			code = tmp1[2];
			stext = stext + "  and eax,eax\n  jnz .P" + tmp1[3] + "\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*↑\\([[:space:]]*([^<>=≠≥≤]+)[[:space:]]*(<|>|=|≠|≥|≤)[[:space:]]*([^)]+)[[:space:]]*\\)([0-9]+)", code);
		if(tmp1[0] != "$$$") {// Переход по отношению
//Debug(tmp1[1]+" | "+tmp1[3]+" | "+tmp1[4]+" | "+tmp1[5]+" | "+tmp1[6]);
			code = tmp1[2];
			if((tmp1[4] != "≥") && (tmp1[4] != "≤") && (tmp1[4] != ">") && (tmp1[4] != "<") && (tmp1[4] != "=") && (tmp1[4] != "≠"))
				Internal_Error(stext, "\"" + tmp1[1] + "\" - некорректное выражение");
			stext = stext + CmpJmp(tmp1[3], tmp1[4], tmp1[5], tmp1[6]);
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇻([0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Уход
			code = tmp1[2];
			stext = stext + "  call .P" + tmp1[3] + "\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇞", code); 
		if(tmp1[0] != "$$$") { // Возврат
			code = tmp1[2];
			stext = stext + "  ret\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*↑X([0-9]+)([a-z])([a-z])", code);  // Переход по перебору единиц
		if(tmp1[0] != "$$$") { // Переход по перебору единиц
			code = tmp1[2];
			stext = stext + "  mov eax,[ebp+" + NToS(GetAddr(tmp1[4])) + "]\n  and eax,eax\n  jz .P" + tmp1[3] + "\n  mov edx,eax\n";
			stext = stext + "  dec edx\n  mov esi,eax\n  and esi,edx\n  mov [ebp+" + NToS(GetAddr(tmp1[4])) + "],esi\n  xor eax,edx\n";
			stext = stext + "  xor ecx,ecx\n  mov ebx,_vesa\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  shr eax,8\n";
			stext = stext + "  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  mov eax,ecx\n  dec ecx\n  mov [ebp+" + NToS(GetAddr(tmp1[5])) + "],ecx\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*(\\+|\\-|\\*|∨|&|⊕|<|>|:|/|;)[[:space:]]*(([a-fA-F0-9]+h)|([01]+b)|(0[0-7]+)|([0-9]+)|('.'))", code);
		if(tmp1[0] != "$$$"){ // операция с константой
			string res; // значение константы
			unsigned long k=0;
//if(name == "pramor") Debug(tmp1[1]);
			code = tmp1[2]; 
			if(tmp1[4][strlen(tmp1[4].c_str())-1] == 'h')	
				res = "0"+tmp1[4];
			else if(tmp1[4][0] == '0') {
				sscanf(tmp1[4].c_str(),"%lo",&k);
				res = NToS(k);
			}
			else if(tmp1[4][0] == '\'')
				res = NToS((int)tmp1[4][1]);
			else
				res = tmp1[4];
			if(tmp1[3] == "+")
				stext = stext + "  add eax," + res + "\n";
			else if(tmp1[3] == "-")
				stext = stext + "  sub eax," + res + "\n";
			else if(tmp1[3] == "*")
				stext = stext + "  mov edx," + res + "\n  mul edx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == "∨")
				stext = stext + "  or eax," + res + "\n";
			else if(tmp1[3] == "&")
				stext = stext + "  and eax," + res + "\n";
			else if(tmp1[3] == "⊕")
				stext = stext + "  xor eax," + res + "\n";
			else if(tmp1[3] == "<")
				stext = stext + "  shl eax,byte " + res + "\n";
			else if(tmp1[3] == ">")
				stext = stext + "  shr eax,byte " + res + "\n";
			else if(tmp1[3] == ":")
				stext = stext + "  mov ebx," + res + "\n  mov edx,[ebp+108]\n  div ebx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == "/")
				stext = stext + "  mov ebx," + res + "\n  xor edx,edx\n  div ebx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == ";")
				stext = stext + "  mov ebx," + res + "\n  xor edx,edx\n  div ebx\n  mov [ebp+108],eax\n  mov eax,edx\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*(([a-fA-F0-9]+h)|([01]+b)|(0[0-7]+)|([0-9]+)|('.'))", code);
		if(tmp1[0] != "$$$"){ // константа
			string res; // значение константы
			unsigned long k=0;
//Debug("константа = "+	tmp1[3]);			
			code = tmp1[2]; 
			if(tmp1[3][strlen(tmp1[3].c_str())-1] == 'h')	
				res = "0"+tmp1[3];
//			else if(tmp1[3][strlen(tmp1[3].c_str())-1]  == 'b')	{
//				for(int i=0; i<strlen(tmp1[3].c_str())-1;i++,k<<=1)
//					if(tmp1[3][i] == '1') ++k;
//				res = NToS(k);
//			}
			else if(tmp1[3][0] == '0') {
				sscanf(tmp1[3].c_str(),"%lo",&k);
				res = NToS(k);
			}
			else if(tmp1[3][0] == '\'')
				res = NToS((int)tmp1[3][1]);
			else
				res = tmp1[3];
//Debug("константа = "+	res);			
			stext = stext + "  mov eax," + res + "\n";
			tau=0;
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*(⇔|⇒|O|∆|∇|⁻|\\+|\\-|\\*|∨|&|⊕|<|>|:|/|;|)[[:space:]]*(([a-zZ])|(I[0-9]+)|(I[a-z])|([QS][0-9]+)|([QS][a-zZ])|([LF][0-9]+[a-z])|([LF][0-9]+\\([a-z][\\+\\-][0-9]+\\))|([LF][0-9]+\\.[0-9]+))", code);
		if(tmp1[0] != "$$$") { //операция с переменной
			code = tmp1[2];//if(name == "pramor") Debug(tmp1[1]);
			if(tmp1[3] == "") {
				stext = stext + SetReg(tmp1[4], "mov", "eax");
				tau=0;
			}
			else if(tmp1[3] == "⇔")
				stext = stext  + SetReg(tmp1[4],"mov","edx")+ StoreReg(tmp1[4], "mov","eax")+ "  mov eax,edx\n  ";
			else if(tmp1[3] == "⇒")
				stext = stext + StoreTau(tmp1[4], "mov");
			else if(tmp1[3] == "∆")	
				stext = stext + SetReg(tmp1[4], "mov","eax") + "  inc eax\n" + StoreTau(tmp1[4], "mov");
			else if(tmp1[3] == "∇")	
				stext = stext + SetReg(tmp1[4], "mov","eax") + "  dec eax\n" + StoreTau(tmp1[4], "mov");
			else if(tmp1[3] == "O")	
				stext = stext + "  xor eax,eax\n" + StoreTau(tmp1[4], "mov");
			else if(tmp1[3] == "⁻")
				stext = stext + "  mov eax,0xffffffff\n" + StoreTau(tmp1[4], "mov");
			else if(tmp1[3] == "+")
				stext = stext + SetReg(tmp1[4], "add", "eax");
			else if(tmp1[3] == "-")
				stext = stext + SetReg(tmp1[4], "sub", "eax");
			else if(tmp1[3] == "*")
				stext = stext + SetReg(tmp1[4], "mov", "ebx") + "  mul ebx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == "∨")
				stext = stext + SetReg(tmp1[4], "or", "eax");
			else if(tmp1[3] == "&")
				stext = stext + SetReg(tmp1[4], "and", "eax");
			else if(tmp1[3] == "⊕")
				stext = stext + SetReg(tmp1[4], "xor", "eax");
			else if(tmp1[3] == "<")
				stext = stext + SetReg(tmp1[4], "mov", "cl") + "  shl eax,cl\n";
			else if(tmp1[3] == ">")
				stext = stext + SetReg(tmp1[4], "mov", "cl") + "  shr eax,cl\n";
			else if(tmp1[3] == ":")
				stext = stext + "  mov edx,[ebp+108]\n" + SetReg(tmp1[4], "mov", "ebx") + "  div ebx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == "/")
				stext = stext + "  xor edx,edx\n" + SetReg(tmp1[4], "mov", "ebx") + "  div ebx\n  mov [ebp+108],edx\n";
			else if(tmp1[3] == ";")
				stext = stext + "  xor edx,edx\n" + SetReg(tmp1[4], "mov", "ebx") + "  div ebx\n  push eax\n  push edx\n  pop eax\n  pop dword[ebp+108]\n";
			continue;
		}
		
		tmp1 = GetStr("^[[:space:]]*(⇒|∆|∇|⁻|\\+|\\-|\\*|∨|&|⊕|<|>|:|/|;|⇕/|)[[:space:]]*L([0-9]+)", code);
		if(tmp1[0] != "$$$") { //операция с комплексом
			code = tmp1[2];// Debug(tmp1[1]);
			if(tmp1[3] == "") { //копирование комплекса в тау
				tau=SToN(tmp1[4]);
				stext = stext + "  mov ecx,[ebp+" + NToS(((tau-1)*4)+1020) + "]\n  mov [ebp+112],ecx\n";
				stext = stext + "  mov esi,[ebp+" + NToS(((tau-1)*4)+220) + "]\n  mov edi,ebp\n  sub edi,64\n";
				stext = stext + ".M"+NToS(metka)+":\n  mov eax,[esi]\n  mov [edi],eax\n  add esi,4\n  sub edi,4\n  loop .M"+NToS(metka)+"\n";
				metka++;
				}
			else if((tmp1[3] == "⇒") && (tau != 0)) { // tau⇒L2
				stext = stext + "  mov ecx,[ebp+112]\n  shl ecx,2\n  cmp [ebp+620+" + NToS(SToN(tmp1[4])-1) + "*4],ecx\n  jb _errend\n";
				stext = stext + "  mov esi,ebp\n   sub esi,64\n  mov edi,[ebp+220+" + NToS(SToN(tmp1[4])-1) + "*4]\n  shr ecx,2\n";
				stext = stext + ".M"+NToS(metka)+":\n  mov eax,[esi]\n  mov [edi],eax\n  sub esi,4\n  add edi,4\n  loop .M"+NToS(metka)+"\n";
				stext = stext + "  mov ecx,[ebp+112]\n  mov [ebp+1020+" + NToS(SToN(tmp1[4])-1) + "*4],ecx\n";
				metka++;
				}
			else if((tmp1[3] == "+") && (tau != 0)) 
				stext = stext + tauaddcomp(tmp1[4], metka);
			else if((tmp1[3] == "-") && (tau != 0)) 
				stext = stext + tausubcomp(tmp1[4], metka);
			else if((tmp1[3] == "*") && (tau != 0)) 
				stext = stext + taumulcomp(tmp1[4], metka);
			else if((tmp1[3] == "/") && (tau != 0)) 
				stext = stext + taudivcomp(tmp1[4], metka);
			else if((tmp1[3] == ";") && (tau != 0)) 
				stext = stext + taumodcomp(tmp1[4], metka);
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*O([LF][0-9]+)", code); 
		if(tmp1[0] != "$$$") { // Обнуление комплекса
			string oper;
			int ncomp = SToN(SubStr(tmp1[3],1));
			code = tmp1[2];
			if(tmp1[3][0] == 'L')	{
				stext = stext + "  xor eax,eax\n";
				oper = "  rep stosd\n";
			}
			else {
				stext = stext + "  xor al,al\n";
				oper = "  rep stosb\n";
			}
			stext = stext + "  mov ecx,[ebp+" + NToS((ncomp-1)*4 + 1020) + "]\n";
			stext = stext + "  mov edi,[ebp+" + NToS((ncomp-1)*4 + 220) + "]\n  cld\n" + oper;
			continue;
		}
    tmp1 = GetStr("^[[:space:]]*⇒[[:space:]]*X", code); 
		if(tmp1[0] != "$$$") { // Присвоение генератору случайного значения начального значения
			code = tmp1[2];
			stext = stext + "  mov [_rand],eax\n";
			continue;
		}
    tmp1 = GetStr("^[[:space:]]*X", code); 
		if(tmp1[0] != "$$$") { // Присвоение собственной переменной случайного значения
			code = tmp1[2];
			stext = stext + "  mov eax,[_rand]\n  mov edx,97781173\n  mul edx\n  add eax,800001\n  mov [_rand],eax\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*T", code);
		if(tmp1[0] != "$$$") { // Установка значения собственной переменной в зависимости от времени
			code = tmp1[2];
			stext = stext + "  mov eax,13\n  mov ebx,0\n  int 0x80\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*%", code);
		if(tmp1[0] != "$$$") { // Взвешивание
			code = tmp1[2];
			stext = stext + "  xor ecx,ecx\n  mov ebx,_vesa\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  mov eax,ecx\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*¬", code);
		if(tmp1[0] != "$$$") { // Инверсия
			code = tmp1[2];
			stext = stext + "  not eax\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*!", code);
		if(tmp1[0] != "$$$") { // Номер младшей единицы
			code = tmp1[2];
			stext = stext + "  mov ebx,eax\n  dec eax\n  xor eax,ebx\n";
			stext = stext + "  xor ecx,ecx\n  mov ebx,_vesa\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  shr eax,8\n  xlat\n  add cl,al\n  dec cl\n  mov eax,ecx\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*([a-z])⇔([a-z])", code); // Обмен значениями переменных
		if(tmp1[0] != "$$$") {
			code = tmp1[2];
			stext = stext + "  mov esi,[ebp+" + NToS(((int)tmp1[3][0] - (int)'a' + 1)*4) + "]\n  mov edi,[ebp+" + NToS(((int)tmp1[3][1] - (int)'a' + 1)*4) + "]\n";
			stext = stext + "  push dword [esi]\n  push dword [edi]\n  pop dword [edi]\n  pop dword [esi]\n";
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇔\\(([LF][0-9]+)([a-z][a-z])\\)", code);
		if(tmp1[0] != "$$$") { // Обмен элементов комплекса =(L1ab)
			code = tmp1[2];
			string nachkom = NToS((SToN(SubStr(tmp1[3], 1)) - 1)*4 + 220);
			if(tmp1[3][0] == 'F')	{
				stext = stext + "  mov esi,[ebp+" + NToS(((int)tmp1[4][0] - (int)'a' + 1)*4) + "]\n  add esi,[ebp+" + nachkom + "]\n  mov edi,[ebp+" + NToS(((int)tmp1[4][1] - (int)'a' + 1)*4) + "]\n  add edi,[ebp+" + nachkom + "]\n";
				stext = stext + "  mov al,[esi]\n  mov bl,[edi]\n  mov [esi],bl\n  mov [edi],al\n";
			}
			else {
				stext = stext + "  mov ebx,[ebp+" + NToS(((int)tmp1[3][0] - (int)'a' + 1)*4) + "]\n  mov edx,[ebp+" + nachkom + "]\n  lea esi,[edx+ebx*4]\n  mov ebx,[ebp+" + NToS(((int)tmp1[4][1] - (int)'a' + 1)*4) + "]\n  mov edx,[ebp+" + nachkom + "]\n  lea edi,[edx+ebx*4]\n";
				stext = stext + "  push dword [esi]\n  push dword [edi]\n  pop dword [esi]\n  pop dword [edi]\n";
			}
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇔\\(([LF][0-9]+)\\.([0-9]+)([a-z])\\)", code);
		if(tmp1[0] != "$$$") { // Обмен элементов комплекса =(L1.4b)
			code = tmp1[2];
			string nachkom = NToS((SToN(SubStr(tmp1[3], 1)) - 1)*4 + 220);
			if(tmp1[3][0] == 'F')	{
				stext = stext + "  mov esi,[ebp+" + nachkom + "]\n  add esi," + tmp1[4] + "\n  mov edi,[ebp+" + NToS(((int)tmp1[5][0] - (int)'a' + 1)*4) + "]\n  add edi,[ebp+" + nachkom + "]\n";
				stext = stext + "  mov al,[esi]\n  mov bl,[edi]\n  mov [esi],bl\n  mov [edi],al\n";
			}
			else {
				stext = stext + "  mov esi,[ebp+" + nachkom + "]\n  add esi," + tmp1[4] + "*4\n  mov ebx,[ebp+" + NToS(((int)tmp1[5][0] - (int)'a' + 1)*4) + "]\n  mov edx,[ebp+" + nachkom + "]\n  lea edi,[edx+ebx*4]\n";
				stext = stext + "  push dword [esi]\n  push dword [edi]\n  pop dword [esi]\n  pop dword [edi]\n";
			}
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇔\\(([LF][0-9]+)([a-z])([0-9]+)\\)", code);
		if(tmp1[0] != "$$$") { // Обмен элементов комплекса =(L1b4)
			code = tmp1[2];
			string nachkom = NToS((SToN(SubStr(tmp1[3], 1)) - 1)*4 + 220);
			if(tmp1[3][0] == 'F')	{
				stext = stext + "  mov esi,[ebp+" + NToS(((int)tmp1[4][0] - (int)'a' + 1)*4) + "]\n  add esi,[ebp+" + nachkom + "]\n  mov edi,[ebp+" + nachkom + "]\n  add edi," + tmp1[5] + "\n";
				stext = stext + "  mov al,[esi]\n  mov bl,[edi]\n  mov [esi],bl\n  mov [edi],al\n";
			}
			else {
				stext = stext + "  mov ebx,[ebp+" + NToS(((int)tmp1[4][0] - (int)'a' + 1)*4) + "]\n  mov edx,[ebp+" + nachkom + "]\n  lea esi,[edx+ebx*4]\n  mov edi,[ebp+" + nachkom + "]\n  add edi," + tmp1[5] + "*4\n";
				stext = stext + "  push dword [esi]\n  push dword [edi]\n  pop dword [esi]\n  pop dword [edi]\n";
			}
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*⇔\\(([LF][0-9]+)\\.([0-9]+)\\.([0-9]+)\\)", code);
		if(tmp1[0] != "$$$") { // Обмен элементов комплекса =(L1.4.1)
			code = tmp1[2];
			string nachkom = NToS((SToN(SubStr(tmp1[3], 1)) - 1)*4 + 220);
			if(tmp1[3][0] == 'F')	{
				stext = stext + "  mov esi,[ebp+" + nachkom + "]\n add esi," + tmp1[4] + "\n  mov edi,[ebp+" + nachkom + "]\n add edi," + tmp1[5] + "\n";
				stext = stext + "  mov al,[esi]\n  mov bl,[edi]\n  mov [esi],bl\n  mov [edi],al\n";
			}
			else {
				stext = stext + "  mov esi,[ebp+" + nachkom + "]\n  add esi," + tmp1[4] + "*4\n  mov edi,[ebp+" + nachkom + "]\n  add edi," + tmp1[5] + "*4\n";
				stext = stext + "  push dword [esi]\n  push dword [edi]\n  pop dword [esi]\n  pop dword [edi]\n";
			}
			continue;
		}
		tmp1 = GetStr("^[[:space:]]*@#L([0-9]+)L([0-9]+)\\(([^\\)]*)\\)", code);
		if(tmp1[0] != "$$$") { //Копирование @#L1L2(h1,h2,h3) :: ecx=h1; esi=A1+h2; edi=A2+h3; if(h3+h1<=S2)  {cld; rep movsb}
//Debug("tmp[1]=" + tmp1[1]);
//Debug("tmp[2]=" +tmp1[2]);
//Debug("tmp[3]=" + tmp1[3] + " tmp[4]="+tmp1[4] + " tmp[5]="+tmp1[5]);
			code = tmp1[2];
			tmp2 = GetStr("^[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
			if(tmp2[0] != "$$$") { // h1
				stext = stext + SetReg(tmp2[3],"mov","ecx") + "  shl ecx, byte 2\n  mov esi,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n";
				tmp1[5] = tmp2[2];
				tmp2 = GetStr("^,[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
				if(tmp2[0] != "$$$") { // h2
					stext = stext + SetReg(tmp2[3],"mov","ebx") + "  shl ebx, byte 2\n  add esi,ebx\n";
					stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
					tmp1[5] = tmp2[2];
					tmp2 = GetStr("^,[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
					if(tmp2[0] != "$$$")  // h3
						stext = stext + SetReg(tmp2[3],"mov","ebx") + "  shl ebx, byte 2\n  add edi,ebx\n";
					else { // h3=Q2
						stext = stext + SetReg("Q"+tmp1[4],"add","edi");
//						Debug("h3 = Q(L2) tmp[5]=|"+tmp1[5]+"|");
						tmp2 = GetStr("^$",tmp1[5]);
						if(tmp2[0] == "$$$") 
							Internal_Error(stext, "Неизвестная конструкция: " + tmp1[1]);
					}
				}
				else { // h2=0 h3=Q2
					stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
					stext = stext + SetReg("Q"+tmp1[4],"mov","ebx") + "  shl ebx, byte 2\n  add edi,ebx\n";
					tmp2 = GetStr("^$",tmp1[5]);
					if(tmp2[0] == "$$$") 
						Internal_Error(stext, "Неизвестная конструкция: " + tmp1[1]);
				}		
			}
			else {// h1=Q1 h2=0 h3=Q2
				stext = stext + SetReg("Q" + tmp1[3],"mov","ecx") + "  shl ecx, byte 2\n  mov esi,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n";
				stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
				stext = stext + SetReg("Q"+tmp1[4],"mov","ebx") + "  shl ebx, byte 2\n  add edi,ebx\n";
			}
			stext = stext + "  add ebx,ecx\n" + SetReg('S'+tmp1[4],"mov","edx") + "  cmp ebx,edx\n  ja _errend\n  cld\n  rep movsb\n";
			continue;
		} 
		tmp1 = GetStr("^[[:space:]]*@#F([0-9]+)F([0-9]+)\\(([^\\)]*)\\)", code);
		if(tmp1[0] != "$$$") { //Копирование @#F1F2(h1,h2,h3) :: ecx=h1; esi=A1+h2; edi=A2+h3; if(h3+h1<=S2)  {cld; rep movsb}
//Debug("tmp[1]=" + tmp1[1]);
//Debug("tmp[2]=" +tmp1[2]);
//Debug("tmp[3]=" + tmp1[3] + " tmp[4]="+tmp1[4] + " tmp[5]="+tmp1[5]);
			code = tmp1[2];
			tmp2 = GetStr("^[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
			if(tmp2[0] != "$$$") { // h1
				stext = stext + SetReg(tmp2[3],"mov","ecx") + "  mov esi,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n";
				tmp1[5] = tmp2[2];
				tmp2 = GetStr("^,[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
				if(tmp2[0] != "$$$") { // h2
					stext = stext + SetReg(tmp2[3],"mov","ebx") + "  add esi,ebx\n";
					stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
					tmp1[5] = tmp2[2];
					tmp2 = GetStr("^,[[:space:]]*(([0-9]+)|([a-z]))",tmp1[5]);
					if(tmp2[0] != "$$$")  // h3
						stext = stext + SetReg(tmp2[3],"mov","ebx") + "  add edi,ebx\n";
					else { // h3=Q2
						stext = stext + SetReg("Q"+tmp1[4],"add","edi");
//						Debug("h3 = Q(L2) tmp[5]=|"+tmp1[5]+"|");
						tmp2 = GetStr("^$",tmp1[5]);
						if(tmp2[0] == "$$$") 
							Internal_Error(stext, "Неизвестная конструкция: " + tmp1[1]);
					}
				}
				else { // h2=0 h3=Q2
					stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
					stext = stext + SetReg("Q"+tmp1[4],"mov","ebx") + "  add edi,ebx\n";
					tmp2 = GetStr("^$",tmp1[5]);
					if(tmp2[0] == "$$$") 
						Internal_Error(stext, "Неизвестная конструкция: " + tmp1[1]);
				}		
			}
			else {// h1=Q1 h2=0 h3=Q2
				stext = stext + SetReg("Q" + tmp1[3],"mov","ecx") + "  mov esi,[ebp+" + NToS((SToN(tmp1[3])-1)*4 + 220) + "]\n";
				stext = stext + "  mov edi,[ebp+"+ NToS((SToN(tmp1[4])-1)*4 + 220) + "]\n";
				stext = stext + SetReg("Q"+tmp1[4],"mov","ebx") + "  add edi,ebx\n";
			}
			stext = stext + "  add ebx,ecx\n" + SetReg('S'+tmp1[4],"mov","edx") + "  cmp ebx,edx\n  ja _errend\n  cld\n  rep movsb\n";
			continue;
		} 
		tmp1 = GetStr("^[[:space:]]*\\*\\*", code); // Конец программы
		if(tmp1[0] != "$$$") 	break;
//Debug(code);
		tmp1 = GetStr("^[[:space:]]*", code);
		tmp1 = GetStr("[ ]", tmp1[2]);
		Internal_Error(stext, "Неизвестная конструкция: " + tmp1[0]);
	}
	stext = stext + "  ret\n"; //Debug("SubProgram stext="+stext);
	p = functions.size();
	functions.resize(p+3);
	functions[p] = stext;
	functions[p+1] = sdata;
	functions[p+2] = NToS(mstring);
	return functions;
} //SubProgram

//******************************************************************************** 
string Compil(string code)
{ vector<string> tmp, functions;
  map<string, string> lmas, cmas;
  string name, source, stext, sdata;
  int mstring = 1, i;
  tmp = Split(code);// Debug(tmp[1]);
  functions.push_back(tmp[1]);
  for(i=1;i<tmp.size(); i+=3) {
    name = tmp[i]; //Debug(name);
	  if(lmas.find(name) != lmas.end()) continue;
	  lmas[name] = tmp[i+1]; //Debug(tmp[i+1]);
	  cmas[name] = tmp[i+2]; //Debug(tmp[i+2]);
  }
  sdata = "section .data\n_sizebim dd 0\n_rand dd 0xa1248aa9\n\
_vesa:\n\
  db 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4\n\
  db 1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5\n\
  db 1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7\n\
  db 1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7\n\
  db 2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6\n\
  db 3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7\n\
  db 3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7\n\
  db 4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8\n\n\
_I:\n\
  dd 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000\n\
  dd 0x10000,0x20000,0x40000,0x80000,0x100000,0x200000,0x400000,0x800000\n\
  dd 0x1000000,0x2000000,0x4000000,0x8000000,0x10000000,0x20000000,0x40000000,0x80000000\n";
  stext = "global _start\nsection .text\n_start:\n  mov eax,45\n  xor ebx,ebx\n  int 0x80\n\
  mov [_sizebim],eax\n  push eax\n  sub esp,1412\n  push ebp\n  mov ebp,esp\n";
  stext = stext + "  call " + functions[0] + "\n  mov eax,1\n  mov ebx,0\n  int 0x80\n";
  for(int i = 0; i < functions.size(); i++)	{
//Debug("Compil name = "+functions[i]+" mstring ="+NToS(mstring));  
	  if(lmas.find(functions[i]) == lmas.end()) { printf("нет программы %s\n", functions[i].c_str()); exit(1);}
	  functions = SubProgram(functions[i], functions, lmas, cmas, mstring);
	  mstring = SToN(functions[functions.size() - 1]); //Debug("mstring="+NToS(mstring));
	  sdata = sdata + functions[functions.size() - 2]; //Debug("sdata="+sdata);
	  stext = stext + functions[functions.size() - 3]; //Debug("stext="+stext);
	  functions.erase(functions.end()-3,functions.end());
	  //if(mstring == 0)
	    //break;
  }
  stext = stext + "_addmem:\n  push eax\n  push ebx\n  mov ebx,[ebp+1416]\n  cmp ebx,[_sizebim]\n\
  jbe .end\n  sub ebx,[_sizebim]\n  add ebx,1000h\n  add ebx,[_sizebim]\n  mov eax,45\n  int 0x80\n\
  and eax,eax\n  jnz ._errend\n  mov eax,1\n  mov ebx,1\n  int 0x80\n._errend:\n  mov [_sizebim],ebx\n.end:\n  pop ebx\n  pop eax\n  ret\n\
_errend:\n  mov eax,1\n  mov ebx,1\n  int 0x80\n";
  stext = stext + sdata;
  return stext;
} //Compil

//******************************************************************************** 
int main(int argc, char *argv[])
{ int i;
	if(argc!=2) {
    printf("Запуск: %s <Л-файл>\n", argv[0]); 
    return 0;
  }
  for(i=0;i<strlen(argv[1]); i++)
  	if(argv[1][i]=='.') break;
  strncpy(FileName,argv[1],400); 
  FileName[i] = '\0'; //Debug("imja faila = "+FileName);
	string code = ReadFile(argv[1]);
  code = code+ReadFile("libl0.l"); //Debug(code);
	string text=Compil(code);
	ExecF(text);
	return 0;
}

