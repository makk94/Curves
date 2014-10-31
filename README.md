Curves
======

Class assignment demonstrating knowledge of curves.

The program displays any (reasonable) number of such objects. The color of the objects depends on their type (polyline, bezier 
curve, lagrange curve), but none of them is blue. Zero or one object can be selected. The selected object is displayed in blue, 
and with double width.

The user can create objects by pressing down the 'P', 'L', or 'B' key --- Polyline, Lagrange, or Bezier, respectively. 
Control points can be added by mouse clicks as long as the key is held down. Releasing the key deletes the selected object if 
it has less then two control points. The object being created is always the selected one, and is drawn as such. The control 
points of the selected curve are also drawn.

Implemented :
- Pressing SPACE makes the next object selected instead of the current one, repeatedly pressing SPACE cycles over all curves.
- When no keys are pressed and the user presses the mouse button, the object at the mouse cursor (if there is any) becomes selected.
- When an object is selected, the user may hold down 'A' to add control points to the selected object by clicking.
- Support removing control points of the selected curve, by holding 'D' and clicking them.
- Display the current number of curves in the top corner using Bezier curves to form the numbers.
- Display an animated Bezier clock in bottom corner.
