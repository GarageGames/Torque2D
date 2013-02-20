//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

function ShapeVectorToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation(pan);
    
    // Set the manipulation mode.
    Sandbox.useManipulation(pan);

    // Set the toy properties

    // Shape determines the poly points for the ShapeVector
    // "Square": Simple box
    // "Triangle": Equilateral triangle
    // "Circle": Simple circle
    // "Complex": Shape with enough vertices to make an uncommon shape
    ShapeVectorToy.shape = "Square";

    // Toggles filling the shape with color or leaving as an outline
    ShapeVectorToy.fillMode = true;

    // Color of the filling
    ShapeVectorToy.fillColor = "0.5 1 1 0.5";

    // Color of the border lines
    ShapeVectorToy.lineColor = "0.5 1 1 1";

    // Add custom controls for toy
    addSelectionOption("Square,Triangle,Circle,Complex", "Shape", 4, "setShape", true, "Selects the shape to add to the scene");
    addFlagOption("Fill mode", "setFillMode", ShapeVectorToy.fillMode, true, "Whether new shapes are filled in or not");

    // Reset the toy.
    ShapeVectorToy.reset();
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();

    // Add a single shape
    %this.addShape();
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::setShape( %this, %value )
{
    %this.shape = %value;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::setFillMode( %this, %value)
{
    %this.fillMode = %value;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::generateShape( %this )
{
    // Start with default values
    %points = "0 0 0 0 0 0";
    %isCircle = false;
    %radius = 0;
    %size = "40";

    // Create the poly point list based on the selected shape
    switch$(%this.shape)
    {
        case "Square":
            %points = "-0.5 -0.5 0.5 -0.5 0.5 0.5 -0.5 0.5";

        case "Triangle":
            %points = "-0.0025 0.5 0.5 -0.5 -0.5 -0.5";

        case "Circle":
            %radius = %size / 2;
            %isCircle = true;

        case "Complex":
            %points = "-0.997 0.005 -0.737 -0.750 -0.010 -0.993 0.746 -0.750 0.997 0.005 0.742 0.740 0.005 0.998 -0.761 0.740";
    }

    // Create the shape vector
    %shape = new ShapeVector()
    {
        position = "0 0";
        size = %size;
        LineColor = %this.lineColor;
        FillColor = %this.fillColor;
        FillMode = %this.fillMode;
        PolyList = %points;
        isCircle = %isCircle;
        circleRadius = %radius;
    };

    // Return the shape to be added to the scene
    return %shape;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::addShape( %this )
{
    // Create the shape.
    %object = %this.generateShape();

    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}
