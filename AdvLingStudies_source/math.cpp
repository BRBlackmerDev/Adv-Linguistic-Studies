
#define PI							3.14159274f
#define RADIANS_TO_DEGREES		57.2957795f
#define DEGREES_TO_RADIANS  	0.0174533f

internal flo32
minValue( flo32 a, flo32 b ) {
	flo32 result = ( ( a < b ) ? a : b );
	return result;
}

internal flo32
maxValue( flo32 a, flo32 b ) {
	flo32 result = ( ( a > b ) ? a : b );
	return result;
}

internal int32
minValue( int32 a, int32 b ) {
	int32 result = ( ( a < b ) ? a : b );
	return result;
}

internal int32
maxValue( int32 a, int32 b ) {
	int32 result = ( ( a > b ) ? a : b );
	return result;
}

internal uint32
minValue( uint32 a, uint32 b ) {
	uint32 result = ( ( a < b ) ? a : b );
	return result;
}

internal uint32
maxValue( uint32 a, uint32 b ) {
	uint32 result = ( ( a > b ) ? a : b );
	return result;
}

internal flo32
clamp( flo32 value, flo32 lo, flo32 hi ) {
	Assert( lo <= hi );
	flo32 result = maxValue( minValue( value, hi ), lo );
	return result;
}

internal flo32
clamp01( flo32 value ) {
	flo32 result = clamp( value, 0.0f, 1.0f );
	return result;
}

internal int32
roundToInt32( flo32 value ) {
	int32 result = ( int32 )( value >= 0.0f ? value + 0.5f : value - 0.5f );
	return result;
}

inline uint32
roundToU32( flo32 value ) {
	Assert( value >= 0.0f && value <= ( flo32 )0xFFFFFFFF );
	uint32 result = ( uint32 )( value + 0.5f );
	return result;
}

internal int32
ceilToInt32( flo32 value ) {
	int32 result = ( int32 )value;
	if( value - ( int32 )value != 0.0f ) {
		result = ( int32 )( value >= 0.0f ? value + 1.0f : value - 1.0f );
	}
	return result;
}

internal flo32
roundToPoint5( flo32 value ) {
	int32 truncatedValue = ( int32 )value;
	flo32 result = ( value >= 0.0f ? ( flo32 )truncatedValue + 0.5f : ( flo32 )truncatedValue - 0.5f );
	return result;
}

internal flo32
floorValue( flo32 value ) {
	flo32 result = ( flo32 )( ( int32 )value );
	return result;
}

internal flo32
ceilValue( flo32 value ) {
	flo32 result = value;
	if( value - ( int32 )value != 0.0f ) {
		result = ( flo32 )( ( int32 )( value >= 0.0f ? value + 1.0f : value - 1.0f ) );
	}
	return result;
}

internal uint32
truncToU32( uint64 value ) {
	Assert( value <= 0xFFFFFFFF );
	uint32 result = ( uint32 )value;
	return result;
}

internal uint16
truncToU16( uint32 value ) {
	Assert( value <= 0xFFFF );
	uint16 result = ( uint16 )value;
	return result;
}

internal uint8
truncToU8( uint32 value ) {
	Assert( value <= 0xFF );
	uint8 result = ( uint8 )value;
	return result;
}

internal uint8
truncToU8( int32 value ) {
	Assert( value >= 0 && value <= 0xFF );
	uint8 result = ( uint8 )value;
	return result;
}

internal uint8
toU8( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFF + 1.0f );
	uint8 result = ( uint8 )value;
	return result;
}

internal uint16
toU16( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFFFF + 1.0f );
	uint16 result = ( uint16 )value;
	return result;
}

internal uint16
toU16( char * string ) {
	Assert( strlen( string ) <= 2 );
	char * at = string;
	uint16 result = 0;
	uint32 char_index = 0;
	while( *at ) {
		result |= ( *at++ << ( 8 * char_index++ ) );
	}
	return result;
}

internal uint32
toU32( flo32 value ) {
	Assert( value >= 0.0f && value < ( flo32 )0xFFFFFFFF + 1.0f );
	uint32 result = ( uint32 )value;
	return result;
}

internal uint32
toU32( char * string ) {
	Assert( strlen( string ) <= 4 );
	char * at = string;
	uint32 result = 0;
	uint32 char_index = 0;
	while( *at ) {
		result |= ( *at++ << ( 8 * char_index++ ) );
	}
	return result;
}

internal uint32
absValue( int32 value ) {
	uint32 result = ( value < 0 ? -value : value );
	return result;
}

internal flo32
absValue( flo32 value ) {
	flo32 result = ( value < 0.0f ? -value : value );
	return result;
}

internal flo32
expValue( flo32 base, int32 exponent ) {
	uint32 count = absValue( exponent );
	flo32 result = 1.0f;
	for( uint32 counter = 0; counter < count; counter++ ) {
		result *= base;
	}
	return result;
}

internal int32
expValue( int32 base, int32 exponent ) {
	uint32 count = absValue( exponent );
	int32 result = 1;
	for( uint32 counter = 0; counter < count; counter++ ) {
		result *= base;
	}
	return result;
}

internal uint32
expValue( uint32 base, uint32 exponent ) {
	Assert( exponent >= 0 );
	uint32 count = exponent;
	uint32 result = 1;
	for( uint32 counter = 0; counter < count; counter++ ) {
		result *= base;
	}
	return result;
}

internal int32
randomValue( int32 lowBound, int32 highBound ) {
	int32 range = highBound - lowBound;
	int32 value = rand() % range;
	int32 result = value + lowBound;
	return result;
}

internal flo32
randomFloat( flo32 lowBound, flo32 highBound ) {
	int32 value = randomValue( ( int32 )( lowBound * 10.0f ), ( int32 )( highBound * 10.0f ) );
	flo32 result = ( flo32 )value / 10.0f;
	return result;
}

inline flo32
squareValue( flo32 value ) {
	flo32 result = value * value;
	return result;
}

inline flo32
toDegrees( flo32 radians ) {
	flo32 result = radians * RADIANS_TO_DEGREES;
	return result;
}

inline flo32
toRadians( flo32 degrees ) {
	flo32 result = degrees * DEGREES_TO_RADIANS;
	return result;
}

inline flo32
cosRadians( flo32 radians ) {
	flo32 result = cosf( radians );
	return result;
}

inline flo32
sinRadians( flo32 radians ) {
	flo32 result = sinf( radians );
	return result;
}

inline flo32
cosDegrees( flo32 degrees ) {
	flo32 result = cosRadians( toRadians( degrees ) );
	return result;
}

inline flo32
getDegrees( flo32 x, flo32 y ) {
	flo32 result = toDegrees( ( flo32 )atan2f( y, x ) );
	if( result < 0.0f ) {
		result += 360.0f;
	}
	return result;
}

inline flo32
sinDegrees( flo32 degrees ) {
	flo32 result = sinRadians( toRadians( degrees ) );
	return result;
}

inline flo32
lerp( flo32 a, flo32 t, flo32 b ) {
	flo32 result = ( 1.0f - t ) * a + t * b;
	return result;
}