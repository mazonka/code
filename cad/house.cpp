#include "model.h"

void model()
{
    Model house;

    Point a(0, 0);
    Point b(X, 0);
    a - b = 100;
    Point c;
    a - c = 64;
    b - c = 36;

    Line line;
    a - b = line;
    a - c = line;
    b - c = line;

    draw();
}
