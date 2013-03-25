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

function JointsToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );   
   
   %this.jointtype = "DistanceJoint";
   
     // Add configuration option.
   addSelectionOption("DistanceJoint,FrictionJoint,MotorJoint,PrismaticJoint,PulleyJoint,RevoluteJoint,RopeJoint,WeldJoint,WheelJoint,TargetJoint", "Joint Type", 10, "setJointType", true, "Selects the Joint Type to demonstrate");
      
     // Reset the toy initially.
    JointsToy.reset();
}

//-----------------------------------------------------------------------------

function JointsToy::destroy( %this )
{   
       
}

//-----------------------------------------------------------------------------

function JointsToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );
    SandboxScene.setGravity("0 -9.28");
    // Create a background.
    %this.createBackground();
    %this.createObjects();
    %this.SetJointType(%this.jointtype);
}

//-----------------------------------------------------------------------------

function JointsToy::createObjects(%this)
{
   
   JointsToy.object1 = new Sprite(){
   Image = "ToyAssets:WhiteSphere";
   Size = "1 1";
   Position = "2.5 0";
   };
   JointsToy.object1.createCircleCollisionShape(0.5);   
   
   JointsToy.object2 = new Sprite(){
   Image = "ToyAssets:WhiteSphere";
   Size = "1 1";
   Position = "-2.5 0"; 
   };
   JointsToy.object2.createCircleCollisionShape(0.5);
   
   JointsToy.object3 = new Sprite(){
   Image = "ToyAssets:WhiteSphere";
   Size = "1 1";
   Position = "-5.5 0"; 
   };
   JointsToy.object3.createCircleCollisionShape(0.5);
   
   SandboxScene.add(JointsToy.object1);
   SandboxScene.add(JointsToy.object2);
   SandboxScene.add(JointsToy.object3);
}

//-----------------------------------------------------------------------------

function JointsToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "40 30";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:jungleSky";
    %object.createEdgeCollisionShape( -20, -15, 20, -15 );
    %object.createEdgeCollisionShape( 20, -15, 20, 15 );
    %object.createEdgeCollisionShape( 20, 15, -20, 15 );
    %object.createEdgeCollisionShape( -20, 15, -20, -15 );
    
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}


//-----------------------------------------------------------------------------

function JointsToy::SetJointType(%this, %value)
{
   %this.jointtype = %value;
   echo("Selected Joint Type" SPC %this.jointtype);
   %this.createJoint();
}

//-----------------------------------------------------------------------------

function JointsToy::createJoint(%this)
{
   %jcount = SandboxScene.getJointCount();
   for(%i=0; %i<%jcount;%i++)
   {
      SandboxScene.deleteJoint(%i);   
   }
   echo("deleted" SPC %jcount SPC "joints");
   %this.object1.setLinearVelocity("0 0");
   %this.object1.setAngularVelocity("0");
   %this.object1.setPosition("2.5 0");
   
   %this.object2.setLinearVelocity("0 0");
   %this.object2.setAngularVelocity("0");

   %this.object2.setPosition("-2.5 0");   

   switch$(%this.jointtype)
    {
       case "DistanceJoint":
       %jointID = SandboxScene.createDistanceJoint(%this.object1,%this.object2,"0 0","0 0",3 ,1,0.8,true);
           
       case "FrictionJoint": 
       %jointID = SandboxScene.createFrictionJoint(%this.object1,%this.object2,"0 0","0 0",10,10,true);

       case "MotorJoint":
       //%jointID = SandboxScene.createMotorJoint(%this.object1,%this.object2, linearOffset, AngularOffset , maxForce, maxTorque,correctionFActor,collideconnected)
       %jointID = SandboxScene.createMotorJoint(%this.object1,%this.object2, "0 0", 90 , 10, 1000, 0);
       echo("boo");
             
       case "PrismaticJoint":
       echo("boo");
       
       case "PulleyJoint":
       %this.object1.DefaultDensity = 10;
       %this.object1.setPosition("-2.5 10");
      
       %this.object2.DefaultDensity = 10;
       %this.object2.setPosition("2.5 10");
       
       %jointID = SandboxScene.createPulleyJoint(%this.object1,%this.object2,"0 0","0 0","-2.5 10", "2.5 10", 2);       
       
       case "RevoluteJoint":       
       //%jointID = SandboxScene.createRevoluteJoint(%this.object1,%this.object2,"-1 1","1 1");
       %jointID = SandboxScene.createRevoluteJoint(%this.object1,%this.object2,%this.object1.getWorldCenter(),%this.object2.getWorldCenter());
       
       case "RopeJoint":
       %jointID = SandboxScene.createRopeJoint(%this.object1, %this.object2, %this.object1.getWorldCenter(),%this.object2.getWorldCenter(),5);
       
       case "WeldJoint":
       %jointID = SandboxScene.createWeldJoint(%this.object1,%this.object2, "0 0", %this.object2.getWorldCenter(),1,0.0);
       
       case "WheelJoint":    
       %this.object1.setSize("1.5 1.5");
      
       %jointID = SandboxScene.createWheelJoint(%this.object1,%this.object2, "-1.4 2.25", "0 -2","0 1" ,true);
                     
       %jointID2 = SandboxScene.createWheelJoint(%this.object1,%this.object3, "1.7 2.25", "0 -2","0 1",true);
       
       SandboxScene.setWheelJointMotor(%jointID,true,550,1500);
       SandboxScene.setWheelJointMotor(%jointID2,true,550,1500);
       
       SandboxScene.setWheelJointDampingRatio(%jointID, 0.35);
       SandboxScene.setWheelJointDampingRatio(%jointID2, 0.6);
       
       SandboxScene.setWheelJointFrequency(%jointID, 10);
       SandboxScene.setWheelJointFrequency(%jointID2, 10);
       
        case "TargetJoint":
       %jointID = SandboxScene.createTargetJoint(%this.object1, "10 10", 10, false, 5,0.0);
    }
    
   echo("Succesfully created joint" SPC %jointID);
   
}