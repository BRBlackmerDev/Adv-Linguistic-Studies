
#ifdef	STD_INCLUDE_DECL

struct FILE_DATA {
	uint32 size;
	void * contents;
};

typedef void ( WRITE_FILE )( const char * saveDir, const char * filename, void * data, uint32 size );
typedef FILE_DATA ( READ_FILE  )(                      MEMORY * memory, const char * saveDir, const char * filename );
typedef FILE_DATA ( READ_ASSET )( PLATFORM * platform, MEMORY * memory,                       const char * filename ); 
typedef boo32 ( READ_FILE_CHUNK )( uint64 fileHandle, uint32 offset, uint32 size, void * buffer );
typedef boo32 ( READ_ASSET_CHUNK )( uint64 fileHandle, uint32 offset, uint32 size, void * buffer );
typedef void ( APPEND_FILE )( const char * saveDir, const char * filename, void * data, uint32 size );
typedef void ( END_APPLICATION )( void * platform, void * appState );

struct OPENGL_STATE {
	boo32 shouldInit;
	boo32 shouldKill;
	
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32 screenWidth;
	int32 screenHeight;
	
	int32 shader_program;
	int32 shader_uniform_modColor;
	
	rect clipRect;
};

struct PLATFORM {
	boo32 isActive;
	boo32 hasFocus;
	boo32 hasSurface;
	
	MOUSE_STATE mouse;
	KEYBOARD_STATE keyboard;
	
	OPENGL_STATE openGLState;
	
	int32 select_transitionCount;
	boo32 select_isDown;
	vec2	select_pos;
	
	android_app * application;
	const char * saveDir;
	const char * extDir;
	
	MEMORY permMemory;
	MEMORY tempMemory;
	
	WRITE_FILE * writeFile;
	READ_FILE  * readFile;
	READ_ASSET * readAsset;
	READ_FILE_CHUNK * readFile_chunk;
	READ_ASSET_CHUNK * readAsset_chunk;
	APPEND_FILE * appendFile;
	END_APPLICATION * endApplication;
	
	ASSET_SYSTEM assetSystem;
	RENDER_SYSTEM renderSystem;
	flo32 targetSec;
	
	vec2 camera_origin;
	vec2 camera_dim;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

internal int32
getFileSize( FILE * file ) {
	int32 result = 0;
	if( file ) {
		fseek( file, 0, SEEK_END );
		result = ftell( file );
		fseek( file, 0, SEEK_SET );
	}
	return result;
}

internal FILE_DATA
android_readFile( MEMORY * memory, const char * baseDir, const char * filename ) {
	char filePath[ 1024 ] = {};
	sprintf( filePath, "%s/%s", baseDir, filename );
	
	FILE_DATA result = {};
	FILE * file = fopen( filePath, "rb" );
	if( file ) {
		int32 file_size = getFileSize( file );
		uint8 * data = ( uint8 * )_pushSize_clear( memory, file_size );
		int32 bytesRead = fread( data, 1, file_size, file );
		if( bytesRead == file_size ) {
			result.size = file_size;
			result.contents 	 = data;
		} else {
			char debug_string[ 1024 ] = {};
			sprintf( debug_string, "ERROR. Unable to read file: %s", filePath );
			CONSOLE_STRING( debug_string );
		}
		fclose( file );
	} else {
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "ERROR. Unable to open file: %s", filePath );
		CONSOLE_STRING( debug_string );
	}
	return result;
}

internal FILE_DATA
android_readAsset( PLATFORM * platform, MEMORY * memory, const char * filename ) {
	FILE_DATA result = {};
	
	AAsset * file = AAssetManager_open( platform->application->activity->assetManager, filename, AASSET_MODE_STREAMING );
	if( file ) {
		int32 size = ( int32 )AAsset_getLength( file );
		void * buffer = _pushSize_clear( memory, size );
		int32 bytesRead = ( int32 )AAsset_read( file, buffer, size );
		if( bytesRead == size ) {
			result.contents = buffer;
			result.size     = size;
		} else {
			char debug_string[ 1024 ] = {};
			sprintf( debug_string, "ERROR. Error reading asset: %s", filename );
			CONSOLE_STRING( debug_string );
		}
		
		AAsset_close( file );
	} else {
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "ERROR. Unable to open asset: %s", filename );
		CONSOLE_STRING( debug_string );
	}
	return result;
}

