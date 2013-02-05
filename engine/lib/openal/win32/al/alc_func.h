
//AL_FUNCTION(ALCubyte*,   alcGetString, (ALCdevice *device,ALCenum param), return NULL; )
//AL_FUNCTION(ALCvoid,     alcGetIntegerv, (ALCdevice * device,ALCenum param,ALCsizei size,ALCint *data), return; )

AL_FUNCTION(ALCvoid*,	 alcOpenDevice, (const ALCchar *deviceName), return NULL; )
AL_FUNCTION(ALCvoid,     alcCloseDevice, (ALCvoid *device), return; )

AL_FUNCTION(ALCvoid*, alcCreateContext, (ALCvoid *device,ALCint *attrList), return NULL; )
AL_FUNCTION(ALCboolean,  alcMakeContextCurrent, (ALCvoid *context), return AL_FALSE; )
AL_FUNCTION(ALCvoid,     alcProcessContext, (ALCvoid *context), return; )
AL_FUNCTION(ALCvoid*, alcGetCurrentContext, (ALCvoid), return NULL; )
AL_FUNCTION(ALCvoid*,  alcGetContextsDevice, (ALCvoid *context), return NULL; )
AL_FUNCTION(ALCvoid,     alcSuspendContext, (ALCvoid *context), return; )
AL_FUNCTION(ALCvoid,     alcDestroyContext, (ALCvoid *context), return; )

AL_FUNCTION(ALCenum,     alcGetError, (ALCvoid *device), return ALC_INVALID_DEVICE;  )

//AL_FUNCTION(ALCboolean,  alcIsExtensionPresent, (ALCdevice *device,ALCubyte *extName), return AL_FALSE; )
//AL_FUNCTION(ALCvoid*,    alcGetProcAddress, (ALCdevice *device,ALCubyte *funcName), return NULL; )
//AL_FUNCTION(ALCenum,     alcGetEnumValue, (ALCdevice *device,ALCubyte *enumName), return ALC_INVALID_ENUM; )
