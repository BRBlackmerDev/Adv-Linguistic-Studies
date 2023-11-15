
internal void
render( RENDER_SYSTEM * renderSystem, int32 shader_uniform_modColor ) {
	uint32 vertex_bufferID = 0;
	glGenBuffers( 1, &vertex_bufferID );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_bufferID );

	uint32 stride = sizeof( VERTEX_TEXMESH_2D );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, stride, ( GLvoid * )( 0 ) );
	glEnableVertexAttribArray( 0 );
	
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, ( GLvoid * )( sizeof( flo32 ) * 2 ) );
	glEnableVertexAttribArray( 1 );
	
	glBufferData( GL_ARRAY_BUFFER, sizeof( VERTEX_TEXMESH_2D ) * renderSystem->vertex_count, renderSystem->vertex, GL_STATIC_DRAW );
	
	for( int32 object_index = 0; object_index < renderSystem->object_count; object_index++ ) {
		RENDER_OBJECT object = renderSystem->object[ object_index ];
		
		GLuint drawType = 0;
		switch( object.drawType ) {
			case meshDrawType_triangles:   { drawType = GL_TRIANGLES;    } break;
			case meshDrawType_triangleFan: { drawType = GL_TRIANGLE_FAN; } break;
			case meshDrawType_lineLoop:    { drawType = GL_LINE_LOOP;    } break;
			default: { InvalidCodePath; } break;
		}
		
		glBindTexture( GL_TEXTURE_2D, object.texture_bufferID );
		glUniform4fv( shader_uniform_modColor, 1, object.color.elem );
		glDrawArrays( drawType, object.vertex_index, object.vertex_count );
	}
	
	glDeleteBuffers( 1, &vertex_bufferID );
	
	renderSystem->object_count = 0;
	renderSystem->vertex_count = 0;
}

internal void
loadDictionary( APP_STATE * appState, PLATFORM * platform ) {
	FILE_DATA dictionary_file = platform->readAsset( platform, &platform->permMemory, "dictionary.dat" );
	if( dictionary_file.contents ) {
		int32 dictionary_count = 0;
		char * ptr = ( char * )dictionary_file.contents;
		while( *ptr ) {
			appState->dictionary_word[ dictionary_count ] = ptr;   while( *ptr++ );
			appState->dictionary_def[  dictionary_count ] = ptr;   while( *ptr++ );
			dictionary_count++;
		}
		appState->dictionary_count = dictionary_count;
	}
}

internal boo32
autoGuessIsValid( SAVE_STATE * saveState ) {
	char * guess = saveState->autoGuess;
	boo32 result = ( ( guess[ 0 ] >= 'A' ) && ( guess[ 0 ] <= 'Z' ) ) &&
	               ( ( guess[ 1 ] >= 'A' ) && ( guess[ 1 ] <= 'Z' ) ) &&
						( ( guess[ 2 ] >= 'A' ) && ( guess[ 2 ] <= 'Z' ) ) && 
						( ( guess[ 3 ] >= 'A' ) && ( guess[ 3 ] <= 'Z' ) ) && 
						( ( guess[ 4 ] >= 'A' ) && ( guess[ 4 ] <= 'Z' ) );
	return result;
}

internal flo32
word_getLength( char * str, int32 str_length, flo32 letter_width, flo32 special_width, flo32 gap_width ) {
	flo32 result = 0.0f;
	for( int32 iter = 0; iter < str_length; iter++ ) {
		char word_char = str[ iter ];
		if( ( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) || ( word_char == ' ' ) ) {
			result += (  letter_width + gap_width );
		} else {
			result += ( special_width + gap_width );
		}
	}
	result -= gap_width;
	return result;
}

