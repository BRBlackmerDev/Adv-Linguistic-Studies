
#ifdef	STD_INCLUDE_DECL

//----------
// mouse declarations
//----------

enum MOUSE_BUTTON_ID {
	mouseButton_left,
	mouseButton_right,
	mouseButton_middle,
	
	mouseButton_count,
};

struct MOUSE_BUTTON_STATE {
	void * hotID;
	void * activeID;
	void * doubleClickID;
	boo32 isDown;
	int32 halfTransitionCount;
	flo32 timer_doubleClick;
	boo32 selectBound_isActive;
	vec2 selectPos;
};

struct MOUSE_STATE {
	MOUSE_BUTTON_STATE button[ mouseButton_count ];
	vec2 position;
	flo32 targetTime_doubleClick;
	boo32 control_isDown;
	boo32 shift_isDown;
	int32 wheelClicks;
};

//----------
// keyboard declarations
//----------

enum KEY_STATE_ID {
	keyCode_null,
	
	keyCode_backspace,
	keyCode_shift,
	keyCode_control,
	keyCode_alt,
	keyCode_capsLock,
	keyCode_escape,
	keyCode_pageUp,
	keyCode_pageDown,
	keyCode_end,
	keyCode_home,
	keyCode_left,
	keyCode_right,
	keyCode_up,
	keyCode_down,
	keyCode_insert,
	keyCode_delete,
	keyCode_enter,
	keyCode_tab,
	keyCode_space,
	keyCode_0,
	keyCode_1,
	keyCode_2,
	keyCode_3,
	keyCode_4,
	keyCode_5,
	keyCode_6,
	keyCode_7,
	keyCode_8,
	keyCode_9,
	keyCode_a,
	keyCode_b,
	keyCode_c,
	keyCode_d,
	keyCode_e,
	keyCode_f,
	keyCode_g,
	keyCode_h,
	keyCode_i,
	keyCode_j,
	keyCode_k,
	keyCode_l,
	keyCode_m,
	keyCode_n,
	keyCode_o,
	keyCode_p,
	keyCode_q,
	keyCode_r,
	keyCode_s,
	keyCode_t,
	keyCode_u,
	keyCode_v,
	keyCode_w,
	keyCode_x,
	keyCode_y,
	keyCode_z,
	keyCode_semicolon,
	keyCode_equal,
	keyCode_comma,
	keyCode_dash,
	keyCode_period,
	keyCode_forwardSlash,
	keyCode_tilde,
	keyCode_openBracket,
	keyCode_backslash,
	keyCode_closeBracket,
	keyCode_quote,
	keyCode_num0,
	keyCode_num1,
	keyCode_num2,
	keyCode_num3,
	keyCode_num4,
	keyCode_num5,
	keyCode_num6,
	keyCode_num7,
	keyCode_num8,
	keyCode_num9,
	
	keyCode_count,
};

struct KEY_LIST {
	int32 count;
	uint32 ID[4];
};

struct KEY_STATE {
	uint32 ID;
	boo32 down;
	boo32 shift;
};

