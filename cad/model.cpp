#include <iostream>
#include <vector>
#include <string>

#include "bad.h"
#include "model.h"

Fcoord X;
std::vector<Model *> g_model_stack;
Model * g_model_current = nullptr;
Model g_model_main;

using std::string;
using std::cout;

Model::Model()
{
    g_model_stack.push_back(this);
    g_model_current = this;
}

Model::~Model()
{
    if (this != g_model_current) return; // submodel

    g_model_stack.pop_back();
    if (g_model_stack.empty())
    {
        g_model_current = nullptr;
        return;
    }
    g_model_current = *g_model_stack.rbegin();
    g_model_current->submodels.push_back(*this);
}

void Point::reg()
{
    index = g_model_current->addPoint(*this);
    x = y = z = X;
}

void Model::draw()
{
    for ( auto & a : submodels ) a.draw();
    for ( auto e : edges ) e.draw(points);
}

void draw()
{
    g_model_current -> calc();
    g_model_current -> draw();
}

void Span::operator=(double a)
{
    g_model_current->addDist(Distance { *this, a });
}


void Span::operator=(Line ln)
{
    g_model_current->addEdge(Edge { *this, ln });
}

Span Point::operator-(const Point & p) const
{
    return Span { index, p.index };
}

void Edge::draw(const std::vector<Point> & points)
{
    auto dr = [](Point p) -> string
    {
        cout << p.x.str() << ' ' << p.y.str();
        return "";
    };

    cout << dr(points[s.i1]) << '\n';
    cout << dr(points[s.i2]) << "\n\n";
}


string Fcoord::str() const
{
    if ( typ == Typ::unknown ) return "?";
    return std::to_string(v);
}


double Model::calc()
{
    return 0;
}