internal void
selectWord( APP_STATE * appState ) {
	SAVE_STATE * saveState = &appState->saveState;
	
	int32 select = rand() % saveState->newWord_count;
	appState->select_index = saveState->newWord_index[ select ];
	saveState->newWord_index[ select ] = saveState->newWord_index[ saveState->newWord_count - 1 ];
	saveState->newWord_index[ saveState->newWord_count - 1 ] = appState->select_index;
	saveState->newWord_count--;
	if( saveState->newWord_count == 0 ) {
		saveState->newWord_count = appState->dictionary_count;
	}
	
	// appState->select_index = 127;
	char * word = appState->dictionary_word[ appState->select_index ];
	strcpy( appState->word, word );
	strcpy( appState->word_check, word );
	appState->word_length = ( int32 )strlen( word );
	
	for( int32 iter = 0; iter < appState->word_length; iter++ ) {
		char word_char = appState->word[ iter ];
		appState->word_isGuessed[ iter ] = ( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) ? false : true;
	}
	
	for( int32 iter = 0; iter < 26; iter++ ) {
		appState->letter_isGuessed[ iter ] = false;
	}
	
	rect guess_bound = appState->guess_bound;
	boo32 shouldBreakWord = ( appState->word_length > 11 );
	
	int32 special_count = 0;
	boo32 canBreakWord = false;
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		char word_char = appState->word[ word_index ];
		if( ( word_char == ' ' ) || ( word_char == '-' ) ) {
			canBreakWord = true;
		}
		
		if( ( word_char == '\'' ) || ( word_char == '.' ) || ( word_char == '-' ) ) {
			special_count++;
		}
	}
	int32 letter_count = appState->word_length - special_count;
	int32    gap_count = appState->word_length - 1;
	
	flo32  letter_maxWidth = getHeight( guess_bound ) * 0.4f;
	flo32 special_maxWidth = letter_maxWidth * 0.5f;
	flo32     gap_maxWidth = 2.0f;
	
	flo32 line_charX = appState->line_charX;
	flo32 line_charY = appState->line_charY;
	
	if( shouldBreakWord && canBreakWord ) {
		letter_maxWidth  *= 0.80f;
		special_maxWidth *= 0.80f;
		gap_maxWidth     *= 0.80f;
		
		int32 breakIndex = -1;
		int32 breakDist  = 100;
		int32 midIndex   = ( ( appState->word_length % 2 ) == 0 ) ? ( appState->word_length / 2 + 1 ) : ( appState->word_length / 2 );
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			char word_char = appState->word[ word_index ];
			if( ( word_char == ' ' ) || ( word_char == '-' ) ) {
				int32 dist = absValue( word_index - midIndex );
				if( dist <= breakDist ) {
					breakIndex = word_index;
					breakDist  = dist;
				}
			}
		}
		Assert( breakIndex != -1 );
		
		boo32 breakIsSpace = ( appState->word[ breakIndex ] == ' ' );
		
		int32 start_indexA = 0;
		int32   end_indexA = ( breakIsSpace ) ? ( breakIndex ) : ( breakIndex + 1 );
		
		int32 start_indexB = breakIndex + 1;
		int32   end_indexB = appState->word_length;
		
		flo32 lengthA = word_getLength( appState->word + start_indexA, end_indexA - start_indexA, letter_maxWidth, special_maxWidth, gap_maxWidth );
		flo32 lengthB = word_getLength( appState->word + start_indexB, end_indexB - start_indexB, letter_maxWidth, special_maxWidth, gap_maxWidth );
		Assert( lengthA < getWidth( guess_bound ) );
		Assert( lengthB < getWidth( guess_bound ) );
		
		flo32 line_width  = letter_maxWidth - 3.0f;
		flo32 line_height = line_width / line_charX * line_charY;
		
		appState->line_offset = Vec2( 1.5f, -0.75f );
		appState->line_dim    = Vec2( line_width, line_height );
		
#if WINDOWS_BUILD
		vec2 atPosA = getTC( guess_bound ) + Vec2( -lengthA * 0.5f, -( letter_maxWidth + getHeight( guess_bound ) * 0.25f ) );
		vec2 atPosB = getTC( guess_bound ) + Vec2( -lengthB * 0.5f, -( letter_maxWidth + getHeight( guess_bound ) * 0.75f ) );
#endif // WINDOWS_BUILD
		
#if ANDROID_BUILD
		vec2 atPosA = getTC( guess_bound ) + Vec2( -lengthA * 0.5f, -( letter_maxWidth - 8.0f ) );
		vec2 atPosB = getTC( guess_bound ) + Vec2( -lengthB * 0.5f, -( letter_maxWidth * 2.0f + 76.0f ) );
#endif // ANDROID_BUILD
		
		char * strA = appState->word + start_indexA;
		char * strB = appState->word + start_indexB;
		int32  str_lengthA = end_indexA - start_indexA;
		int32  str_lengthB = end_indexB - start_indexB;
		
		int32 letter_index = 0;
		for( int32 iter = 0; iter < str_lengthA; iter++, letter_index++ ) {
			char word_char = strA[ iter ];
			if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
				appState->word_bound[ letter_index ] = rectBLD( atPosA, Vec2( letter_maxWidth, letter_maxWidth ) );
				atPosA.x += (  letter_maxWidth + gap_maxWidth );
			} else if( word_char == ' ' ) {
				atPosA.x += (  letter_maxWidth + gap_maxWidth );
			} else {
				vec2 offset = Vec2( 0.0f, 0.0f );
				if( word_char == '-'  ) { offset.y = letter_maxWidth * 0.25f; }
				if( word_char == '\'' ) { offset.y = letter_maxWidth * 0.50f; }
				appState->word_bound[ letter_index ] = rectBLD( atPosA + offset, Vec2( special_maxWidth, special_maxWidth ) );
				atPosA.x += ( special_maxWidth + gap_maxWidth );
			}
		}
		if( breakIsSpace ) { letter_index++; }
		for( int32 iter = 0; iter < str_lengthB; iter++, letter_index++ ) {
			char word_char = strB[ iter ];
			if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
				appState->word_bound[ letter_index ] = rectBLD( atPosB, Vec2( letter_maxWidth, letter_maxWidth ) );
				atPosB.x += (  letter_maxWidth + gap_maxWidth );
			} else if( word_char == ' ' ) {
				atPosB.x += (  letter_maxWidth + gap_maxWidth );
			} else {
				vec2 offset = Vec2( 0.0f, 0.0f );
				if( word_char == '-'  ) { offset.y = letter_maxWidth * 0.25f; }
				if( word_char == '\'' ) { offset.y = letter_maxWidth * 0.50f; }
				appState->word_bound[ letter_index ] = rectBLD( atPosB + offset, Vec2( special_maxWidth, special_maxWidth ) );
				atPosB.x += ( special_maxWidth + gap_maxWidth );
			}
		}
	} else {
		flo32 length = ( flo32 )letter_count * letter_maxWidth + ( flo32 )special_count * special_maxWidth + ( flo32 )gap_count * gap_maxWidth;
		flo32 scale  = minValue( getWidth( guess_bound ) / length, 1.2f );
		
		flo32 letter_width  = letter_maxWidth  * scale;
		flo32 special_width = special_maxWidth * scale;
		flo32 gap_width     = gap_maxWidth     * scale;
		
		vec2  letter_dim = Vec2( letter_width, letter_width );
		
		flo32 line_width  = letter_width - ( 4.0f * scale );
		flo32 line_height = line_width / line_charX * line_charY;
		
		appState->line_offset = Vec2( 2.0f, -1.0f ) * scale;
		appState->line_dim    = Vec2( line_width, line_height );
		
		vec2 word_dim   = Vec2( length * scale, letter_width );
		rect word_bound = rectCD( getCenter( guess_bound ), word_dim );
		
		vec2 atPos = getBL( word_bound );
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			char word_char = appState->word[ word_index ];
			if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
				appState->word_bound[ word_index ] = rectBLD( atPos, letter_dim );
				atPos.x += ( letter_dim.x + gap_width );
			} else if( word_char == ' ' ) {
				atPos.x += ( letter_dim.x + gap_width );
			} else {
				vec2 offset = Vec2( 0.0f, 0.0f );
				if( word_char == '-'  ) { offset.y = letter_dim.x * 0.25f; }
				if( word_char == '\'' ) { offset.y = letter_dim.x * 0.50f; }
				appState->word_bound[ word_index ] = rectBLD( atPos + offset, Vec2( special_width, special_width ) );
				atPos.x += ( special_width + gap_width );
			}
		}
	}
}

