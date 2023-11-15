

#define internal				static
#define local_persist		static
#define global_variable		static

#if DEBUG_BUILD
#define Assert( Expression ) if( !( Expression ) ) { *( int32 * )0 = 0; }
#else
#define Assert( Expression )
#endif

#define InvalidCodePath		Assert( 0 )
#define NotImplemented  	Assert( 0 )

#define _arrayCount( array )  ( sizeof( array ) / sizeof( array[0] ) )
#define _arrayBytes( array )  ( sizeof( array ) )

#define _KB( value ) ( value * 1024LL )
#define _MB( value ) ( value * 1024LL * 1024LL )
#define _GB( value ) ( value * 1024LL * 1024LL * 1024LL )

#define COLOR_BLACK  		Vec4( 0.0f, 0.0f, 0.0f, 1.0f )
#define COLOR_WHITE			Vec4( 1.0f, 1.0f, 1.0f, 1.0f )
#define COLOR_RED	 			Vec4( 1.0f, 0.0f, 0.0f, 1.0f )
#define COLOR_GREEN  		Vec4( 0.0f, 1.0f, 0.0f, 1.0f )
#define COLOR_BLUE			Vec4( 0.0f, 0.0f, 1.0f, 1.0f )
#define COLOR_YELLOW			Vec4( 1.0f, 1.0f, 0.0f, 1.0f )
#define COLOR_MAGENTA		Vec4( 1.0f, 0.0f, 1.0f, 1.0f )
#define COLOR_CYAN			Vec4( 0.0f, 1.0f, 1.0f, 1.0f )
#define COLOR_GRAY( value )  Vec4( value, value, value, 1.0f )
#define COLOR_TRANSPARENT	Vec4( 0.0f, 0.0f, 0.0f, 0.0f )

#define _minValue( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define _maxValue( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define _swap( type, valueA, valueB )  { type temp = valueA; valueA = valueB; valueB = temp; }
#define _addr( ptr, type )  ( type * )ptr;  ptr += sizeof( type );
#define _addra( ptr, type, count )  ( type * )ptr;  ptr += ( sizeof( type ) * count );
#define _read( ptr, type )  ( ( type * )ptr )[0];  ptr += sizeof( type );
#define _readStr( src, dest )  while( *src ) { *dest++ = *src++; }  *dest = 0;
#define _write( ptr, type, value )   ( ( type * )ptr )[0] = value;  ptr += sizeof( type );
#define _writem( memory, type, value )   { type * var = _pushType( memory, type );  *var = value; }
#define _copys( src, dest, count )  { for( int32 index = 0; index < count; index++ ) { *dest++ = *src++; } }
#define _copy( type, src, dest, count ) { type * ptrA = ( type * )src; type * ptrB = ( type * )dest; for( int32 index = 0; index < count; index++ ) { ptrB[ index ] = ptrA[ index ]; } }
#define _copym( memory, ptr, size ) { uint8 * dest = ( uint8 * )_pushSize( memory, size ); uint8 * src = ( uint8 * )ptr; for( int32 iter = 0; iter < size; iter++ ) { *dest++ = *src++; } }

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  flo32;
typedef double flo64;

typedef int32 boo32;