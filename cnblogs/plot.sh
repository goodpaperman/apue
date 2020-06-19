#! /usr/bin/gnuplot
#gnuplot   #使用gnuplot绘图
set terminal png size 1080,720   #建立1080*720空白图片
set title "cnblogs/goodcitizen"   #注明曲线图标题
set output "goodcitizen.png"   #设置文件名
set grid

set xdata time
set timefmt "%Y-%m-%d"
set format x "%m/%d"

# plot in double Y axises
set xlabel "day(s)"
set ylabel "score"
set y2label "rank"
set y2tics
#set ytics nomirror

set yrange [0:6000]
set y2range [0:120000]
#set ytics ("0" 0,"1" 1000,"2" 2000,"3" 3000,"4" 4000,"5" 5000,"6" 6000)   #设置y轴范围、刻度参数
#set y2tics ("0" 0,"0.2" 20000,"0.4" 40000,"0.6" 60000,"0.8" 80000,"1.0" 100000,"1.2" 120000)   #设置y轴范围、刻度参数

plot "score.txt" using 1:2 w lp pt 5 title "score" axis x1y1, "score.txt" using 1:3 w lp pt 7 title "rank" axis x1y2

# multi-plot
#set size 1,1 # 总的大小
#set origin 0,0 # 总的起点
#set multiplot # 进入多图模式
#set size 0.5,0.5 # 第一幅图大小
#set origin 0,0.5 # 第一幅图起点
#
#set xlabel "day(s)"   #设置X轴名称
#set ylabel "score"   #设置y轴名称
#set yrange [0:6000]
#set ytics ("0" 0,"1" 1000,"2" 2000,"3" 3000,"4" 4000,"5" 5000,"6" 6000)   #设置y轴范围、刻度参数
#plot "score.txt" using 1:2 w lp pt 5 title "score"
#
#set size 0.5,0.5
#set origin 0.5,0.5
#set xlabel "day(s)"   #设置X轴名称
#set ylabel "rank"   #设置y轴名称
#set yrange [0:120000]
#set ytics ("0" 0,"0.2" 20000,"0.4" 40000,"0.6" 60000,"0.8" 80000,"1.0" 100000,"1.2" 120000)   #设置y轴范围、刻度参数
#plot "score.txt" using 1:3 w lp pt 7 title "rank" 
#
#unset multiplot
#分别取dash1和dash2数据表的第1和第7列分别作为x，y变量绘制曲线
#plot "score.txt" using 1:2 w lp pt 5 title "score","score.txt" using 1:3 w lp pt 7 title "rank" 

quit   #退出软件