internal void
guessChar( APP_STATE * appState, char guess_char ) {
	int32 index = guess_char - 'A';
	if( !appState->letter_isGuessed[ index ] ) {
		appState->letter_isGuessed[ index ] = true;
		
		boo32 correctGuess = false;
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			if( appState->word[ word_index ] == guess_char ) {
				appState->word_isGuessed[ word_index ] = true;
				correctGuess = true;
			}
		}
		if( !correctGuess ) {
			appState->incorrectGuess_count++;
		}
	}
}

internal void
autoGuess( APP_STATE * appState ) {
	SAVE_STATE * saveState = &appState->saveState;
	
	for( int32 autoGuess_index = 0; autoGuess_index < 5; autoGuess_index++ ) {
		char autoGuess_char = saveState->autoGuess[ autoGuess_index ];
		
		int32 index = autoGuess_char - 'A';
		if( !appState->letter_isGuessed[ index ] ) {
			appState->letter_isGuessed[ index ] = true;
			
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				if( appState->word[ word_index ] == autoGuess_char ) {
					appState->word_isGuessed[ word_index ] = true;
				}
			}
		}
	}
}

internal void
scrambleWord( APP_STATE * appState ) {
	int32 buffer_count = 0;
	char buffer[ 128 ] = {};
	
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		if( !appState->word_isGuessed[ word_index ] ) {
			buffer[ buffer_count++ ] = appState->word_check[ word_index ];
		}
	}
	
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		if( !appState->word_isGuessed[ word_index ] ) {
			int32 select = rand() % buffer_count;
			
			char char_select = buffer[ select ];
			buffer[ select ] = buffer[ --buffer_count ];
			appState->word[ word_index ] = char_select;
		}
	}
	Assert( buffer_count == 0 );
}

internal void
newRound( APP_STATE * appState ) {
	appState->incorrectGuess_count = 0;
	appState->wonRound      = false;
	appState->move_srcIndex = -1;
	appState->match_count   = 0;
	
	selectWord( appState );
	if( appState->appMode.mode == appMode_guessALetter  ) {
		autoGuess( appState );
	}
	if( appState->appMode.mode == appMode_wordScramble ) {
		scrambleWord( appState );
	}
}

internal void
initDefinition( APP_STATE * appState ) {
	int32 offsetB = 0;
	int32 offsetA = ( rand() % ( appState->dictionary_count - 1 ) ) + 1;
	do {  offsetB = ( rand() % ( appState->dictionary_count - 1 ) ) + 1; } while( offsetB == offsetA );
	
	int32 def_indexA = ( appState->select_index + offsetA ) % appState->dictionary_count;
	int32 def_indexB = ( appState->select_index + offsetB ) % appState->dictionary_count;
	
	int32 select = rand() % 3;
	appState->def_guess[ select ] = appState->select_index;
	appState->def_guess[ ( select + 1 ) % 3 ] = def_indexA;
	appState->def_guess[ ( select + 2 ) % 3 ] = def_indexB;
	
	appState->def_select = -1;
}

internal void
checkScramble( APP_STATE * appState ) {
	boo32 correctGuess = true;
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		if( appState->word[ word_index ] == appState->word_check[ word_index ] ) {
			appState->word_isGuessed[ word_index ] = true;
		} else {
			correctGuess = false;
		}
	}
	
	if( !correctGuess ) {
		appState->incorrectGuess_count++;
	}
}