internal boo32
android_readFile_chunk( uint64 fileHandle, uint32 offset, uint32 size, void * buffer ) {
	boo32 result = false;
	if( fileHandle ) {
		// TODO: check that file handle is still open
		int32 error = fseek( ( FILE * )fileHandle, offset, SEEK_SET );
		if( !error ) {
			int32 bytesRead = fread( buffer, 1, size, ( FILE * )fileHandle );
			if( bytesRead == size ) {
				result = true;
			} else {
				CONSOLE_STRING( "ERROR. Error reading in requested file chunk." );
			}
		} else {
			CONSOLE_STRING( "ERROR. Unable to set file pointer in requested file." );
		}
	} else {
		CONSOLE_STRING( "ERROR. fileHandle is not valid." );
	}
	return result;
}

internal boo32
android_readAsset_chunk( uint64 fileHandle, uint32 offset, uint32 size, void * buffer ) {
	boo32 result = false;
	if( fileHandle ) {
		// TODO: check that file handle is still open
		AAsset * assetFile_handle = ( AAsset * )fileHandle;
		int32 error = AAsset_seek( assetFile_handle, offset, SEEK_SET );
		if( error != -1 ) {
			int32 bytesRead = AAsset_read( assetFile_handle, buffer, size );
			if( bytesRead == size ) {
				result = true;
			} else {
				CONSOLE_STRING( "ERROR. Error reading in requested file chunk." );
			}
		} else {
			CONSOLE_STRING( "ERROR. Unable to set file pointer in requested file." );
		}
	} else {
		CONSOLE_STRING( "ERROR. fileHandle is not valid." );
	}
	return result;
}

internal void
android_writeFile( const char * saveDir, const char * filename, void * data, uint32 size ) {
	char filePath[ 1024 ] = {};
	sprintf( filePath, "%s/%s", saveDir, filename );
	
	FILE * file = fopen( filePath, "wb" );
	if( file ) {
		int32 bytesWritten = fwrite( data, 1, size, file );
		if( bytesWritten != size ) {
			char debug_string[ 1024 ] = {};
			sprintf( debug_string, "ERROR. Unable to write file: %s", filePath );
			CONSOLE_STRING( debug_string );
		}
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "SUCCESS. Wrote file: %s", filePath );
		CONSOLE_STRING( debug_string );
		fclose( file );
	} else {
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "ERROR. Unable to open file for writing: %s", filePath );
		CONSOLE_STRING( debug_string );
	}
}

internal void
android_appendFile( const char * saveDir, const char * filename, void * data, uint32 size ) {
	char filePath[ 1024 ] = {};
	sprintf( filePath, "%s/%s", saveDir, filename );
	
	FILE * file = fopen( filePath, "ab" );
	if( file ) {
		int32 bytesWritten = fwrite( data, 1, size, file );
		if( bytesWritten != size ) {
			char debug_string[ 1024 ] = {};
			sprintf( debug_string, "ERROR. Unable to append file: %s", filePath );
			CONSOLE_STRING( debug_string );
		}
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "SUCCESS. Appended file: %s", filePath );
		CONSOLE_STRING( debug_string );
		fclose( file );
	} else {
		char debug_string[ 1024 ] = {};
		sprintf( debug_string, "ERROR. Unable to open file for appending: %s", filePath );
		CONSOLE_STRING( debug_string );
	}
}

