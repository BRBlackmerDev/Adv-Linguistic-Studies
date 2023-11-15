
#ifdef	STD_INCLUDE_DECL

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

internal void
addTask( RENDER_SYSTEM * renderSystem, MESH * mesh, vec2 position, vec2 scale, flo32 degrees, vec4 color ) {
	ASSET_SYSTEM * assetSystem = renderSystem->assetSystem;
	uint32 texture_bufferID = getTexture( assetSystem, assetID_texture_whiteTexture ).bufferID;
	
	if( renderSystem->object_count < RENDER_SYSTEM__MAX_OBJECT_COUNT ) {
		uint32 vertex_index = renderSystem->vertex_count;
		
		vec2 xAxis = Vec2( cosDegrees( degrees ), sinDegrees( degrees ) );
		vec2 yAxis = getPerp( xAxis );
		
		xAxis *= scale.x;
		yAxis *= scale.y;
		
		for( int32 iter = 0; iter < mesh->vertex_count; iter++ ) {
			vec2 vert    = mesh->vertex[ iter ];
			vec2 adjVert = position + xAxis * vert.x + yAxis * vert.y;
			
			VERTEX_TEXMESH_2D finalVert = VertexTexMesh2D( adjVert, 0.0f, 0.0f );
			addVertex( renderSystem, finalVert );
		}
		
		renderSystem->object[ renderSystem->object_count++ ] = RenderObject( mesh->drawType, vertex_index, mesh->vertex_count, texture_bufferID, color );
	} else {
		CONSOLE_STRING( "ERROR. RenderObject list is full. Unable to add RenderObject to RenderSystem." );
	}
}

internal void
addTask( RENDER_SYSTEM * renderSystem, MESH_DRAW_TYPE drawType, uint32 texture_bufferID, rect bound, vec4 color ) {
	beginTask( renderSystem, drawType, texture_bufferID, color );
	addQuad( renderSystem, bound, Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
	endTask( renderSystem );
}

internal void
drawRect( RENDER_SYSTEM * renderSystem, rect bound, vec4 color ) {
	uint32 texture_bufferID = getTexture( renderSystem->assetSystem, assetID_texture_whiteTexture ).bufferID;
	addTask( renderSystem, meshDrawType_triangleFan, texture_bufferID, bound, color );
}

internal void
drawRectOutline( RENDER_SYSTEM * renderSystem, rect bound, vec4 color ) {
	uint32 texture_bufferID = getTexture( renderSystem->assetSystem, assetID_texture_whiteTexture ).bufferID;
	addTask( renderSystem, meshDrawType_lineLoop, texture_bufferID, bound, color );
}

internal void
drawRect( RENDER_SYSTEM * renderSystem, ASSET_ID textureID, rect bound, vec4 color ) {
	uint32 texture_bufferID = getTexture( renderSystem->assetSystem, textureID ).bufferID;
	addTask( renderSystem, meshDrawType_triangleFan, texture_bufferID, bound, color );
}

internal void
drawMonoString( RENDER_SYSTEM * renderSystem, RENDER_MONOSPACE_FONT * renderFont, const char * string, int32 char_count, vec2 position, vec4 color, flo32 scale ) {
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, color );
	
	MONOSPACE_FONT * font = &renderFont->font;
	
	vec2 atPos = position;
	for( int32 counter = 0; counter < char_count; counter++ ) {
		char c = string[ counter ];
		if( c != ' ' ) {
			int32 char_index = c - FONT_START_CHAR;
			MONOSPACE_FONT_CHAR _char = font->alphabet[ char_index ];
			
			flo32 left   = atPos.x + _char.offset.x * scale;
			flo32 right  = left    + _char.dim.x    * scale;
			flo32 top    = atPos.y + _char.offset.y * scale;
			flo32 bottom = top     - _char.dim.y    * scale;
			rect bound = Rect( left, bottom, right, top );
			
			addQuad( renderSystem, bound, rectMM( _char.texCoord_min, _char.texCoord_max ) );
		}
		atPos.x += font->advanceWidth * scale;
	}
	endTask( renderSystem );
}

internal boo32
isWhiteSpace( char c ) {
	boo32 result = ( c == ' ' ) || ( c == '\r' ) || ( c == '\n' ) || ( c == '\t' );
	return result;
}

