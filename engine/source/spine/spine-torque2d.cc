/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef _SPINE_TORQUE2D_H_
#include <spine/spine-torque2d.h>
#endif

#ifndef SPINE_EXTENSION_H_
#include <spine/extension.h>
#endif

#ifndef _SKELETON_OBJECT_H_
#include "2d/sceneobject/SkeletonObject.h"
#endif

namespace spine {

void _AtlasPage_createTexture (AtlasPage* self, const char* path) {
	Texture* texture = new Texture();
	if (!texture->loadFromFile(path)) return;
	self->texture = texture;
	Vector2u size = texture->getSize();
	self->width = size.x;
	self->height = size.x;
}

void _AtlasPage_disposeTexture (AtlasPage* self) {
	delete (Texture*)self->texture;
}

char* _Util_readFile (const char* path, int* length) {
	return _readFile(path, length);
}

/**/

void _Torque2DAtlasPage_dispose (AtlasPage* page) {
	Torque2DAtlasPage* self = SUB_CAST(Torque2DAtlasPage, page);
	_AtlasPage_deinit(SUPER(self));

	//

	FREE(self);
}

AtlasPage* AtlasPage_create (const char* name, const char* path) {
	Torque2DAtlasPage* self = NEW(Torque2DAtlasPage);
    _AtlasPage_init(SUPER(self), name, _Torque2DAtlasPage_dispose);

	//

	return SUPER(self);
}

/**/

void _Torque2DSkeleton_dispose (Skeleton* skeleton) {
	Torque2DSkeleton* self = SUB_CAST(Torque2DSkeleton, skeleton);
	_Skeleton_deinit(SUPER(self));

	//

	FREE(self);
}

Skeleton* _Torque2DSkeleton_create (SkeletonData* data, SkeletonObject* skeletonObject) {
	Torque2DSkeleton* self = NEW(Torque2DSkeleton);
    _Skeleton_init(SUPER(self), data, _Torque2DSkeleton_dispose);

	self->skeletonObject = skeletonObject;

	return SUPER(self);
}

/**/

void _Torque2DRegionAttachment_dispose (Attachment* attachment) {
	Torque2DRegionAttachment* self = SUB_CAST(Torque2DRegionAttachment, attachment);
	_RegionAttachment_deinit(SUPER(self));

	//

	FREE(self);
}

void _Torque2DRegionAttachment_draw (Attachment* attachment, Slot* slot) {
	// Torque2DRegionAttachment* self = (Torque2DRegionAttachment*)attachment;
	// Draw or queue region for drawing.
}

RegionAttachment* RegionAttachment_create (const char* name, AtlasRegion* region) {
	Torque2DRegionAttachment* self = NEW(Torque2DRegionAttachment);
    _RegionAttachment_init(SUPER(self), name, _Torque2DRegionAttachment_dispose, _Torque2DRegionAttachment_draw);

	//

	return SUPER(self);
}

/**/

char* _Util_readFile (const char* path, int* length) {
	return _readFile(path, length);
}

} /* namespace spine */
