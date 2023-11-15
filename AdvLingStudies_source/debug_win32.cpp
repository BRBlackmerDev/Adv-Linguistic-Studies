
#ifdef	STD_INCLUDE_DECL

//----------
// debug declarations
//----------

internal void CONSOLE_STRING( const char * string );

struct DEBUG_SYSTEM {
	RENDER_SYSTEM * 	renderSystem;
	vec2					atPos;
	flo32					advanceHeight;
};

#if DEBUG_BUILD
#define DISPLAY_VALUE( type, name ) {\
													char __debug_string[ 2048 ] = {};\
													serialize_##type( __debug_string, #name, name );\
													DISPLAY_STRING( __debug_string );\
												}
#define CONSOLE_VALUE( type, name ) {\
													char __debug_string[ 2048 ] = {};\
													serialize_##type( __debug_string, #name, name );\
													strcat( __debug_string, "\n" );\
													OutputDebugString( __debug_string );\
												}

#else
#define DISPLAY_STRING( str )
#define DISPLAY_VALUE( type, name )
#define CONSOLE_STRING( str )

internal void PROFILE_string( char * string ) {}
internal void PROFILE_tooltip( char * string, vec2 position ) {}

#endif

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// debug functions
//----------

#if DEBUG_BUILD
#define DEBUG_SYSTEM__MAX_VERTEX_COUNT		16384
#define DEBUG_SYSTEM__MAX_MEMORY_SIZE    _MB( 2 )
#define DEBUG_SYSTEM__SIZE_IN_BYTES  		( sizeof( RENDER_SYSTEM ) + DEBUG_SYSTEM__MAX_MEMORY_SIZE )
DEBUG_SYSTEM globalVar_debugSystem = {};

internal void
CONSOLE_STRING( const char * string ) {
	OutputDebugString( string );
	OutputDebugString( "\n" );
}

internal void
DEBUG_addTask( uint32 texture_bufferID, rect bound, vec4 color ) {
	RENDER_SYSTEM * renderSystem = globalVar_debugSystem.renderSystem;
	
	beginTask( renderSystem, meshDrawType_triangleFan, texture_bufferID, color );
	addQuad( renderSystem, bound, Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
	endTask( renderSystem );
}

internal void
DEBUG_drawString( char * string, vec2 position ) {
	RENDER_SYSTEM * renderSystem = globalVar_debugSystem.renderSystem;
	
	RENDER_MONOSPACE_FONT * renderFont = getMonoFont( renderSystem->assetSystem, assetID_monoFont_default );
	MONOSPACE_FONT * font = &renderFont->font;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_BLACK );
	
	int32 char_count = ( int32 )strlen( string );
	
	vec2 atPos = position;
	for( int32 counter = 0; counter < char_count; counter++ ) {
		char c = string[ counter ];
		if( c != ' ' ) {
			int32 char_index = c - FONT_START_CHAR;
			MONOSPACE_FONT_CHAR _char = font->alphabet[ char_index ];
			
			flo32 left   = atPos.x + _char.offset.x;
			flo32 right  = left    + _char.dim.x;
			flo32 top    = atPos.y + _char.offset.y;
			flo32 bottom = top     - _char.dim.y;
			rect bound = Rect( left, bottom, right, top );
			
			addQuad( renderSystem, bound, rectMM( _char.texCoord_min, _char.texCoord_max ) );
		}
		atPos.x += font->advanceWidth;
	}
	endTask( renderSystem );
}

internal void
DEBUG_drawRect( rect bound, vec4 color ) {
	uint32 texture_bufferID = getTexture( globalVar_debugSystem.renderSystem->assetSystem, assetID_texture_whiteTexture ).bufferID;
	DEBUG_addTask( texture_bufferID, bound, color );
}

internal void
DISPLAY_STRING( char * string ) {
	DEBUG_drawString( string, globalVar_debugSystem.atPos );
	globalVar_debugSystem.atPos.y -= globalVar_debugSystem.advanceHeight;
}

internal void
DISPLAY_STRING( char * string, vec2 position ) {
	DEBUG_drawString( string, position );
}

internal void
PROFILE_string( char * string ) {
	DEBUG_drawString( string, globalVar_profileSystem.atPos );
	globalVar_profileSystem.atPos.y -= globalVar_profileSystem.advanceHeight;
}

internal void
PROFILE_tooltip( char * string, vec2 position ) {
	DEBUG_drawString( string, position );
}

internal void serialize_int32 ( char * buffer, const char * name, int32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_uint64( char * buffer, const char * name, uint64 value ) { sprintf( buffer, "%s: %llu",   name, value ); }
internal void serialize_boo32 ( char * buffer, const char * name, boo32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_flo32 ( char * buffer, const char * name, flo32  value ) { sprintf( buffer, "%s: %f",     name, value ); }
internal void serialize_vec2  ( char * buffer, const char * name, vec2   value ) { sprintf( buffer, "%s: %f, %f", name, value.x, value.y ); }
internal void serialize_vec3  ( char * buffer, const char * name, vec3   value ) { sprintf( buffer, "%s: %f, %f, %f", name, value.x, value.y, value.z ); }
internal void serialize_rect  ( char * buffer, const char * name, rect   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.left, value.bottom, value.right, value.top ); }
internal void serialize_MEMORY ( char * buffer, const char * name, MEMORY memory ) { sprintf( buffer, "%s: base: %llx, size: %llu, used: %llu", name, ( uint64 )memory.base, memory.size, memory.used ); }
internal void serialize_RENDER_OBJECT ( char * buffer, const char * name, RENDER_OBJECT object ) { sprintf( buffer, "%s: vertex_index = %u, texture_bufferID = %u, color: { %.02f, %.02f, %.02f, %.02f }", name, object.vertex_index, object.texture_bufferID, object.color.r, object.color.g, object.color.b, object.color.a ); }

#endif // DEBUG_BUILD
#endif	// STD_INCLUDE_FUNC