internal void
drawMonoSubString( RENDER_SYSTEM * renderSystem, ASSET_ID fontID, const char * string, int32 char_count, vec2 position, vec4 color ) {
	RENDER_MONOSPACE_FONT * renderFont 	= getMonoFont( renderSystem->assetSystem, fontID );
	drawMonoString( renderSystem, renderFont, string, char_count, position, color, 1.0f );
}

internal void
drawMonoString( RENDER_SYSTEM * renderSystem, ASSET_ID monoFontID, const char * string, vec2 position, vec4 color, TEXT_ALIGNMENT align = ALIGN_DEFAULT ) {
	RENDER_MONOSPACE_FONT * 	renderFont 	= getMonoFont( renderSystem->assetSystem, monoFontID );
	MONOSPACE_FONT * font = &renderFont->font;
	
	flo32 scale = 1.0f;
	
	int32 char_count = ( int32 )strlen( string );
	flo32 string_width = char_count * font->advanceWidth;
	vec2 string_offset = getOffset( align, string_width, font->ascent, font->descent, scale );
	
	drawMonoString( renderSystem, renderFont, string, char_count, position + string_offset, color, scale );
}

internal flo32
getWidth( FONT * font, const char * string, int32 length ) {
	flo32 result = 0.0f;
	for( int32 char_index = 0; char_index < length; char_index++ ) {
		char string_charA = string[ char_index ];
		if( string_charA == ' ' ) {
			result += ( font->advanceWidth_space );
		} else {		
			int32 char_indexA     = string_charA - FONT_START_CHAR;
			FONT_CHAR * font_char = font->alphabet + char_indexA;
			result += font_char->advanceWidth;
			
			if( ( char_index + 1 ) < length ) {
				char string_charB = string[ char_index + 1 ];
				if( string_charB != ' ' ) {
					int32 char_indexB = string_charB - FONT_START_CHAR;
					int32 kernValue = font->kernValue[ char_indexA * FONT_CHAR_COUNT + char_indexB ];
					result += ( ( flo32 )kernValue );
				}
			}
		}
	}
	return result;
}

internal void
drawString( RENDER_SYSTEM * renderSystem, RENDER_FONT * renderFont, const char * string, int32 length, vec2 position, vec4 color, flo32 scale ) {
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, color );
	
	FONT * font = &renderFont->font;
	
	vec2 atPos = position;
	for( int32 char_index = 0; char_index < length; char_index++ ) {
		char string_charA = string[ char_index ];
		if( string_charA == ' ' ) {
			atPos.x += ( font->advanceWidth_space * scale );
		} else {
			int32 char_indexA = string_charA - FONT_START_CHAR;
			FONT_CHAR * font_char = font->alphabet + char_indexA;
			
			flo32 left   = atPos.x + font_char->offset.x * scale;
			flo32 right  = left    + font_char->dim.x    * scale;
			flo32 top    = atPos.y + font_char->offset.y * scale;
			flo32 bottom = top     - font_char->dim.y    * scale;
			rect bound = Rect( left, bottom, right, top );
			
			addQuad( renderSystem, bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
			
			atPos.x += font_char->advanceWidth * scale;
		
			if( ( char_index + 1 ) < length ) {
				char string_charB = string[ char_index + 1 ];
				if( string_charB != ' ' ) {
					int32 char_indexB = string_charB - FONT_START_CHAR;
					int32 kernValue = font->kernValue[ char_indexA * FONT_CHAR_COUNT + char_indexB ];
					atPos.x += ( ( flo32 )kernValue * scale );
				}
			}
		}
	}
	endTask( renderSystem );
}

internal void
drawString( RENDER_SYSTEM * renderSystem, ASSET_ID fontID, const char * string, vec2 position, vec4 color, TEXT_ALIGNMENT align = ALIGN_DEFAULT, flo32 scale = 1.0f ) {
	RENDER_FONT * 	renderFont = getFont( renderSystem->assetSystem, fontID );
	FONT * font = &renderFont->font;
	
	int32 char_count = ( int32 )strlen( string );
	flo32 string_width = getWidth( font, string, char_count );
	vec2 string_offset = getOffset( align, string_width, font->ascent, font->descent, scale );
	
	drawString( renderSystem, renderFont, string, char_count, position + string_offset, color, scale );
}

#endif	// STD_INCLUDE_FUNC
