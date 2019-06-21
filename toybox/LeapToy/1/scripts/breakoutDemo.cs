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

function LeapToy::createBreakoutLevel( %this )
{    
    // Create background.
    %this.createBackground();
    
    // Create the ground.
    %this.createGround();
    
    // Create the breakable bricks
    %this.createBricks();
    
    %this.createPaddle();
    
    %this.createBreakOutBall();    
    
    // Se the gravity.
    SandboxScene.setGravity( 0, 0);
    
    // Set the manipulation mode.
    Sandbox.useManipulation( off );

    // Swap action maps
    GestureMap.pop();
    FingerMap.pop();
    BreakoutMap.push();
    
    // Create the help text scene
    %helpText = new SimSet();
    %helpText.add(new ScriptObject() { Text = "Roll your hand back and forth to control the paddle"; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "When a block is hit twice, it will be destroyed"; });
    %helpText.add(new ScriptObject() { Text = " "; });
    %helpText.add(new ScriptObject() { Text = "Press H to return to the demo."; });
    %this.createHelpTextScene(%helpText);
    %helpText.delete();
}

//-----------------------------------------------------------------------------

function LeapToy::createBricks( %this )
{
    // Fetch the block count.
    %brickCount = LeapToy.BrickRows;
    %brickColumsn = LeapToy.BrickColumns;

    // Sanity!
    if ( %brickCount < 1 )
    {
        echo( "Cannot have a brick count count less than one." );
        return;
    }

    // Set the block size.
    %brickSize = LeapToy.BrickSize;

    // Calculate a block building position.
    %posx = %brickCount * %brickSize._0 * -1;
    %posy = 3 + (%brickSize._1 * 0.5) + 3;

    // Build the stack of blocks.
    for( %stack = 0; %stack < %brickCount; %stack++ )
    {
        // Calculate the stack position.
        %stackIndexCount = %brickCount;
        
        %stackX = %posX + ( %stack * %brickSize._0 );
        %stackY = %posY + ( %stack * %brickSize._1 );

        // Build the stack.
        for ( %stackIndex = 0; %stackIndex < LeapToy.BrickColumns; %stackIndex++ )
        {
            // Calculate the block position.
            %brickX = (%stackIndex*%brickSize._0)+%posx;
            %brickY = %stackY;
            %brickFrames = "0 2 4 6 8 10";
            %randomNumber = getRandom(0, 5);
            %frame = getWord(%brickFrames, %randomNumber);
            
            // Create the sprite.
            %obj = new Sprite()
            {
                class = "Brick";
            };            
            
            %obj.setPosition( %brickX, %brickY );
            %obj.setSize( %brickSize );
            %obj.setBodyType("Kinematic");
            %obj.setImage( "LeapToy:objectsBricks", %frame );
            %obj.setDefaultFriction( 1.0 );
            %obj.createPolygonBoxCollisionShape( %brickSize );
            %obj.CollisionCallback = true;
            
            %takesDamage = %this.TakesDamageBehavior.createInstance();
            %takesDamage.initialize(20, 100, 10, 0, "", "LeapToy:blockFadeParticle", %frame);
            
            %obj.addBehavior(%takesDamage);
            
            %swapImage = %this.SwapImageBehavior.createInstance();
            %swapImage.initialize("LeapToy:objectsBricks", %frame+1);
            %obj.addBehavior(%swapImage);
            
            // Add to the scene.
            SandboxScene.add( %obj );
        }
    }
}

//-----------------------------------------------------------------------------

function LeapToy::createPaddle(%this)
{
    // Create the player paddle
    %obj = new Sprite();
    
    %obj.setPosition( 0, -8);
    %obj.setSize( 5, 2.5 );
    %obj.setImage( "LeapToy:playerUfo");
    %obj.setDefaultFriction( 0 );
    %obj.createPolygonBoxCollisionShape( 4.8, 2.3);
    %obj.CollisionCallback = true;
    %obj.setFixedAngle( true );
    %this.paddle = %obj;
    
    SandboxScene.add(%obj);
}

//-----------------------------------------------------------------------------

function LeapToy::createBreakoutBall(%this)
{
    // Create the ball.
    %ball = new Sprite()
    {
        class = "Ball";
    };
    
    %ball.Position = "5 5";
    %ball.Size = 2;
    %ball.Image = "LeapToy:widgetBall";        
    %ball.setDefaultDensity( 1 );
    %ball.setDefaultRestitution( 1.0 );
    %ball.setDefaultFriction(0);
    %ball.createCircleCollisionShape( 0.8 );
    %ball.CollisionCallback = true;
    %ball.setLinearVelocity(-5, 20);
    %ball.setPosition(-5, -5);
    
    %dealsDamage = %this.DealsDamageBehavior.createInstance();
    %dealsDamage.initialize(10, false, "");
    %ball.addBehavior(%dealsDamage);    
    
    %this.breakoutBall = %ball;
    
    // Add to the scene.
    SandboxScene.add( %ball );
}

//-----------------------------------------------------------------------------

function Ball::onCollision(%this, %object, %collisionDetails)
{
    %xVelocity = %this.getLinearVelocity()._0;
    %yVelocity = %this.getLinearVelocity()._1;
    
    %newXVelocity = mClamp(%xVelocity, (LeapToy.maxBallSpeed*-1), LeapToy.maxBallSpeed);
    %newYVelocity = mClamp(%yVelocity, (LeapToy.maxBallSpeed*-1), LeapToy.maxBallSpeed);
    
    %this.setLinearVelocity(%newXVelocity, %newYVelocity);
}

//-----------------------------------------------------------------------------

function LeapToy::movePaddle(%this, %speed)
{
    %this.paddle.setLinearVelocity(%speed, 0);
}