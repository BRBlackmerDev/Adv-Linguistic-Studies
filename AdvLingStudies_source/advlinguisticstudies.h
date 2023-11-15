
//----------
// AUDIT
//
// TODO
// - add desk clock
// - laser pointer as cursor
// - lightning on fail result
// - add multiple dictionary support
// - publish to google play store
//----------

enum APP_MODE_TYPE {
	appMode_null,
	
	appMode_guessALetter,
	appMode_wordScramble,
	appMode_definition,
	appMode_menu,
	
	appMode_count,
};

struct APP_MODE {
	APP_MODE_TYPE mode;
	boo32         isInitialized;
};

internal boo32
initMode( APP_MODE * appMode ) {
	boo32 result = !appMode->isInitialized;
	appMode->isInitialized = true;
	return result;
}

internal void
setAppMode( APP_MODE * appMode, APP_MODE_TYPE mode ) {
	appMode->mode = mode;
	appMode->isInitialized = false;
}

struct SAVE_STATE {
	int32 newWord_count;
	int32 newWord_index[ 365 ];
	char autoGuess[ 5 ];
};

#define PARTICLE__MAX_COUNT  ( 1024 )
#define PARTICLE__MAX_INDEX  (    3 )
struct PARTICLE_SYSTEM {
	int32 currentIndex;
	vec2 position [ PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX ];
	vec2 velocity [ PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX ];
	flo32 dragX   [ PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX ];
	flo32 dragYp  [ PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX ];
	flo32 dragYn  [ PARTICLE__MAX_COUNT * PARTICLE__MAX_INDEX ];
};

struct ANIM_CHAR {
	char value;
	vec2 position;
	vec2 moveVector;
};

internal ANIM_CHAR
AnimChar( char value, vec2 position, vec2 moveVector ) {
	ANIM_CHAR result = {};
	result.value      = value;
	result.position   = position;
	result.moveVector = moveVector;
	return result;
}

struct APP_STATE {
	boo32 isRunning;
	boo32 isInitialized;
	
	APP_MODE appMode;
	boo32 confirmExit;
	
	vec2 app_dim;
	vec2 app_halfDim;
	rect app_bound;
	
	char  word          [ 128 ];
	char  word_check    [ 128 ];
	boo32 word_isGuessed[ 128 ];
	rect  word_bound    [ 128 ];
	int32 word_length;
	
	rect guess_bound;
	
	flo32 anim_timer;
	boo32 finishRound;
	
	int32 select_index;
	int32 def_guess[ 3 ];
	int32 def_select;
	
	rect  letter_bound[ 26 ];
	boo32 letter_isGuessed[ 26 ];
	boo32 autoGuess_isSelected [ 26 ];
	rect  autoGuess_letterBound[ 26 ];
	
	vec2 select_position;
	boo32 select_wasPressed;
	boo32 select_wasReleased;
	
	int32 incorrectGuess_count;
	boo32 wonRound;
	
	boo32 doorOpen;
	
	flo32 line_charX;
	flo32 line_charY;
	vec2  line_offset;
	vec2  line_dim;
	
	int32 dictionary_count;
	char * dictionary_word[ 365 ];
	char * dictionary_def [ 365 ];
	
	int32 move_srcIndex;
	char  move_srcChar;
	
	int32 match_count;
	int32 subString_index[ 12 ];
	int32 subString_count[ 12 ];
	
	boo32 autoGuess_panelIsOpen;
	int32 autoGuess_selectCount;
	
	SAVE_STATE saveState;
	
	PARTICLE_SYSTEM particleSystem;
	
	#define DICTIONARY__MAX_BUFFER_SIZE  ( _KB( 128 ) )
	char  dictionary_buffer[ DICTIONARY__MAX_BUFFER_SIZE ];
	int32 dictionary_size;
	
	#define MAX_ANIM_CHAR_COUNT  ( 64 )
	int32 animChar_count;
	ANIM_CHAR animChar[ MAX_ANIM_CHAR_COUNT ];
};