internal OPENGL_STATE
createOpenGLContext( android_app * application ) {
	EGLDisplay display = 0;
	EGLSurface surface = 0;
	EGLContext context = 0;
		
	display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	if( !display ) {
		// Log::info( "DEBUG: eglGetDisplay(): ERROR" );
	}
	
	EGLint majorVersion = 0;
	EGLint minorVersion = 0;
	EGLBoolean EGL_displayIsInitialized = 0;
	EGL_displayIsInitialized = eglInitialize( display, &majorVersion, &minorVersion );
	if( EGL_displayIsInitialized ) {
		// Log::info( "DEBUG: eglInitialize(): SUCCESS: Major: %d, Minor: %d", majorVersion, minorVersion );
	} else {
		// Log::info( "DEBUG: eglInitialize(): ERROR" );
	}
	
	EGLint displayAttribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE,
	};
	
	EGLConfig matchingConfig = 0;
	EGLint matchingConfig_count = 0;
	
	EGLBoolean EGL_foundMatchingConfig = 0;
	EGL_foundMatchingConfig = eglChooseConfig( display, displayAttribs, &matchingConfig, 1, &matchingConfig_count );
	if( EGL_foundMatchingConfig ) {
		// Log::info( "DEBUG: eglChooseConfig(): SUCCESS: Match Count: %d" , matchingConfig_count );
	} else {
		// Log::info( "DEBUG: eglChooseConfig(): ERROR" );
	}
	
	EGLint nativeVisualID = 0;
	EGLBoolean EGL_gotNativeVisualID = 0;
	EGL_gotNativeVisualID = eglGetConfigAttrib( display, matchingConfig, EGL_NATIVE_VISUAL_ID, &nativeVisualID );
	if( EGL_gotNativeVisualID ) {
		// Log::info( "DEBUG: eglGetConfigAttrib( EGL_NATIVE_VISUAL_ID ): SUCCESS: Native ID: %d", nativeVisualID );
	} else {
		// Log::info( "DEBUG: eglGetConfigAttrib( EGL_NATIVE_VISUAL_ID ): ERROR" );
	}
	
	int setBufferGeometry_returnValue = 0;
	setBufferGeometry_returnValue = ANativeWindow_setBuffersGeometry( application->window, 0, 0, nativeVisualID );
	if( setBufferGeometry_returnValue == 0 ) {
		// Log::info( "DEBUG: ANativeWindow_setBuffersGeometry(): SUCCESS" );
	} else {
		// Log::info( "DEBUG: ANativeWindow_setBuffersGeometry(): ERROR" );
	}
	
	surface = eglCreateWindowSurface( display, matchingConfig, application->window, 0 );
	if( surface ) {
		// Log::info( "DEBUG: eglCreateWindowSurface(): SUCCESS" );
	} else {
		// Log::info( "DEBUG: eglCreateWindowSurface(): ERROR" );
	}
	
	EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE,
	};
	
	context = eglCreateContext( display, matchingConfig, 0, contextAttribs );
	if( context ) {
		// Log::info( "DEBUG: eglCreateContext(): SUCCESS" );
	} else {
		// Log::info( "DEBUG: eglCreateContext(): ERROR" );
	}
	
	EGLBoolean contextIsCurrent = 0;
	contextIsCurrent = eglMakeCurrent( display, surface, surface, context );
	if( contextIsCurrent ) {
		// Log::info( "DEBUG: new openGL context created and set" );
		// // Log::info( "DEBUG: eglMakeCurrent(): SUCCESS" );
	} else {
		// Log::info( "DEBUG: eglMakeCurrent(): ERROR" );
	}
	
	EGLint screenWidth = 0;
	EGLBoolean widthIsValid = 0;
	widthIsValid = ( eglQuerySurface( display, surface, EGL_WIDTH, &screenWidth ) && ( screenWidth > 0 ) );
	if( widthIsValid ) {
		// appState->appWidth = screenWidth;
		// Log::info( "DEBUG: Screen Width: %d", screenWidth );
	} else {
		// Log::info( "DEBUG: Screen Width: ERROR" );
	}
	
	EGLint screenHeight = 0;
	EGLBoolean heightIsValid = 0;
	heightIsValid = ( eglQuerySurface( display, surface, EGL_HEIGHT, &screenHeight ) && ( screenHeight > 0 ) );
	if( heightIsValid ) {
		// appState->appHeight = screenHeight;
		// Log::info( "DEBUG: Screen Height: %d", screenHeight );
	} else {
		// Log::info( "DEBUG: Screen Height: ERROR" );
	}
	
	glEnable( GL_SCISSOR_TEST );
	
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	
	OPENGL_STATE result = {};
	result.display = display;
	result.surface = surface;
	result.context = context;
	result.screenWidth = screenWidth;
	result.screenHeight = screenHeight;
	return result;
}

internal void
appSaveState( APP_STATE * appState, PLATFORM * platform ) {
	
}

