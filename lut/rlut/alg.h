#pragma once

#include <string>

#include "lut.h"

using std::string;

void buildLut(Lut & t);
void runLut(Lut & t, string out);
void cmpLut(Lut & t);
void drawLutEdge(Lut & t, string slev);
void drawLutPoint(Lut & t);

inline void drawLut(Lut & t, string slev) { drawLutEdge(t, slev); drawLutPoint(t); }

inline int build(string nm) { Lut t(nm); buildLut(t); return 0; }
inline int run(string nm, string out) { Lut t(nm); runLut(t, out); return 0; }
inline int cmp(string nm) { Lut t(nm); cmpLut(t); return 0; }
inline int draw(string nm, string slev) { Lut t(nm); drawLut(t, slev); return 0; }
