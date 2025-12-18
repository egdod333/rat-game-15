#!/bin/bash
#-n=no libs
#-l=only libs
#-e=editor
cmd=""
NCURSES_PATH="/opt/c9/ncurses-6.3/"
echo "">log.txt
args=$1;args=${args:1}
while read -n1 i;do
  if   test "$i" = "m" ;then
    echo Compiling main program
    gcc ./main.c -o main.out -I/opt/c9/ncurses-6.3/include -L/opt/c9/ncurses-6.3/lib/ -lm -I./lib -L./lib -l:util.a -l:r@@ui.a -l:assets.a -lncurses 2>log.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Main program compilation error!
      cat log.txt
      exit 1
    fi
    echo Running main program
    cmd+="./main.out ${@:2};"
  elif test "$i" = "e" ;then
    echo Compiling asset editor
    gcc ./lib/tools/editor.c -o ./lib/tools/editor.out -I/opt/c9/ncurses-6.3/include -L/opt/c9/ncurses-6.3/lib/ -lm -I./lib -L./lib -l:util.a -l:r@@ui.a -l:assets.a -lncurses 2>log.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Asset editor compilation error!
      cat log.txt
      exit 1
    fi
    echo Running asset editor
    cmd+="./lib/tools/editor.out ${@:2};"
  elif test "$i" = "l";then
    rm ./lib/*.a 2>/dev/null
    echo Compiling libraries
    while read file; do
      #file=line in order.txt
      libs=($file)
      #libs=list of tokens in line
      echo "  - ${libs[0]}"
      for i in "${!libs[@]}"; do
      #indexing by @ returns every element, using ! takes array of that element
        if test ! $i -eq 0;then
          libs[$i]=-l${libs[$i]}
          #for each element past 0 prefix -l
          if test ${libs[$i]} = "-lncurses";then
            libs+=("-L${NCURSES_PATH}libs")
            libs+=(" -I${NCURSES_PATH}include")
            #parentheses are important because appending a string fucks it up :/
          fi
        fi
      done
      gcc -c "./lib/src/${libs[0]}.c" -o "./lib/src/${libs[0]}.o" -I./lib -L./lib ${libs[@]:1} 2>>log.txt
      ar rcs ./lib/${libs[0]}.a ./lib/src/${libs[0]}.o ./lib/${libs[0]}.h 2>>log.txt
    done <./lib/src/order.txt
    if test $(stat -c%s ./log.txt) -gt 1; then
      echo Library error!
      cat log.txt
      exit 1
    fi
  elif test "$i" = "r";then
    exit
  fi
done < <(echo "$args")
bash -c "$cmd"
exit