internal void
checkForMatches( APP_STATE * appState ) {
	int32 subString_index[ 64 ] = {};
	int32 subString_count[ 64 ] = {};
	int32 match_count = 0;
	
	int32 minMatchLength = 2;
	
	for( int32 word_index = 0; word_index < ( appState->word_length - ( minMatchLength - 1 ) ); word_index++ ) {
		char word_char = appState->word[ word_index ];
		if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
			for( int32 check_index = 0; check_index < ( appState->word_length - ( minMatchLength - 1 ) ); check_index++ ) {
				char check_char = appState->word_check[ check_index ];
				if( word_char == check_char ) {
					boo32 validMatch  = true;
					
					int32 valid_index = check_index;
					int32 valid_count = 0;
					
					int32 indexA =  word_index;
					int32 indexB = check_index;
					
					while( validMatch ) {
						if( ( indexA < appState->word_length ) && ( indexB < appState->word_length ) ) {
							char charA = appState->word[       indexA ];
							char charB = appState->word_check[ indexB ];
							if( ( charA >= 'A' ) && ( charA <= 'Z' ) && ( charA == charB ) ) {
								valid_count++;
								indexA++;
								indexB++;
							} else {
								validMatch = false;
							}
						} else {
							validMatch = false;
						}
					}
					Assert( valid_count > 0 );
					
					if( valid_count >= minMatchLength ) {
						boo32 hasBeenFound = true;
						for( int32 found_index = valid_index; found_index < valid_index + valid_count; found_index++ ) {
							if( !appState->word_isGuessed[ found_index ] ) {
								hasBeenFound = false;
							}
						}
						if( !hasBeenFound ) {
							boo32 isNewMatch = true;
							
							int32 min_index = valid_index;
							int32 max_index = valid_index + valid_count - 1;
							for( int32 match_index = 0; match_index < match_count; match_index++ ) {
								int32 min_check = subString_index[ match_index ];
								int32 max_check = subString_count[ match_index ] + min_check - 1;
								
								if( ( min_index >= min_check ) && ( max_index <= max_check ) ) {
									isNewMatch = false;
								}
							}
							
							if( isNewMatch ) {
								subString_index[ match_count ] = valid_index;
								subString_count[ match_count ] = valid_count;
								match_count++;
								Assert( match_count < 16 );
							}
						}
					}
				}
			}
		}
	}
	
	boo32 removeMatch[ 64 ] = {};
	for( int32 indexA = 0; indexA < match_count; indexA++ ) {
		int32 minA = subString_index[ indexA ];
		int32 maxA = subString_count[ indexA ] + minA - 1;
		for( int32 indexB = 0; indexB < match_count; indexB++ ) {
			if( indexA != indexB ) {				
				int32 minB = subString_index[ indexB ];
				int32 maxB = subString_count[ indexB ] + minB - 1;
				
				if( ( minA >= minB ) && ( maxA <= maxB ) ) {
					removeMatch[ indexA ] = true;
				}
			}
		}
	}
	
	int32 newMatchCount = 0;
	for( int32 iter = 0; iter < match_count; iter++ ) {
		if( !removeMatch[ iter ] ) {
			appState->subString_index[ newMatchCount ] = subString_index[ iter ];
			appState->subString_count[ newMatchCount ] = subString_count[ iter ];
			newMatchCount++;
		}
	}
	appState->match_count = newMatchCount;
}

internal void
saveGame( PLATFORM * platform, SAVE_STATE * saveState ) {
	platform->writeFile( platform->saveDir, "save.dat", saveState, sizeof( SAVE_STATE ) );
}

internal boo32
loadGame( PLATFORM * platform, SAVE_STATE * saveState ) {
	boo32 result = false;
	
	MEMORY load_memory  = Memory( saveState, sizeof( SAVE_STATE ) );
	FILE_DATA load_file = platform->readFile( &load_memory, platform->saveDir, "save.dat" );
	if( load_file.contents ) {
		result = true;
	}
	return result;
}

internal void
drawLetterBoard( RENDER_SYSTEM * renderSystem, APP_STATE * appState ) {
	RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_pen );
	FONT        * font       = &renderFont->font;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_BLACK );
	
	for( uint8 iter = 0; iter < 26; iter++ ) {
		rect letter_bound = appState->letter_bound[ iter ];
		
		FONT_CHAR * font_char = font->alphabet + ( iter + 'A' - FONT_START_CHAR );
		rect bound = rectCD( getCenter( letter_bound ), font_char->dim * Vec2( 1.0f, 1.06f ) );
		
		addQuad( renderSystem, bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
	}
	endTask( renderSystem );
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, Vec4( 0.90f, 0.0f, 0.0f, 1.0f ) );
	
	FONT_CHAR * x_char = font->alphabet + ( 'X' - FONT_START_CHAR );
	vec2 x_dim = x_char->dim * 1.4f;
	
	for( int32 iter = 0; iter < 26; iter++ ) {
		if( appState->letter_isGuessed[ iter ] ) {
			rect letter_bound = appState->letter_bound[ iter ];
			rect bound        = rectCD( getCenter( letter_bound ), x_dim );
	
			addQuad( renderSystem, bound, rectMM( x_char->texCoord_min, x_char->texCoord_max ) );
		}
	}
	endTask( renderSystem );
}