internal void
callback_processAppEvent( android_app * application, int32_t pCommand ) {
	APP_STATE * appState = ( APP_STATE * )application->userData;
	PLATFORM * platform = ( PLATFORM * )( ( uint8 * )application->userData + sizeof( APP_STATE ) );
	
	switch( pCommand ) {
		case APP_CMD_CONFIG_CHANGED: {
			CONSOLE_STRING( "AppEvent: APP_CMD_CONFIG_CHANGED" );
		} break;
		
		case APP_CMD_INPUT_CHANGED: {
			CONSOLE_STRING( "AppEvent: APP_CMD_INPUT_CHANGED" );
		} break;
		
		case APP_CMD_INIT_WINDOW: {
			CONSOLE_STRING( "AppEvent: APP_CMD_INIT_WINDOW" );
			
			platform->hasSurface = true;
			platform->openGLState.shouldInit = true;
		} break;
		
		case APP_CMD_DESTROY: {
			CONSOLE_STRING( "AppEvent: APP_CMD_DESTROY" );
			appState->isRunning = false;
		} break;
		
		case APP_CMD_GAINED_FOCUS: {
			CONSOLE_STRING( "AppEvent: APP_CMD_GAINED_FOCUS" );
			
			platform->hasFocus = true;
		} break;
		
		case APP_CMD_LOST_FOCUS: {
			CONSOLE_STRING( "AppEvent: APP_CMD_LOST_FOCUS" );
			
			platform->hasFocus = false;
		} break;
		
		case APP_CMD_LOW_MEMORY: {
			CONSOLE_STRING( "AppEvent: APP_CMD_LOW_MEMORY" );
		} break;
		
		case APP_CMD_PAUSE: {
			CONSOLE_STRING( "AppEvent: APP_CMD_PAUSE" );
			
			platform->isActive = false;
		} break;
		
		case APP_CMD_RESUME: {
			CONSOLE_STRING( "AppEvent: APP_CMD_RESUME" );
			
			platform->isActive = true;
		} break;
		
		case APP_CMD_SAVE_STATE: {
			CONSOLE_STRING( "AppEvent: APP_CMD_SAVE_STATE" );
			appSaveState( appState, platform );
		} break;
		
		case APP_CMD_START: {
			CONSOLE_STRING( "AppEvent: APP_CMD_START" );
		} break;
		
		case APP_CMD_STOP: {
			CONSOLE_STRING( "AppEvent: APP_CMD_STOP" );
			appSaveState( appState, platform );
			
			// ANativeActivity_finish( application->activity );
		} break;
		
		case APP_CMD_TERM_WINDOW: {
			CONSOLE_STRING( "AppEvent: APP_CMD_TERM_WINDOW" );
			
			platform->hasSurface = false;
			platform->openGLState.shouldKill = true;
		} break;
		
		default: {
			char debug_string[ 2048 ] = {};
			sprintf( debug_string, "AppEvent: Unhandled event: %d", pCommand );
			CONSOLE_STRING( debug_string );
		} break;
	}
}

