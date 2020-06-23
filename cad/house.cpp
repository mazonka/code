#include "model.h"

void model()
{
    Model house("house");

    Point a(fix(0), fix(0));
    Point b("b", 0., fix(0));
    a - b = 1;
    Point c;
    a - c = 0.64;
    b - c = 0.36;

    Line line {"wall"};
    a - b = line;
    a - c = line;
    b - c = line;

    {
        Model m("sub");
        Point k;
        k - k = 1;
    }

    {
        Model empty("empty");
        //Point a("a"), b("a");
    }

    calc();
    save("house.dat");
    draw();

    load("house.dat");
    save("house2.dat");
}