internal void
drawWord_guessALetter( RENDER_SYSTEM * renderSystem, APP_STATE * appState ) {
	RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_chalk );
	FONT        * font       = &renderFont->font;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
	
	FONT_CHAR * line_char = font->alphabet + ( '_' - FONT_START_CHAR );
	
	vec2 line_offset = appState->line_offset;
	vec2 line_dim    = appState->line_dim;
	
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		rect word_bound = appState->word_bound[ word_index ];
		char word_char  = appState->word[ word_index ];
		if( ( appState->word_isGuessed[ word_index ] ) && ( word_char != ' ' ) ) {
			FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
			if( word_char == '\'' ) {
				word_bound = rectCD( getCenter( word_bound ), font_char->dim * 2.0f );
			}
			if( word_char == '.' ) {
				word_bound = rectBLD( getBL( word_bound ), font_char->dim * 1.5f );
			}
			if( word_char == '-' ) {
				word_bound = rectCD( getCenter( word_bound ), font_char->dim * ( getWidth( word_bound ) / font_char->dim.x ) );
			}
			
			addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
		}
		
		if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
			rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
			addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
		}
	}
	endTask( renderSystem );
	
	if( ( appState->incorrectGuess_count == 5 ) && ( appState->anim_timer > 0.0f ) ) {
		flo32 alpha = clamp01( ( appState->anim_timer - 0.4f ) / 1.8f );
		beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, Vec4( alpha, alpha, alpha, alpha ) );
	
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			rect word_bound = appState->word_bound[ word_index ];
			char word_char  = appState->word[ word_index ];
			if( !appState->word_isGuessed[ word_index ] ) {
				FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
				addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
			}
		}
		endTask( renderSystem );
	}
}

internal void
drawWord_wordScramble( RENDER_SYSTEM * renderSystem, APP_STATE * appState, int32 hover_index ) {
	RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_chalk );
	FONT        * font       = &renderFont->font;
	
	FONT_CHAR * line_char = font->alphabet + ( '_' - FONT_START_CHAR );
	
	vec2 line_offset = appState->line_offset;
	vec2 line_dim    = appState->line_dim;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		char word_char = appState->word[ word_index ];
		if( ( word_char == '\'' ) || ( word_char == '.' ) || ( word_char == '-' ) ) {
			rect word_bound = appState->word_bound[ word_index ];
			
			FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
			if( word_char == '\'' ) {
				word_bound = rectCD( getCenter( word_bound ), font_char->dim * 2.0f );
			}
			if( word_char == '.' ) {
				word_bound = rectBLD( getBL( word_bound ), font_char->dim * 1.5f );
			}
			if( word_char == '-' ) {
				word_bound = rectCD( getCenter( word_bound ), font_char->dim * ( getWidth( word_bound ) / font_char->dim.x ) );
			}
			
			addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
		}
	}
	endTask( renderSystem );
	
	if( appState->incorrectGuess_count == 3 ) {
		if( appState->anim_timer < 0.6f ) {
#if WINDOWS_BUILD
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
					FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
					addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
					if( appState->word_isGuessed[ word_index ] ) {
						rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
						addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
					}
				}			
			}
			endTask( renderSystem );
#endif // WINDOWS_BUILD

#if ANDROID_BUILD
			flo32 t = appState->anim_timer / 0.6f;
			flo32 alphaA = lerp( 0.7f, t, 1.0f );
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, Vec4( alphaA, alphaA, alphaA, alphaA ) );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				if( ( appState->word_isGuessed[ word_index ] ) && ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
					FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
					addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
					
					rect line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
					addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
				}
			}
			endTask( renderSystem );
			
			flo32 alphaB = lerp( 1.0f, t, 0.0f );
			vec4  tColor = Vec4( 0.8f, 0.8f, 0.5f, 1.0f ) * alphaB;
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				if( !appState->word_isGuessed[ word_index ] ) {
					rect   word_bound = appState->word_bound[ word_index ];
					rect select_bound = Rect( word_bound.left, word_bound.bottom - 56.0f, word_bound.right, word_bound.bottom - 8.0f );
					drawRect( renderSystem, assetID_texture_moveBlock, select_bound, tColor );
				}
			}
			
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				if( !appState->word_isGuessed[ word_index ] ) {
					FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
					addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
				}
			}
			endTask( renderSystem );
