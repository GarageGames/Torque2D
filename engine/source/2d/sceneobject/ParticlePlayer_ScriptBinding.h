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

ConsoleMethodGroupBeginWithDocs(ParticlePlayer, SceneObject)

/*! Sets the particle asset Id to play.
    @param particleAssetId The particle asset Id to play.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setParticleAsset, ConsoleVoid, 3, 3, (particleAssetId?))
{
    object->setParticle( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the particle asset Id.
    @return The particle asset Id.
*/
ConsoleMethodWithDocs(ParticlePlayer, getParticleAsset, ConsoleString, 2, 2, ())
{
    return object->getParticle();
}

//-----------------------------------------------------------------------------

/*! Sets the distance from any camera when the particle player will become idle i.e. stop integrating and rendering.
    @param pauseDistance The distance from any camera when the particle player will become idle i.e. stop integrating and rendering.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setCameraIdleDistance, ConsoleVoid, 3, 3, (idleDistance))
{
    object->setCameraIdleDistance( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the distance from any camera when the particle player will become idle i.e. stop integrating and rendering.
    @return The distance from any camera when the particle player will become idle i.e. stop integrating and rendering.
*/
ConsoleMethodWithDocs(ParticlePlayer, getCameraIdleDistance, ConsoleFloat, 2, 2, ())
{
    return object->getCameraIdleDistance();
}

//-----------------------------------------------------------------------------

/*! Sets whether the intermediate world position, rotation, and size data points of particles between ticks are calculated.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setParticleInterpolation, ConsoleVoid, 3, 3, (bool status))
{
    object->setParticleInterpolation( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the intermediate world position, rotation, and size data points of particles between ticks are calculated.
    @return (bool status) Whether interpolation is calculated or not.
*/
ConsoleMethodWithDocs(ParticlePlayer, getParticleInterpolation, ConsoleFloat, 2, 2, ())
{
    return object->getParticleInterpolation();
}

//-----------------------------------------------------------------------------

/*! Sets the scale for the particle player emission rate.
    @param scale The scale for the particle player emission rate.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setEmissionRateScale, ConsoleVoid, 3, 3, (scale))
{
    object->setEmissionRateScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scale for the particle player emission rate.
    @return The scale for the particle player emission rate.
*/
ConsoleMethodWithDocs(ParticlePlayer, getEmissionRateScale, ConsoleFloat, 2, 2, ())
{
    return object->getEmissionRateScale();
}

//-----------------------------------------------------------------------------

/*! Sets the scale for the particle player particle sizes.
    @param scale The scale for the particle player particle sizes.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setSizeScale, ConsoleVoid, 3, 3, (scale))
{
    object->setSizeScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scale for the particle player particle sizes.
    @return The scale for the particle player particle sizes.
*/
ConsoleMethodWithDocs(ParticlePlayer, getSizeScale, ConsoleFloat, 2, 2, ())
{
    return object->getSizeScale();
}

//-----------------------------------------------------------------------------

/*! Sets the scale for the particle player forces.
    @param scale The scale for the particle player forces.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setForceScale, ConsoleVoid, 3, 3, (scale))
{
    object->setForceScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scale for the particle player forces.
    @return The scale for the particle player forces.
*/
ConsoleMethodWithDocs(ParticlePlayer, getForceScale, ConsoleFloat, 2, 2, ())
{
    return object->getForceScale();
}

//-----------------------------------------------------------------------------

/*! Sets the scale for the particle player lifetimes.
    @param scale The scale for the particle player lifetimes.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setTimeScale, void, 3, 3, (scale))
{
    object->setTimeScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scale for the particle player particle lifetimes.
    @return The scale for the particle player lifetimes.
*/
ConsoleMethodWithDocs(ParticlePlayer, getTimeScale, ConsoleFloat, 2, 2, ())
{
    return object->getTimeScale();
}

//-----------------------------------------------------------------------------

/*! Starts the particle player playing.
    @param resetParticles Whether to reset any existing particles before starting to play.  Default is true.
    @return Returns true on success and false otherwise.
*/
ConsoleMethodWithDocs(ParticlePlayer, play, ConsoleBool, 2, 3, ([resetParticles]))
{
    // Fetch the reset-particle flag.
    const bool resetParticles = argc >= 3 ? dAtob(argv[2]) : true;

    return object->play( resetParticles );
}

//-----------------------------------------------------------------------------

/*! Stops the Particle Effect.
    @param waitForParticles Whether or not the effect should wait until all of its particles have run their course, or just stop immediately and delete the particles (default true).
    @param killEffect Whether or not the effect should be deleted after it has stopped (default false).
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, stop, ConsoleVoid, 2, 4, ([waitForParticles?, killEffect?]))
{
    // Fetch the wait-for-particles flag.
   const bool waitForParticles = argc >= 3 ? dAtob(argv[2]) : true;

   // Fetch the kill-effect flag.
   const bool killEffect = argc >= 4 ? dAtob(argv[3]) : false;

   // Stop playing.
   object->stop( waitForParticles, killEffect );
}

//-----------------------------------------------------------------------------

/*! Gets whether the particle player is playing or not.
    @return Whether the particle player is playing or not.
*/
ConsoleMethodWithDocs(ParticlePlayer, getIsPlaying, ConsoleBool, 2, 2, ())
{
   return object->getIsPlaying();
}

//-----------------------------------------------------------------------------

/*! Sets whether the particle player is paused or not.
    @param paused Whether the particle player is paused or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setPaused, ConsoleVoid, 3, 3, (paused?))
{
    object->setPaused( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the particle player is paused or not.
    @return Whether the particle player is paused or not.
*/
ConsoleMethodWithDocs(ParticlePlayer, getPaused, ConsoleBool, 2, 2, ())
{
    return object->getPaused();
}

//-----------------------------------------------------------------------------

/*! Sets whether the specified particle emitter is paused or not.
    @param paused Whether the specified particle emitter is paused or not.
    @param emitterIndex The index of the emitter to modify.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setEmitterPaused, ConsoleVoid, 4, 4, (paused, emitterIndex))
{
    object->setEmitterPaused( dAtob(argv[2]), dAtoi(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the specified particle emitter is paused or not.
    @param emitterIndex The index of the emitter to modify.
    @return Whether the specified particle emitter is paused or not.
*/
ConsoleMethodWithDocs(ParticlePlayer, getEmitterPaused, ConsoleBool, 3, 3, (emitterIndex))
{
    return object->getEmitterPaused( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets whether the specified particle emitter is visible or not.
    @param paused Whether the specified particle emitter is visible or not.
    @param emitterIndex The index of the emitter to modify.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticlePlayer, setEmitterVisible, ConsoleVoid, 4, 4, (visible, emitterIndex))
{
    object->setEmitterVisible( dAtob(argv[2]), dAtoi(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the specified particle emitter is visible or not.
    @param emitterIndex The index of the emitter to modify.
    @return Whether the specified particle emitter is visible or not.
*/
ConsoleMethodWithDocs(ParticlePlayer, getEmitterVisible, ConsoleBool, 3, 3, (emitterIndex))
{
    return object->getEmitterVisible( dAtob(argv[2]) );
}

ConsoleMethodGroupEndWithDocs(ParticlePlayer)