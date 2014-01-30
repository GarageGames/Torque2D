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

function LeapToy::createSwapImageBehavior( %this )
{
    if (!isObject(SwapImageOnCollisionBehavior))
    {
        %this.SwapImageBehavior = new BehaviorTemplate(SwapImageOnCollisionBehavior);

        %this.SwapImageBehavior.friendlyName = "Swap Image On Collision";
        %this.SwapImageBehavior.behaviorType = "Effects";
        %this.SwapImageBehavior.description  = "Switches the image of an object when it collides";

        %this.SwapImageBehavior.addBehaviorField(image, "The image to swap to", asset, "", ImageAsset);
        %this.SwapImageBehavior.addBehaviorField(frame, "The frame of the image to swap to", int, 0);
        %this.add(%this.SwapImageBehavior);
    }
}

//-----------------------------------------------------------------------------

function SwapImageOnCollisionBehavior::initialize(%this, %image, %frame)
{
    %this.image = %image;
    %this.frame = %frame;
}

//-----------------------------------------------------------------------------

function SwapImageOnCollisionBehavior::onBehaviorAdd(%this)
{
   %this.owner.collisionCallback = true;
}

//-----------------------------------------------------------------------------

function SwapImageOnCollisionBehavior::onCollision(%this, %object, %collisionDetails)
{
   %this.owner.setImage(%this.image, %this.frame);
}
