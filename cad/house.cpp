#include "model.h"

void model()
{
    //{
    Model house("house");
    load("house.dat");
    calc();
    prn_loss();
    //}
    draw();
    save();
}

void modelXX()
{
    Model house("house");

    Point a("a", fix(0), fix(0));
    Point b("b", 0., fix(0));
    a - b = 1;
    Point c("c");
    a - c = 0.8;
    b - c = 0.6;

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
