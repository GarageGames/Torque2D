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

/// <ClassName>AnimationSet</ClassName>
/// <summary>
/// <para>An AnimationSet is actually a ScriptObject which acts as a 
/// container for dynamic fields related to Animation Set information. This will
/// only work if the ScriptObject is given the class of "AnimationSet"</para>
/// <para></para>
/// </summary>
/// <example> 
/// This sample shows how to create an AnimationSet by hand when you know what
/// the animation names and datablocks will be:
/// <code>
/// new ScriptObject(OrcAnimationSet)
/// {
///    northWalkAnimation = "orcNorthWalk";
///    southWalkAnimation = "orcSouthWalk";
///    eastWalkAnimation = "orcEastWalk";
///    westWalkAnimation = "orcWestWalk";
///    class = "AnimationSet";
/// };
/// </code>
/// If you only know the name of the set, you can dynamically create the fields and their values:
/// <code>
/// new ScriptObject(OrcAnimationSet){ class = "AnimationSet" };
/// OrcAnimationSet.AddAnimation("northWalkAnimation", "orcNorthWalk");
/// OrcAnimationSet.AddAnimation("eastWalkAnimation", "orcEastWalk");
/// OrcAnimationSet.AddAnimation("southWalkAnimation", "orcSouthWalk");
/// </code>
/// You can mix the above two examples together when you have a pre-defined set of animation names,
/// but you do not know the field values yet:
/// <code>
/// new ScriptObject(OrcAnimationSet)
/// {
///    northWalkAnimation = "";
///    southWalkAnimation = "";
///    eastWalkAnimation = "";
///    westWalkAnimation = "";
///    class = "AnimationSet";
/// };
/// OrcAnimationSet.ChangeDatablock("northWalkAnimation", "orcNorthWalk");
/// OrcAnimationSet.ChangeDatablock("southWalkAnimation", "orcSouthWalk");
/// OrcAnimationSet.ChangeDatablock("eastWalkAnimation", "orcEastWalk");
/// OrcAnimationSet.ChangeDatablock("westWalkAnimation", "orcWestWalk");
/// </code>
/// </example>
/// <seealso cref="ScriptObject"/>

/// <ClassMethod>AnimationSet</ClassMethod>
/// <summary>Returns the number of animations in the set</summary>
/// <returns>Numeric value of how many dynamic animation fields in ScriptObject</returns>
/// <example>
/// This example shows how to print out the number of animations in this set to the console:
/// <code>
/// %animationCount = OrcAnimationSet.GetAnimationCount();
/// echo("The Orc Animation Set contains" SPC %animationCount SPC "animations.");
/// </code>
/// </example>
function AnimationSet::GetAnimationCount(%this)
{
   //return %this.getDynamicFieldCount();
   error("% - Not implemented");
   return 0;
}

/// <ClassMethod>AnimationSet::AddAnimation</ClassMethod>
/// <summary>
/// Adds a new animation field and corresponding datablock to the set </summary>
/// <param name="name">Name of the new animation field.</param>
/// <param name="animationDatablock">The AnimationAsset assigned to the new animation field</param>
/// <returns>True if the animation was successfully added. False if the field already existed or the animationDatablock is invalid</returns>
/// <example>
/// This example shows how to add a new animation and its value to a set. 
/// <code>OrcAnimationSet.AddAnimation("westWalkAnimation", "orcWestWalk");</code>
/// </example>
function AnimationSet::AddAnimation(%this, %name, %animationDatablock)
{
   if(%name $= "")
   {
      error("% - Invalid name specified for AnimationSet::AddAnimation");
      return false;
   }
   
   if(!isObject(%animationDatablock))
   {
      error("% - Invalid animation datablock specified for AnimationSet::AddAnimation");
      return false;
   }
   
   %this.setFieldValue(%name, %animationDatablock);
   
   return true;
}

/// <ClassMethod>AnimationSet::RemoveAnimation</ClassMethod>
/// <summary>
/// Deletes an animation field from the set. Note: Currently not implemented.
/// </summary>
/// <param name="parameterName">Parameter description</param>
/// <returns>Returns true if the animation was successfully removed. False if the animation did not exist</returns>
/// <example>
/// This example shows how to remove a specific animation from a set
/// <code>OrcAnimationSet.RemoveAnimation("westWalkAnimation");</code>
/// </example>
function AnimationSet::RemoveAnimation(%this, %name)
{
   error("% - Not implemented");
   return true;
}

/// <ClassMethod>AnimationSet::ChangeDatablock</ClassMethod>
/// <summary>
/// Changes the animation datablock on the specified field
/// </summary>
/// <param name="name">The animation field to modify</param>
/// <param name="animationDatablock">The new animation datablock</param>
/// <returns>Returns true if the change was successful. False if the animation does not exist or the animationDatablock is invalid</returns>
/// <example>
/// This example shows how to change the animation datablock for a specific animation in a set
/// <code>OrcAnimationSet.ChangeDatablock("westWalkAnimation", "orcWestWalk");</code>
/// </example>
function AnimationSet::ChangeDatablock(%this, %name, %animationDatablock)
{
   if(!isObject(%animationDatablock))
   {
      error("% - Invalid animation datablock specified for AnimationSet::ChangeDatablock");
      return false;
   }
   
   %fieldCount = %this.getDynamicFieldCount();
   
   for(%i = 0; %i < %fieldCount; %i++)
   {
      %currentField = %this.getDynamicField(%i);
      
      if(%currentField $= %name)
      {
         %this.setFieldValue(%name, %animationDatablock);
         return true;
      }
   }
   
   error("% - Could not find specified name in AnimationSet::ChangeDatablock");
   
   return false;
}

/// <ClassMethod>AnimationSet::GetAnimationDatablock</ClassMethod>
/// <summary>
/// Accessor for the animation datablock on the specified animation field
/// </summary>
/// <param name="name">The name of the animation field</param>
/// <returns>Returns a AnimationAsset attached to the specified name. "" Will be returned if the specified name is invalid</returns>
/// <example>
/// This example shows how to get the AnimationAsset of a specific animation and print it to the console.
/// <code>
/// %animationDatablock = OrcAnimationSet.GetAnimationDatablock("westWalkAnimation");
/// echo("The Orc's west walk animation is " SPC %animationDatablock.getName());
/// </code>
/// </example>
function AnimationSet::GetAnimationDatablock(%this, %name)
{
   %animationDatablock = %this.getFieldValue(%name);
   
   if(%animationDatablock $= "")
      warn("% - Warning: Returning a null animation datablock in AnimationSet::GetAnimationDatablock");
   
   return %animationDatablock;
}