
#ifdef 	STD_INCLUDE_DECL

#define MAX_ASSET_ENTRY_COUNT  256
struct ASSET_ENTRY {
	uint32 offset;
	uint32 size;
};

struct ASSET_TEXTURE {
	int32 width;
	int32 height;
};

struct PLATFORM;
struct ASSET_SYSTEM {
	RENDER_TEXTURE	       defaultTexture;
	RENDER_MONOSPACE_FONT defaultMonoFont;
	
	PLATFORM * platform;
	uint64 assetFile_handle;
	
	ASSET_ENTRY entry[ assetID_count ];
	
	#define ASSET_SYSTEM__MAX_TEXTURE_COUNT  24
	int32					texture_count;
	ASSET_ID 			textureID_list	[ ASSET_SYSTEM__MAX_TEXTURE_COUNT ];
	RENDER_TEXTURE		texture_list	[ ASSET_SYSTEM__MAX_TEXTURE_COUNT ];
	
	#define ASSET_SYSTEM__MAX_MONOSPACE_FONT_COUNT	12
	int32							monoFont_count;
	ASSET_ID						monoFontID_list[ ASSET_SYSTEM__MAX_MONOSPACE_FONT_COUNT ];
	RENDER_MONOSPACE_FONT	monoFont_list	[ ASSET_SYSTEM__MAX_MONOSPACE_FONT_COUNT ];
	