#endif // ANDROID_BUILD
		} else if( appState->anim_timer < 2.6f ) {
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				if( ( appState->word_isGuessed[ word_index ] ) && ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
					FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
					addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
					
					rect line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
					addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
				}
			}
			endTask( renderSystem );
			
			flo32 alpha = clamp01( ( appState->anim_timer - 0.6f ) / 1.4f );
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, Vec4( alpha, alpha, alpha, alpha ) );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				if( !appState->word_isGuessed[ word_index ] ) {
					rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
					addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
				}
			}
			endTask( renderSystem );
			
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
			flo32 animChar_width = getWidth( appState->word_bound[ 0 ] );
			vec2  animChar_dim   = Vec2( animChar_width, animChar_width );
			for( int32 animChar_index = 0; animChar_index < appState->animChar_count; animChar_index++ ) {
				ANIM_CHAR animChar = appState->animChar[ animChar_index ];
				FONT_CHAR * font_char = font->alphabet + ( animChar.value - FONT_START_CHAR );
				
				rect animChar_bound = rectBLD( animChar.position + animChar.moveVector * alpha, animChar_dim );
				addQuad( renderSystem, animChar_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
			}
			endTask( renderSystem );
		} else {
			beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word_check[ word_index ];
				if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
					FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
					addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
					
					rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
					addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
				}
			}
			endTask( renderSystem );
		}
	} else {
#if WINDOWS_BUILD
		if( hover_index > -1 ) {
#endif // WINDOWS_BUILD
#if ANDROID_BUILD
		if( ( appState->move_srcIndex > -1 ) && ( hover_index > -1 ) ) {
#endif // ANDROID_BUILD
			char hover_char = appState->word[ hover_index ];
			if( ( hover_char >= 'A' ) && ( hover_char <= 'Z' ) ) {
				rect bound = appState->word_bound[ hover_index ];
				
				beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_highlight ).bufferID, Vec4( 0.30f, 0.30f, 0.30f, 0.30f ) );
				addQuad( renderSystem, addRadius( bound, getWidth( bound ) * 0.1f ), Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
				endTask( renderSystem );			
			}
		}
		
#if WINDOWS_BUILD
		beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			rect word_bound = appState->word_bound[ word_index ];
			char word_char  = appState->word[ word_index ];
			if( ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
				FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
				addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
				if( appState->word_isGuessed[ word_index ] ) {
					rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
					addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
				}
			}			
		}
		endTask( renderSystem );
#endif // WINDOWS_BUILD
		
#if ANDROID_BUILD
		vec4 color = ( appState->anim_timer == 0.0f ) ? Vec4( 0.7f, 0.7f, 0.7f, 0.7f ) : COLOR_WHITE;
		beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, color );
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			char word_char = appState->word[ word_index ];
			if( ( appState->word_isGuessed[ word_index ] ) && ( word_char >= 'A' ) && ( word_char <= 'Z' ) ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				
				FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
				addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
				
				rect  line_bound = rectTLD( getBL( word_bound ) + line_offset, line_dim );
				addQuad( renderSystem, line_bound, rectMM( line_char->texCoord_min, line_char->texCoord_max ) );
			}
		}
		endTask( renderSystem );
		
		beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_WHITE );
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			if( !appState->word_isGuessed[ word_index ] ) {
				rect word_bound = appState->word_bound[ word_index ];
				char word_char  = appState->word[ word_index ];
				
				FONT_CHAR * font_char = font->alphabet + ( word_char - FONT_START_CHAR );
				addQuad( renderSystem, word_bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
			}
		}
		endTask( renderSystem );
		
		for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
			if( !appState->word_isGuessed[ word_index ] ) {
				rect   word_bound = appState->word_bound[ word_index ];
				rect select_bound = Rect( word_bound.left, word_bound.bottom - 56.0f, word_bound.right, word_bound.bottom - 8.0f );
				drawRect( renderSystem, assetID_texture_moveBlock, select_bound, Vec4( 0.8f, 0.8f, 0.5f, 1.0f ) );
			}
		}
#endif // ANDROID_BUILD
	}
}