#define KEYBOARD_STATE_MAX_EVENT_COUNT  16
struct KEYBOARD_STATE {
	KEY_STATE event[ KEYBOARD_STATE_MAX_EVENT_COUNT ];
	int32 count;
	boo32 shift;
	boo32 control;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// mouse functions
//----------

internal void
MOUSE_endOfFrame( MOUSE_STATE * mouse, flo32 timeElapsed = 0.0f ) {
	mouse->wheelClicks = 0;
	for( uint32 buttonIndex = 0; buttonIndex < mouseButton_count; buttonIndex++ ) {
		MOUSE_BUTTON_STATE * button = &mouse->button[ buttonIndex ];
		button->timer_doubleClick += timeElapsed;
		button->hotID = 0;
		button->halfTransitionCount = 0;
		if( !button->isDown ) {
			button->activeID = 0;
		} else if( button->activeID == 0 ) {
			button->activeID = &button->activeID;
		}
	}
}

internal boo32
isDown( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	boo32 result = mouse->button[ buttonID ].isDown;
	return result;
}

internal boo32
wasButtonPressed( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( button->isDown && button->halfTransitionCount >= 1 ) ||
		 ( !button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasPressed( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	boo32 result = wasButtonPressed( button );
	return result;
}

internal boo32
wasReleased( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( !button->isDown && button->halfTransitionCount >= 1 ) ||
		 ( button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasReleased( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	boo32 result = wasReleased( button );
	return result;
}

internal void
processButtonEvent( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, boo32 isDown ) {
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	button->isDown = isDown;
	button->halfTransitionCount++;
	if( wasButtonPressed( button ) ) {
		button->selectPos = mouse->position;
	}
}

internal boo32
processClick( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, void * targetID, boo32 isInBound ) {
	boo32 result = false;
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	if( button->hotID == 0 && isInBound ) {
		button->hotID = targetID;
	}
	if( button->activeID == targetID && !button->isDown && isInBound ) {
		result = true;
	} else if( button->hotID == targetID && button->activeID == 0 && button->isDown ) {
		button->activeID = targetID;
	}
	return result;
}

internal boo32
processDoubleClick( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, void * targetID, boo32 isInBound ) {
	boo32 result = false;
	if( processClick( mouse, buttonID, targetID, isInBound ) ) {
		MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
		if( button->doubleClickID == button->hotID && button->timer_doubleClick <= mouse->targetTime_doubleClick ) {
			result = true;
		} else {
			button->doubleClickID = button->hotID;
		}
		button->timer_doubleClick = 0.0f;
	}
	return result;
}

internal boo32
processButtonDown( MOUSE_BUTTON_STATE * button, void * targetID, boo32 isInBound ) {
	boo32 result = false;
	if( button->hotID == 0 && isInBound ) {
		button->hotID = targetID;
	}
	if( button->hotID == targetID && button->activeID == 0 && button->isDown ) {
		button->activeID = targetID;
		result = true;
	}
	return result;
}

internal boo32
processButtonDown( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, void * targetID, boo32 isInBound ) {
	boo32 result = false;
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	if( button->hotID == 0 && isInBound ) {
		button->hotID = targetID;
	}
	if( button->hotID == targetID && button->activeID == 0 && button->isDown ) {
		button->activeID = targetID;
		result = true;
	}
	return result;
}

internal boo32
processButtonUp( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, void * targetID, boo32 isInBound ) {
	boo32 result = false;
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	if( button->hotID == 0 && isInBound ) {
		button->hotID = targetID;
	}
	if( button->activeID == targetID && !button->isDown && isInBound ) {
		button->activeID = 0;
		result = true;
	}
	return result;
}

internal rect
getSelectBound( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	vec2 mousePos = mouse->position;
	vec2 selectPos = mouse->button[ buttonID ].selectPos;
	rect result = {
		minValue( mousePos.x, selectPos.x ),
		minValue( mousePos.y, selectPos.y ),
		maxValue( mousePos.x, selectPos.x ),
		maxValue( mousePos.y, selectPos.y )
	};
	return result;
}

inline vec2
getSelectVector( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	vec2 result = mouse->position - mouse->button[ buttonID ].selectPos;
	return result;
}

inline flo32
getSelectVectorX( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	flo32 result = mouse->position.x - mouse->button[ buttonID ].selectPos.x;
	return result;
}

inline flo32
getSelectVectorY( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	flo32 result = mouse->position.y - mouse->button[ buttonID ].selectPos.y;
	return result;
}

//----------
// keyboard functions
//----------

internal KEY_STATE
KeyState( uint32 ID, boo32 down, boo32 shift ) {
	KEY_STATE result = { ID, down, shift };
	return result;
}

internal KEY_LIST Keys( uint32 ID_a ) { KEY_LIST result = { 1, ID_a }; return result; }
internal KEY_LIST Keys( uint32 ID_a, uint32 ID_b ) { KEY_LIST result = { 2, ID_a, ID_b }; return result; }
internal KEY_LIST Keys( uint32 ID_a, uint32 ID_b, uint32 ID_c ) { KEY_LIST result = { 3, ID_a, ID_b, ID_c }; return result; }
internal KEY_LIST Keys( uint32 ID_a, uint32 ID_b, uint32 ID_c, uint32 ID_d ) { KEY_LIST result = { 4, ID_a, ID_b, ID_c, ID_d }; return result; }

internal boo32
isChar( uint32 keyID ) {
	boo32 result = ( keyID >= keyCode_enter && keyID <= keyCode_quote );
	return result;
}

internal boo32
wasAnyPressed( KEYBOARD_STATE * keyboard ) {
	boo32 result = false;
	for( int32 index = 0; index < keyboard->count; index++ ) {
		KEY_STATE * key = keyboard->event + index;
		if( key->down ) {
			result = true;
		}
	}
	return result;
}

internal boo32
wasPressed( KEYBOARD_STATE * keyboard, KEY_LIST keys ) {
	boo32 result = false;
	for( int32 index = 0; index < keyboard->count; index++ ) {
		KEY_STATE * key = keyboard->event + index;
		for( int32 keyIndex = 0; keyIndex < keys.count; keyIndex++ ) {
			if( key->ID == keys.ID[ keyIndex ] && key->down ) {
				result = true;
			}
		}
	}
	return result;
}

internal boo32
wasPressed( KEYBOARD_STATE * keyboard, uint32 keyID ) {
	boo32 result = wasPressed( keyboard, Keys( keyID ) );
	return result;
}

internal void
addKeyEvent( KEYBOARD_STATE * keyboard, uint32 keyID, boo32 down, boo32 shift = false ) {
	Assert( keyboard->count < KEYBOARD_STATE_MAX_EVENT_COUNT );
	keyboard->event[ keyboard->count++ ] = KeyState( keyID, down, shift );
}

internal uint8
getCharCode( uint32 keyID, boo32 shift ) {
	uint8 result = 0;
	if( keyID >= keyCode_a && keyID <= keyCode_z ) {
		result = ( uint8 )( keyID - keyCode_a ) + ( shift ? 'A' : 'a' );
	} else {
		switch( keyID ) {
			case keyCode_0: { result = ( shift ? ')' : '0' ); } break;
			case keyCode_1: { result = ( shift ? '!' : '1' ); } break;
			case keyCode_2: { result = ( shift ? '@' : '2' ); } break;
			case keyCode_3: { result = ( shift ? '#' : '3' ); } break;
			case keyCode_4: { result = ( shift ? '$' : '4' ); } break;
			case keyCode_5: { result = ( shift ? '%' : '5' ); } break;
			case keyCode_6: { result = ( shift ? '^' : '6' ); } break;
			case keyCode_7: { result = ( shift ? '&' : '7' ); } break;
			case keyCode_8: { result = ( shift ? '*' : '8' ); } break;
			case keyCode_9: { result = ( shift ? '(' : '9' ); } break;
			case keyCode_enter: { result = '\n'; } break;
			case keyCode_tab: { result = '\t'; } break;
			case keyCode_space: { result = ' '; } break;
			case keyCode_semicolon: { result = ( shift ? ':' : ';' ); } break;
			case keyCode_equal: { result = ( shift ? '+' : '=' ); } break;
			case keyCode_comma: { result = ( shift ? '<' : ',' ); } break;
			case keyCode_dash: { result = ( shift ? '_' : '-' ); } break;
			case keyCode_period: { result = ( shift ? '>' : '.' ); } break;
			case keyCode_forwardSlash: { result = ( shift ? '?' : '/' ); } break;
			case keyCode_tilde: { result = ( shift ? '~' : '`' ); } break;
			case keyCode_openBracket: { result = ( shift ? '{' : '[' ); } break;
			case keyCode_backslash: { result = ( shift ? '|' : '\\' ); } break;
			case keyCode_closeBracket: { result = ( shift ? '}' : ']' ); } break;
			case keyCode_quote: { result = ( shift ? '"' : '\'' ); } break;
		}
	}
	return result;
}

internal void
update( KEYBOARD_STATE * keyboard, KEY_LIST keys, boo32 * state ) {
	for( int32 index = 0; index < keyboard->count; index++ ) {
		KEY_STATE * key = keyboard->event + index;
		for( int32 keyIndex = 0; keyIndex < keys.count; keyIndex++ ) {
			if( key->ID == keys.ID[ keyIndex ] ) {
				*state = key->down;
			}
		}
	}
}

internal void
update( KEYBOARD_STATE * keyboard, uint32 keyID, boo32 * state ) {
	update( keyboard, Keys( keyID ), state );
}

internal void
update( KEYBOARD_STATE * keyboard, uint8 * buffer, int32 * atIndex, int32 maxIndex ) {
	int32 at = *atIndex;
	
	for( int32 index = 0; index < keyboard->count; index++ ) {
		KEY_STATE * key = keyboard->event + index;
		if( key->down ) {
			if( key->ID == keyCode_backspace ) {
				at = _maxValue( at - 1, 0 );
				buffer[ at ] = 0;
			}
			if( at < maxIndex && isChar( key->ID ) ) {
				buffer[ at++ ] = getCharCode( key->ID, key->shift );
			}
		}
	}
	*atIndex = at;
}

#endif	// STD_INCLUDE_FUNC