	#define ASSET_SYSTEM__MAX_FONT_COUNT	12
	int32			font_count;
	ASSET_ID		fontID_list[ ASSET_SYSTEM__MAX_FONT_COUNT ];
	RENDER_FONT	font_list  [ ASSET_SYSTEM__MAX_FONT_COUNT ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

internal int32
findAsset( ASSET_SYSTEM * assetSystem, ASSET_ID assetID, ASSET_ID * list, int32 count ) {
	int32 result = -1;
	
	int32 index 	= 0;
	boo32 wasFound = false;
	while( ( !wasFound ) && ( index < count ) ) {
		ASSET_ID currID = list[ index ];
		if( assetID == currID ) {
			wasFound = true;
			result = index;
		}
		index++;
	}
	
	return result;
}

internal void
addTexture( ASSET_SYSTEM * assetSystem, ASSET_ID textureID, RENDER_TEXTURE texture ) {
	// NOTE: Does NOT check for duplicates. Function assumes that texture was not already found in textureID_list or is being added at startup.
	int32 count = assetSystem->texture_count;
	Assert( count < ASSET_SYSTEM__MAX_TEXTURE_COUNT );
	assetSystem->textureID_list[ count ] = textureID;
	assetSystem->texture_list	[ count ] = texture;
	assetSystem->texture_count++;
}

internal uint32
GL_loadTexture_returnBufferID( int32 width, int32 height, void * texture ) {
	uint32 bufferID = 0;
	glGenTextures( 1, &bufferID );
	glBindTexture( GL_TEXTURE_2D, bufferID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture ); // android
	// glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture ); // win32
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	return bufferID;
}

internal RENDER_TEXTURE
GL_loadTexture( int32 width, int32 height, void * texture ) {
	uint32 bufferID = GL_loadTexture_returnBufferID( width, height, texture );
	RENDER_TEXTURE result = RenderTexture( bufferID, width, height );
	return result;
}

internal RENDER_TEXTURE
loadTexture( ASSET_SYSTEM * assetSystem, ASSET_ID textureID ) {
	RENDER_TEXTURE result = assetSystem->defaultTexture;
	if( assetSystem->texture_count < ASSET_SYSTEM__MAX_TEXTURE_COUNT ) {
		ASSET_ENTRY entry = assetSystem->entry[ textureID ];
		if( entry.size > 0 ) {
			PLATFORM * platform = assetSystem->platform;
			MEMORY * tempMemory = &platform->tempMemory;
		
			uint8 * memory = ( uint8 * )_pushSize_clear( tempMemory, entry.size );
			platform->readAsset_chunk( assetSystem->assetFile_handle, ( assetID_count * sizeof( ASSET_ENTRY ) ) + entry.offset, entry.size, memory );
			
			ASSET_TEXTURE * texture = ( ASSET_TEXTURE * )memory;
			result = GL_loadTexture( texture->width, texture->height, memory + sizeof( ASSET_TEXTURE ) );
			
			assetSystem->textureID_list[ assetSystem->texture_count ] = textureID;
			assetSystem->texture_list	[ assetSystem->texture_count ] = result;
			assetSystem->texture_count++;
			
			_popSize( tempMemory, entry.size );
		}
	} else {
		CONSOLE_STRING( "ERROR. AssetSystem texture list is full. Unable to load texture." );
	}
	return result;
}

internal RENDER_TEXTURE 
getTexture( ASSET_SYSTEM * assetSystem, ASSET_ID textureID ) {
	RENDER_TEXTURE result = assetSystem->defaultTexture;
	
	if( textureID != assetID_texture_default ) {
		int32 select_index = findAsset( assetSystem, textureID, assetSystem->textureID_list, assetSystem->texture_count );
		if( select_index > -1 ) {
			result = assetSystem->texture_list[ select_index ];
		} else {
			result = loadTexture( assetSystem, textureID );
		}
	}
	return result;
}

internal RENDER_FONT *
loadFont( ASSET_SYSTEM * assetSystem, ASSET_ID fontID ) {
	RENDER_FONT * result = 0;
	if( assetSystem->font_count < ASSET_SYSTEM__MAX_FONT_COUNT ) {
		ASSET_ENTRY entry = assetSystem->entry[ fontID ];
		if( entry.size > 0 ) {
			PLATFORM * platform = assetSystem->platform;
			MEMORY * tempMemory = &platform->tempMemory;
			
			assetSystem->fontID_list[ assetSystem->font_count ] = fontID;
			result = assetSystem->font_list + assetSystem->font_count;
			assetSystem->font_count++;
			
			platform->readAsset_chunk( assetSystem->assetFile_handle, ( assetID_count * sizeof( ASSET_ENTRY ) ) + entry.offset, sizeof( FONT ), &result->font );
			
			int32 size_left = entry.size - sizeof( FONT );
			uint8 * memory = ( uint8 * )_pushSize_clear( tempMemory, size_left );
			platform->readAsset_chunk( assetSystem->assetFile_handle, ( assetID_count * sizeof( ASSET_ENTRY ) ) + entry.offset + sizeof( FONT ), size_left, memory );
			
			int32 width  = *( ( int32 * )memory );   memory += sizeof( int32 );
			int32 height = *( ( int32 * )memory );   memory += sizeof( int32 );
			uint8 * 	texture = memory;
			result->texture_bufferID = GL_loadTexture_returnBufferID( width, height, texture );
			
			_popSize( tempMemory, size_left );
		}
	} else {
		CONSOLE_STRING( "ERROR. AssetSystem font list is full. Unable to load font." );
	}
	Assert( result );
	return result;
}

internal RENDER_FONT *
getFont( ASSET_SYSTEM * assetSystem, ASSET_ID fontID ) {
	RENDER_FONT * result = 0;
	
	int32 select_index = findAsset( assetSystem, fontID, assetSystem->fontID_list, assetSystem->font_count );
	if( select_index > -1 ) {
		result = assetSystem->font_list + select_index;
	} else {
		result = loadFont( assetSystem, fontID );
	}
	Assert( result );
	return result;
}

internal RENDER_MONOSPACE_FONT *
loadMonoFont( ASSET_SYSTEM * assetSystem, ASSET_ID monoFontID ) {
	RENDER_MONOSPACE_FONT * result = &assetSystem->defaultMonoFont;
	if( assetSystem->monoFont_count < ASSET_SYSTEM__MAX_MONOSPACE_FONT_COUNT ) {
		ASSET_ENTRY entry = assetSystem->entry[ monoFontID ];
		if( entry.size > 0 ) {
			PLATFORM * platform = assetSystem->platform;
			MEMORY * tempMemory = &platform->tempMemory;
			
			assetSystem->monoFontID_list[ assetSystem->monoFont_count ] = monoFontID;
			result = assetSystem->monoFont_list + assetSystem->monoFont_count;
			assetSystem->monoFont_count++;
			
			platform->readAsset_chunk( assetSystem->assetFile_handle, ( assetID_count * sizeof( ASSET_ENTRY ) ) + entry.offset, sizeof( MONOSPACE_FONT ), &result->font );
			
			int32 size_left = entry.size - sizeof( MONOSPACE_FONT );
			uint8 * memory = ( uint8 * )_pushSize_clear( tempMemory, size_left );
			platform->readAsset_chunk( assetSystem->assetFile_handle, ( assetID_count * sizeof( ASSET_ENTRY ) ) + entry.offset + sizeof( MONOSPACE_FONT ), size_left, memory );
			
			int32 width  = *( ( int32 * )memory );   memory += sizeof( int32 );
			int32 height = *( ( int32 * )memory );   memory += sizeof( int32 );
			uint8 * 	texture = memory;
			result->texture_bufferID = GL_loadTexture_returnBufferID( width, height, texture );
			
			_popSize( tempMemory, size_left );
		}
	} else {
		CONSOLE_STRING( "ERROR. AssetSystem font list is full. Unable to load font." );
	}
	return result;
}

internal RENDER_MONOSPACE_FONT *
getMonoFont( ASSET_SYSTEM * assetSystem, ASSET_ID monoFontID ) {
	RENDER_MONOSPACE_FONT * result = &assetSystem->defaultMonoFont;
	
	if( monoFontID != assetID_monoFont_default ) {
		int32 select_index = findAsset( assetSystem, monoFontID, assetSystem->monoFontID_list, assetSystem->monoFont_count );
		if( select_index > -1 ) {
			result = assetSystem->monoFont_list + select_index;
		} else {
			result = loadMonoFont( assetSystem, monoFontID );
		}
	}
	return result;
}

#endif	// STD_INCLUDE_FUNC