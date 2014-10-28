//
//  main.cpp
//  Homework 2: Curves
//
//  Created by mahina kaholokula on 10/17/14.
//  Copyright (c) 2014 mahina kaholokula. All rights reserved.
//


#pragma once

#include "Curve.cpp"
#include <time.h>
#include <ctime>


////////GLOBAL VARIABLES

double win_width = 700;
double win_height = 700;

std::vector<Freeform*> curves;
Freeform* selected;
int selectedIndex;      // with respect to curves vector
char keyPressed = 'X';      // 'X' means no key pressed

time_t currentTime;
struct tm *localTime;

double dt;
int Hour, Min, Sec;
BezierDigit second = *new BezierDigit(0);
BezierDigit tenSec = *new BezierDigit(0);
BezierDigit minute = *new BezierDigit(0);
BezierDigit tenMin = *new BezierDigit(0);
BezierDigit hour = *new BezierDigit(0);
BezierDigit tenHour = *new BezierDigit(0);
std::vector<BezierDigit> digits = {*new BezierDigit(0), *new BezierDigit(1), *new BezierDigit(2), *new BezierDigit(3), *new BezierDigit(4), *new BezierDigit(5), *new BezierDigit(6), *new BezierDigit(7), *new BezierDigit(8), *new BezierDigit(9)};

/////////OTHER FUNCTIONS

void selectNext() {
    selectedIndex = (selectedIndex + 1) % curves.size();
    selected = curves[selectedIndex];
}

void drawCounter() {
    int digit = curves.size() % 10;
    BezierDigit num = *new BezierDigit(digit);
    glPushMatrix();
    glScaled(0.1, 0.1, 1);
    glTranslated(9, 9, 1);
    num.setColor();
    num.draw();
    glPopMatrix();
    
    digit = curves.size() / 10;
    BezierDigit tens = *new BezierDigit(digit);
    glPushMatrix();
    glScaled(0.1, 0.1, 1);
    glTranslated(8, 9, 1);
    tens.setColor();
    tens.draw();
    glPopMatrix();
}

void initClock() {
    
    time( &currentTime );// Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time
    
    Hour = localTime->tm_hour;
    Min = localTime->tm_min;
    Sec = localTime->tm_sec;
    
    second = *new BezierDigit(Sec % 10);
    tenSec = *new BezierDigit(Sec / 10);
    minute = *new BezierDigit(Min % 10);
    tenMin = *new BezierDigit(Min / 10);
    hour = *new BezierDigit(Hour % 10);
    tenHour = *new BezierDigit(Hour / 10);
}

void drawClock(){
//    int one = int(time (NULL)) % 10;
//    int ten = int(time (NULL)) % 60 / 10;
//    int hun = int(time(NULL)) / 60 % 10;
//    int thou = int(time(NULL)) / 60 % 60 / 10;
//    int tthou = int(time(NULL)) / 3600 % 24 % 10;
//    int hthou = int(time(NULL)) / 3600 % 24 / 10;
    

    for (int i = 0; i < 12; i++) {
        second.controlPoints[i] = second.controlPoints[i] + (digits[Sec % 10].controlPoints[i] - second.controlPoints[i]) * dt/10.0;
    }
    
    if (Sec % 10 == 0) {
        for (int i = 0; i < 12; i++) {
            tenSec.controlPoints[i] = tenSec.controlPoints[i] + (digits[Sec / 10].controlPoints[i] - tenSec.controlPoints[i]) * dt/10.0;
        }
    }
    
    if (Sec / 10 == 0) {
        for (int i = 0; i < 12; i++) {
            minute.controlPoints[i] = minute.controlPoints[i] + (digits[Min % 10].controlPoints[i] - minute.controlPoints[i]) * dt/10.0;
        }
    }
    
    if (Min % 10 == 0) {
        for (int i = 0; i < 12; i++) {
            tenMin.controlPoints[i] = tenMin.controlPoints[i] + (digits[Min / 10].controlPoints[i] - tenMin.controlPoints[i]) * dt/10.0;
        }
    }
    
    if (Min / 10 == 0) {
        for (int i = 0; i < 12; i++) {
            hour.controlPoints[i] = hour.controlPoints[i] + (digits[(Hour % 10)].controlPoints[i] - hour.controlPoints[i]) * dt/10.0;
        }
    }
    
    if (Hour % 10 == 0) {
        for (int i = 0; i < 12; i++) {
            tenHour.controlPoints[i] = tenHour.controlPoints[i] + (digits[(Hour / 10)].controlPoints[i] - tenHour.controlPoints[i]) * dt/10.0;
        }
    }
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(19, -19, 1);
    second.setColor();
    second.draw();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(18, -19, 1);
    tenSec.setColor();
    tenSec.draw();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(16, -19, 1);
    minute.setColor();
    minute.draw();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(15, -19, 1);
    tenMin.setColor();
    tenMin.draw();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(13, -19, 1);
    hour.setColor();
    hour.draw();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.05, 0.05, 1);
    glTranslated(12, -19, 1);
    tenHour.setColor();
    tenHour.draw();
    glPopMatrix();
}

