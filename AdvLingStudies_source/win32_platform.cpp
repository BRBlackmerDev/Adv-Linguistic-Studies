
#ifdef	STD_INCLUDE_DECL

struct FILE_DATA {
	uint32 size;
	void * contents;
};

typedef boo32 ( WRITE_FILE )( const char * saveDir, const char * filename, void * data, uint32 size );
typedef FILE_DATA ( READ_FILE  )(                      MEMORY * memory, const char * saveDir, const char * filename );
typedef FILE_DATA ( READ_ASSET )( PLATFORM * platform, MEMORY * memory,                       const char * filename );
typedef boo32 ( READ_FILE_CHUNK  )( uint64 fileHandle, uint32 offset, uint32 size, void * buffer );
typedef boo32 ( READ_ASSET_CHUNK )( uint64 fileHandle, uint32 offset, uint32 size, void * buffer );
typedef void ( END_APPLICATION )( void * platform, void * appState );

struct PLATFORM {
	MOUSE_STATE mouse;
	KEYBOARD_STATE keyboard;
	
	MEMORY permMemory;
	MEMORY tempMemory;
	
	char * saveDir;
	char * extDir;
	
	WRITE_FILE * writeFile;
	READ_FILE  * readFile;
	READ_ASSET * readAsset;
	READ_FILE_CHUNK  * readFile_chunk;
	READ_ASSET_CHUNK * readAsset_chunk;
	END_APPLICATION * endApplication;
	
	ASSET_SYSTEM assetSystem;
	
	RENDER_SYSTEM renderSystem;
	
