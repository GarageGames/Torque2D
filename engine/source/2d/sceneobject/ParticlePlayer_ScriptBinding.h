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

ConsoleMethod(ParticlePlayer, setParticleAsset, void, 3, 3,     "(particleAssetId?) Sets the particle asset Id to play.\n"
                                                                "@param particleAssetId The particle asset Id to play.\n"
                                                                "@return No return value.")
{
    object->setParticle( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getParticleAsset, const char*, 2, 2,  "() Gets the particle asset Id.\n"
                                                                    "@return The particle asset Id.")
{
    return object->getParticle();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setCameraIdleDistance, void, 3, 3,    "(idleDistance) Sets the distance from any camera when the particle player will become idle i.e. stop integrating and rendering.\n"
                                                                    "@param pauseDistance The distance from any camera when the particle player will become idle i.e. stop integrating and rendering.\n"
                                                                    "@return No return value.")
{
    object->setCameraIdleDistance( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getCameraIdleDistance, F32, 2, 2,     "() Gets the distance from any camera when the particle player will become idle i.e. stop integrating and rendering.\n"
                                                                    "@return The distance from any camera when the particle player will become idle i.e. stop integrating and rendering.")
{
    return object->getCameraIdleDistance();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setEmissionRateScale, void, 3, 3,     "(scale) Sets the scale for the particle player emission rate.\n"
                                                                    "@param scale The scale for the particle player emission rate.\n"
                                                                    "@return No return value.")
{
    object->setEmissionRateScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getEmissionRateScale, F32, 2, 2,     "() Gets the scale for the particle player emission rate.\n"
                                                                    "@return The scale for the particle player emission rate.")
{
    return object->getEmissionRateScale();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setSizeScale, void, 3, 3,     "(scale) Sets the scale for the particle player particle sizes.\n"
                                                            "@param scale The scale for the particle player particle sizes.\n"
                                                            "@return No return value.")
{
    object->setSizeScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getSizeScale, F32, 2, 2,      "() Gets the scale for the particle player particle sizes.\n"
                                                            "@return The scale for the particle player particle sizes.")
{
    return object->getSizeScale();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setForceScale, void, 3, 3,    "(scale) Sets the scale for the particle player forces.\n"
                                                            "@param scale The scale for the particle player forces.\n"
                                                            "@return No return value.")
{
    object->setForceScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getForceScale, F32, 2, 2,     "() Gets the scale for the particle player forces.\n"
                                                            "@return The scale for the particle player forces.")
{
    return object->getForceScale();
}
//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, play, bool, 2, 3, "([resetParticles]) Starts the particle player playing.\n"
                                                "@param resetParticles Whether to reset any existing particles before starting to play.  Default is true.\n"
                                                "@return Returns true on success and false otherwise.")
{
    // Fetch the reset-particle flag.
    const bool resetParticles = argc >= 3 ? dAtob(argv[2]) : true;

    return object->play( resetParticles );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, stop, void, 2, 4, "([waitForParticles?, killEffect?]) - Stops the Particle Effect.\n"
              "@param waitForParticles Whether or not the effect should wait until all of its particles have run their course, or just stop immediately and delete the particles (default true).\n"
              "@param killEffect Whether or not the effect should be deleted after it has stopped (default false).\n"
              "@return No return value.")
{
    // Fetch the wait-for-particles flag.
   const bool waitForParticles = argc >= 3 ? dAtob(argv[2]) : true;

   // Fetch the kill-effect flag.
   const bool killEffect = argc >= 4 ? dAtob(argv[3]) : false;

   // Stop playing.
   object->stop( waitForParticles, killEffect );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getIsPlaying, bool, 2, 2, "() Gets whether the particle player is playing or not.\n"
                                                        "@return Whether the particle player is playing or not." )
{
   return object->getIsPlaying();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setPaused, void, 3, 3,    "(paused?) Sets whether the particle player is paused or not.\n"
                                                        "@param paused Whether the particle player is paused or not.\n"
                                                        "@return No return value.")
{
    object->setPaused( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getPaused, bool, 2, 2,    "() Gets whether the particle player is paused or not.\n"
                                                        "@return Whether the particle player is paused or not.")
{
    return object->getPaused();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setEmitterPaused, void, 4, 4, "(paused, emitterIndex) Sets whether the specified particle emitter is paused or not.\n"
                                                            "@param paused Whether the specified particle emitter is paused or not.\n"
                                                            "@param emitterIndex The index of the emitter to modify.\n"
                                                            "@return No return value.")
{
    object->setEmitterPaused( dAtob(argv[2]), dAtoi(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getEmitterPaused, bool, 3, 3, "(emitterIndex) Gets whether the specified particle emitter is paused or not.\n"
                                                            "@param emitterIndex The index of the emitter to modify.\n"
                                                            "@return Whether the specified particle emitter is paused or not.")
{
    return object->getEmitterPaused( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, setEmitterVisible, void, 4, 4,    "(visible, emitterIndex) Sets whether the specified particle emitter is visible or not.\n"
                                                                "@param paused Whether the specified particle emitter is visible or not.\n"
                                                                "@param emitterIndex The index of the emitter to modify.\n"
                                                                "@return No return value.")
{
    object->setEmitterVisible( dAtob(argv[2]), dAtoi(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticlePlayer, getEmitterVisible, bool, 3, 3,    "(emitterIndex) Gets whether the specified particle emitter is visible or not.\n"
                                                                "@param emitterIndex The index of the emitter to modify.\n"
                                                                "@return Whether the specified particle emitter is visible or not.")
{
    return object->getEmitterVisible( dAtob(argv[2]) );
}
