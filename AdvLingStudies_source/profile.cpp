
#ifdef	STD_INCLUDE_DECL

//----------
// profile declarations
//----------

#define FUNCTION_NAME_MAX_LENGTH  256
struct PROFILE_EVENT {
	char functionName[ FUNCTION_NAME_MAX_LENGTH ];
	uint64 clock_start;
	uint64 clock_end;
};

#define PROFILE_EVENT_LIST_MAX_COUNT  16384
struct PROFILE_EVENT_LIST {
	PROFILE_EVENT event[ PROFILE_EVENT_LIST_MAX_COUNT ];
	uint64 frameClockStart;
	int32 count;
};

struct PROFILE_NODE {
	char functionName[ FUNCTION_NAME_MAX_LENGTH ];
	uint64 clock_total;
	uint32 count_total;
};

#define PROFILE_NODE_LIST_MAX_COUNT  128
struct PROFILE_NODE_LIST {
	PROFILE_NODE node[ PROFILE_NODE_LIST_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_STACK_MAX_COUNT  16
struct PROFILE_EVENT_STACK {
	PROFILE_EVENT event[ PROFILE_EVENT_STACK_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_DATABASE_MAX_COUNT  ( 5 * 60 )
struct PROFILE_EVENT_DATABASE {
	PROFILE_EVENT_LIST * eventList;
	int32 maxCount;
	int32 index;
	int32 selectIndex;
};

#if DEBUG_BUILD
#define PROFILE_FUNCTION()  PROFILE_OBJ profileObj = PROFILE_OBJ( globalVar_profileSystem.eventList_currentFrame, __FUNCTION__ )
#else
#define PROFILE_FUNCTION()
#endif

struct PROFILE_EVENT_DATABASE_CLOCKS {
	uint64 frameStart;
	uint64 platformStart;
	uint64 appStart;
	uint64 renderStart;
	uint64 frameEnd;
	int64  counter_frameStart;
	int64  counter_frameEnd;
};

class PROFILE_OBJ {
	public:
		PROFILE_EVENT * event;
		PROFILE_OBJ( PROFILE_EVENT_LIST * eventList, char * funcName );
		~PROFILE_OBJ();
};

struct PROFILE_SYSTEM {
	vec2  atPos;
	flo32 advanceHeight;
	boo32 showProfile;
	boo32 appPaused;
	PROFILE_EVENT_DATABASE	eventDatabase;
	PROFILE_EVENT_LIST *		eventList_currentFrame;
	PROFILE_NODE_LIST * 		nodeList_currentFrame;
	
	PROFILE_EVENT_DATABASE_CLOCKS		eventDatabase_clocks_currentFrame;
	PROFILE_EVENT_DATABASE_CLOCKS 	eventDatabase_clocks_previousFrame;
	PROFILE_EVENT_DATABASE_CLOCKS *	eventDatabase_clocks;
	
	flo32 eventDatabase_posOffset;
	flo32 eventDatabase_width;
	flo32 eventDatabase_height;
	boo32 eventDatabase_scrollActive;
	boo32 frameBreakdown_scrollActive_bar;
	boo32 frameBreakdown_scrollActive_left;
	boo32 frameBreakdown_scrollActive_right;
	flo32 frameBreakdown_scrollX_left;
	flo32 frameBreakdown_scrollX_right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

#if DEBUG_BUILD
#define PROFILE_SYSTEM__SIZE_IN_BYTES	( ( ( PROFILE_EVENT_DATABASE_MAX_COUNT + 1 ) * sizeof( PROFILE_EVENT_LIST ) ) + sizeof( PROFILE_NODE_LIST ) + ( PROFILE_EVENT_DATABASE_MAX_COUNT * sizeof( PROFILE_EVENT_DATABASE_CLOCKS ) ) )
PROFILE_SYSTEM globalVar_profileSystem = {};

//----------
// profile functions
//----------

internal void
setString( char * srcStr, char * destStr ) {
	char * dest = destStr;
	char * src = srcStr;
	while( *src ) {
		*dest++ = *src++;
	}
	*dest = 0;
}

internal boo32
matchString( char * strA, char * strB ) {
	boo32 result = true;
	
	boo32 checkString = true;
	while( checkString ) {
		if( strA[ 0 ] && strB[ 0 ] ) {
			if( strA[ 0 ] != strB[ 0 ] ) {
				result = false;
				checkString = false;
			} else {
				strA++;
				strB++;
			}
		} else if( ( strA[ 0 ] && !strB[ 0 ] ) || ( !strA[ 0 ] && strB[ 0 ] ) ) {
			result = false;
			checkString = false;
		} else {
			checkString = false;
		}
	}
	return result;
}

internal PROFILE_EVENT *
newEvent( PROFILE_EVENT_LIST * profileList ) {
	PROFILE_EVENT * event = profileList->event + profileList->count++;
	Assert( profileList->count < PROFILE_EVENT_LIST_MAX_COUNT );
	event->clock_start = 0;
	event->clock_end = 0;
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		event->functionName[ iter ] = 0;
	}
	return event;
}

internal PROFILE_NODE *
newNode( PROFILE_NODE_LIST * profileList ) {
	PROFILE_NODE * node = profileList->node + profileList->count++;
	Assert( profileList->count < PROFILE_NODE_LIST_MAX_COUNT );
	node->clock_total = 0;
	node->count_total = 0;
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		node->functionName[ iter ] = 0;
	}
	return node;
}

internal void
collateEvents( PROFILE_EVENT_LIST * eventList, PROFILE_NODE_LIST * nodeList ) {
	PROFILE_EVENT_STACK _eventStack = {};
	PROFILE_EVENT_STACK * eventStack = &_eventStack;
	
	for( int32 event_index = 0; event_index < eventList->count; event_index++ ) {
		PROFILE_EVENT * event = eventList->event + event_index;
		
		PROFILE_NODE * currentNode = 0;
		boo32 isInList = false;
		
		for( int32 node_index = 0; node_index < nodeList->count && !isInList; node_index++ ) {
			PROFILE_NODE * node = nodeList->node + node_index;
			
			if( matchString( event->functionName, node->functionName ) ) {
				isInList = true;
				currentNode = node;
			}
		}
		
		if( !isInList ) {
			currentNode = newNode( nodeList );
			setString( event->functionName, currentNode->functionName );
		}
		Assert( currentNode );
		currentNode->count_total++;
		uint64 clock_total = event->clock_end - event->clock_start;
		currentNode->clock_total += clock_total;
		
		for( int32 iter = eventStack->count - 1; iter >= 0; iter-- ) {
			PROFILE_EVENT * stackEvent = eventStack->event + iter;
			
			if( event->clock_start > stackEvent->clock_end ) {
				eventStack->count--;
			}
		}
		
		if( eventStack->count > 0 ) {
			PROFILE_EVENT * parentEvent = eventStack->event + eventStack->count - 1;
			for( int32 node_index = 0; node_index < nodeList->count; node_index++ ) {
				PROFILE_NODE * node = nodeList->node + node_index;
				
				if( matchString( parentEvent->functionName, node->functionName ) ) {
					node->clock_total -= clock_total;
					break;
				}
			}
		}
		
		PROFILE_EVENT * pushEvent = eventStack->event + eventStack->count++;
		Assert( eventStack->count < PROFILE_EVENT_STACK_MAX_COUNT );
		pushEvent->clock_start = event->clock_start;
		pushEvent->clock_end = event->clock_end;
		setString( event->functionName, pushEvent->functionName );
	}
	
	boo32 isSorted = true;
	do {
		isSorted = true;
		for( int32 node_index = 0; node_index < nodeList->count - 1; node_index++ ) {
			PROFILE_NODE * nodeA = nodeList->node + node_index;
			PROFILE_NODE * nodeB = nodeList->node + node_index + 1;
			
			if( nodeA->clock_total < nodeB->clock_total ) {
				PROFILE_NODE _temp = {};
				PROFILE_NODE * temp = &_temp;
				
				setString( nodeA->functionName, temp->functionName );
				temp->clock_total = nodeA->clock_total;
				temp->count_total = nodeA->count_total;
				
				setString( nodeB->functionName, nodeA->functionName );
				nodeA->clock_total = nodeB->clock_total;
				nodeA->count_total = nodeB->count_total;
				
				setString( temp->functionName, nodeB->functionName );
				nodeB->clock_total = temp->clock_total;
				nodeB->count_total = temp->count_total;
				
				isSorted = false;
			}
		}
	} while( !isSorted );
}

PROFILE_OBJ::PROFILE_OBJ( PROFILE_EVENT_LIST * eventList, char * funcName ) {
	event = newEvent( eventList );
	setString( funcName, event->functionName );
	event->clock_start = __rdtsc();
}
		
PROFILE_OBJ::~PROFILE_OBJ() {
	event->clock_end = __rdtsc();
}

#endif // DEBUG_BUILD

#endif	// STD_INCLUDE_FUNC