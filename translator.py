#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
from datetime import datetime





def old_lyapas_sintax_replace(file_from, file_to):
	arrow_up = "↑"
	arrow_right = "→"
	not_eq_zero = '↦'
	eq_zero = '↪'
	dub_arrow_right = "⇒"
	paragraph = "§"
	more_or_eq = "≥"
	less_or_eq = "≤"
	not_eq = "≠"
	triangle_up = "∆"
	triangle_down = "∇"
	much_greater = "≫"
	much_less = "≪"
	disunction = "∨"
	xor = "⊕"
	reverse = "¬"
	countStars = 0
	x = file_from.read(1)
	while x != '':
		if x == '\'':
			file_to.write(x)
			x = file_from.read(1)
			while x != '\'':
				file_to.write(x)
				x = file_from.read(1)
			file_to.write(x)
			x = file_from.read(1)
		elif x == '?':
			prev = x
			x = file_from.read(1)
			if x == '=':
				file_to.write(arrow_right)
				x = file_from.read(1)
			elif x == '(':
				file_to.write(arrow_up)
			elif x == '+' or x == '-':
				if x == '+':
					x = not_eq_zero
				else:
					x = eq_zero
				file_to.write(x)
				x = file_from.read(1)
			else:
				file_to.write(prev)
				file_to.write(x)
				x = file_from.read(1)
		elif x == '(':
			file_to.write(x)
			x = file_from.read(1)
			while x != ')':
				if x == '<' or x == '>':
					prev = x
					x = file_from.read(1)
					if x == '=':
						if prev == '>':
							file_to.write(more_or_eq)
							x = file_from.read(1)
						if prev == '<':
							file_to.write(less_or_eq)
							x = file_from.read(1)
					else:
						file_to.write(prev)
						file_to.write(x)
						x = file_from.read(1)
				elif x == '#':
					file_to.write(not_eq)
					x = file_from.read(1)
				else:
					file_to.write(x)
					x = file_from.read(1)
			file_to.write(x)
			x = file_from.read(1)
		elif x == '=':
			prev = x
			x = file_from.read(1)
			if x == '(':
				file_to.write(prev)
			else:
				file_to.write(dub_arrow_right)
				file_to.write(x)
				x = file_from.read(1)
		#elif x == '/':
			#file_to.write(x)
			#x = file_from.read(1)
			#if x == '\'':
				#file_to.write(x)
				#x = file_from.read(1)
				#while x != '\'':
					#file_to.write(x)
					#x = file_from.read(1)
		elif x == 'D' or x == 'Y' or x == 'P':
			prev = x
			prev2 = file_from.read(1) #x = file_from.read(1)
			#prev2 = x
			x = file_from.read(1)
			if x == ' ' or x == '\n' or x == '\t' or x == "=" or x == '⇒':
				if prev2 > '`' and prev2 < '{':
					if prev == 'D':
						prev = triangle_up
					elif prev == 'Y':
						prev = triangle_down
			if prev2 > '0' and prev2 < ':':
				prev = paragraph
			file_to.write(prev)
			file_to.write(prev2)
		elif x == '|':
			file_to.write(disunction)
			x = x = file_from.read(1)
		elif x == '^':
			file_to.write(xor)
			x = x = file_from.read(1)
		elif x == '~':
			file_to.write(reverse)
			x = x = file_from.read(1)
		elif x == '*':
			while x == '*':
				countStars += 1
				file_to.write(x)
				x = file_from.read(1)
				if countStars == 3:
					while x != '\n':
						file_to.write(x)
						x = file_from.read(1)
					file_to.write(x)
					x = file_from.read(1)
			countStars = 0
		else:
			file_to.write(x)
			x = file_from.read(1)
	
	
	
	
	
	
	
	














if len(sys.argv) < 2:
	file_name = input("Первым аргументом должно быть название программы, введите его сейчас: ")
else:
	file_name = sys.argv[1]

new_file_name = ""
"""
for i in range(len(file_name)):
	if file_name[i] == '.':
		time = str(datetime.now())
		time = time.replace(' ', '.')
		time = time.replace(':', '.')
		time = time.replace('-', '.')
		time = time[len(time)-6:]
		new_file_name = new_file_name + '_' + time
	new_file_name = new_file_name + file_name[i]
"""
new_file_name = file_name[:len(file_name)-2] + '_' + file_name[len(file_name)-2:]



fread = open(file_name, 'rt')
fwrite = open(new_file_name, 'wt')


old_lyapas_sintax_replace(fread, fwrite)









fread.close()
fwrite.close()