internal int32_t
callback_processInputEvent( android_app * application, AInputEvent * event ) {
	APP_STATE * appState = ( APP_STATE * )application->userData;
	PLATFORM * platform = ( PLATFORM * )( ( uint8 * )application->userData + sizeof( APP_STATE ) );
	OPENGL_STATE * openGLState = &platform->openGLState;
	if( appState->isRunning ) {
		int32 eventType = AInputEvent_getType( event );
		switch( eventType ) {
			case AINPUT_EVENT_TYPE_KEY: {
				// CONSOLE_STRING( "InputEvent: AINPUT_EVENT_TYPE_KEY" );
				
				// int32 action 		= AKeyEvent_getAction( event );
				// int32 flags			= AKeyEvent_getFlags( event );
				// int32 keyCode		= AKeyEvent_getKeyCode( event );
				// int32 scanCode		= AKeyEvent_getScanCode( event );
				// int32 metaState	= AKeyEvent_getMetaState( event );
				
				// char debug_string[ 512 ] = {};
				// sprintf( debug_string, "AC: %d, FL: %d, KC: %d, SC: %d, MS: %d", action, flags, keyCode, scanCode, metaState );
				// CONSOLE_STRING( debug_string );
			} break;
			
			case AINPUT_EVENT_TYPE_MOTION: {
				// CONSOLE_STRING( "InputEvent: AINPUT_EVENT_TYPE_MOTION" );
				
				int32 motionEvent_type = AMotionEvent_getAction( event );
				switch( motionEvent_type ) {
					case AMOTION_EVENT_ACTION_DOWN: {
						// CONSOLE_STRING( "InputEvent: AINPUT_EVENT_TYPE_MOTION : AMOTION_EVENT_ACTION_DOWN" );
						
						platform->select_transitionCount++;
						platform->select_isDown = true;
						
						flo32 xPos = AMotionEvent_getX( event, 0 );
						flo32 yPos = AMotionEvent_getY( event, 0 );
						yPos = ( flo32 )openGLState->screenHeight - yPos;
						platform->select_pos = Vec2( xPos, yPos );
					} break;
					
					case AMOTION_EVENT_ACTION_UP: {
						// CONSOLE_STRING( "InputEvent: AINPUT_EVENT_TYPE_MOTION : AMOTION_EVENT_ACTION_UP" );
						
						platform->select_transitionCount++;
						platform->select_isDown = false;
						
						flo32 xPos = AMotionEvent_getX( event, 0 );
						flo32 yPos = AMotionEvent_getY( event, 0 );
						yPos = ( flo32 )openGLState->screenHeight - yPos;
						platform->select_pos = Vec2( xPos, yPos );
					} break;
					
					case AMOTION_EVENT_ACTION_MOVE: {
						// CONSOLE_STRING( "InputEvent: AINPUT_EVENT_TYPE_MOTION : AMOTION_EVENT_ACTION_MOVE" );
						
						flo32 xPos = AMotionEvent_getX( event, 0 );
						flo32 yPos = AMotionEvent_getY( event, 0 );
						yPos = ( flo32 )openGLState->screenHeight - yPos;
						platform->select_pos = Vec2( xPos, yPos );
					} break;
					
					default: {
						// InvalidCodePath;
					} break;
				}
			} break;
		}
	}
	return 0;
}

