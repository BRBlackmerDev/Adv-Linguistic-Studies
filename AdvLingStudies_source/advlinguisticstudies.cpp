
internal void
APP_updateAndRender( APP_STATE * appState, PLATFORM * platform ) {
	PROFILE_FUNCTION();
	
	RENDER_SYSTEM * renderSystem = &platform->renderSystem;
	ASSET_SYSTEM * assetSystem = &platform->assetSystem;
	
	MEMORY * permMemory = &platform->permMemory;
	MEMORY * tempMemory = &platform->tempMemory;
	
	MOUSE_STATE    * mouse    = &platform->mouse;
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	
	SAVE_STATE * saveState = &appState->saveState;
	PARTICLE_SYSTEM * particleSystem = &appState->particleSystem;
	
	APP_MODE * appMode = &appState->appMode;
	
	RENDER_MONOSPACE_FONT * font_main = &assetSystem->defaultMonoFont;
	
	vec2 app_dim = appState->app_dim;
	vec2 app_halfDim = appState->app_halfDim;
	rect app_bound = appState->app_bound;
	
	vec2 word_dim   = Vec2( 740.0f, 180.0f );
	rect word_bound = rectTLD( getTL( app_bound ) + Vec2( 40.0f, -82.0f ), word_dim );
	
	vec2 letter_dim = Vec2( 660.0f, 160.0f );
	vec2 letter_pos = Vec2( app_halfDim.x + 20.0f, 30.0f );
	rect letter_bound = rectBCD( letter_pos, letter_dim );
	
	if( !appState->isInitialized ) {
		appState->isInitialized = true;
		
		// FILE_DATA dictionary_file = platform->readFile( tempMemory, 0, "../../art/word_data.txt" );
		// if( dictionary_file.contents ) {
			// char * src_start  = ( char * )dictionary_file.contents;
			// char * dest_start = ( char * )appState->dictionary_buffer;
			
			// char * src  =  src_start;
			// char * dest = dest_start;
			
			// int32 dictionary_count = 0;
			
			// while( ( src - src_start ) < dictionary_file.size ) {				
				// src++;  src++;  src++;  src++;
				
				// appState->dictionary_word[ dictionary_count ] = dest;
				// while( *src != ':'  ) { *dest++ = *src++; }
				// *dest++ = 0;
				
				// appState->dictionary_def[ dictionary_count ] = dest;
				// while( *src != '\r' ) {
					// *dest++ = *src++;
				// }
				// Assert( *src++ == '\r' );
				// Assert( *src++ == '\n' );
				// *dest++ = 0;
				
				// dictionary_count++;
			// }
			// *dest++ = 0;
			
			// appState->dictionary_count = dictionary_count;
			// appState->dictionary_size  = ( int32 )( dest - dest_start );
			
			// _popSize( tempMemory, dictionary_file.size );
			
			// platform->writeFile( 0, "dictionary.dat", appState->dictionary_buffer, appState->dictionary_size );
		// }
		
		loadDictionary( appState, platform );
		if( !loadGame( platform, saveState ) ) {
			saveState->autoGuess[ 0 ] = '*';
			saveState->autoGuess[ 1 ] = '*';
			saveState->autoGuess[ 2 ] = '*';
			saveState->autoGuess[ 3 ] = '*';
			saveState->autoGuess[ 4 ] = '*';
			
			saveState->newWord_count = 365;
			for( int32 iter = 0; iter < saveState->newWord_count; iter++ ) {
				saveState->newWord_index[ iter ] = iter;
			}
		}
		
		{ // get line dim
			RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_chalk );
			FONT        * font       = &renderFont->font;
			
			FONT_CHAR * line_char = font->alphabet + ( '_' - FONT_START_CHAR );
			
			appState->line_charX = line_char->dim.x;
			appState->line_charY = line_char->dim.y;
		}
		
		{ // set bounds for letters
			flo32 letter_gap   = 2.0f;
			flo32 letter_width  = ( getWidth(  letter_bound ) - ( letter_gap * 12.0f ) ) / 13.0f;
			flo32 letter_height = ( getHeight( letter_bound ) - letter_gap ) * 0.5f;
			
			vec2 letter_dim = Vec2( letter_width, letter_height );
			
			vec2 atPos = getTL( letter_bound );
			for( int32 iter =  0; iter < 13; iter++ ) { appState->letter_bound[ iter ] = rectTLD( atPos, letter_dim );   atPos.x += ( letter_dim.x + letter_gap ); }
			
			atPos = getBL( letter_bound );
			for( int32 iter = 13; iter < 26; iter++ ) { appState->letter_bound[ iter ] = rectBLD( atPos, letter_dim );   atPos.x += ( letter_dim.x + letter_gap ); }
		}
		
		appState->guess_bound = word_bound;
		
		appState->confirmExit  = false;
		appState->select_index = -1;
		setAppMode( appMode, appMode_menu );
	}
	
	vec2  select_position    = appState->select_position;
	boo32 select_wasPressed  = appState->select_wasPressed;
	boo32 select_wasReleased = appState->select_wasReleased;
	
	drawRect( renderSystem, assetID_texture_wall, app_bound, COLOR_WHITE );
	
	vec2 defBubble_dim = Vec2( app_dim.x * 0.9f, app_dim.y * 0.5f );
	vec2 defBubble_pos = Vec2( app_halfDim.x, 8.0f );
	rect defBubble_bound = rectBCD( defBubble_pos, defBubble_dim );
	rect exitCancel_bound = addRadius( defBubble_bound, -8.0f );
	
	rect exit_bound = rectTRD( getTR( app_bound ) + Vec2( 0.0f, -54.0f ), Vec2( 124.0f, 440.0f ) );
	if( select_wasReleased ) {
		boo32 exitIsValid = !( ( appState->appMode.mode == appMode_definition ) && isInBound( select_position, exitCancel_bound ) );
		if( exitIsValid && isInBound( select_position, exit_bound ) ) {
			if( appState->confirmExit ) {
				platform->endApplication( platform, appState );
			} else {
				appState->confirmExit = true;
			}
		} else {
			appState->confirmExit = false;
		}
	}
	vec2 door_dim = Vec2( 273.0f, 556.0f ) * 0.96f;
	vec2 door_pos = getTL( exit_bound ) + Vec2( -24.0f, 40.0f );
	rect door_bound = rectTLD( door_pos, door_dim );
	drawRect( renderSystem, ( appState->confirmExit ? assetID_texture_door_open : assetID_texture_door_closed ), door_bound, COLOR_WHITE );
	
	vec2 chalkboard_dim   = Vec2( 840.0f, app_dim.y * 0.6f );
	rect chalkboard_bound = rectTLD( getTL( app_bound ) + Vec2( 2.0f, -2.0f ), chalkboard_dim );
	drawRect( renderSystem, assetID_texture_chalkboard, chalkboard_bound, COLOR_WHITE );
	
	vec2 desk_dim = Vec2( 400.0f, 380.0f ) * 0.8f;
	rect desk_bound = rectTRD( Vec2( 180.0f, 300.0f ), desk_dim );
	drawRect( renderSystem, assetID_texture_desk, desk_bound, COLOR_WHITE );
	
	vec2 tail_dimH = Vec2( 100.0f,  30.0f );
	vec2 tail_dimV = Vec2(  50.0f, 100.0f );
	vec2 teacher_dim = Vec2( 355.0f, 620.0f ) * 0.78f;
	vec2 teacher_pos = Vec2( 792.0f, 34.0f );
	
	vec2 notebook_dim   = Vec2( app_dim.x * 0.9f, app_dim.y * 0.8f );
	rect notebook_bound = rectCD( Vec2( app_halfDim.x, -20.0f ), notebook_dim );
	
	vec2 incorrectGuess_dim = Vec2( 48.0f, 48.0f );
	vec2 incorrectGuess_pos = getTR( app_bound ) + Vec2( -260.0f, -26.0f );
	
	DISPLAY_VALUE( vec2, select_position );
	
	switch( appMode->mode ) {
		case appMode_guessALetter: {
			if( initMode( appMode ) ) {
				newRound( appState );
				appState->autoGuess_panelIsOpen = false;
				appState->anim_timer = 0.0f;
			}
			
			if( ( select_wasReleased ) && ( appState->anim_timer == 0.0f ) ) {
				vec2 atPos = getBL( app_bound );
				for( uint8 letter_index = 0; letter_index < 26; letter_index++ ) {
					rect bound = appState->letter_bound[ letter_index ];
					if( isInBound( select_position, bound ) ) {
						guessChar( appState, letter_index + 'A' );
					}
				}
			}
			
			if( appState->incorrectGuess_count == 5 ) {
				if( appState->anim_timer >= 3.2f ) {
					setAppMode( appMode, appMode_menu );
				}
				appState->anim_timer += platform->targetSec;
			}
			
			vec2 atPos = incorrectGuess_pos + Vec2( -( 5.0f * incorrectGuess_dim.x ), 0.0f );
			for( int32 iter = 0; iter < 5; iter++ ) {
				rect bound = rectTLD( atPos, incorrectGuess_dim );
				drawRect( renderSystem, assetID_texture_checkbox, bound, COLOR_WHITE );
				if( iter < appState->incorrectGuess_count ) {
					drawX( renderSystem, bound, appState, 5 );
				}
				atPos.x += incorrectGuess_dim.x;
			}
			
			drawWord_guessALetter( renderSystem, appState );
			
			if( appState->confirmExit ) {
				teacher_pos += Vec2( -60.0f, -6.0f );
			}
			rect teacher_bound = rectBLD( teacher_pos, teacher_dim );
			drawTeacher( renderSystem, teacher_bound, true );
			
			drawParticle( renderSystem, particleSystem );
			
			drawRect( renderSystem, assetID_texture_notebook, notebook_bound, COLOR_WHITE );
			drawLetterBoard( renderSystem, appState );
			
			boo32 wordWasGuessed = true;
			for( int32 iter = 0; iter < appState->word_length; iter++ ) {
				if( !appState->word_isGuessed[ iter ] ) {
					wordWasGuessed = false;
				}
			}
			if( wordWasGuessed ) {
				if( appState->anim_timer == 0.0f ) {
					initParticle( particleSystem );
				}
				if( appState->anim_timer >= 1.2f ) {
					setAppMode( appMode, appMode_definition );
				}
				appState->anim_timer += platform->targetSec;
			}
		} break;
		
		case appMode_wordScramble: {
			if( initMode( appMode ) ) {
				newRound( appState );
				appState->anim_timer = 0.0f;
			}
			
			flo32 button_width = app_dim.x / 26.0f;
			vec2 button_dim = Vec2( button_width, button_width );
			
			int32 hover_index = -1;
			vec2 atPos = app_halfDim + Vec2( -200.0f, 200.0f );
			for( int32 word_index = 0; word_index < appState->word_length; word_index++ ) {
				rect        bound = appState->word_bound[ word_index ];
				
#if ANDROID_BUILD
				rect select_bound = Rect( bound.left, bound.bottom - 56.0f, bound.right, bound.bottom - 8.0f );
				if( ( !appState->word_isGuessed[ word_index ] ) && ( isInBound( select_position, select_bound ) ) ) {
					hover_index = word_index;
				}
#endif // ANDROID_BUILD

#if WINDOWS_BUILD
				if( ( !appState->word_isGuessed[ word_index ] ) && ( isInBound( select_position, bound ) ) ) {
					hover_index = word_index;
				}
#endif // WINDOWS_BUILD

				atPos.x += button_dim.x;
			}
			
			if( ( select_wasPressed ) && ( appState->anim_timer == 0.0f ) ) {
				if( ( hover_index > -1 ) && ( !appState->word_isGuessed[ hover_index ] ) ) {
					appState->move_srcIndex = hover_index;
					appState->move_srcChar  = appState->word[ hover_index ];
#if ANDROID_BUILD					
					hover_index = -1;
#endif // ANDROID_BUILD
				}
			}
			
			atPos = incorrectGuess_pos + Vec2( -( 3.0f * incorrectGuess_dim.x ), 0.0f );
			for( int32 iter = 0; iter < 3; iter++ ) {
				rect bound = rectTLD( atPos, incorrectGuess_dim );
				drawRect( renderSystem, assetID_texture_checkbox, bound, COLOR_WHITE );
				if( iter < appState->incorrectGuess_count ) {
					drawX( renderSystem, bound, appState, 3 );
				}
				atPos.x += incorrectGuess_dim.x;
			}
			
			rect wordScramble_bound = rectTRD( app_halfDim + Vec2( -60.0f, -116.0f ), Vec2( 240.0f, 60.0f ) );
			rect   submit_bound = rectTCD( getBC( wordScramble_bound ) + Vec2( 0.0f, -20.0f ), Vec2( 214.0f, 60.0f ) );
			
			if( ( select_wasReleased ) && ( appState->anim_timer == 0.0f ) ) {
				if( appState->move_srcIndex > -1 ) {
					if( ( hover_index > -1 ) && ( !appState->word_isGuessed[ hover_index ] ) ) {
						appState->word[ appState->move_srcIndex ] = appState->word[ hover_index ];
						appState->word[ hover_index ] = appState->move_srcChar;
					}
					appState->move_srcIndex = -1;
				} else {				
					if( isInBound( select_position, submit_bound ) ) {
						checkScramble( appState );
						checkForMatches( appState );
					}
					if( isInBound( select_position, wordScramble_bound ) ) {
						scrambleWord( appState );
					}
				}
			}
			
			drawWord_wordScramble( renderSystem, appState, hover_index );
			
			if( appState->confirmExit ) {
				teacher_pos += Vec2( -60.0f, -6.0f );
			}
			rect teacher_bound = rectBLD( teacher_pos, teacher_dim );
			drawTeacher( renderSystem, teacher_bound, true );
			
			drawParticle( renderSystem, particleSystem );
			
			drawRect( renderSystem, assetID_texture_notebook, notebook_bound, COLOR_WHITE );
			
			drawString( renderSystem, assetID_font_pen, "RE-SCRAMBLE", getBR( wordScramble_bound ), COLOR_BLACK, ALIGN_BOTTOMRIGHT, 0.9f );
			drawString( renderSystem, assetID_font_pen, "CHECK GUESS", getBR(       submit_bound ), COLOR_BLACK, ALIGN_BOTTOMRIGHT, 0.8f );
			
			{
				rect boundA = rectTCD( getBC( wordScramble_bound ) + Vec2( 4.0f, 6.0f ), Vec2( 260.0f, 8.0f ) );
				beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_underline ).bufferID, COLOR_GRAY( 0.2f ) );
				addQuad( renderSystem, boundA, Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
				endTask( renderSystem );
				
				rect boundB = rectTCD( getBC( submit_bound ) + Vec2( 4.0f, 6.0f ), Vec2( 234.0f, 8.0f ) );
				beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_underline ).bufferID, COLOR_GRAY( 0.2f ) );
				addQuad( renderSystem, boundB, Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
				endTask( renderSystem );
			}
			
			if( appState->move_srcIndex > -1 ) {
				flo32 width = getWidth( appState->word_bound[ 0 ] ) * 0.8f;
				
#if WINDOWS_BUILD
				rect bound = rectBCD( select_position + Vec2( 0.0f, 6.0f ), Vec2( width, width ) );
#endif // WINDOWS_BUILD

#if ANDROID_BUILD
				rect bound = rectBCD( select_position + Vec2( 0.0f, 60.0f ), Vec2( width, width ) );
#endif // ANDROID_BUILD
				
				beginTask( renderSystem, meshDrawType_triangleFan, getTexture( renderSystem->assetSystem, assetID_texture_highlight ).bufferID, Vec4( 0.0f, 0.0f, 0.0f, 0.80f ) );
				addQuad( renderSystem, addRadius( bound, 3.0f ), Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
				endTask( renderSystem );
				
				RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_chalk );
				FONT        * font       = &renderFont->font;
				FONT_CHAR   * font_char  = font->alphabet + ( appState->move_srcChar - FONT_START_CHAR );
				
				beginTask( renderSystem, meshDrawType_triangleFan, renderFont->texture_bufferID, Vec4( 0.9f, 0.9f, 0.9f, 0.9f ) );
				addQuad( renderSystem, bound, rectMM( font_char->texCoord_min, font_char->texCoord_max ) );
				endTask( renderSystem );
			}
			
			if( appState->incorrectGuess_count == 3 ) {
				if( appState->anim_timer == 0.0f ) {
					initUnscrambleAnim( appState );
				}
				if( appState->anim_timer >= 3.6f ) {
					setAppMode( appMode, appMode_menu );
				}
				appState->anim_timer += platform->targetSec;
			}
			
			boo32 wordWasGuessed = true;
			for( int32 iter = 0; iter < appState->word_length; iter++ ) {
				if( !appState->word_isGuessed[ iter ] ) {
					wordWasGuessed = false;
				}
			}
			if( wordWasGuessed ) {
				if( appState->anim_timer == 0.0f ) {
					initParticle( particleSystem );
				}
				if( appState->anim_timer >= 1.2f ) {
					setAppMode( appMode, appMode_definition );
				}
				appState->anim_timer += platform->targetSec;
			}
			
			{
				RENDER_FONT * renderFont = getFont( renderSystem->assetSystem, assetID_font_pen );
				FONT * font = &renderFont->font;
				
				flo32 scaleA = 0.55f;
				flo32 scaleB = 0.75f;
				
				vec2 basePos = Vec2( 640.0f, 188.0f );
				drawString( renderSystem, assetID_font_pen, "Combos I found:", basePos, COLOR_BLACK, ALIGN_TOPLEFT, scaleA );
				atPos = basePos + Vec2( -40.0f, -( ( font->advanceHeight * scaleA ) + ( font->ascent * scaleB ) + 16.0f ) );
				int32 match_count = minValue( appState->match_count, 8 );
				for( int32 match_index = 0; match_index < match_count; match_index++ ) {
					if( match_index == 4 ) {
						atPos = basePos + Vec2( 130.0f, -( ( font->advanceHeight * scaleA ) + ( font->ascent * scaleB ) + 16.0f ) );
					}
					char num_string[ 4 ] = {};
					sprintf( num_string, "%d.", match_index + 1 );
					drawString( renderSystem, assetID_font_pen, num_string, atPos + Vec2( -8.0f, 0.0f ), COLOR_BLACK, ALIGN_BASERIGHT, scaleB );
					
					int32 subString_index = appState->subString_index[ match_index ];
					int32 subString_count = appState->subString_count[ match_index ];
					drawString( renderSystem, renderFont, appState->word_check + subString_index, subString_count, atPos, COLOR_BLACK, scaleB );
					atPos.y -= ( ( font->advanceHeight * scaleB ) + 2.0f );
				}
			}
		} break;
		
		case appMode_definition: {
			if( initMode( appMode ) ) {
				initDefinition( appState );
				appState->anim_timer = 0.0f;
				appState->finishRound = false;
			}
			
			drawWord_guessALetter( renderSystem, appState );
			
			if( appState->confirmExit ) {
				teacher_pos += Vec2( -60.0f, -6.0f );
			}
			rect teacher_bound = rectBLD( teacher_pos, teacher_dim );
			drawTeacher( renderSystem, teacher_bound, true );
			
			drawParticle( renderSystem, particleSystem );
			
			vec2 notebook_position = Vec2( app_halfDim.x, 120.0f );
			rect notebook_bound    = rectTCD( notebook_position, notebook_dim * 1.10f );
			drawRect( renderSystem, assetID_texture_notebook, notebook_bound, COLOR_WHITE );
			
			rect bubble_bound = defBubble_bound;
			drawRect( renderSystem, assetID_texture_bubble, bubble_bound, COLOR_WHITE );
			
			vec2 tail_position = getTR( bubble_bound ) + ( appState->confirmExit ? Vec2( -230.0f, -6.0f ) : Vec2( -170.0f, -6.0f ) );
			rect tail_bound    = rectBRD( tail_position, tail_dimH );
			drawBubbleTail( renderSystem, tail_bound, true );
			
			rect panel_bound = addDim( bubble_bound, -16.0f, -14.0f, -16.0f, -54.0f );
			drawString( renderSystem, assetID_font_speech, "Good job! Now, what is the correct definition?", getTL( panel_bound ) + Vec2( 20.0f, 8.0f ), COLOR_BLACK, ALIGN_BOTTOMLEFT, 0.88f );
			
			vec2 def_dim = Vec2( getWidth( panel_bound ), getHeight( panel_bound ) * 0.3333f );
			if( ( select_wasReleased ) && ( !appState->finishRound ) ) {
				int32 def_select = -1;
				vec2 atPos = getTL( panel_bound );
				
				for( int32 guess_index = 0; guess_index < 3; guess_index++ ) {
					rect bound = rectTLD( atPos, def_dim );
					if( isInBound( select_position, bound ) ) {
						def_select = appState->def_guess[ guess_index ];
						appState->def_select = guess_index;
					}
					atPos.y -= def_dim.y;
				}
				
				if( def_select != -1 ) {
					appState->finishRound = true;
					if( appState->select_index == def_select ) {
						initParticle( particleSystem );
						appState->wonRound = true;
					}
				}
			}
			
			vec2 atPos = getTL( panel_bound );
			for( int32 iter = 0; iter < 3; iter++ ) {
				rect button_bound = rectTLD( atPos, def_dim );
				rect   text_bound = addDim( button_bound, Vec2( -14.0f, -6.0f ) );
				
				vec4 color = COLOR_GRAY( 0.9f );
				if( appState->finishRound ) {
					if( appState->def_guess[ iter ] == appState->select_index ) {
						flo32 t0 = cosf( appState->anim_timer * 2.0f * PI * 1.75f ) * 0.5f + 0.5f;
						flo32 t1 = lerp( 0.0f, t0, 0.8f ); 
						color = Vec4( t1, 1.0, t1, 1.0f );
					} else if( iter == appState->def_select ) {
						color = Vec4( 1.0f, 0.2f, 0.2f, 1.0f );
					}
				}
				
				drawRect( renderSystem, assetID_texture_defBubble, button_bound, color );
				drawString_wrap( renderSystem, assetID_font_speech, appState->dictionary_def[ appState->def_guess[ iter ] ], getTL( text_bound ), getWidth( text_bound ), COLOR_BLACK, 0.65f, tempMemory );
				
				atPos.y -= def_dim.y;
			}
			
			if( appState->finishRound ) {
				appState->anim_timer += platform->targetSec;
				if( appState->anim_timer >= 2.4f ) {
					setAppMode( appMode, appMode_menu );
				}
			}
		} break;
		
		case appMode_menu: {
			if( initMode( appMode ) ) {
				saveGame( platform, saveState );
				appState->confirmExit = true;
			}
			
			if( appState->select_index != -1 ) {
				vec2 def_position = getTL( app_bound ) + Vec2( 36.0f, -110.0f );
				vec2 def_dim      = Vec2( 360.0f, 300.0f );
				rect def_bound    = rectTLD( def_position, def_dim );
				
				FONT * font = &( getFont( assetSystem, assetID_font_chalk )->font );
				flo32 word_width = getWidth( font, appState->dictionary_word[ appState->select_index ], appState->word_length );
				flo32 word_scale = minValue( ( def_dim.x * 0.9f ) / word_width, 0.9f );
				
				drawString(      renderSystem, assetID_font_chalk, appState->dictionary_word[ appState->select_index ], getTC( def_bound ) + Vec2( 0.0f, 4.0f ), COLOR_WHITE, ALIGN_BOTTOMCENTER, word_scale );
				drawString_wrap( renderSystem, assetID_font_chalk, appState->dictionary_def[  appState->select_index ], getTL( def_bound ), getWidth( def_bound ), COLOR_WHITE, 0.42f, tempMemory );
			}
			
			vec2 teacher_position = ( appState->select_index == -1 ) ? Vec2( app_halfDim.x - 260.0f, 20.0f ) : Vec2( app_halfDim.x - 80.0f, 20.0f );
			rect teacher_bound    = rectBCD( teacher_position, teacher_dim * 1.10f );
			drawTeacher( renderSystem, teacher_bound, false );
			
			drawParticle( renderSystem, particleSystem );
			
			vec2 notebook_position = Vec2( app_halfDim.x, 120.0f );
			rect notebook_bound    = rectTCD( notebook_position, notebook_dim * 1.10f );
			drawRect( renderSystem, assetID_texture_notebook, notebook_bound, COLOR_WHITE );
			
			drawString( renderSystem, assetID_font_pen, "AutoGuess:", Vec2( app_halfDim.x - 80.0f, 40.0f ), COLOR_BLACK, ALIGN_BASERIGHT, 0.8f );
			vec2 autoGuess_pos = Vec2( app_halfDim.x + 12.0f, 36.0f );
			char autoGuess_string[ 2 ] = {};
			for( int32 autoGuess_index = 0; autoGuess_index < 5; autoGuess_index++ ) {
				autoGuess_string[ 0 ] = saveState->autoGuess[ autoGuess_index ];
				drawString( renderSystem, assetID_font_pen, autoGuess_string, autoGuess_pos, COLOR_BLACK, ALIGN_BASECENTER, 1.20f );
				autoGuess_pos.x += 44.0f;
			}
			
			if( autoGuessIsValid( saveState ) ) {
				vec2 line_dim = Vec2( 500.0f, 10.0f );
				vec2 line_pos = Vec2( app_halfDim.x - 20.0f, 20.0f );
				rect line_bound = rectBCD( line_pos, line_dim );
				drawRect( renderSystem, assetID_texture_underline, line_bound, COLOR_GRAY( 0.1f ) );
			}
			
			vec2 panel_dim = Vec2( 320.0f, 440.0f );
			vec2 panel_pos = teacher_position + Vec2( 154.0f, 60.0f ); 
			rect panel_bound = rectBLD( panel_pos, panel_dim );
			rect autoGuess_bound = rectBCD( Vec2( app_halfDim.x, 18.0f ), Vec2( 480.0f, 80.0f ) );
			
			if( appState->autoGuess_panelIsOpen ) {
				if( select_wasReleased ) {
					if( isInBound( select_position, panel_bound ) ) {
						for( int32 iter = 0; iter < 26; iter++ ) {
							rect bound = appState->autoGuess_letterBound[ iter ];
							if( isInBound( select_position, bound ) && ( !appState->autoGuess_isSelected[ iter ] ) ) {
								appState->autoGuess_isSelected[ iter ] = true;
								appState->autoGuess_selectCount++;
							}
						}
					} else {
						appState->autoGuess_panelIsOpen = false;
					}
				}
				drawRect( renderSystem, assetID_texture_bubble, panel_bound, COLOR_WHITE );
				
				vec2 tail_pos = getTL( panel_bound ) + Vec2( 6.0f, -170.0f );
				rect tail_bound = rectTRD( tail_pos, tail_dimV );
				drawRect( renderSystem, assetID_texture_bubbleTailV, tail_bound, COLOR_WHITE );
				
				rect autoGuess_select = addRadius( rectCD(  getCenter( panel_bound ) + Vec2( 0.0f, 44.0f ), Vec2( getWidth( panel_bound ),        getHeight( panel_bound ) * 0.6f ) ), -30.0f );
				rect autoGuess_text   = addRadius( rectLCD( getCenter( panel_bound ),                       Vec2( getWidth( panel_bound ) * 0.5f, getHeight( panel_bound )        ) ), -24.0f );
				
				{ // autoGuess_select panel
					flo32 width  = getWidth(  autoGuess_select );
					flo32 height = getHeight( autoGuess_select );
					
					flo32 letter_widthA = width / 5.0f;
					flo32 letter_widthB = width / 6.0f;
					flo32 letter_height = height / 5.0f;
					
					vec2 letter_dimA = Vec2( letter_widthA, letter_height );
					vec2 letter_dimB = Vec2( letter_widthB, letter_height );
					
					int32 letter_index = 0;
					vec2 basePos = getTL( autoGuess_select );
					
					vec2 atPos = basePos;  for( int32 iter = 0; iter < 5; iter++ ) { appState->autoGuess_letterBound[ letter_index++ ] = rectTLD( atPos, letter_dimA );  atPos.x += letter_widthA; }  basePos.y -= letter_height;
					     atPos = basePos;  for( int32 iter = 0; iter < 5; iter++ ) { appState->autoGuess_letterBound[ letter_index++ ] = rectTLD( atPos, letter_dimA );  atPos.x += letter_widthA; }  basePos.y -= letter_height;
					     atPos = basePos;  for( int32 iter = 0; iter < 5; iter++ ) { appState->autoGuess_letterBound[ letter_index++ ] = rectTLD( atPos, letter_dimA );  atPos.x += letter_widthA; }  basePos.y -= letter_height;
					     atPos = basePos;  for( int32 iter = 0; iter < 5; iter++ ) { appState->autoGuess_letterBound[ letter_index++ ] = rectTLD( atPos, letter_dimA );  atPos.x += letter_widthA; }  basePos.y -= letter_height;
					     atPos = basePos;  for( int32 iter = 0; iter < 6; iter++ ) { appState->autoGuess_letterBound[ letter_index++ ] = rectTLD( atPos, letter_dimB );  atPos.x += letter_widthB; }
						  
					for( int32 letter_index = 0; letter_index < 26; letter_index++ ) {
						rect bound = appState->autoGuess_letterBound[ letter_index ];
					}
					
					drawRect( renderSystem, assetID_texture_bubble, addRadius( autoGuess_select, 10.0f ), COLOR_GRAY( 0.98f ) );
					drawAutoGuess( renderSystem, appState );
				}
				
				{
					vec2 atPos = getTL( autoGuess_text );
					
					char num_string[ 4 ] = {};
					sprintf( num_string, "%d", ( 5 - appState->autoGuess_selectCount ) );
					
					char panel_string[ 256 ] = {};
					strcpy( panel_string, "Select " );
					strcat( panel_string, num_string );
					strcat( panel_string, " letter" );
					if( appState->autoGuess_selectCount != 4 ) { strcat( panel_string, "s" ); }
					strcat( panel_string, ":" );
					
					flo32 scaleA = 1.0f;
					flo32 scaleB = 0.69f;
					
					const char * info_string[] = {
						"These are 5 FREE guesses",
						"given AUTOMATICALLY",
						"at the beginning of each",
						"round of 'GUESS A LETTER'.",
						"They can be changed anytime.",
					};
					
					flo32 advanceHeight = getFont( assetSystem, assetID_font_speech )->font.advanceHeight;
					drawString( renderSystem, assetID_font_speech, panel_string,     atPos + Vec2(  0.0f, 4.0f ), COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleA );  atPos.y -= ( advanceHeight * scaleA * 1.10f );  atPos.x += 8.0f;
					atPos += Vec2( -10.0f, -( getHeight( autoGuess_select ) + 40.0f ) );
					drawString( renderSystem, assetID_font_speech, info_string[ 0 ], atPos, COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleB );  atPos.y -= ( advanceHeight * scaleB );
					drawString( renderSystem, assetID_font_speech, info_string[ 1 ], atPos, COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleB );  atPos.y -= ( advanceHeight * scaleB );
					drawString( renderSystem, assetID_font_speech, info_string[ 2 ], atPos, COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleB );  atPos.y -= ( advanceHeight * scaleB );
					drawString( renderSystem, assetID_font_speech, info_string[ 3 ], atPos, COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleB );  atPos.y -= ( advanceHeight * scaleB );
					drawString( renderSystem, assetID_font_speech, info_string[ 4 ], atPos, COLOR_GRAY( 0.05f ), ALIGN_TOPLEFT, scaleB );  atPos.y -= ( advanceHeight * scaleB );
				}
				
				if( appState->autoGuess_selectCount == 5 ) {
					appState->autoGuess_panelIsOpen = false;
					
					boo32 startHangman = !autoGuessIsValid( saveState );
					
					int32 select_count = 0;
					for( uint8 iter = 0; iter < 26; iter++ ) {
						if( appState->autoGuess_isSelected[ iter ] ) {
							saveState->autoGuess[ select_count++ ] = iter + 'A';
						}
					}
					Assert( select_count == 5 );
					saveGame( platform, saveState );
					
					if( startHangman ) {
						setAppMode( appMode, appMode_guessALetter );
					}
				}
			} else {
				if( select_wasReleased && isInBound( select_position, autoGuess_bound ) ) {
					appState->autoGuess_panelIsOpen = true;
					appState->autoGuess_selectCount = 0;
					for( int32 iter = 0; iter < 26; iter++ ) {
						appState->autoGuess_isSelected[ iter ] = false;
					}
				}
				
				vec2 bubble_dim = Vec2( 280.0f, 400.0f );
				vec2 bubble_pos = teacher_position + Vec2( 174.0f, 80.0f );
				rect bubble_bound = rectBLD( bubble_pos, bubble_dim );
				drawRect( renderSystem, assetID_texture_bubble, bubble_bound, COLOR_WHITE );
				
				vec2 tail_pos = getTL( bubble_bound ) + Vec2( 5.0f, -170.0f );
				rect tail_bound = rectTRD( tail_pos, tail_dimV );
				drawRect( renderSystem, assetID_texture_bubbleTailV, tail_bound, COLOR_WHITE );
				
				drawString( renderSystem, assetID_font_speech, "Choose:", getTL( bubble_bound ) + Vec2( 32.0f, -24.0f ), COLOR_BLACK, ALIGN_TOPLEFT, 1.1f );
				
				vec2 button_dim = Vec2( getWidth( bubble_bound ), getHeight( bubble_bound ) * 0.45f );
				rect guessALetter_bound = addRadius( rectBLD( getBL( bubble_bound ) + Vec2( 0.0f, button_dim.y - 20.0f ), button_dim ), -14.0f );
				rect wordScramble_bound = addRadius( rectBLD( getBL( bubble_bound ) + Vec2( 0.0f, 0.0f                 ), button_dim ), -14.0f );
				
				drawRect( renderSystem, assetID_texture_bubble, guessALetter_bound, COLOR_GRAY( 0.90f ) );
				drawRect( renderSystem, assetID_texture_bubble, wordScramble_bound, COLOR_GRAY( 0.90f ) );
				drawString( renderSystem, assetID_font_speech, "GUESS  A", getCenter( guessALetter_bound ), COLOR_BLACK, ALIGN_BOTTOMCENTER, 1.2f );
				drawString( renderSystem, assetID_font_speech, "LETTER",   getCenter( guessALetter_bound ), COLOR_BLACK, ALIGN_TOPCENTER,    1.2f );
				drawString( renderSystem, assetID_font_speech, "WORD",     getCenter( wordScramble_bound ), COLOR_BLACK, ALIGN_BOTTOMCENTER, 1.2f );
				drawString( renderSystem, assetID_font_speech, "SCRAMBLE", getCenter( wordScramble_bound ), COLOR_BLACK, ALIGN_TOPCENTER,    1.2f );
				
				if( select_wasReleased ) {
					if( isInBound( select_position, guessALetter_bound ) ) {
						if( autoGuessIsValid( saveState ) ) {
							setAppMode( appMode, appMode_guessALetter );
						} else {
							appState->autoGuess_panelIsOpen = true;
							appState->autoGuess_selectCount = 0;
							for( int32 iter = 0; iter < 26; iter++ ) {
								appState->autoGuess_isSelected[ iter ] = false;
							}
						}
					}
					if( isInBound( select_position, wordScramble_bound ) ) {
						setAppMode( appMode, appMode_wordScramble );
					}
				}
			}
		} break;
		
		default: {
			InvalidCodePath;
		} break;
	}
	
	updateParticle( appState, platform->targetSec );
	
#if WINDOWS_BUILD
	vec2 cursor_pos = select_position;
	vec2 cursor_dim = Vec2( 20.0f, 20.0f );
	rect cursor_bound = rectCD( cursor_pos, cursor_dim );
	drawRect( renderSystem, assetID_texture_cursor, cursor_bound, COLOR_WHITE );
#endif // WINDOWS_BUILD
}