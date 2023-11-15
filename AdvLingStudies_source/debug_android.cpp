
#ifdef	STD_INCLUDE_DECL

//----------
// debug declarations
//----------

#define PROFILE_FUNCTION()
#define DISPLAY_VALUE( type, name )

#if ANDROID_DEBUG_BUILD

#define CONSOLE_VALUE( type, name )	{\
													char __debug_string[ 2048 ] = {};\
													sprintf( __debug_string, "DEBUG: " );\
													serialize_##type( __debug_string + 7, #name, name );\
													Log::info( __debug_string );\
												}
												
#define CONSOLE_PTR( name ) {\
									    char __debug_string[ 2048 ] = {};\
										 sprintf( __debug_string, "DEBUG: %s: 0x%llx", #name, ( uint64 )name );\
										 Log::info( __debug_string );\
									 }
									 
internal void CONSOLE_STRING( const char * string );

#else
	
#define CONSOLE_STRING( str )
#define CONSOLE_VALUE( type, value )

#endif // ANDROID_DEBUG_BUILD

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC
#if ANDROID_DEBUG_BUILD

internal void
CONSOLE_STRING( const char * string ) {
	char debug_string[ 2048 ] = {};
	sprintf( debug_string, "DEBUG: %s", string );
	Log::info( debug_string );
}

internal void
CONSOLE_POINTER( const char * name, void * ptr ) {
	char debug_string[ 2048 ] = {};
	sprintf( debug_string, "DEBUG: %s = %016llx", name, ( uint64 )ptr );
	Log::info( debug_string );
}

internal void serialize_int32 ( char * buffer, const char * name, int32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_uint64( char * buffer, const char * name, uint64 value ) { sprintf( buffer, "%s: %llu",   name, value ); }
internal void serialize_boo32 ( char * buffer, const char * name, boo32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_flo32 ( char * buffer, const char * name, flo32  value ) { sprintf( buffer, "%s: %f",     name, value ); }
internal void serialize_vec2  ( char * buffer, const char * name, vec2   value ) { sprintf( buffer, "%s: %f, %f", name, value.x, value.y ); }
internal void serialize_vec3  ( char * buffer, const char * name, vec3   value ) { sprintf( buffer, "%s: %f, %f, %f", name, value.x, value.y, value.z ); }
internal void serialize_vec4  ( char * buffer, const char * name, vec4   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_rect  ( char * buffer, const char * name, rect   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.left, value.bottom, value.right, value.top ); }
internal void serialize_MEMORY ( char * buffer, const char * name, MEMORY memory ) { sprintf( buffer, "%s: base: %llx, size: %llu, used: %llu", name, ( uint64 )memory.base, memory.size, memory.used ); }
internal void serialize_RENDER_OBJECT ( char * buffer, const char * name, RENDER_OBJECT object ) { sprintf( buffer, "%s: vertex_index = %u, texture_bufferID = %u, color: { %.02f, %.02f, %.02f, %.02f }", name, object.vertex_index, object.texture_bufferID, object.color.r, object.color.g, object.color.b, object.color.a ); }
internal void serialize_mat4( char * buffer, const char * name, mat4 value ) { sprintf( buffer, "%s: { %f, %f, %f, %f }, { %f, %f, %f, %f }, { %f, %f, %f, %f }, { %f, %f, %f, %f }", name, value.elem[ 0 ], value.elem[ 1 ], value.elem[ 2 ], value.elem[ 3 ], value.elem[ 4 ], value.elem[ 5 ], value.elem[ 6 ], value.elem[ 7 ], value.elem[ 8 ], value.elem[ 9 ], value.elem[ 10 ], value.elem[ 11 ], value.elem[ 12 ], value.elem[ 13 ], value.elem[ 14 ], value.elem[ 15 ] ); }
internal void serialize_VERTEX_TEXMESH_2D( char * buffer, const char * name, VERTEX_TEXMESH_2D value ) { sprintf( buffer, "%s, { %f, %f }, { %f, %f }", name, value.position.x, value.position.y, value.texCoord.x, value.texCoord.y ); }

#endif // ANDROID_DEBUG_BUILD
#endif	// STD_INCLUDE_FUNC