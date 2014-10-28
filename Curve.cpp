//
//  Curve.cpp
//  Homework 2: Curves
//
//  Created by mahina kaholokula on 10/17/14.
//  Copyright (c) 2014 mahina kaholokula. All rights reserved.
//

#include <GLUT/glut.h>
#include <vector>

#include "float2.cpp"

//abstract base class
class Curve
{
public:
    virtual float2 getPoint(float t) = 0;
    
    virtual bool isNear(double x, double y) {
        for (float i = 0; i <= 1; i += 0.01) {
            if ((sqrt((getPoint(i).x - x)*(getPoint(i).x - x) + (getPoint(i).y - y)*(getPoint(i).y - y))) < 0.03){
                return true;
            }
        }
        return false;
    }
    
    virtual void draw() {
        glBegin(GL_LINE_STRIP);
        for (float i = 0; i <= 1; i += 0.01) {
            glVertex2d(getPoint(i).x, getPoint(i).y);
        }
        glEnd();
    }
};


class Freeform : public Curve
{
public:
    std::vector<float2> controlPoints;

    virtual float2 getPoint(float t) = 0;
    virtual void setColor() = 0;
    
    virtual void addControlPoint(float2 p) {
        controlPoints.push_back(p);
    }
    
    //returns bool if removes a point so Lagrange curve knows to update knots values
    virtual bool removeControlPoint (double xcoor, double ycoor) {
        double x, y;
        
        for (int i = 0; i < controlPoints.size(); i++) {
            x = controlPoints[i].x;
            y = controlPoints[i].y;
            if (sqrt((x-xcoor) * (x - xcoor) + (y - ycoor) * (y - ycoor)) < 0.02) {
                controlPoints.erase(controlPoints.begin() + i);
                return true;
            }
        }
        
        return false;
    }
    
    void drawControlPoints() {
        glPointSize(7);
        glColor3d(1.0,1.01,1.0);
        
        glBegin(GL_POINTS);
        for (float2 each: controlPoints){
            glVertex2d(each.x, each.y);
        }
        glEnd();
    }
    
    void drawSelected() {
        glLineWidth(6.0);
        glColor3d(0.0, 0.0, 1.0);   // color line blue
        draw();
    }
};

class PolyLine: public Freeform
{
public:
    // never used - just must be defined since its virtual in superclasses
    float2 getPoint(float t) {
        return float2(0,0);
    }
    
    void setColor() {
        glColor3d(0.0, 0.5, 0.0);       // green
        glLineWidth(3.0);
    }

    void draw() {
        glBegin(GL_LINE_STRIP);
        for (float2 point: controlPoints) {
            glVertex2d(point.x, point.y);
        }
        glEnd();
    }
    
    float getDist(float2 startLine, float2 endLine, float2 point) {
        float dot = ((point.x - startLine.x) * (endLine.x - startLine.x)) + ((point.y - startLine.y) * (endLine.y - startLine.y));
        float len_sq = (endLine.x - startLine.x) * (endLine.x - startLine.x) + (endLine.y - startLine.y) * (endLine.y - startLine.y);
        float param = dot / len_sq;
        
        float2 p;
        
        if (param < 0) {
            p = startLine;
        }
        
        else if (param > 1) {
            p = endLine;
        }
        
        else {
            p.x = startLine.x + param * (endLine.x - startLine.x);
            p.y = startLine.y + param * (endLine.y - startLine.y);
        }
        
        return (sqrt((p.x-point.x) * (p.x - point.x) + (p.y - point.y) * (p.y - point.y)));
    }
    
    bool isNear(double x, double y) {
        for (int i = 0; i < controlPoints.size() - 1; i++) {
            if (getDist(controlPoints[i], controlPoints[i+1], float2(x,y)) < 0.03) {
                return true;
            }
        }
        return false;
    }
};

class BezierCurve: public Freeform
{
public:
    static double bernstein(int i, int n, double t) {
        if (n == 1) {
            if (i == 0) return 1-t;
            if (i == 1) return t;
            return 0;
        }
        
        if (i < 0 || i > n) return 0;
        
        return (1 - t) * bernstein(i, n-1, t) + t * bernstein(i-1, n-1, t);
    }
    
    float2 getPoint(float t) {
        float2 r;
        double weight;
        
        for (int i = 0; i < controlPoints.size(); i++) {
            weight = bernstein(i, controlPoints.size() - 1, t);
            r += (controlPoints[i] * weight);
        }
        return r;
    }
    
