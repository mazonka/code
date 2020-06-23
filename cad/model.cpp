//#include <iostream>

#include <vector>

#include "bad.h"
#include "model.h"

Fcoord X;
std::vector<Model *> g_model_stack;
Model * g_model_current = nullptr;
Model g_model_main;

Model::Model()
{
    g_model_stack.push_back(this);
    g_model_current = this;
}

Model::~Model()
{
    g_model_stack.pop_back();
    if (g_model_stack.empty()) return;
    g_model_current = *g_model_stack.rbegin();
    g_model_current->submodels.push_back(*this);
}

void Point::reg()
{
    index = g_model_current->addPoint(*this);
    x = y = z = X;
}

void draw()
{
    BAD(1);
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



