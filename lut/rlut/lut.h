#pragma once

#include "cube.h"

struct PtsClass
{
    vd precX, precY;
    vvd points;
};

struct Config
{
    string name;
    int dimX, dimY;
    vdd rangeX, rangeY;

    std::vector<PtsClass> ptsClasses;
};

class Lut
{
        Cube * root = nullptr;

        std::map<Point, vd> tbl;

    public:
        Config cfg;

        struct Stat
        {
            unsigned totalCubes = 0;
            unsigned builtCubes = 0;
        }
        stat;

    public:
        Lut(string file);
        ~Lut() { delete root; }

        pair<Point, Cube *> getSubCube(vd point, Cube * parent) const;
        Cube * addCube(Point p, Cube * parent);
        bool checkPrecX(const Cube & cb, const PtsClass & pc) const;
        bool checkPrecY(const Cube & cb, const PtsClass & pc, const vd & pt) const;
        void save() const;
        Cube * getRoot() const { return root;  }
        vd translate(Point p) const;
        const Cube & cube(const vd & pt) const;

        vd f(const vd & p) const { return linef(cube(p), p); }

    private:
        void insertLut(Point p);
        double translate1(Point p, size_t index) const;
        vd linef(const Cube & cb, const vd & pt) const;
        vd lnfr(Point p, vd rr) const; // descent: linear F recursive
        void buildCubes();
        void buildCube(Cube *);
        bool isCubeInLut(const Cube &) const;
};