    void setColor() {
        glColor3d(1.0,1.0,0.0);     // yellow
        glLineWidth(3.0);
    }
    
    void draw() {
        Curve::draw();
    }
};

class LagrangeCurve: public Freeform
{
public:
    std::vector<float> knots;
    
    void addControlPoint(float2 p){
        Freeform::addControlPoint(p);
        knots.push_back(1.0);
        knots[0] = 0.0;
        for (int i = 1; i < knots.size() - 1; i++){
            knots[i] = float(i) / (knots.size() - 1.0);
        }
    }
    
    bool removeControlPoint(double xcoor, double ycoor) {
        if (Freeform::removeControlPoint(xcoor, ycoor)) {
            knots.pop_back(); 
            for (int i = 1; i < knots.size(); i++) {
                knots[i] = float(i) / (knots.size() - 1);
            }
            return true;
        }
        
        return false;
    }
    
    static double Lagrange(int i, int n, double t, std::vector<float> knots) {
        double num = 1.0;
        double den = 1.0;
        for (int j = 0; j < n; j++) {
            if (j != i) {
                num *= (t - knots[j]);
                den *= knots[i] - knots[j];
            }
        }
        return num/den;
    }
    
    float2 getPoint(float t) {
        float2 r;
        double weight;
        
        for (int i = 0; i < controlPoints.size(); i++) {
            weight = Lagrange(i, controlPoints.size(), t, knots);
            r += (controlPoints[i] * weight);
        }
        return r;
    }
    
    void setColor() {
        glColor3d(1.0,0.0,0.0);     // red
        glLineWidth(3.0);
    }
    
    void draw(){
        Curve::draw();
    }
};