internal void
initOpenGL( APP_STATE * appState, PLATFORM * platform, android_app * application ) {
	CONSOLE_STRING( "initOpenGL()" );
	OPENGL_STATE * openGLState   = &platform->openGLState;
	RENDER_SYSTEM * renderSystem = &platform->renderSystem;
	ASSET_SYSTEM * assetSystem   = &platform->assetSystem;
	MEMORY * tempMemory          = &platform->tempMemory;
	
	*openGLState = createOpenGLContext( application );
	
	// appState->isRunning = true;
	// appState->isFocus = true;
	
	int32 client_width	= openGLState->screenWidth;
	int32 client_height	= openGLState->screenHeight;
	CONSOLE_VALUE( int32, client_width );
	CONSOLE_VALUE( int32, client_height );
	
	vec2 client_dim = Vec2( ( flo32 )client_width, ( flo32 )client_height );
	// NOTE: game is always displayed in landscape, so height and width are swapped
	vec2 client_halfDim = client_dim * 0.5f;
	
	// vec2 app_dim = client_dim;
	vec2 app_dim = Vec2( ( flo32 )APP_width, ( flo32 )APP_height );
	vec2 app_halfDim = app_dim * 0.5f;
	rect app_bound = rectBLD( Vec2( 0.0f, 0.0f ), app_dim );
	appState->app_dim = app_dim;
	appState->app_halfDim = app_halfDim;
	appState->app_bound = app_bound;
	
	vec2 camera_dim = app_dim;
	flo32 client_aspectRatio = client_dim.y / client_dim.x; // NOTE: swap for landscape mode
	flo32    app_aspectRatio =    app_dim.x /    app_dim.y;
	
	if( app_aspectRatio < client_aspectRatio ) {
		camera_dim.x *= ( client_aspectRatio / app_aspectRatio );
	} else {
		camera_dim.y *= ( app_aspectRatio / client_aspectRatio );
	}
	rect camera_bound = rectCD( app_halfDim, camera_dim );
	
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, openGLState->screenWidth, openGLState->screenHeight );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	
	const char * shader_vertexCode = "\
	uniform mat4 transform;\n\
	attribute vec2 inPosition;\n\
	attribute vec2 inTexCoord;\n\
	varying vec2 textureCoord;\n\
	void main( void ) {\n\
		vec4 adjPos = transform * vec4( inPosition, 0.0f, 1.0f );\n\
		gl_Position = vec4( adjPos.y, -adjPos.x, 0.0, 1.0f );\n\
		textureCoord = inTexCoord;\n\
	}\n";
	
	const char * shader_fragmentCode = "\
	uniform sampler2D textureSampler;\n\
	uniform vec4 modColor;\n\
	varying vec2 textureCoord;\n\
	void main( void ) {\n\
		gl_FragColor = modColor * texture2D( textureSampler, textureCoord.st );\n\
	}\n";
	
	CREATE_SHADER_RESULT shader = createShader( shader_vertexCode, shader_fragmentCode );
	
	glUseProgram( shader.program );
	openGLState->shader_program = shader.program;
	
	mat4 camera_transform = mat4_orthographic( camera_bound.left, camera_bound.bottom, camera_bound.right, camera_bound.top, -1.0f, 1.0f );
	
	vec4 app_min_v4 = camera_transform * Vec4(               0.0f,                0.0f, 0.0f, 1.0f );
	vec4 app_max_v4 = camera_transform * Vec4( ( flo32 )APP_width, ( flo32 )APP_height, 0.0f, 1.0f );
	vec2 app_min = ( Vec2( app_min_v4.y, -app_min_v4.x ) + Vec2( 1.0f, 1.0f ) ) * client_halfDim;
	CONSOLE_VALUE( vec2, app_min );
	vec2 app_max = ( Vec2( app_max_v4.y, -app_max_v4.x ) + Vec2( 1.0f, 1.0f ) ) * client_halfDim;
	CONSOLE_VALUE( vec2, app_max );
	vec2 adj_origin   = -getBL( camera_bound ) / camera_dim;
		  adj_origin   = Vec2( adj_origin.y * client_dim.x, ( 1.0f - adj_origin.x ) * client_dim.y );
	CONSOLE_VALUE( vec2, adj_origin );
	rect adj_bound    = Rect( app_min.x, app_max.y, app_max.x, app_min.y );
	vec2 adj_dim      = Vec2( getWidth( adj_bound ), getHeight( adj_bound ) );
	
	platform->camera_dim    = camera_dim;
	platform->camera_origin = adj_origin;
	
	openGLState->clipRect = adj_bound;
	
	int32 uniform_transform = glGetUniformLocation( shader.program, "transform" );
	glUniformMatrix4fv( uniform_transform, 1, GL_FALSE, camera_transform.elem );
	
	openGLState->shader_uniform_modColor = glGetUniformLocation( shader.program, "modColor" );
	
	glUniform1i( glGetUniformLocation( shader.program, "textureSampler" ), 0 );
	
	{ // gen default texture
		uint8 defaultTexture[] = { 80, 80, 80, 255,   255, 0, 255, 255,   255, 0, 255, 255,   80, 80, 80, 255 };
		assetSystem->defaultTexture = GL_loadTexture( 2, 2, defaultTexture );
	}
	
	{ // gen default font
		genDefaultMonoFont( assetSystem, tempMemory );
	}
}