////////IMAGE DISPLAY FUNCTIONS

void onDisplay(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (Freeform* curve: curves){
        if (curve == selected) {
            curve->drawControlPoints();
            curve->drawSelected();  //sets color special!
        }
        else {
            curve->setColor();  //to normal color and line width
            curve->draw();
        }
    }
    
    drawCounter();
    drawClock();
    
    glutSwapBuffers();
}

void onMouse(int button, int state, int x, int y){
    double xcoor = x/(win_width/2) - 1;
    double ycoor = 1 - y/(win_height/2);

    // trying to draw a line
    if (state == GLUT_DOWN) {
        //new curve being made
        if (keyPressed == 'B' or keyPressed == 'L' or keyPressed == 'P')
            curves[curves.size() - 1]->addControlPoint(float2(xcoor, ycoor));
        
        //add control point to selected curve
        else if (keyPressed == 'A')
            selected->addControlPoint(float2(xcoor, ycoor));
        
        //delete control point of selected curve
        else if (keyPressed == 'D')
            selected->removeControlPoint(xcoor, ycoor);
        
        //selecting a curve
        else if (keyPressed == 'X') {
            selected = nullptr; // if not near any curve, nothing is selected
            
            for (int i = 0; i < curves.size(); i++) {
                if (curves[i]->isNear(xcoor,ycoor)){
                    selected = curves[i];
                    selectedIndex = i;
                }
            }
        }
        
    }

    glutPostRedisplay();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'B') {
        keyPressed = 'B';
        selected = new BezierCurve;
        curves.push_back(selected);
        selectedIndex = curves.size() - 1;
    }
    
    else if (key == 'L') {
        keyPressed = 'L';
        selected = new LagrangeCurve;
        curves.push_back(selected);
        selectedIndex = curves.size() - 1;
    }
    
    else if (key == 'P') {
        keyPressed = 'P';
        selected = new PolyLine;
        curves.push_back(selected);
        selectedIndex = curves.size() - 1;
    }
    
    else if (selected != nullptr) {
        if (key == ' ') {
            selectNext();
        }
        
        else if (key == 'A' or key == 'D') {
            keyPressed = key;
        }
    }
    
    glutPostRedisplay();
}

void onKeyboardUp(unsigned char key, int x, int y) {
    keyPressed = 'X';
    
    //if curve now has < 3 control points, remove it
    if (curves[selectedIndex]->controlPoints.size() < 3) {
        curves.erase(curves.begin() + selectedIndex);
        selected = nullptr;
    }
    
    glutPostRedisplay();
}

void onIdle() {
    
    double t = glutGet(GLUT_ELAPSED_TIME) * 0.001;
    dt = double(int(floor(t)) % 10) + 1.0;
    
    time( &currentTime );// Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time
    
    Hour = localTime->tm_hour;
    Min = localTime->tm_min;
    Sec = localTime->tm_sec;
    
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);                 		// GLUT initialization
    glutInitWindowSize(win_width, win_height);				// Initial resolution of the MsWindows Window is 600x600 pixels
    glutInitWindowPosition(100, 100);            // Initial location of the MsWindows window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);    // Image = 8 bit R,G,B + double buffer + depth buffer
    
    glutCreateWindow("Curves");        	// Window is born
    
    initClock();
    
    glutDisplayFunc(onDisplay);                	// Register event handlers
    glutMouseFunc(onMouse);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutIdleFunc(onIdle);
    
    glutIgnoreKeyRepeat(1);     //don't register auto repeat (keyboard func only registers once per char now)
    
    glutMainLoop();
    
    return 0;
}