class BezierDigit : public BezierCurve {
public:
    BezierDigit (int digit) {
        switch(digit) {
            case 0:
                addControlPoint(float2(254.0/350.0 - 1, 1 - 47.0/350.0));
                addControlPoint(float2(159.0/350.0 - 1, 1 - 84.0/350.0));
                addControlPoint(float2(123.0/350.0 - 1, 1 - 158.0/350.0));
                addControlPoint(float2(131.0/350.0 - 1, 1 - 258.0/350.0));
                addControlPoint(float2(139.0/350.0 - 1, 1 - 358.0/350.0));
                addControlPoint(float2(167.0/350.0 - 1, 1 - 445.0/350.0));
                addControlPoint(float2(256.0/350.0 - 1, 1 - 446.0/350.0));
                addControlPoint(float2(345.0/350.0 - 1, 1 - 447.0/350.0));
                addControlPoint(float2(369.0/350.0 - 1, 1 - 349.0/350.0));
                addControlPoint(float2(369.0/350.0 - 1, 1 - 261.0/350.0));
                addControlPoint(float2(365.0/350.0 - 1, 1 - 81.0/350.0));
                addControlPoint(float2(231.0/350.0 - 1, 1 - 75.0/350.0));
            break;
            case 1: {
                addControlPoint(float2(138.0/350.0 - 1, 1 - 180.0/350.0));
                addControlPoint(float2(226.0/350.0 - 1, 1 - 99.0/350.0));
                addControlPoint(float2(230.0/350.0 - 1, 1 - 58.0/350.0));
                addControlPoint(float2(243.0/350.0 - 1, 1 - 43.0/350.0));
                addControlPoint(float2(256.0/350.0 - 1, 1 - 28.0/350.0));
                addControlPoint(float2(252.0/350.0 - 1, 1 - 100.0/350.0));
                addControlPoint(float2(253.0/350.0 - 1, 1 - 167.0/350.0));
                addControlPoint(float2(254.0/350.0 - 1, 1 - 234.0/350.0));
                addControlPoint(float2(254.0/350.0 - 1, 1 - 194.0/350.0));
                addControlPoint(float2(255.0/350.0 - 1, 1 - 303.0/350.0));
                addControlPoint(float2(256.0/350.0 - 1, 1 - 412.0/350.0));
                addControlPoint(float2(255.0/350.0 - 1, 1 - 424.0/350.0));
                break;
            }
            case 2: {
                addControlPoint(float2(110.0/350.0 - 1, 1 - 130.0/350.0));
                addControlPoint(float2(208.0/350.0 - 1, 1 - 1.0/350.0));
                addControlPoint(float2(355.0/350.0 - 1, 1 - 110.0/350.0));
                addControlPoint(float2(386.0/350.0 - 1, 1 - 251.0/350.0));
                addControlPoint(float2(352.0/350.0 - 1, 1 - 343.0/350.0));
                addControlPoint(float2(136.0/350.0 - 1, 1 - 454.0/350.0));
                addControlPoint(float2(120.0/350.0 - 1, 1 - 405.0/350.0));
                addControlPoint(float2(120.0/350.0 - 1, 1 - 405.0/350.0));
                addControlPoint(float2(120.0/350.0 - 1, 1 - 405.0/350.0));
                addControlPoint(float2(120.0/350.0 - 1, 1 - 405.0/350.0));
                addControlPoint(float2(104.0/350.0 - 1, 1 - 356.0/350.0));
                addControlPoint(float2(353.0/350.0 - 1, 1 - 414.0/350.0));
                break;
            }
            case 3: {
                addControlPoint(float2(96.0/350.0 - 1, 1 - 132.0/350.0));
                addControlPoint(float2(193.0/350.0 - 1, 1 - 16.0/350.0));
                addControlPoint(float2(311.0/350.0 - 1, 1 - 107.0/350.0));
                addControlPoint(float2(355.0/350.0 - 1, 1 - 197.0/350.0));
                addControlPoint(float2(190.0/350.0 - 1, 1 - 285.0/350.0));
                addControlPoint(float2(182.0/350.0 - 1, 1 - 250.0/350.0));
                addControlPoint(float2(182.0/350.0 - 1, 1 - 250.0/350.0));
                addControlPoint(float2(174.0/350.0 - 1, 1 - 215.0/350.0));
                addControlPoint(float2(396.0/350.0 - 1, 1 - 273.0/350.0));
                addControlPoint(float2(338.0/350.0 - 1, 1 - 388.0/350.0));
                addControlPoint(float2(200.0/350.0 - 1, 1 - 475.0/350.0));
                addControlPoint(float2(93.0/350.0 - 1, 1 - 391.0/350.0));
                break;
            }
            case 4: {
                addControlPoint(float2(344.0/350.0 - 1, 1 - 244.0/350.0));
                addControlPoint(float2(192.0/350.0 - 1, 1 - 234.0/350.0));
                addControlPoint(float2(131.0/350.0 - 1, 1 - 239.0/350.0));
                addControlPoint(float2(70.0/350.0 - 1, 1 - 244.0/350.0));
                addControlPoint(float2(70.0/350.0 - 1, 1 - 244.0/350.0));
                addControlPoint(float2(70.0/350.0 - 1, 1 - 244.0/350.0));
                addControlPoint(float2(142.0/350.0 - 1, 1 - 138.0/350.0));
                addControlPoint(float2(192.0/350.0 - 1, 1 - 84.0/350.0));
                addControlPoint(float2(242.0/350.0 - 1, 1 - 30.0/350.0));
                addControlPoint(float2(283.0/350.0 - 1, 1 + 30.0/350.0));
                addControlPoint(float2(260.0/350.0 - 1, 1 - 108.0/350.0));
                addControlPoint(float2(247.0/350.0 - 1, 1 - 438.0/350.0));
                break;
            }
            case 5: {
                addControlPoint(float2(330.0/350.0 - 1, 1 - 52.0/350.0));
                addControlPoint(float2(153.0/350.0 - 1, 1 - 44.0/350.0));
                addControlPoint(float2(144.0/350.0 - 1, 1 - 44.0/350.0));
                addControlPoint(float2(135.0/350.0 - 1, 1 - 44.0/350.0));
                addControlPoint(float2(135.0/350.0 - 1, 1 - 253.0/350.0));
                addControlPoint(float2(144.0/350.0 - 1, 1 - 253.0/350.0));
                addControlPoint(float2(153.0/350.0 - 1, 1 - 253.0/350.0));
                addControlPoint(float2(461.0/350.0 - 1, 1 - 135.0/350.0));
                addControlPoint(float2(401.0/350.0 - 1, 1 - 190.0/350.0));
                addControlPoint(float2(301.0/350.0 - 1, 1 - 272.0/350.0));
                addControlPoint(float2(481.0/350.0 - 1, 1 - 429.0/350.0));
                addControlPoint(float2(147.0/350.0 - 1, 1 - 389.0/350.0));
                break;
            }
            case 6: {
                addControlPoint(float2(301.0/350.0 - 1, 1 - 26.0/350.0));
                addControlPoint(float2(191.0/350.0 - 1, 1 - 104.0/350.0));
                addControlPoint(float2(149.0/350.0 - 1, 1 - 296.0/350.0));
                addControlPoint(float2(118.0/350.0 - 1, 1 - 368.0/350.0));
                addControlPoint(float2(153.0/350.0 - 1, 1 - 455.0/350.0));
                addControlPoint(float2(321.0/350.0 - 1, 1 - 465.0/350.0));
                addControlPoint(float2(367.0/350.0 - 1, 1 - 402.0/350.0));
                addControlPoint(float2(348.0/350.0 - 1, 1 - 321.0/350.0));
                addControlPoint(float2(348.0/350.0 - 1, 1 - 321.0/350.0));
                addControlPoint(float2(329.0/350.0 - 1, 1 - 240.0/350.0));
                addControlPoint(float2(220.0/350.0 - 1, 1 - 243.0/350.0));
                addControlPoint(float2(168.0/350.0 - 1, 1 - 285.0/350.0));
                break;
            }
            case 7: {
                addControlPoint(float2(108.0/350.0 - 1, 1 - 52.0/350.0));
                addControlPoint(float2(168.0/350.0 - 1, 1 - 34.0/350.0));
                addControlPoint(float2(379.0/350.0 - 1, 1 - 34.0/350.0));
                addControlPoint(float2(379.0/350.0 - 1, 1 - 34.0/350.0));
                addControlPoint(float2(379.0/350.0 - 1, 1 - 34.0/350.0));
                addControlPoint(float2(305.0/350.0 - 1, 1 - 145.0/350.0));
                addControlPoint(float2(294.0/350.0 - 1, 1 - 166.0/350.0));
                addControlPoint(float2(283.0/350.0 - 1, 1 - 187.0/350.0));
                addControlPoint(float2(243.0/350.0 - 1, 1 - 267.0/350.0));
                addControlPoint(float2(231.0/350.0 - 1, 1 - 295.0/350.0));
                addControlPoint(float2(219.0/350.0 - 1, 1 - 323.0/350.0));
                addControlPoint(float2(198.0/350.0 - 1, 1 - 452.0/350.0));
                break;
            }
            case 8: {
                addControlPoint(float2(243.0/350.0 - 1, 1 - 242.0/350.0));
                addControlPoint(float2(336.0/350.0 - 1, 1 - 184.0/350.0));
                addControlPoint(float2(353.0/350.0 - 1, 1 - 52.0/350.0));
                addControlPoint(float2(240.0/350.0 - 1, 1 - 23.0/350.0));
                addControlPoint(float2(87.0/350.0 - 1, 1 - 54.0/350.0));
                addControlPoint(float2(83.0/350.0 - 1, 1 - 225.0/350.0));
                addControlPoint(float2(225.0/350.0 - 1, 1 - 247.0/350.0));
                addControlPoint(float2(337.0/350.0 - 1, 1 - 259.0/350.0));
                addControlPoint(float2(423.0/350.0 - 1, 1 - 377.0/350.0));
                addControlPoint(float2(248.0/350.0 - 1, 1 - 482.0/350.0));
                addControlPoint(float2(90.0/350.0 - 1, 1 - 417.0/350.0));
                addControlPoint(float2(217.0/350.0 - 1, 1 - 255.0/350.0));
                break;
            }
            case 9: {
                addControlPoint(float2(322.0/350.0 - 1, 1 - 105.0/350.0));
                addControlPoint(float2(323.0/350.0 - 1, 1 - 6.0/350.0));
                addControlPoint(float2(171.0/350.0 - 1, 1 - 33.0/350.0));
                addControlPoint(float2(131.0/350.0 - 1, 1 - 137.0/350.0));
                addControlPoint(float2(161.0/350.0 - 1, 1 - 184.0/350.0));
                addControlPoint(float2(219.0/350.0 - 1, 1 - 190.0/350.0));
                addControlPoint(float2(219.0/350.0 - 1, 1 - 190.0/350.0));
                addControlPoint(float2(277.0/350.0 - 1, 1 - 196.0/350.0));
                addControlPoint(float2(346.0/350.0 - 1, 1 - 149.0/350.0));
                addControlPoint(float2(322.0/350.0 - 1, 1 - 122.0/350.0));
                addControlPoint(float2(298.0/350.0 - 1, 1 - 95.0/350.0));
                addControlPoint(float2(297.0/350.0 - 1, 1 - 448.0/350.0));
                break;
            }
        }
    }
    
    void setColor() {
        glColor3d(1.0,1.0,1.0);     // white
        glLineWidth(1.0);
    }
};