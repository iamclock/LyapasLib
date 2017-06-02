#! /bin/bash


# скрипт копирует нужные ссылки в папку с проектом на ляпасе




if [ $# -eq 0 ]
	then
		echo -n "Enter path to project by first argument: "
		#var file_name
		read path
else
	path=$1
fi


mkdir $path/for_compile
currentPath=`pwd`

ln -s $currentPath/{ltc,translator.py,BN_comparison_ops.l} $path/for_compile
ln -s $currentPath/{libl0.l,allbn.o} $path
cp ./compile.sh $path