internal void
killOpenGL( OPENGL_STATE * openGLState, ASSET_SYSTEM * assetSystem ) {
	CONSOLE_STRING( "killOpenGL()" );
	glDeleteTextures( 1, &assetSystem->defaultTexture.bufferID );
	glDeleteTextures( 1, &assetSystem->defaultMonoFont.texture_bufferID );
	
	// delete textures
	int32 texture_count = assetSystem->texture_count;
	for( int32 iter = 0; iter < texture_count; iter++ ) {
		RENDER_TEXTURE * texture = assetSystem->texture_list + iter;
		glDeleteTextures( 1, &texture->bufferID );
	}
	memset( &assetSystem->texture_count, 0, sizeof( int32 ) + _arrayBytes( assetSystem->textureID_list ) + _arrayBytes( assetSystem->texture_list ) );
	
	// delete fonts
	for( int32 iter = 0; iter < assetSystem->font_count; iter++ ) {
		RENDER_FONT * font = assetSystem->font_list + iter;
		glDeleteTextures( 1, &font->texture_bufferID );
	}
	for( int32 iter = 0; iter < assetSystem->monoFont_count; iter++ ) {
		RENDER_MONOSPACE_FONT * monoFont = assetSystem->monoFont_list + iter;
		glDeleteTextures( 1, &monoFont->texture_bufferID );
	}
	memset( &assetSystem->font_count, 0, sizeof( int32 ) + _arrayBytes( assetSystem->fontID_list ) + _arrayBytes( assetSystem->font_list ) );
	
	// delete shaders
	glDeleteProgram( openGLState->shader_program );
	
	// delete context
	if( openGLState->display != EGL_NO_DISPLAY ) {
		eglMakeCurrent( openGLState->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		if( openGLState->context != EGL_NO_CONTEXT ) {
			eglDestroyContext( openGLState->display, openGLState->context );
			openGLState->context = EGL_NO_CONTEXT;
		}
		if( openGLState->surface != EGL_NO_SURFACE ) {
			eglDestroySurface( openGLState->display, openGLState->surface );
			openGLState->surface = EGL_NO_SURFACE;
		}
		eglTerminate( openGLState->display );
		openGLState->display = EGL_NO_DISPLAY;
	}
}

internal void
android_endApplication( void * platform, void * appState ) {
	PLATFORM *  myPlatform = ( PLATFORM *  )platform;
	APP_STATE * myAppState = ( APP_STATE * )appState;
	
	ANativeActivity_finish( myPlatform->application->activity );
}

void android_main( android_app * application ) {
	CONSOLE_STRING( "android_main()" );
	
	srand( time( 0 ) );
	
	MEMORY platformMemory = {};
	platformMemory.size = ( sizeof( APP_STATE ) + sizeof( PLATFORM ) + APP_permMemorySize + APP_tempMemorySize );
	platformMemory.base = calloc( platformMemory.size, 1 );
	
	if( platformMemory.base ) {
		APP_STATE * appState = _pushType( &platformMemory, APP_STATE );
		appState->isRunning = true;
		
		PLATFORM * android_platform 		 = _pushType( &platformMemory, PLATFORM );
		android_platform->application 	 = application;
		android_platform->readFile			 = android_readFile;
		android_platform->readAsset       = android_readAsset;
		android_platform->writeFile		 = android_writeFile;
		android_platform->readFile_chunk  = android_readFile_chunk;
		android_platform->readAsset_chunk = android_readAsset_chunk;
		android_platform->appendFile      = android_appendFile;
		android_platform->saveDir			 = android_platform->application->activity->internalDataPath;
		android_platform->endApplication  = android_endApplication;
		CONSOLE_STRING( android_platform->saveDir );
		android_platform->extDir			 = android_platform->application->activity->externalDataPath;
		CONSOLE_STRING( android_platform->extDir );
		android_platform->permMemory 		 = Memory( _pushSize( &platformMemory, APP_permMemorySize ), APP_permMemorySize );
		android_platform->tempMemory 		 = Memory( _pushSize( &platformMemory, APP_tempMemorySize ), APP_tempMemorySize );
		android_platform->targetSec 		 = 1.0f / APP_targetFPS;
		
		RENDER_SYSTEM * renderSystem = &android_platform->renderSystem;
		ASSET_SYSTEM * assetSystem = &android_platform->assetSystem;
		renderSystem->assetSystem = assetSystem;
		
		AAssetManager * 	assetManager 	= application->activity->assetManager;
		AAsset * 			assetFile 		= AAssetManager_open( assetManager, "assets.pak", AASSET_MODE_RANDOM );
		if( assetFile ) {
			assetSystem->assetFile_handle = ( uint64 )assetFile;
			assetSystem->platform = android_platform;
			
			int32 read_size = sizeof( ASSET_ENTRY ) * assetID_count;
			int32 bytesRead = AAsset_read( assetFile, assetSystem->entry, read_size );
			if( read_size != bytesRead ) {
				CONSOLE_STRING( "ERROR! Error occurred reading asset header" );
			}
		} else {
			CONSOLE_STRING( "ERROR! Unable to open assets.pak" );
		}
		
		MEMORY * tempMemory = &android_platform->tempMemory;
		MEMORY * permMemory = &android_platform->permMemory;
		
		application->userData = appState;
		application->onAppCmd = callback_processAppEvent;
		application->onInputEvent = callback_processInputEvent;
		
		app_dummy();
		
		int32_t result;
		int32_t events;
		android_poll_source * source;

		timespec currentTime = {};
		timespec previousTime = {};
		clock_gettime( CLOCK_MONOTONIC, &previousTime );
		
		int32 saveState_size = sizeof( SAVE_STATE );
		CONSOLE_VALUE( int32, saveState_size );
		
		// int32 checklist_size = sizeof( CHECKLIST );
		// CONSOLE_VALUE( int32, checklist_size );
		
		// int32 newSaveState_size = sizeof( NEW_SAVE_STATE );
		// CONSOLE_VALUE( int32, newSaveState_size );
		
		// int32 newChecklist_size = sizeof( NEW_CHECKLIST );
		// CONSOLE_VALUE( int32, newChecklist_size );
		
		while( appState->isRunning ) {
			// frame_counter++;
			
			PLATFORM * platform = android_platform;
			OPENGL_STATE * openGLState = &platform->openGLState;
			// appState->select_transitionCount = 0;
			
			// clock_gettime( CLOCK_MONOTONIC, &currentTime );
			// // TODO : timespec is a looping counter and current-previous will sometimes return a negative number
			// flo32 msElapsed = ( flo32 )( currentTime.tv_nsec - previousTime.tv_nsec ) / 1000000.0f;
			// // Assert( currentTime.tv_nsec >= previousTime.tv_nsec );
			// // Assert( msElapsed > 0.0f );
			// // appState->msElapsed = msElapsed;
			// // Log::info( "DEBUG: Frame Time: %f", msElapsed );
			// previousTime = currentTime;
			
			platform->select_transitionCount = 0;
			while ( ( result = ALooper_pollAll( 0, NULL, &events, ( void ** )&source ) ) >= 0 ) {
				if( source ) {
					source->process( application, source );
				}
			}
			
			
			boo32 canUpdate = ( ( platform->hasFocus ) && ( platform->isActive ) && ( platform->hasSurface ) );
			if( canUpdate ) {
				if( openGLState->shouldKill ) {
					openGLState->shouldKill = false;
					killOpenGL( openGLState, assetSystem );
				}
				if( openGLState->shouldInit ) {
					openGLState->shouldInit = false;
					initOpenGL( appState, android_platform, application );
				}
				vec2    app_dim = appState->app_dim;
				vec2 client_dim = Vec2( ( flo32 )openGLState->screenWidth, ( flo32 )openGLState->screenHeight );
				// vec2 adjSelect_pos = Vec2( client_dim.y - platform->select_pos.y, platform->select_pos.x );
				     // adjSelect_pos -= platform->camera_origin;
				vec2 adjSelect_pos = ( platform->select_pos - platform->camera_origin );
				appState->select_position    = Vec2( -adjSelect_pos.y, adjSelect_pos.x ) / Vec2( client_dim.y, client_dim.x ) * platform->camera_dim;
				appState->select_wasPressed  = ( platform->select_isDown && ( platform->select_transitionCount > 0 ) ) || ( !platform->select_isDown && ( platform->select_transitionCount > 1 ) );
				appState->select_wasReleased = ( platform->select_isDown && ( platform->select_transitionCount > 1 ) ) || ( !platform->select_isDown && ( platform->select_transitionCount > 0 ) );
				
				CONSOLE_VALUE( vec2, appState->select_position );
				
				APP_updateAndRender( appState, android_platform );
				
				glScissor( 0, 0, openGLState->screenWidth, openGLState->screenHeight );
				glClear( GL_COLOR_BUFFER_BIT );
				
				glScissor( ( int32 )openGLState->clipRect.left, ( int32 )openGLState->clipRect.bottom, ( int32 )getWidth( openGLState->clipRect ), ( int32 )getHeight( openGLState->clipRect ) );
				render( renderSystem, openGLState->shader_uniform_modColor );
				
				eglSwapBuffers( openGLState->display, openGLState->surface );
				
				renderSystem->object_count = 0;
				renderSystem->vertex_count = 0;
			}
			if( openGLState->shouldKill ) {
				openGLState->shouldKill = false;
				killOpenGL( openGLState, assetSystem );
			}
		}
		
		free( platformMemory.base );
	} else {
		CONSOLE_STRING( "ERROR: Unable to malloc() memory needed for application. Exiting..." );
	}
	CONSOLE_STRING( "APP EXIT" );
}

#endif	// STD_INCLUDE_FUNC
