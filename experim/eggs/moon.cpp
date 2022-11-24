

#include <iostream>
#include <cmath>

int main()
{
double G=6.67e-11;
double L=149600*1000.0*1000;
double M=5.972e24;
double vs = 2*std::acos(-1)*6371000/86400;


//double d = 140*1000*1000;
double d = 2000*1000.0*1000*10;

double vm = std::sqrt(G*M/d);

//std::cout<<"VM="<<vm<<'\n';

for ( double x=0; x<d; x+= (d/2000) )
if( x>0 )
std::cout<< x/1000/1000 <<' '<< std::sqrt(G*M/x)*L/(L-x)<<' '<<vs<<' '<<std::sqrt(G*M/x)<<'\n';
//std::cout<< x/1000/1000 <<' '<< std::sqrt(G*M/x)<<' '<<vs<<'\n';

}

/*

I was discussing this with Dmitri at lunch, and he had a good follow up question:

At what distance would the Moon have to be from the Earth for the shadow to travel East to West.

I said below 36000km (geostationary orbit), but now that I think about it, I'm pretty sure that is incorrect. In fact, I could hardly have been more incorrect!!!

Hint, if the moon was at geostationary orbit, the shadow would still be travelling West to East (in fact, it would be travelling even quicker).

Paul Dawson
Electro-Optic Specialist
Defence Science & Technology Group
Bay 14, Building 71, Third Avenue, Edinburgh SA 5111 Australia
P: 08 7389 5617  F: 08 7389 5200
E: paul.dawson@dst.defence.gov.au


-----Original Message-----
From: Dawson, Paul
Sent: Tuesday, 22 August 2017 10:51 AM
To: Kamenetsky, Dmitri; Salazar, Manuel; Davies, Adam; Mazonka, Oleg; Chiknaikin, Timothy
Subject: RE: Oleg's puzzles [SEC=UNOFFICIAL]

UNOFFICIAL

Here's one I just thought of.

The Earth spins from West to East, the same way as the Moon orbits the Earth (anti-clockwise when viewed from above the North pole - the Earth also orbits the Sun in this direction). The Earth only takes a day to spin, whereas the Moon takes 28 days to orbit, so during an eclipse, you'd think somebody on the surface of the Earth would "catch up" to the Moon's shadow, and hence the shadow would appear to travel across the Earth from East to West. However, the eclipse this morning started on the West coast of the USA, and travelled to the East coast.

Why is this so?

Hint, it's not because the USA is going backwards ;-)


*/
