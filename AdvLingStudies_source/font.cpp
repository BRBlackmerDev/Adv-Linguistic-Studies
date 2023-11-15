
#ifdef	STD_INCLUDE_DECL

#define FONT_START_CHAR			'!'
#define FONT_END_CHAR			'~'
#define FONT_CHAR_COUNT		( '~' - '!' + 1 )

struct FONT_CHAR {
	vec2 dim;
	vec2 offset;
	vec2 texCoord_min;
	vec2 texCoord_max;
	flo32 advanceWidth;
};

struct FONT {
	int32 advanceWidth_space;
	flo32 ascent;
	flo32 descent;
	flo32 lineGap;
	flo32 advanceHeight;
	FONT_CHAR alphabet[ FONT_CHAR_COUNT ];
	int32 kernValue[ FONT_CHAR_COUNT * FONT_CHAR_COUNT ];
};

struct MONOSPACE_FONT_CHAR {
	vec2 dim;
	vec2 offset;
	vec2 texCoord_min;
	vec2 texCoord_max;
};

struct MONOSPACE_FONT {
	flo32 advanceWidth;
	flo32 advanceHeight;
	flo32 ascent;
	flo32 descent;
	flo32 lineGap;
	MONOSPACE_FONT_CHAR alphabet[ FONT_CHAR_COUNT ];
};

enum TEXT_ALIGNMENT {
	ALIGN_DEFAULT,
	
	ALIGN_BOTTOMLEFT,
	ALIGN_BOTTOMCENTER,
	ALIGN_BOTTOMRIGHT,
	
	ALIGN_CENTERLEFT,
	ALIGN_CENTER,
	ALIGN_CENTERRIGHT,
	
	ALIGN_TOPLEFT,
	ALIGN_TOPCENTER,
	ALIGN_TOPRIGHT,
	
	ALIGN_BASELEFT,
	ALIGN_BASECENTER,
	ALIGN_BASERIGHT,
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

// internal flo32
// getWidth( FONT * font, const char * string ) {
	// flo32 result = 0.0f;
	// char * ptr = ( char * )string;
	// while( *ptr ) {
		// int32 charIndex = *ptr - FONT_START_CHAR;
		// if( *ptr != ' ' ) {
			// result += font->alphabet[ charIndex ].advanceWidth;
			// if( *( ptr + 1 ) && ( *( ptr + 1 ) != ' ' ) ) {
				// int32 nextCharIndex = *( ptr + 1 ) - FONT_START_CHAR;
				// int32 kernValue = font->kernValue[ charIndex * FONT_CHAR_COUNT + nextCharIndex ];
				// result += kernValue;
			// }
		// } else {
			// result += font->advanceWidth_space;
		// }
		// ptr++;
	// }
	// return result;
// }

internal vec2
getOffset( TEXT_ALIGNMENT align, flo32 string_width, flo32 font_ascent, flo32 font_descent, flo32 scale = 1.0f ) {
	vec2 result = {};
	
	flo32 width     = string_width;
	flo32 halfWidth = width * 0.5f;
	
	flo32 ascent  = font_ascent;
	flo32 descent = font_descent;
	
	flo32 height     = ascent + descent;
	flo32 halfHeight = height * 0.5f;
	
	switch( align ) {
		case ALIGN_BOTTOMLEFT:   { result = Vec2(       0.0f, descent ); } break;
		case ALIGN_BOTTOMCENTER: { result = Vec2( -halfWidth, descent ); } break;
		case ALIGN_BOTTOMRIGHT:  { result = Vec2(     -width, descent ); } break;
		
		case ALIGN_CENTERLEFT:   { result = Vec2(       0.0f, -halfHeight + descent ); } break;
		case ALIGN_CENTER: 	    { result = Vec2( -halfWidth, -halfHeight + descent ); } break;
		case ALIGN_CENTERRIGHT:  { result = Vec2(     -width, -halfHeight + descent ); } break;
		
		case ALIGN_TOPLEFT:      { result = Vec2(       0.0f, -ascent ); } break;
		case ALIGN_TOPCENTER:    {	result = Vec2( -halfWidth, -ascent ); } break;
		case ALIGN_TOPRIGHT:     { result = Vec2(     -width, -ascent ); } break;
		
		case ALIGN_DEFAULT:
		case ALIGN_BASELEFT:		 { /* no change */ } break;
		case ALIGN_BASECENTER:	 { result = Vec2( -halfWidth, 0.0f ); } break;
		case ALIGN_BASERIGHT:	 { result = Vec2(     -width, 0.0f ); } break;
		
		default: { InvalidCodePath; } break;
	};
	
	result *= scale;
	return result;
}

#endif	// STD_INCLUDE_FUNC