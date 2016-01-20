// routines to allow us to put texture maps on a parameterized group
#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "PictRead.h"		// this is a library file from QD3D applications folder

TQ3Status AddTextureToGroup( TQ3GroupObject	theGroup, TQ3StoragePixmap *textureImage) ;
void PictureFileToPixmap( TQ3StoragePixmap *bMap ) ;
void TextureGroup( TQ3GroupObject	theGroup) ;
void AddResourceTextureToGroup( short pictResID, TQ3GroupObject	theGroup ) ;

#endif