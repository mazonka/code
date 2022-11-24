set term png size 1600,800 enhanced font 'Arial,15'
set output 'safety.png'

#set grid 30,30,40
set isosamples 240
set samples 1000
#set view 78,25
set view 0,90
#unset mouse

x0=10
y0=10
v=12
xb=20
yb=30

d(xa,ya,xb,yb)=sqrt( (xa-xb)**2 + (ya-yb)**2 )
r(x,y)=d(x,y,x0,y0)
rxy(x,y)=d(x,y,xb,yb)
rmax=d(xb,yb,x0,y0)+v
rmin=d(xb,yb,x0,y0)-v
ptu(x,y) = (rmax-rxy(x,y))/(rmax-rmin)
pt(x,y) = (r(x,y)<v?ptu(x,y):0/0)

a=0.7
s=5
ps(x,y,xa,ya) = 1 - a*exp( -(d(x,y,xa,ya)/s)**2 )

x1=15
y1=5

x2=15
y2=15

p(x,y)=pt(x,y)*ps(x,y,x1,y1)*ps(x,y,x2,y2)

splot [0:xb] [0:yb] p(x,y) title "" with pm3d


#pause 100