	flo32 targetSec;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

internal FILE_DATA
win32_readFile( MEMORY * memory, const char * saveDir, const char * filename ) {
	FILE_DATA result = {};
	
	char filePath[ 2048 ] = {};
	if( saveDir ) {
		sprintf( filePath, "%s/%s", saveDir, filename );
	} else {
		sprintf( filePath, "%s", filename );
	}
	
	HANDLE fileHandle = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( fileHandle != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER fileSize;
		if( GetFileSizeEx( fileHandle, &fileSize ) ) {
			result.size = truncToU32( fileSize.QuadPart );
			result.contents = _pushSize_clear( memory, result.size );
			
			DWORD bytesRead;
			if( ReadFile( fileHandle, result.contents, result.size, &bytesRead, 0 ) && ( result.size == bytesRead ) ) {
			} else {
				_popSize( memory, result.size );
				result.contents = 0;
				OutputDebugString( "ERROR. Unable to read file contents.\n" );
			}
			CloseHandle( fileHandle );
		} else {
			OutputDebugString( "ERROR. File does not contain any data.\n" );
		}
	} else {
		char debugStr[128] = {};
		sprintf( debugStr, "ERROR. Unable to open file: %s\n", filename );
		OutputDebugString( debugStr );
	}
	
	return result;
}

internal FILE_DATA
win32_readAsset( PLATFORM * platform, MEMORY * memory, const char * filename ) {
	FILE_DATA result = win32_readFile( memory, 0, filename );
	return result;
}

internal boo32
win32_readFile_chunk( uint64 fileHandle, uint32 offset, uint32 size, void * buffer ) {
	boo32 result = false;
	if( fileHandle ) {
		// TODO: check that file handle is still open
		DWORD error = SetFilePointer( ( HANDLE )fileHandle, offset, 0, FILE_BEGIN );
		if( error != INVALID_SET_FILE_POINTER ) {
			DWORD bytesRead = 0;
			ReadFile( ( HANDLE )fileHandle, buffer, size, &bytesRead, 0 );
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
win32_writeFile( const char * saveDir, const char * filename, void * data, uint32 size ) {
	boo32 result = false;
	
	char filePath[ 2048 ] = {};
	if( saveDir ) {
		sprintf( filePath, "%s/%s", saveDir, filename );
	} else {
		sprintf( filePath, "%s", filename );
	}

	HANDLE fileHandle = CreateFile( filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( fileHandle != INVALID_HANDLE_VALUE ) {
		DWORD bytesWritten;
		if( WriteFile( fileHandle, data, size, &bytesWritten, 0 ) ) {
			result = ( bytesWritten == size );
		}

		CloseHandle( fileHandle );
	} else {
		Assert( !"Unable to write file!" );
	}

	return result;
}

internal int64
win32_getPerfCount() {
	LARGE_INTEGER perfCounter = {};
	QueryPerformanceCounter( &perfCounter );
	
	int64 result = *( ( int64 * )&perfCounter );
	return result;
}

internal flo32
win32_getMSElapsed( int64 startCounter, int64 endCounter, LARGE_INTEGER perfFrequency ) {
	LARGE_INTEGER counterA = *( ( LARGE_INTEGER * )&startCounter );
	LARGE_INTEGER counterB = *( ( LARGE_INTEGER * )&endCounter );
	flo32 result = 1000.0f * ( ( flo32 )( counterB.QuadPart - counterA.QuadPart ) / ( flo32 )perfFrequency.QuadPart );
	return result;
}

#if DEBUG_BUILD

internal void
updateProfile( PROFILE_SYSTEM * profileSystem, PLATFORM * platform, LARGE_INTEGER perfFrequency, RENDER_SYSTEM * renderSystem, rect client_bound ) {
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	if( wasPressed( keyboard, keyCode_p ) ) {
		globalVar_profileSystem.appPaused = !globalVar_profileSystem.appPaused;
	}
	
	if( wasPressed( keyboard, keyCode_o ) ) {
		globalVar_profileSystem.showProfile = !globalVar_profileSystem.showProfile;
		if( globalVar_profileSystem.showProfile ) {
			profileSystem->eventDatabase_scrollActive = false;
		}
	}
	
	if( !globalVar_profileSystem.showProfile ) {
		flo32 msElapsed 	= win32_getMSElapsed( globalVar_profileSystem.eventDatabase_clocks_previousFrame.counter_frameStart, globalVar_profileSystem.eventDatabase_clocks_previousFrame.counter_frameEnd, perfFrequency );
		uint64 cycElapsed = globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameEnd - globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameStart;
		
		char time_string[ 128 ] = {};
		sprintf( time_string, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", msElapsed, cycElapsed, cycElapsed / 1000 );
		DISPLAY_STRING( time_string );
	}
	
	{ // profile previous frame
		if( !globalVar_profileSystem.appPaused ) {
			PROFILE_EVENT_LIST * database_eventList = globalVar_profileSystem.eventDatabase.eventList + globalVar_profileSystem.eventDatabase.index;
			database_eventList->count = 0;
			database_eventList->frameClockStart = globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameStart;
			
			globalVar_profileSystem.eventDatabase_clocks[ globalVar_profileSystem.eventDatabase.index ] = globalVar_profileSystem.eventDatabase_clocks_previousFrame;
			
			for( int32 event_index = 0; event_index < globalVar_profileSystem.eventList_currentFrame->count; event_index++ ) {
				PROFILE_EVENT * srcEvent = globalVar_profileSystem.eventList_currentFrame->event + event_index;
				PROFILE_EVENT * destEvent = database_eventList->event + database_eventList->count++;
				Assert( database_eventList->count < PROFILE_EVENT_LIST_MAX_COUNT );
				
				destEvent->clock_start = srcEvent->clock_start;
				destEvent->clock_end = srcEvent->clock_end;
				setString( srcEvent->functionName, destEvent->functionName );
			}
			globalVar_profileSystem.eventDatabase.index = ( globalVar_profileSystem.eventDatabase.index + 1 ) % globalVar_profileSystem.eventDatabase.maxCount;
		}
		if( globalVar_profileSystem.showProfile ) {
			DEBUG_drawRect( client_bound, Vec4( 0.0f, 0.0f, 0.0f, 0.7f ) );
			vec2  eventDatabase_basePos 	= getBL( client_bound ) + Vec2( profileSystem->eventDatabase_posOffset, profileSystem->eventDatabase_posOffset );
			rect  eventDatabase_bound		= rectBLD( eventDatabase_basePos, Vec2( profileSystem->eventDatabase_width, profileSystem->eventDatabase_height ) );
			flo32 eventDatabase_slotWidth = getWidth( eventDatabase_bound ) / ( flo32 )PROFILE_EVENT_DATABASE_MAX_COUNT;
			
			MOUSE_STATE * mouse = &platform->mouse;
			rect frameBreakdown_scrollBound = rectBLD( getTL( eventDatabase_bound ) + Vec2( 0.0f, 4.0f ), Vec2( getWidth( eventDatabase_bound ), 12.0f ) );
			
			vec2 scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_left, frameBreakdown_scrollBound.bottom );
			vec2 scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left, getHeight( frameBreakdown_scrollBound ) );
			rect frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
			rect frameBreakdown_leftBound  = rectBLD( getBL(  frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
			rect frameBreakdown_rightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
			
			rect eventDatabase_resizeBound = rectBLD( getTL( eventDatabase_bound ) + Vec2( -2.0f, -2.0f ), Vec2( getWidth( eventDatabase_bound ) + 4.0f, 4.0f ) );
			if( profileSystem->eventDatabase_scrollActive ) {
				profileSystem->eventDatabase_height = mouse->position.y - eventDatabase_bound.bottom;
				profileSystem->eventDatabase_height = maxValue( profileSystem->eventDatabase_height, 100.0f );
			}
			if( profileSystem->frameBreakdown_scrollActive_left ) {
				profileSystem->frameBreakdown_scrollX_left = mouse->position.x;
				profileSystem->frameBreakdown_scrollX_left = maxValue( profileSystem->frameBreakdown_scrollX_left, profileSystem->eventDatabase_posOffset );
				profileSystem->frameBreakdown_scrollX_left = minValue( profileSystem->frameBreakdown_scrollX_left, profileSystem->frameBreakdown_scrollX_right - 40.0f );
			}
			if( profileSystem->frameBreakdown_scrollActive_right ) {
				profileSystem->frameBreakdown_scrollX_right = mouse->position.x;
				profileSystem->frameBreakdown_scrollX_right = minValue( profileSystem->frameBreakdown_scrollX_right, frameBreakdown_scrollBound.right );
				profileSystem->frameBreakdown_scrollX_right = maxValue( profileSystem->frameBreakdown_scrollX_right, profileSystem->frameBreakdown_scrollX_left + 40.0f );
			}
			if( profileSystem->frameBreakdown_scrollActive_bar ) {
				flo32 width = profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left;
				
				flo32 left  = mouse->position.x - width * 0.5f;
				flo32 right = mouse->position.x + width * 0.5f;
				if( left < profileSystem->eventDatabase_posOffset ) {
					left  = profileSystem->eventDatabase_posOffset;
					right = profileSystem->eventDatabase_posOffset + width;
				}
				if( right > frameBreakdown_scrollBound.right ) {
					right = frameBreakdown_scrollBound.right;
					left  = frameBreakdown_scrollBound.right - width;
				}
				
				profileSystem->frameBreakdown_scrollX_left  = left;
				profileSystem->frameBreakdown_scrollX_right = right;
			}
			if( isInBound( mouse->position, eventDatabase_resizeBound ) ) {
				if( wasPressed( mouse, mouseButton_left ) ) {
					profileSystem->eventDatabase_scrollActive = true;
				}
				DEBUG_drawRect( eventDatabase_resizeBound, COLOR_RED );
			}
			if( isInBound( mouse->position, frameBreakdown_leftBound ) ) {
				if( wasPressed( mouse, mouseButton_left ) ) {
					profileSystem->frameBreakdown_scrollActive_left = true;
				}
				DEBUG_drawRect( frameBreakdown_leftBound, COLOR_RED );
			} else if( isInBound( mouse->position, frameBreakdown_rightBound ) ) {
				if( wasPressed( mouse, mouseButton_left ) ) {
					profileSystem->frameBreakdown_scrollActive_right = true;
				}
				DEBUG_drawRect( frameBreakdown_rightBound, COLOR_RED );
			} else if( isInBound( mouse->position, frameBreakdown_scrollBarBound ) ) {
				if( wasPressed( mouse, mouseButton_left ) ) {
					profileSystem->frameBreakdown_scrollActive_bar = true;
				}
				DEBUG_drawRect( frameBreakdown_scrollBarBound, COLOR_RED );
			}
			if( wasReleased( mouse, mouseButton_left ) ) {
				profileSystem->eventDatabase_scrollActive = false;
				profileSystem->frameBreakdown_scrollActive_bar = false;
				profileSystem->frameBreakdown_scrollActive_left = false;
				profileSystem->frameBreakdown_scrollActive_right = false;
			}
			scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_left, frameBreakdown_scrollBound.bottom );
			scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left, getHeight( frameBreakdown_scrollBound ) );
			frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
			frameBreakdown_leftBound  = rectBLD( getBL(  frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
			frameBreakdown_rightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
			DEBUG_drawRect( frameBreakdown_scrollBound, COLOR_GRAY( 0.20f ) );
			DEBUG_drawRect( frameBreakdown_scrollBarBound, COLOR_GRAY( 0.50f ) );
			
			DEBUG_drawRect( eventDatabase_bound, COLOR_GRAY( 0.80f ) );
			
			rect eventDatabase_highlightBound = rectBLD( eventDatabase_basePos + Vec2( eventDatabase_slotWidth * ( flo32 )globalVar_profileSystem.eventDatabase.index, 0.0f ), Vec2( eventDatabase_slotWidth, profileSystem->eventDatabase_height ) );
			DEBUG_drawRect( eventDatabase_highlightBound, COLOR_YELLOW );
			
			int32 collateIndex = globalVar_profileSystem.eventDatabase.index;
			if( wasPressed( mouse, mouseButton_right ) ) {
				globalVar_profileSystem.eventDatabase.selectIndex = -1;
			}
			if( globalVar_profileSystem.eventDatabase.selectIndex != -1 ) {
				collateIndex = globalVar_profileSystem.eventDatabase.selectIndex;
				vec2 collateIndex_position = eventDatabase_basePos + Vec2( eventDatabase_slotWidth * ( flo32 )collateIndex, 0.0f );
				DEBUG_drawRect( rectBLD( collateIndex_position, Vec2( eventDatabase_slotWidth, profileSystem->eventDatabase_height ) ), COLOR_RED );
			}
			if( isInBound( mouse->position, eventDatabase_bound ) ) {
				int32 hoverIndex = ( int32 )( ( mouse->position.x - eventDatabase_bound.left ) / eventDatabase_slotWidth );
				if( wasPressed( mouse, mouseButton_left ) ) {
					globalVar_profileSystem.eventDatabase.selectIndex = hoverIndex;
					collateIndex = hoverIndex;
				}
				if( globalVar_profileSystem.eventDatabase.selectIndex == -1 ) {
					collateIndex = hoverIndex;
					vec2 collateIndex_position = eventDatabase_basePos + Vec2( eventDatabase_slotWidth * ( flo32 )collateIndex, 0.0f );
					DEBUG_drawRect( rectBLD( collateIndex_position, Vec2( eventDatabase_slotWidth, profileSystem->eventDatabase_height ) ), COLOR_RED );
				}
			}
			PROFILE_EVENT_LIST * collateList = globalVar_profileSystem.eventDatabase.eventList + collateIndex;
			PROFILE_EVENT_DATABASE_CLOCKS * collateClocks = globalVar_profileSystem.eventDatabase_clocks + collateIndex;
	
			flo32 msElapsed 	= win32_getMSElapsed( collateClocks->counter_frameStart, collateClocks->counter_frameEnd, perfFrequency );
			uint64 cycElapsed = collateClocks->frameEnd - collateClocks->frameStart;
			
			char time_string[ 128 ] = {};
			sprintf( time_string, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", msElapsed, cycElapsed, cycElapsed / 1000 );
			PROFILE_string( time_string );
		
			collateEvents( collateList, globalVar_profileSystem.nodeList_currentFrame );
			uint64 frameClock_total = collateClocks->frameEnd - collateClocks->frameStart;
			
			for( int32 node_index = 0; node_index < globalVar_profileSystem.nodeList_currentFrame->count; node_index++ ) {
				PROFILE_NODE * node = globalVar_profileSystem.nodeList_currentFrame->node + node_index;
				
				char profile_string[ 512 ] = {};
				sprintf( profile_string, "%-24s: %10I64u  %6I64u  %6d  %6.02f", node->functionName, node->clock_total, node->clock_total / 1000, node->count_total, ( ( flo32 )node->clock_total / ( flo32 )frameClock_total ) * 100.0f );
				PROFILE_string( profile_string );
			}
		
			vec4 colorTable[] = {
				COLOR_RED,
				COLOR_GREEN,
				COLOR_BLUE,
				COLOR_GRAY( 0.50f ),
				COLOR_CYAN,
				COLOR_GRAY( 0.20f ),
				COLOR_MAGENTA,
				COLOR_GRAY( 0.80f ),
				COLOR_YELLOW,
				COLOR_WHITE,
			};
			
			#define CLOCK_BOUND_MAX_STACK_COUNT  16
			uint64 clockBound_stack[ CLOCK_BOUND_MAX_STACK_COUNT ] = {};
			int32 stack_count = 0;
			
			flo32 frameBreakdown_minWidth = 40.0f;
			flo32 frameBreakdown_maxWidth = getWidth( frameBreakdown_scrollBound );
			flo32 frameBreakdown_ratio = ( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left - frameBreakdown_minWidth ) / ( frameBreakdown_maxWidth - frameBreakdown_minWidth );
			// DISPLAY_VALUE( flo32, frameBreakdown_ratio );
			
			vec2	client_dim = getDim( client_bound );
			flo32 client_width = client_dim.x;
			flo32 profile_minWidth = client_dim.x;
			flo32 profile_maxWidth = profile_minWidth * 10.0f;
			flo32 profile_width   = lerp( profile_maxWidth, frameBreakdown_ratio, profile_minWidth );
			
			vec2  profile_basePos = Vec2( 0.0f, eventDatabase_bound.top + 20.0f );
			flo32 row_height = 20.0f;
			
			flo32 valid_width = getWidth( frameBreakdown_scrollBound ) - ( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left );
			if( valid_width > 0.0f ) {
				flo32 offset_ratio = ( profileSystem->frameBreakdown_scrollX_left - frameBreakdown_scrollBound.left ) / valid_width;
				profile_basePos.x -= ( profile_width - client_width ) * offset_ratio;
			}
			uint64 profile_frameClockStart = collateList->frameClockStart;
			uint64 profile_clocksPerFrame = 60000000;
			
			int32 color_select = 0;
			
			PROFILE_EVENT * profile_tooltip = 0;
			
			for( int32 event_index = 0; event_index < collateList->count; event_index++ ) {
				PROFILE_EVENT * event = collateList->event + event_index;
				
				for( int32 iter = stack_count - 1; iter >= 0; iter-- ) {
					if( event->clock_start > clockBound_stack[ iter ] ) {
						stack_count--;
					}
				}
				
				clockBound_stack[ stack_count++ ] = event->clock_end;
				Assert( stack_count < CLOCK_BOUND_MAX_STACK_COUNT );
				
				int32 level = stack_count - 1;
				rect bound = {};
				bound.left  = profile_basePos.x + ( ( flo32 )( event->clock_start - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
				bound.right = profile_basePos.x + ( ( flo32 )( event->clock_end   - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
				bound.top 	 = profile_basePos.y + row_height * ( flo32 )( level + 1 );
				bound.bottom = profile_basePos.y + row_height * ( flo32 )level;
				
				if( !profile_tooltip && isInBound( mouse->position, bound ) ) {
					profile_tooltip = event;
				}
				
				DEBUG_drawRect( bound, colorTable[ color_select ] );
				
				color_select = ( color_select + 1 ) % _arrayCount( colorTable );
			}
			
			if( profile_tooltip ) {
				PROFILE_tooltip( profile_tooltip->functionName, mouse->position );
			}
		
			for( int32 database_index = 0; database_index < PROFILE_EVENT_DATABASE_MAX_COUNT; database_index++ ) {
				PROFILE_EVENT_DATABASE_CLOCKS clock = globalVar_profileSystem.eventDatabase_clocks[ database_index ];
				
				{
					rect bound = {};
					bound.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
					bound.right = bound.left + eventDatabase_slotWidth;
					bound.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.platformStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					bound.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.appStart 		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					DEBUG_drawRect( bound, COLOR_GREEN );
				}
				
				{
					rect bound = {};
					bound.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
					bound.right = bound.left + eventDatabase_slotWidth;
					bound.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.appStart		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					bound.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					DEBUG_drawRect( bound, COLOR_BLUE );
				}
				
				{
					rect bound = {};
					bound.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
					bound.right = bound.left + eventDatabase_slotWidth;
					bound.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					bound.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.frameEnd		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
					DEBUG_drawRect( bound, COLOR_RED );
				}
			}
		}
		globalVar_profileSystem.eventList_currentFrame->count = 0;
		globalVar_profileSystem.nodeList_currentFrame->count = 0;
	}
	
	globalVar_profileSystem.eventDatabase_clocks_currentFrame.platformStart = __rdtsc();
}

#endif // DEBUG_BUILD

LRESULT CALLBACK
win32_WindowProc( HWND window, uint32 message, WPARAM wParam, LPARAM lParam ) {
	LRESULT result = 0;
	switch( message ) {
		case WM_ACTIVATEAPP: {
		} break;

		case WM_CLOSE: {
		} break;

		case WM_DESTROY: {
		} break;
		
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP: {
			Assert( !"ERROR! Keyboard message was passed directly to WindowProc! All keyboard messages should be handled directly by the game!" );
		} break;
		
		default: {
			result = DefWindowProc( window, message, wParam, lParam );
		} break;
	}
	return result;
}

internal void
win32_processPendingMessages( PLATFORM * platform, HWND windowHandle ) {
	// PROFILE_FUNCTION();
	
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	keyboard->count = 0;
	
	MOUSE_STATE * mouse = &platform->mouse;
	MOUSE_endOfFrame( mouse );
	
	MSG message;
	while( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) ) {
		switch( message.message ) {
			case WM_MOUSEWHEEL: {
				int16 wParam_hi = ( ( message.wParam >> 16 ) & 0xFFFF );
				int32 wheelClicks = wParam_hi / 120;
				mouse->wheelClicks = wheelClicks;
			} break;
			
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				platform->mouse.control_isDown = ( message.wParam & MK_CONTROL );
				platform->mouse.shift_isDown = ( message.wParam & MK_SHIFT );
				processButtonEvent( mouse, mouseButton_left, ( message.wParam & MK_LBUTTON ) );
			} break;
			
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				platform->mouse.control_isDown = ( message.wParam & MK_CONTROL );
				platform->mouse.shift_isDown = ( message.wParam & MK_SHIFT );
				processButtonEvent( mouse, mouseButton_middle, ( message.wParam & MK_MBUTTON ) );
			} break;
			
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN: {
				platform->mouse.control_isDown = ( message.wParam & MK_CONTROL );
				platform->mouse.shift_isDown = ( message.wParam & MK_SHIFT );
				processButtonEvent( mouse, mouseButton_right, ( message.wParam & MK_RBUTTON ) );
			} break;
			
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP: {
				boo32 wasDown = ( ( message.lParam & ( 1 << 30 ) ) != 0 );
				boo32 isDown = ( ( message.lParam & ( 1 << 31 ) ) == 0 );
				if( isDown != wasDown ) {
					switch( message.wParam ) {
						case VK_BACK: 		{ addKeyEvent( keyboard, keyCode_backspace, 	isDown ); } break;
						case VK_TAB: 		{ addKeyEvent( keyboard, keyCode_tab, 			isDown ); } break;
						case VK_RETURN:	{ addKeyEvent( keyboard, keyCode_enter, 		isDown ); } break;
						case VK_SHIFT: 	{
							keyboard->shift = isDown;
							addKeyEvent( keyboard, keyCode_shift, isDown );
						} break;
						case VK_CONTROL: 	{
							keyboard->control = isDown;
							addKeyEvent( keyboard, keyCode_control, isDown );
						} break;
						case VK_MENU: 		{ addKeyEvent( keyboard, keyCode_alt, 			isDown ); } break;
						case VK_CAPITAL: 	{ addKeyEvent( keyboard, keyCode_capsLock, 	isDown ); } break;
						case VK_ESCAPE: 	{ addKeyEvent( keyboard, keyCode_escape, 		isDown ); } break;
						case VK_SPACE: 	{ addKeyEvent( keyboard, keyCode_space, 		isDown ); } break;
						case VK_PRIOR: 	{ addKeyEvent( keyboard, keyCode_pageUp, 		isDown ); } break;
						case VK_NEXT: 		{ addKeyEvent( keyboard, keyCode_pageDown, 	isDown ); } break;
						case VK_END: 		{ addKeyEvent( keyboard, keyCode_end, 			isDown ); } break;
						case VK_HOME: 		{ addKeyEvent( keyboard, keyCode_home, 			isDown ); } break;
						case VK_LEFT: 		{ addKeyEvent( keyboard, keyCode_left, 			isDown ); } break;
						case VK_RIGHT: 	{ addKeyEvent( keyboard, keyCode_right, 		isDown ); } break;
						case VK_UP: 		{ addKeyEvent( keyboard, keyCode_up, 			isDown ); } break;
						case VK_DOWN: 		{ addKeyEvent( keyboard, keyCode_down, 			isDown ); } break;
						case VK_INSERT: 	{ addKeyEvent( keyboard, keyCode_insert, 		isDown ); } break;
						case VK_DELETE: 	{ addKeyEvent( keyboard, keyCode_delete, 		isDown ); } break;
						case 0x30: { addKeyEvent( keyboard, keyCode_0, isDown, keyboard->shift ); } break;
						case 0x31: { addKeyEvent( keyboard, keyCode_1, isDown, keyboard->shift ); } break;
						case 0x32: { addKeyEvent( keyboard, keyCode_2, isDown, keyboard->shift ); } break;
						case 0x33: { addKeyEvent( keyboard, keyCode_3, isDown, keyboard->shift ); } break;
						case 0x34: { addKeyEvent( keyboard, keyCode_4, isDown, keyboard->shift ); } break;
						case 0x35: { addKeyEvent( keyboard, keyCode_5, isDown, keyboard->shift ); } break;
						case 0x36: { addKeyEvent( keyboard, keyCode_6, isDown, keyboard->shift ); } break;
						case 0x37: { addKeyEvent( keyboard, keyCode_7, isDown, keyboard->shift ); } break;
						case 0x38: { addKeyEvent( keyboard, keyCode_8, isDown, keyboard->shift ); } break;
						case 0x39: { addKeyEvent( keyboard, keyCode_9, isDown, keyboard->shift ); } break;
						case 0x41: { addKeyEvent( keyboard, keyCode_a, isDown, keyboard->shift ); } break;
						case 0x42: { addKeyEvent( keyboard, keyCode_b, isDown, keyboard->shift ); } break;
						case 0x43: { addKeyEvent( keyboard, keyCode_c, isDown, keyboard->shift ); } break;
						case 0x44: { addKeyEvent( keyboard, keyCode_d, isDown, keyboard->shift ); } break;
						case 0x45: { addKeyEvent( keyboard, keyCode_e, isDown, keyboard->shift ); } break;
						case 0x46: { addKeyEvent( keyboard, keyCode_f, isDown, keyboard->shift ); } break;
						case 0x47: { addKeyEvent( keyboard, keyCode_g, isDown, keyboard->shift ); } break;
						case 0x48: { addKeyEvent( keyboard, keyCode_h, isDown, keyboard->shift ); } break;
						case 0x49: { addKeyEvent( keyboard, keyCode_i, isDown, keyboard->shift ); } break;
						case 0x4A: { addKeyEvent( keyboard, keyCode_j, isDown, keyboard->shift ); } break;
						case 0x4B: { addKeyEvent( keyboard, keyCode_k, isDown, keyboard->shift ); } break;
						case 0x4C: { addKeyEvent( keyboard, keyCode_l, isDown, keyboard->shift ); } break;
						case 0x4D: { addKeyEvent( keyboard, keyCode_m, isDown, keyboard->shift ); } break;
						case 0x4E: { addKeyEvent( keyboard, keyCode_n, isDown, keyboard->shift ); } break;
						case 0x4F: { addKeyEvent( keyboard, keyCode_o, isDown, keyboard->shift ); } break;
						case 0x50: { addKeyEvent( keyboard, keyCode_p, isDown, keyboard->shift ); } break;
						case 0x51: { addKeyEvent( keyboard, keyCode_q, isDown, keyboard->shift ); } break;
						case 0x52: { addKeyEvent( keyboard, keyCode_r, isDown, keyboard->shift ); } break;
						case 0x53: { addKeyEvent( keyboard, keyCode_s, isDown, keyboard->shift ); } break;
						case 0x54: { addKeyEvent( keyboard, keyCode_t, isDown, keyboard->shift ); } break;
						case 0x55: { addKeyEvent( keyboard, keyCode_u, isDown, keyboard->shift ); } break;
						case 0x56: { addKeyEvent( keyboard, keyCode_v, isDown, keyboard->shift ); } break;
						case 0x57: { addKeyEvent( keyboard, keyCode_w, isDown, keyboard->shift ); } break;
						case 0x58: { addKeyEvent( keyboard, keyCode_x, isDown, keyboard->shift ); } break;
						case 0x59: { addKeyEvent( keyboard, keyCode_y, isDown, keyboard->shift ); } break;
						case 0x5A: { addKeyEvent( keyboard, keyCode_z, isDown, keyboard->shift ); } break;
						case VK_OEM_1: { addKeyEvent( keyboard, keyCode_semicolon, isDown, keyboard->shift ); } break;
						case VK_OEM_PLUS: { addKeyEvent( keyboard, keyCode_equal, isDown, keyboard->shift ); } break;
						case VK_OEM_COMMA: { addKeyEvent( keyboard, keyCode_comma, isDown, keyboard->shift ); } break;
						case VK_OEM_MINUS: { addKeyEvent( keyboard, keyCode_dash, isDown, keyboard->shift ); } break;
						case VK_OEM_PERIOD: { addKeyEvent( keyboard, keyCode_period, isDown, keyboard->shift ); } break;
						case VK_OEM_2: { addKeyEvent( keyboard, keyCode_forwardSlash, isDown, keyboard->shift ); } break;
						case VK_OEM_3: { addKeyEvent( keyboard, keyCode_tilde, isDown, keyboard->shift ); } break;
						case VK_OEM_4: { addKeyEvent( keyboard, keyCode_openBracket, isDown, keyboard->shift ); } break;
						case VK_OEM_5: { addKeyEvent( keyboard, keyCode_backslash, isDown, keyboard->shift ); } break;
						case VK_OEM_6: { addKeyEvent( keyboard, keyCode_closeBracket, isDown, keyboard->shift ); } break;
						case VK_OEM_7: { addKeyEvent( keyboard, keyCode_quote, isDown, keyboard->shift ); } break;
						case VK_NUMPAD0: { addKeyEvent( keyboard, keyCode_num0, isDown, keyboard->shift ); } break;
						case VK_NUMPAD1: { addKeyEvent( keyboard, keyCode_num1, isDown, keyboard->shift ); } break;
						case VK_NUMPAD2: { addKeyEvent( keyboard, keyCode_num2, isDown, keyboard->shift ); } break;
						case VK_NUMPAD3: { addKeyEvent( keyboard, keyCode_num3, isDown, keyboard->shift ); } break;
						case VK_NUMPAD4: { addKeyEvent( keyboard, keyCode_num4, isDown, keyboard->shift ); } break;
						case VK_NUMPAD5: { addKeyEvent( keyboard, keyCode_num5, isDown, keyboard->shift ); } break;
						case VK_NUMPAD6: { addKeyEvent( keyboard, keyCode_num6, isDown, keyboard->shift ); } break;
						case VK_NUMPAD7: { addKeyEvent( keyboard, keyCode_num7, isDown, keyboard->shift ); } break;
						case VK_NUMPAD8: { addKeyEvent( keyboard, keyCode_num8, isDown, keyboard->shift ); } break;
						case VK_NUMPAD9: { addKeyEvent( keyboard, keyCode_num9, isDown, keyboard->shift ); } break;
						default: {
							char string[ 128 ] = {};
							sprintf( string, "Key message received, but not processed: %lu %s\n", message.wParam, ( isDown ? "PRESSED" : "RELEASED" ) );
							OutputDebugString( string );
						} break;
					};
				}
			} break;
			
			default: {
				TranslateMessage( &message );
				DispatchMessage( &message );
			} break;
		}
	}
}

internal void
win32_endApplication( void * platform, void * appState ) {
	APP_STATE * myAppState = ( APP_STATE * )appState;
	myAppState->isRunning = false;
}

int32 CALLBACK
WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32 windowShowCode ) {
	srand( ( uint32 )time( 0 ) );
	
	boo32 sleepFreqIsHiRes = ( timeBeginPeriod( 1 ) == TIMERR_NOERROR );
	
	LARGE_INTEGER perfFrequency;
	QueryPerformanceFrequency( &perfFrequency );
	
	WNDCLASS windowClass = {};
	windowClass.lpfnWndProc = win32_WindowProc;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = "WindowClass";
	
	if( RegisterClass( &windowClass ) ) {
		int32 screen_width = GetSystemMetrics( SM_CXSCREEN );
		int32 screen_height = GetSystemMetrics( SM_CYSCREEN );
		
		HWND window = CreateWindowEx( 0, "WindowClass", "Caption", WS_VISIBLE, 0, 0, screen_width, screen_height, 0, 0, instance, 0 ); // WINDOWED
		// HWND window = CreateWindowEx( 0, "WindowClass", "Caption", WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP, 0, 0, screen_width, screen_height, 0, 0, instance, 0 ); // FULLSCREEN
		ShowCursor( FALSE );

		if( window ) {
			RECT client_rect = {};
			GetClientRect( window, &client_rect );
			
			int32 client_width = client_rect.right - client_rect.left;
			int32 client_height = client_rect.bottom - client_rect.top;
			
			vec2 client_dim = Vec2( ( flo32 )client_width, ( flo32 )client_height );
			vec2 client_halfDim = client_dim * 0.5f;
			rect client_bound = Rect( ( flo32 )client_rect.left, ( flo32 )client_rect.top, ( flo32 )client_rect.right, ( flo32 )client_rect.bottom );
			
			MEMORY platformMemory = {};
			platformMemory.size = ( sizeof( APP_STATE ) + sizeof( PLATFORM ) + APP_permMemorySize + APP_tempMemorySize );
			platformMemory.base = VirtualAlloc( 0, platformMemory.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
			
			APP_STATE * appState = _pushType( &platformMemory, APP_STATE );
			appState->isRunning = true;
			
			PLATFORM * win32_platform = _pushType( &platformMemory, PLATFORM );
			win32_platform->permMemory = { _pushSize( &platformMemory, APP_permMemorySize ), APP_permMemorySize };
			win32_platform->tempMemory = { _pushSize( &platformMemory, APP_tempMemorySize ), APP_tempMemorySize };
			win32_platform->writeFile = win32_writeFile;
			win32_platform->readFile  = win32_readFile;
			win32_platform->readAsset = win32_readAsset;
			win32_platform->readFile_chunk = win32_readFile_chunk;
			win32_platform->readAsset_chunk = win32_readFile_chunk;
			win32_platform->endApplication = win32_endApplication;
			win32_platform->targetSec = 1.0f / APP_targetFPS;
			
			RENDER_SYSTEM * renderSystem = &win32_platform->renderSystem;
			ASSET_SYSTEM * assetSystem = &win32_platform->assetSystem;
			renderSystem->assetSystem = assetSystem;
			
			HANDLE assetFile_handle = CreateFile( "assets.pak", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
			if( assetFile_handle != INVALID_HANDLE_VALUE ) {
				ASSET_SYSTEM * assetSystem = &win32_platform->assetSystem;
				
				assetSystem->platform = win32_platform;
				assetSystem->assetFile_handle = ( uint64 )assetFile_handle;
				
				uint32 read_size = _arrayBytes( assetSystem->entry );
				DWORD bytesRead = 0;
				
				ReadFile( assetFile_handle, assetSystem->entry, read_size, &bytesRead, 0 );
				Assert( bytesRead == read_size );
			} else {
				OutputDebugString( "ERROR: Unable to open asset file: assets.pak" );
			}
			
			HDC win32_deviceContext = GetDC( window );
			
			vec2 app_dim = Vec2( ( flo32 )APP_width, ( flo32 )APP_height );
			vec2 app_halfDim = app_dim * 0.5f;
			rect app_bound = rectBLD( Vec2( 0.0f, 0.0f ), app_dim );
			appState->app_dim = app_dim;
			appState->app_halfDim = app_halfDim;
			appState->app_bound = app_bound;
			vec2 camera_dim = app_dim;
			
			PIXELFORMATDESCRIPTOR pixelFormat = {};
			pixelFormat.nSize = sizeof( PIXELFORMATDESCRIPTOR );
			pixelFormat.nVersion = 1;
			pixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pixelFormat.iPixelType = PFD_TYPE_RGBA;
			pixelFormat.cColorBits = 24;
			pixelFormat.cAlphaBits = 8;
			pixelFormat.iLayerType = PFD_MAIN_PLANE;
			
			int32 pixelFormatIndex = ChoosePixelFormat( win32_deviceContext, &pixelFormat );
			DescribePixelFormat( win32_deviceContext, pixelFormatIndex, sizeof( PIXELFORMATDESCRIPTOR ), &pixelFormat );
			SetPixelFormat( win32_deviceContext, pixelFormatIndex, &pixelFormat );
			
			HGLRC win32_renderContext = wglCreateContext( win32_deviceContext );
			boo32 success_setContext = ( boo32 )wglMakeCurrent( win32_deviceContext, win32_renderContext );
			Assert( success_setContext );
			
			GL_win32_getExtensions();
			
			typedef BOOL ( SWAP_INTERVAL )( int32 interval );
			SWAP_INTERVAL * swapInterval = ( SWAP_INTERVAL * )wglGetProcAddress( "wglSwapIntervalEXT" );
			if( swapInterval ) {
				swapInterval( 1 );
			} else {
				OutputDebugString( "VSync not supported.\n" );
			}
				
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			glViewport( 0, 0, client_width, client_height );
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			
			glEnable( GL_SCISSOR_TEST );
			glScissor( 0, 0, client_width, client_height );
		
			glEnable( GL_BLEND );
			glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
			
			int32 mainShader_transform = -1;
			int32 mainShader_modColor  = -1;
			{ // main shader			
				char * shader_vertexCode = "\
				#version 330 core\n\
				uniform mat4 transform;\n\
				layout( location = 0 ) in vec2 inPosition;\n\
				layout( location = 1 ) in vec2 inTexCoord;\n\
				out vec2 textureCoord;\n\
				void main( void ) {\n\
					gl_Position = transform * vec4( inPosition, 0.0f, 1.0f );\n\
					textureCoord = inTexCoord;\n\
				}\n";
					
				char * shader_fragmentCode = "\
				#version 330 core\n\
				uniform sampler2D textureSampler;\n\
				uniform vec4 modColor;\n\
				in vec2 textureCoord;\n\
				out vec4 outColor;\n\
				void main( void ) {\n\
					outColor = modColor * texture( textureSampler, textureCoord.st );\n\
				}\n";
				
				CREATE_SHADER_RESULT shader = createShader( shader_vertexCode, shader_fragmentCode );
				Assert( shader.isValid );
				
				glUseProgram( shader.program );
				mainShader_modColor  = glGetUniformLocation( shader.program, "modColor" );
				mainShader_transform = glGetUniformLocation( shader.program, "transform" );
				
				flo32 client_aspectRatio = client_dim.x / client_dim.y;
				flo32    app_aspectRatio =    app_dim.x /    app_dim.y;
				
				if( app_aspectRatio < client_aspectRatio ) {
					camera_dim.x *= ( client_aspectRatio / app_aspectRatio );
				} else {
					camera_dim.y *= ( app_aspectRatio / client_aspectRatio );
				}
				
				glUniform1i( glGetUniformLocation( shader.program, "textureSampler" ), 0 );
			}
			rect camera_bound     = rectCD( app_halfDim, camera_dim );
			mat4 camera_transform = mat4_orthographic( camera_bound.left, camera_bound.bottom, camera_bound.right, camera_bound.top, -1.0f, 1.0f );
			mat4 screen_transform = mat4_orthographic( client_bound.left, client_bound.bottom, client_bound.right, client_bound.top, -1.0f, 1.0f );
			
			vec4 app_min_v4 = camera_transform * Vec4(               0.0f,                0.0f, 0.0f, 1.0f );
			vec4 app_max_v4 = camera_transform * Vec4( ( flo32 )APP_width, ( flo32 )APP_height, 0.0f, 1.0f );
			vec2 app_min = ( Vec2( app_min_v4.x, app_min_v4.y ) + Vec2( 1.0f, 1.0f ) ) * client_halfDim;
			vec2 app_max = ( Vec2( app_max_v4.x, app_max_v4.y ) + Vec2( 1.0f, 1.0f ) ) * client_halfDim;
			vec2 adj_origin   = -getBL( camera_bound ) / camera_dim * client_dim;
			rect adj_bound    = Rect( app_min.x, app_min.y, app_max.x, app_max.y );
			vec2 adj_dim      = Vec2( getWidth( adj_bound ), getHeight( adj_bound ) );
			
			{ // gen default texture
				uint8 defaultTexture[] = { 80, 80, 80, 255,   255, 0, 255, 255,   255, 0, 255, 255,   80, 80, 80, 255 };
				int32 width = 2;
				int32 height = 2;
				
				uint32 texture_bufferID = 0;
				glGenTextures( 1, &texture_bufferID );
				glBindTexture( GL_TEXTURE_2D, texture_bufferID );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, defaultTexture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				
				assetSystem->defaultTexture = RenderTexture( texture_bufferID, width, height );
			}
			
			{ // gen default font
				genDefaultMonoFont( assetSystem, &win32_platform->tempMemory );
			}
			
#if DEBUG_BUILD
			int32 	 debugSystem_sizeInBytes = DEBUG_SYSTEM__SIZE_IN_BYTES;
			MEMORY	_debugSystem_memory = Memory( VirtualAlloc( 0, debugSystem_sizeInBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ), debugSystem_sizeInBytes );
			MEMORY *  debugSystem_memory = &_debugSystem_memory;
			globalVar_debugSystem.renderSystem 						= _pushType( debugSystem_memory, RENDER_SYSTEM );
			globalVar_debugSystem.advanceHeight 					= assetSystem->defaultMonoFont.font.advanceHeight;
			globalVar_debugSystem.renderSystem->assetSystem		= assetSystem;
			
			int32 	 profileSystem_sizeInBytes = PROFILE_SYSTEM__SIZE_IN_BYTES;
			MEMORY	_profileSystem_memory = Memory( VirtualAlloc( 0, profileSystem_sizeInBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ), profileSystem_sizeInBytes );
			MEMORY *	 profileSystem_memory = &_profileSystem_memory;
			globalVar_profileSystem.advanceHeight 	= assetSystem->defaultMonoFont.font.advanceHeight;
			globalVar_profileSystem.eventDatabase.eventList 		= _pushArray( profileSystem_memory, PROFILE_EVENT_LIST, PROFILE_EVENT_DATABASE_MAX_COUNT );
			globalVar_profileSystem.eventDatabase.maxCount 			= PROFILE_EVENT_DATABASE_MAX_COUNT;
			globalVar_profileSystem.eventDatabase.selectIndex 		= -1;
			globalVar_profileSystem.eventList_currentFrame			= _pushType( profileSystem_memory, PROFILE_EVENT_LIST );
			globalVar_profileSystem.nodeList_currentFrame			= _pushType( profileSystem_memory, PROFILE_NODE_LIST );
			globalVar_profileSystem.eventList_currentFrame->frameClockStart = __rdtsc();
			
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.frameStart 	= __rdtsc();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.platformStart	= __rdtsc();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.appStart 		= __rdtsc();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.renderStart 	= __rdtsc();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.frameEnd 		= __rdtsc();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.counter_frameStart = win32_getPerfCount();
			globalVar_profileSystem.eventDatabase_clocks_currentFrame.counter_frameEnd   = win32_getPerfCount();
			
			globalVar_profileSystem.eventDatabase_clocks = _pushArray( profileSystem_memory, PROFILE_EVENT_DATABASE_CLOCKS, PROFILE_EVENT_DATABASE_MAX_COUNT );
			
			globalVar_profileSystem.eventDatabase_posOffset 				= 4.0f;
			globalVar_profileSystem.eventDatabase_width 						= client_width - 2.0f * globalVar_profileSystem.eventDatabase_posOffset;
			globalVar_profileSystem.eventDatabase_height 					= 300.0f;
			globalVar_profileSystem.eventDatabase_scrollActive 			= false;
			globalVar_profileSystem.frameBreakdown_scrollActive_bar 		= false;
			globalVar_profileSystem.frameBreakdown_scrollActive_left 	= false;
			globalVar_profileSystem.frameBreakdown_scrollActive_right 	= false;
			globalVar_profileSystem.frameBreakdown_scrollX_left  			= globalVar_profileSystem.eventDatabase_posOffset;
			globalVar_profileSystem.frameBreakdown_scrollX_right 			= client_width - globalVar_profileSystem.eventDatabase_posOffset;
#endif
			
			while( appState->isRunning ) {
#if DEBUG_BUILD
				globalVar_profileSystem.eventDatabase_clocks_previousFrame 							= globalVar_profileSystem.eventDatabase_clocks_currentFrame;
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.frameStart 		  	= __rdtsc();
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.counter_frameStart 	= win32_getPerfCount();
				globalVar_profileSystem.eventList_currentFrame->frameClockStart 					= globalVar_profileSystem.eventDatabase_clocks_currentFrame.frameStart;

				globalVar_debugSystem.atPos 	= getTL( client_bound ) + Vec2( 10.0f, -10.0f );
				globalVar_profileSystem.atPos	= getTL( client_bound ) + Vec2( 10.0f, -10.0f );
				
				updateProfile( &globalVar_profileSystem, win32_platform, perfFrequency, globalVar_debugSystem.renderSystem, client_bound );
#endif
				
				win32_processPendingMessages( win32_platform, window );
				
				POINT mouse_position = {};
				GetCursorPos( &mouse_position );
				ScreenToClient( window, &mouse_position );
				win32_platform->mouse.position = Vec2( ( flo32 )mouse_position.x, client_dim.y - 1.0f - ( flo32 )mouse_position.y );
				
				MOUSE_STATE * mouse = &win32_platform->mouse;
#if DEBUG_BUILD
				if( !globalVar_profileSystem.showProfile ) {
#endif // DEBUG_BUILD
					appState->select_position = ( mouse->position - adj_origin ) / client_dim * camera_dim;
					appState->select_wasPressed  = wasPressed(  mouse, mouseButton_left );
					appState->select_wasReleased = wasReleased( mouse, mouseButton_left );
#if DEBUG_BUILD
				}
				
				if( wasPressed( &win32_platform->keyboard, keyCode_escape ) ) {
					appState->isRunning = false;
				}
#endif // DEBUG_BUILD

				
#if DEBUG_BUILD
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.appStart = __rdtsc();
				if( !globalVar_profileSystem.appPaused ) {
					APP_updateAndRender( appState, win32_platform );
				}
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.renderStart = __rdtsc();
#else
				APP_updateAndRender( appState, win32_platform );
#endif
				glUniformMatrix4fv( mainShader_transform, 1, GL_FALSE, camera_transform.elem );
				
				glClear( GL_COLOR_BUFFER_BIT );
				glScissor( ( int32 )adj_bound.left, ( int32 )adj_bound.bottom, ( int32 )adj_dim.x, ( int32 )adj_dim.y );
				
				render( &win32_platform->renderSystem, mainShader_modColor );
				
#if DEBUG_BUILD
				glUniformMatrix4fv( mainShader_transform, 1, GL_FALSE, screen_transform.elem );
				glScissor( 0, 0, client_width, client_height );
				render( globalVar_debugSystem.renderSystem, mainShader_modColor );
				
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.frameEnd 			= __rdtsc();
				globalVar_profileSystem.eventDatabase_clocks_currentFrame.counter_frameEnd = win32_getPerfCount();
				
				if( wasPressed( &win32_platform->keyboard, keyCode_b ) ) {
					InvalidCodePath;
				}
#endif

				
				SwapBuffers( win32_deviceContext );
			}
		}
	}
   
	return 0;
}

#endif	// STD_INCLUDE_FUNC