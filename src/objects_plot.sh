#!/usr/bin/gnuplot
reset
set encoding utf8

set terminal svg size 768, 512 font ',20' lw 2
set output 'plot.svg' 

set size ratio 0.61803

set ylabel "Время, мс" 
set xlabel "Количество объектов" 

set grid

set title "Время отрисовки изображения"

plot "../data.txt" using 1:2 with lp pt 8 dt 3  title "Время"
 