internal void
drawTeacher( RENDER_SYSTEM * renderSystem, rect bound, boo32 invertX ) {
	beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_teacher ).bufferID, COLOR_WHITE );
	addQuad( renderSystem, bound, ( invertX ? Rect( 1.0f, 0.0f, 0.0f, 1.0f ) : Rect( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
	endTask( renderSystem );
}

internal void
drawBubbleTail( RENDER_SYSTEM * renderSystem, rect bound, boo32 invertX ) {
	beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_bubbleTailH ).bufferID, COLOR_WHITE );
	addQuad( renderSystem, bound, ( invertX ? Rect( 1.0f, 0.0f, 0.0f, 1.0f ) : Rect( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
	endTask( renderSystem );
}

internal void
drawAutoGuess( RENDER_SYSTEM * renderSystem, APP_STATE * appState ) {
	RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_speech );
	FONT        * font       = &renderFont->font;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, COLOR_GRAY( 0.05f ) );
	
	for( int32 letter_index = 0; letter_index < 26; letter_index++ ) {
		rect letter_bound = appState->autoGuess_letterBound[ letter_index ];
		char letter_char  = ( char )( letter_index + 'A' );
		if( !appState->autoGuess_isSelected[ letter_index ] ) {
			FONT_CHAR * font_char = font->alphabet + ( letter_char - FONT_START_CHAR );
			
			rect bound = rectCD( getCenter( letter_bound ), font_char->dim * 1.2f );
			addQuad( renderSystem, bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
		}
	}
	endTask( renderSystem );
}

internal void
drawString_space( RENDER_SYSTEM * renderSystem, RENDER_FONT * renderFont, const char * string, int32 length, vec2 position, vec4 color, flo32 scale, flo32 widthPerSpace ) {
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, color );
	
	FONT * font = &renderFont->font;
	
	vec2 atPos = position;
	for( int32 char_index = 0; char_index < length; char_index++ ) {
		char string_charA = string[ char_index ];
		if( string_charA == ' ' ) {
			atPos.x += widthPerSpace;
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
drawString_wrap( RENDER_SYSTEM * renderSystem, ASSET_ID fontID, const char * string, vec2 panel_topLeft, flo32 panel_width, vec4 color, flo32 scale, MEMORY * tempMemory ) {
	int32 string_length = ( int32 )strlen( string );
	
	RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, fontID );
	FONT * font = &renderFont->font;
	
	#define MAX_WORD_LENGTH_COUNT  ( 1024 )
	flo32 * word_length   = _pushArray_clear( tempMemory, flo32, MAX_WORD_LENGTH_COUNT );
	int32 * word_endIndex = _pushArray_clear( tempMemory, int32, MAX_WORD_LENGTH_COUNT );
	
	flo32 currLength = 0.0f;
	int32 currIndex  = -1;
	
	int32 word_count = 0;
	
	for( int32 char_index = 0; char_index < string_length; char_index++ ) {
		char string_charA = string[ char_index ];
		Assert( string_charA != '\n' );
		Assert( string_charA != '\t' );
		if( string_charA == ' ' ) {
			word_length[   word_count ] = currLength;
			word_endIndex[ word_count ] = currIndex;
			
			word_count++;
			Assert( word_count < MAX_WORD_LENGTH_COUNT );
			
			currLength = 0.0f;
			currIndex  = char_index;
		} else {
			Assert( ( string_charA >= FONT_START_CHAR ) && ( string_charA <= FONT_END_CHAR ) );
			
			int32 char_indexA = string_charA - FONT_START_CHAR;
			FONT_CHAR * font_char = font->alphabet + char_indexA;
			
			currLength += font_char->advanceWidth;
			
			if( ( char_index + 1 ) < string_length ) {
				char string_charB = string[ char_index + 1 ];
				if( string_charB != ' ' ) {
					Assert( ( string_charB >= FONT_START_CHAR ) && ( string_charB <= FONT_END_CHAR ) );
					int32 char_indexB = string_charB - FONT_START_CHAR;
					int32 kernValue = font->kernValue[ char_indexA * FONT_CHAR_COUNT + char_indexB ];
					currLength += ( ( flo32 )kernValue );
				}
			}
		}
	}
	if( currLength > 0.0f ) {
		word_length[   word_count ] = currLength;
		word_endIndex[ word_count ] = currIndex;
		
		word_count++;
		Assert( word_count < MAX_WORD_LENGTH_COUNT );
	}
	
	int32 start_index = 0;
	vec2  atPos       = panel_topLeft + Vec2( 0.0f, -( font->ascent * scale ) );
	flo32 line_length = 0.0f;
	
	int32 space_count = 0;
	
	flo32 scaledWidth_space = font->advanceWidth_space * scale;
	flo32 scaledHeight      = font->advanceHeight      * scale;
	
	for( int32 word_index = 0; word_index < word_count; word_index++ ) {
		flo32 scaledLength = word_length[ word_index ] * scale;
		
		if( ( line_length + scaledLength ) > panel_width ) {
			int32 end_index = word_endIndex[ word_index ];
			
			flo32 widthPerSpace = ( panel_width - ( line_length - scaledWidth_space * ( flo32 )space_count ) ) / ( flo32 )( space_count - 1 );
			drawString_space( renderSystem, renderFont, string + start_index, end_index - start_index, atPos, color, scale, widthPerSpace );
			
			start_index = end_index + 1;
			line_length = scaledLength + scaledWidth_space;
			space_count = 1;
			
			atPos.y -= scaledHeight;
		} else {
			line_length += ( scaledLength + scaledWidth_space );
			space_count++;
		}
	}
	drawString( renderSystem, renderFont, string + start_index, string_length - start_index, atPos, color, scale );
	
	_popArray( tempMemory, int32, MAX_WORD_LENGTH_COUNT );
	_popArray( tempMemory, flo32, MAX_WORD_LENGTH_COUNT );
}

internal void
drawX( RENDER_SYSTEM * renderSystem, rect bound, APP_STATE * appState, int32 flashValue ) {
	vec4 color = COLOR_WHITE;
	if( ( appState->incorrectGuess_count == flashValue ) && ( appState->anim_timer > 0.0f ) ) {
		flo32 t0 = cosf( appState->anim_timer * 2.0f * PI * 1.75f ) * 0.5f + 0.5f;
		flo32 t1 = lerp( 0.0f, t0, 0.8f ); 
		color = Vec4( 1.0f, t1, t1, 1.0f );
	}
	
	RENDER_FONT * 	renderFont = getFont( renderSystem->assetSystem, assetID_font_chalk );
	FONT * font = &renderFont->font;
	
	beginTask( renderSystem, meshDrawType_triangles, renderFont->texture_bufferID, color );
	
	rect adjBound = addDim( bound, Vec2( -2.0f, 2.0f ) );
	
	FONT_CHAR * char_x = font->alphabet + ( 'X' - FONT_START_CHAR );
	uint32 vertex_index = renderSystem->vertex_count;
	
	addQuad( renderSystem, adjBound, rectMM( char_x->texCoord_min, char_x->texCoord_max  ) );
	endTask( renderSystem );
}

internal void
initParticle( PARTICLE_SYSTEM * particleSystem ) {
	int32 offset = particleSystem->currentIndex * PARTICLE__MAX_COUNT;
	particleSystem->currentIndex = ( ( particleSystem->currentIndex + 1 ) % PARTICLE__MAX_INDEX );
	
	vec2 * position  = particleSystem->position + offset;
	vec2 * velocity  = particleSystem->velocity + offset;
	flo32 * dragX    = particleSystem->dragX    + offset;
	flo32 * dragYp   = particleSystem->dragYp   + offset;
	flo32 * dragYn   = particleSystem->dragYn   + offset;
	
	for( int32 particle_index = 0; particle_index < PARTICLE__MAX_COUNT; particle_index++ ) {
		position[ particle_index ] = Vec2( -20.0f, -20.0f );
		
		flo32 degrees = ( ( flo32 )( rand() % 850 ) / ( 10.0f ) ) + 5.0f;
		velocity[ particle_index ] = Vec2( cosDegrees( degrees ), sinDegrees( degrees ) ) * ( ( flo32 )( rand() % 7000 ) + 1000.0f );
		
		dragX[    particle_index ] = ( ( flo32 )( rand() % 1000 ) / 1000.0f ) * 3.0f + 4.5f;
		dragYp[   particle_index ] = ( ( flo32 )( rand() % 1000 ) / 1000.0f ) * 3.0f + 5.5f;
		dragYn[   particle_index ] = ( ( flo32 )( rand() % 1000 ) / 1000.0f ) * 2.0f + 2.5f;
	}
}

internal void
updateParticle( APP_STATE * appState, flo32 dt ) {
	vec2 app_dim = appState->app_dim;
	
	vec2 accel = Vec2( 0.0f, -250.0f );
	
	PARTICLE_SYSTEM * particleSystem = &appState->particleSystem;
	vec2 * position = particleSystem->position;
	vec2 * velocity = particleSystem->velocity;
	
	for( int32 particle_index = 0; particle_index < PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX; particle_index++ ) {
		flo32 dragY = ( ( velocity[ particle_index ].y > 0.0f ) ? particleSystem->dragYp[ particle_index ] : particleSystem->dragYn[ particle_index ] );
		vec2 vel_accel = accel - velocity[ particle_index ] * Vec2( particleSystem->dragX[ particle_index ], dragY );
		velocity[ particle_index ] += vel_accel * dt;
	}
	
	vec2 pos_accel = accel * 0.5f * dt * dt;
	for( int32 particle_index = 0; particle_index < PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX; particle_index++ ) {
		position[ particle_index ] += ( pos_accel + velocity[ particle_index ] * dt );
	}
}

internal void
drawParticle( RENDER_SYSTEM * renderSystem, PARTICLE_SYSTEM * particleSystem ) {
	beginTask( renderSystem, meshDrawType_triangles, getTexture( renderSystem->assetSystem, assetID_texture_confetti ).bufferID, COLOR_WHITE );
	
	rect tex_bound[] = {
		Rect( 0.0f, 0.0f, 1.0f, 0.2f ),
		Rect( 0.0f, 0.2f, 1.0f, 0.4f ),
		Rect( 0.0f, 0.4f, 1.0f, 0.6f ),
		Rect( 0.0f, 0.6f, 1.0f, 0.8f ),
		Rect( 0.0f, 0.8f, 1.0f, 1.0f ),
	};
	
	for( int32 particle_index = 0; particle_index < PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX; particle_index++ ) {
		rect bound = rectCD( particleSystem->position[ particle_index ], Vec2( 12.0f, 12.0f ) );
		addQuad( renderSystem, bound, tex_bound[ ( particle_index % 5 ) ] );
	}
	endTask( renderSystem );
}

internal void
initUnscrambleAnim( APP_STATE * appState ) {
	int32 unscramble_count       = 0;
	char  unscramble_char [ 64 ] = {};
	int32 unscramble_index[ 64 ] = {};
	
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		if( !appState->word_isGuessed[ word_index ] ) {
			char word_char = appState->word_check[ word_index ];
			unscramble_char [ unscramble_count ] = word_char;
			unscramble_index[ unscramble_count ] = word_index;
			unscramble_count++;
		}
	}
	
	int32 animChar_count = 0;
	for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
		if( !appState->word_isGuessed[ word_index ] ) {
			char word_char = appState->word[ word_index ];
			
			int32 hot_index = -1;
			int32 hot_iter  = -1;
			for( int32 iter = 0; iter < unscramble_count; iter++ ) {
				if( unscramble_char[ iter ] == word_char ) {
					hot_iter  = iter;
					hot_index = unscramble_index[ iter ];
				}
			}
			Assert( hot_index != -1 );
			
			unscramble_count--;
			unscramble_char[  hot_iter ] = unscramble_char[  unscramble_count ];
			unscramble_index[ hot_iter ] = unscramble_index[ unscramble_count ];
			
			rect word_bound = appState->word_bound[ word_index ];
			rect dest_bound = appState->word_bound[  hot_index ];
			
			vec2  srcPos = getBL( word_bound );
			vec2 destPos = getBL( dest_bound );
			vec2 moveVector = destPos - srcPos;
			
			appState->animChar[ animChar_count++ ] = AnimChar( word_char, srcPos, moveVector );
		}
	}
	
	appState->animChar_count = animChar_count;
}