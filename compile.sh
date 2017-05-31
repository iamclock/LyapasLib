#! /bin/bash

# скрипт запускает транслятор со старых символов на новые файл name_.l
# затем компилирует выходной файл
# и удаляет промежуточные файлы .s _.l




if [ $# -eq 0 ]
	then
		echo -n "Enter file name by first argument: "
		#var file_name
		read file_name
else
	file_name=$1
fi
var=`basename $file_name .l`
echo "./for_compile/translator.py $file_name"
echo "./for_compile/ltc $var\"_.l\""
rm $var"_.s" $var"_.l"

