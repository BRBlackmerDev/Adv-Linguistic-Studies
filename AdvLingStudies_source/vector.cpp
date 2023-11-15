
#ifdef	STD_INCLUDE_DECL

//----------
// vector declarations
//----------

union vec2 {
	struct {
		flo32 x;
		flo32 y;
	};
	flo32 elem[2];
};

union vec3 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
	};
	flo32 elem[3];
};

union vec4 {
	struct {
		flo32 x;
		flo32 y;
		flo32 z;
		flo32 w;
	};
	struct {
		flo32 r;
		flo32 g;
		flo32 b;
		flo32 a;
	};
	flo32 elem[4];
};

//----------
// rect declarations
//----------

union rect {
	struct {
		vec2 min;
		vec2 max;
	};
	struct {
		flo32 left;
		flo32 bottom;
		flo32 right;
		flo32 top;
	};
	flo32 elem[4];
};

//----------
// mat 4x4 declarations
//----------

union mat4 {
	vec4 column[ 4 ];
	flo32 elem[ 16 ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// vec2 functions
//----------

inline vec2 Vec2( flo32 a, flo32 b ) { vec2 result = { a, b }; return result; }
inline vec2 operator+( vec2 a, vec2 b ) { vec2 result = { a.x + b.x, a.y + b.y }; return result; }
inline vec2 operator-( vec2 a, vec2 b ) { vec2 result = { a.x - b.x, a.y - b.y }; return result; }
inline vec2 operator-( vec2 a ) { vec2 result = { -a.x, -a.y }; return result; }
inline vec2 operator*( vec2 a, flo32 f ) { vec2 result = { a.x * f, a.y * f }; return result; }
inline vec2 operator*( flo32 f, vec2 a ) { vec2 result = a * f; return result; }
inline vec2 operator*( vec2 a, vec2 b ) { vec2 result = { a.x * b.x, a.y * b.y }; return result; }
inline vec2 operator/( vec2 a, flo32 f ) { vec2 result = {}; if( f != 0.0f ) { result = Vec2( a.x / f, a.y / f ); } return result; }
inline vec2 operator/( vec2 a, vec2 b ) { vec2 result = { ( b.x != 0.0f ) ? a.x / b.x : 0.0f, ( b.y != 0.0f ) ? a.y / b.y : 0.0f }; return result; }
inline vec2 & operator+=( vec2 & a, vec2 b ) { a = a + b; return a; }
inline vec2 & operator-=( vec2 & a, vec2 b ) { a = a - b; return a; }
inline vec2 & operator*=( vec2 & a, flo32 f ) { a = a * f; return a; }
inline vec2 & operator/=( vec2 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec2( 0.0f, 0.0f );	return a; }
inline boo32 operator==( vec2 a, vec2 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ); return result; }
inline boo32 operator!=( vec2 a, vec2 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ); return result; }
inline flo32 getLengthSq( vec2 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ); return result; }
inline flo32 getLength( vec2 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec2 getNormal( vec2 a ) { flo32 denom = getLength( a ); vec2 result = a / denom; return result; }
inline vec2 getPerp( vec2 a ) { vec2 result = { -a.y, a.x }; return result; }
inline vec2 getVector( flo32 degrees ) { vec2 result = { cosDegrees( degrees ), sinDegrees( degrees ) }; return result; }
inline flo32 dot( vec2 a, vec2 b ) { flo32 result = a.x * b.x + a.y * b.y; return result; }
inline vec2 lerp( vec2 a, flo32 t, vec2 b ) { vec2 result = Vec2( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ) ); return result; }

//----------
// vec3 functions
//----------

inline vec3 Vec3( flo32 a, flo32 b, flo32 c ) { vec3 result = { a, b, c }; return result; }
inline vec3 Vec3( vec2 v, flo32 z ) { vec3 result = { v.x, v.y, z }; return result; }
inline vec3 operator+( vec3 a, vec3 b ) { vec3 result = { a.x + b.x, a.y + b.y, a.z + b.z }; return result; }
inline vec3 operator-( vec3 a, vec3 b ) { vec3 result = { a.x - b.x, a.y - b.y, a.z - b.z }; return result; }
inline vec3 operator-( vec3 a ) { vec3 result = { -a.x, -a.y, -a.z }; return result; }
inline vec3 operator*( vec3 a, flo32 f ) { vec3 result = { a.x * f, a.y * f, a.z * f }; return result; }
inline vec3 operator*( flo32 f, vec3 a ) { vec3 result = a * f; return result; }
inline vec3 operator/( vec3 a, flo32 f ) { vec3 result = {}; if( f != 0.0f ) { result = Vec3( a.x / f, a.y / f, a.z / f ); } return result; }
inline vec3 & operator+=( vec3 & a, vec3 b ) { a = a + b; return a; }
inline vec3 & operator-=( vec3 & a, vec3 b ) { a = a - b; return a; }
inline vec3 & operator*=( vec3 & a, flo32 f ) { a = a * f; return a; }
inline vec3 & operator/=( vec3 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec3( 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec3 a, vec3 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ); return result; }
inline boo32 operator!=( vec3 a, vec3 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ); return result; }
inline flo32 getLengthSq( vec3 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ); return result; }
inline flo32 getLength( vec3 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec3 getNormal( vec3 a ) { flo32 denom = getLength( a ); vec3 result = a / denom; return result; }
inline vec3 cross( vec3 a, vec3 b ) { vec3 result = getNormal( Vec3( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x ) ); return result; }
inline vec3 cross( vec3 a, vec3 b, vec3 c ) { vec3 result = cross( b - a, c - a ); return result; }
inline flo32 dot( vec3 a, vec3 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z; return result; }
inline vec3 lerp( vec3 a, flo32 t, vec3 b ) { vec3 result = Vec3( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ) ); return result; }

//----------
// vec4 functions
//----------

inline vec4 Vec4( flo32 a, flo32 b, flo32 c, flo32 d ) { vec4 result = { a, b, c, d }; return result; }
inline vec4 Vec4( vec3 vector, flo32 w ) { vec4 result = { vector.x, vector.y, vector.z, w }; return result; }
inline vec4 operator+( vec4 a, vec4 b ) { vec4 result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; return result; }
inline vec4 operator-( vec4 a, vec4 b ) { vec4 result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return result; }
inline vec4 operator*( vec4 a, flo32 f ) { vec4 result = { a.x * f, a.y * f, a.z * f, a.w * f }; return result; }
inline vec4 operator*( flo32 f, vec4 a ) { vec4 result = a * f; return result; }
inline vec4 operator/( vec4 a, flo32 f ) { vec4 result = {}; if( f != 0.0f ) { result = Vec4( a.x / f, a.y / f, a.z / f, a.w / f ); } return result; }
inline vec4 operator-( vec4 a ) { vec4 result = { -a.x, -a.y, -a.z, -a.w }; return result; }
inline vec4 & operator+=( vec4 & a, vec4 b ) { a = a + b; return a; }
inline vec4 & operator-=( vec4 & a, vec4 b ) { a = a - b; return a; }
inline vec4 & operator*=( vec4 & a, flo32 f ) { a = a * f; return a; }
inline vec4 & operator/=( vec4 & a, flo32 f ) { a = ( f != 0.0f ) ? a / f : Vec4( 0.0f, 0.0f, 0.0f, 0.0f ); return a; }
inline boo32 operator==( vec4 a, vec4 b ) { boo32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w ); return result; }
inline boo32 operator!=( vec4 a, vec4 b ) { boo32 result = ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ) || ( a.w != b.w ); return result; }
inline flo32 getLengthSq( vec4 a ) { flo32 result = ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) + ( a.w * a.w ); return result; }
inline flo32 getLength( vec4 a ) { flo32 result = sqrtf( getLengthSq( a ) ); return result; }
inline vec4 getNormal( vec4 a ) { flo32 denom = getLength( a ); vec4 result = a / denom; return result; }
inline flo32 dot( vec4 a, vec4 b ) { flo32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; return result; }
inline vec4 lerp( vec4 a, flo32 t, vec4 b ) { vec4 result = Vec4( lerp( a.x, t, b.x ), lerp( a.y, t, b.y ), lerp( a.z, t, b.z ), lerp( a.w, t, b.w ) ); return result; }

internal vec4
toV4( uint8 r, uint8 g, uint8 b, uint8 a ) {
	vec4 result = { ( flo32 )r / 255.0f, ( flo32 )g / 255.0f, ( flo32 )b / 255.0f, ( flo32 )a / 255.0f };
	return result;
}

//----------
// rect functions
//----------

inline rect Rect( flo32 left, flo32 bottom, flo32 right, flo32 top ) { rect result = { left, bottom, right, top }; return result; }
inline flo32 getWidth(  rect r ) { flo32 result = ( r.right - r.left ); return result; }
inline flo32 getHeight( rect r ) { flo32 result = ( r.top - r.bottom ); return result; }
inline vec2 getDim( rect r ) { vec2 result = { getWidth( r ), getHeight( r ) }; return result; }
inline vec2 getHalfDim( rect r ) { vec2 result = getDim( r ) * 0.5f; return result; }
inline vec2 getTL( rect r ) { vec2 result = Vec2( r.left,  r.top    ); return result; }
inline vec2 getTR( rect r ) { vec2 result = Vec2( r.right, r.top    ); return result; }
inline vec2 getBL( rect r ) { vec2 result = Vec2( r.left,  r.bottom ); return result; }
inline vec2 getBR( rect r ) { vec2 result = Vec2( r.right, r.bottom ); return result; }
inline vec2 getTC( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, r.top    ); return result; }
inline vec2 getBC( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, r.bottom ); return result; }
inline vec2 getLC( rect r ) { vec2 result = Vec2( r.left,  ( r.bottom + r.top ) * 0.5f );  return result; }
inline vec2 getRC( rect r ) { vec2 result = Vec2( r.right, ( r.bottom + r.top ) * 0.5f );  return result; }
inline rect rectTLD( vec2 topLeft,     vec2 dim ) { rect result = Rect( topLeft.x,             topLeft.y - dim.y,  topLeft.x + dim.x,    topLeft.y             ); return result; }
inline rect rectTRD( vec2 topRight,    vec2 dim ) { rect result = Rect( topRight.x - dim.x,    topRight.y - dim.y, topRight.x,           topRight.y            ); return result; }
inline rect rectBLD( vec2 bottomLeft,  vec2 dim ) { rect result = Rect( bottomLeft.x,          bottomLeft.y,       bottomLeft.x + dim.x, bottomLeft.y  + dim.y ); return result; }
inline rect rectBRD( vec2 bottomRight, vec2 dim ) { rect result = Rect( bottomRight.x - dim.x, bottomRight.y,      bottomRight.x,        bottomRight.y + dim.y ); return result; }
inline rect rectLCD( vec2   leftCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect(  leftCenter.x - dim.x,  leftCenter.y - halfDimY,  leftCenter.x,           leftCenter.y + halfDimY ); return result; }
inline rect rectRCD( vec2  rightCenter, vec2 dim ) { flo32 halfDimY = dim.y * 0.5f; rect result = Rect( rightCenter.x,         rightCenter.y - halfDimY, rightCenter.x + dim.x, rightCenter.y + halfDimY ); return result; }
inline rect rectBCD( vec2 bottomCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect( bottomCenter.x - halfDimX, bottomCenter.y,      bottomCenter.x + halfDimX, bottomCenter.y + dim.y ); return result; }
inline rect rectTCD( vec2    topCenter, vec2 dim ) { flo32 halfDimX = dim.x * 0.5f; rect result = Rect(    topCenter.x - halfDimX, topCenter.y - dim.y,    topCenter.x + halfDimX, topCenter.y            ); return result; }
inline rect rectCD(  vec2 center, vec2 dim ) { vec2 halfDim = dim * 0.5f; rect result = Rect( center.x - halfDim.x, center.y - halfDim.y, center.x + halfDim.x, center.y + halfDim.y ); return result; }
inline rect rectCHD( vec2 center, vec2 halfDim ) { rect result = Rect( center.x - halfDim.x, center.y - halfDim.y, center.x + halfDim.x, center.y + halfDim.y ); return result; }
inline rect rectCR(  vec2 center, flo32 radius ) { rect result = Rect( center.x - radius, center.y - radius, center.x + radius, center.y + radius ); return result; }
inline rect rectMM(  vec2 min, vec2 max ) { rect result = Rect( min.x, min.y, max.x, max.y ); return result; }
inline rect addDim( rect r, flo32 x0, flo32 y0, flo32 x1, flo32 y1 ) { rect result = Rect( r.left - x0, r.bottom - y0, r.right + x1, r.top + y1 ); return result; }
inline rect addDim( rect r, vec2 dim ) { rect result = Rect( r.left - dim.x, r.bottom - dim.y, r.right + dim.x, r.top + dim.y ); return result; }
inline rect addRadius( rect r, flo32 radius ) { rect result = Rect( r.left - radius, r.bottom - radius, r.right + radius, r.top + radius ); return result; }
inline vec2 getCenter( rect r ) { vec2 result = Vec2( ( r.left + r.right ) * 0.5f, ( r.bottom + r.top ) * 0.5f ); return result; }

inline boo32 isInBound( vec2 point, rect bound ) { boo32 result = ( point.x >= bound.left ) && ( point.y >= bound.bottom ) && ( point.x <= bound.right ) && ( point.y <= bound.top ); return result; }

//----------
// 4x4 matrix functions
//----------

inline vec4 getRow( mat4 matrix, int32 rowIndex ) {
	vec4 result = {};
	for( int32 counter = 0; counter < 4; counter++ ) {
		result.elem[ counter ] = matrix.elem[ counter * 4 + rowIndex ];
	}
	return result;
}
inline mat4 mat4_identity() { mat4 result = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }; return result; }

internal mat4
mat4_frustum( flo32 left, flo32 right, flo32 bottom, flo32 top, flo32 nearPlane, flo32 farPlane ) {
	flo32 width = right - left;
	flo32 height = top - bottom;
	flo32 depth = farPlane - nearPlane;
	flo32 nearPlane2x = nearPlane * 2.0f;
	
	mat4 result = {
			  nearPlane2x / width, 							  0.0f, 										 0.0f,  0.0f,	// column 1
								  0.0f, 		 nearPlane2x / height, 										 0.0f,  0.0f,	// column 2
		( right + left ) / width, ( top + bottom ) / height, -( farPlane + nearPlane ) / depth, -1.0f,	// column 3
								  0.0f, 							  0.0f, 	 -nearPlane2x * farPlane / depth,  0.0f,	// column 4
	};
	
	return result;
}

internal mat4
mat4_perspective( vec2 window_dim, flo32 fovAngle, flo32 nearPlane, flo32 farPlane ) {
	flo32 aspectRatio = window_dim.x / window_dim.y;
	flo32 fovHalfAngle = fovAngle * 0.5f;
	flo32 cot = cosDegrees( fovHalfAngle ) / sinDegrees( fovHalfAngle );
	flo32 depth = nearPlane - farPlane;
	
	mat4 result = {
		cot / aspectRatio, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, cot, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, ( farPlane + nearPlane ) / depth, -1.0f,	// column 3
		0.0f, 0.0f, ( 2.0f * nearPlane * farPlane ) / depth,  0.0f,	// column 4
	};
	return result;
}

internal mat4
mat4_orthographic( flo32 left, flo32 bottom, flo32 right, flo32 top, flo32 nearPlane, flo32 farPlane ) {
	flo32 width = right - left;
	flo32 height = top - bottom;
	flo32 depth = farPlane - nearPlane;
	
	mat4 result = {
		2.0f / width, 0.0f, 0.0f, 0.0f,	// column 1
		0.0f, 2.0f / height, 0.0f, 0.0f,	// column 2
		0.0f, 0.0f, -2.0f / depth, 0.0f,	// column 3
		-( right + left ) / width, -( top + bottom ) / height, -( farPlane + nearPlane ) / depth, 1.0f,	// column 4
	};
	return result;
}

internal mat4
mat4_orthographic( rect bound, flo32 nearPlane, flo32 farPlane ) {
	mat4 result = mat4_orthographic( bound.left, bound.bottom, bound.right, bound.top, nearPlane, farPlane );
	return result;
}

internal mat4
mat4_transpose( mat4 a ) {
	mat4 result = {};
	for( int32 index = 0; index < 4; index++ ) {
		result.column[ index ] = getRow( a, index );
	}
	return result;
}

internal mat4
mat4_translate( flo32 x, flo32 y, flo32 z ) {
	mat4 result = mat4_identity();
	result.column[3] = Vec4( x, y, z, 1.0f );
	return result;
}

inline mat4
mat4_translate( vec2 xy, flo32 z ) {
	mat4 result = mat4_translate( xy.x, xy.y, z );
	return result;
}

inline mat4
mat4_translate( vec3 a ) {
	mat4 result = mat4_translate( a.x, a.y, a.z );
	return result;
}

internal mat4
mat4_rotate( flo32 x, flo32 y, flo32 z, flo32 degrees ) {
	vec3 axis = getNormal( Vec3( x, y, z ) );
	
	flo32 radians = toRadians( degrees );
	flo32 cosValue = cosRadians( radians );
	flo32 sinValue = sinRadians( radians );
	flo32 invCos = 1.0f - cosValue;
	
	flo32 xyInv = axis.x * axis.y * invCos;
	flo32 xzInv = axis.x * axis.z * invCos;
	flo32 yzInv = axis.y * axis.z * invCos;
	flo32 x2Inv = squareValue( axis.x ) * invCos;
	flo32 y2Inv = squareValue( axis.y ) * invCos;
	flo32 z2Inv = squareValue( axis.z ) * invCos;
	flo32 xSin = axis.x * sinValue;
	flo32 ySin = axis.y * sinValue;
	flo32 zSin = axis.z * sinValue;
	
	mat4 result = {
		x2Inv + cosValue, xyInv + zSin, xzInv - ySin, 0.0f,
		xyInv - zSin, y2Inv + cosValue, yzInv + xSin, 0.0f,
		xzInv + ySin, yzInv - xSin, z2Inv + cosValue, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

inline mat4
mat4_rotate( vec3 axis, flo32 degrees ) {
	mat4 result = mat4_rotate( axis.x, axis.y, axis.z, degrees );
	return result;
}

inline mat4
mat4_rotate( vec4 axisAngle ) {
	mat4 result = mat4_rotate( axisAngle.x, axisAngle.y, axisAngle.z, axisAngle.w );
	return result;
}

internal mat4
operator*( mat4 a, mat4 b ) {
	mat4 result = {};
	for( int32 columnIndex = 0; columnIndex < 4; columnIndex++ ) {
		for( int32 rowIndex = 0; rowIndex < 4; rowIndex++ ) {
			int32 elemIndex = columnIndex * 4 + rowIndex;
			result.elem[ elemIndex ] = dot( getRow( a, rowIndex ), b.column[ columnIndex ] );
		}
	}
	return result;
}

internal vec4
operator*( mat4 a, vec4 b ) {
	vec4 result = {};
	for( int32 elemIndex = 0; elemIndex < 4; elemIndex++ ) {
		result.elem[ elemIndex ] = dot( getRow( a, elemIndex ), b );
	}
	return result;
}

inline mat4 & operator*=( mat4 & a, mat4 b ) { a = a * b; return a; }

internal mat4
mat4_quaternion( vec4 quat ) {
	flo32 x2 = 2.0f * squareValue( quat.x );
	flo32 y2 = 2.0f * squareValue( quat.y );
	flo32 z2 = 2.0f * squareValue( quat.z );
	
	flo32 xy = 2.0f * quat.x * quat.y;
	flo32 xz = 2.0f * quat.x * quat.z;
	flo32 xw = 2.0f * quat.x * quat.w;
	flo32 yz = 2.0f * quat.y * quat.z;
	flo32 yw = 2.0f * quat.y * quat.w;
	flo32 zw = 2.0f * quat.z * quat.w;
	
	mat4 result = { 1.0f - y2 - z2, xy + zw, xz - yw, 0.0f,
						 xy - zw, 1.0f - x2 - z2, yz + xw, 0.0f,
						 xz + yw, yz - xw, 1.0f - x2 - y2, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f };
	return result;
}

internal mat4
mat4_scale( flo32 xScale, flo32 yScale, flo32 zScale ) {
	mat4 result = { xScale, 0.0f, 0.0f, 0.0f,
						 0.0f, yScale, 0.0f, 0.0f,
						 0.0f, 0.0f, zScale, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f,
	};
	return result;
}

inline mat4
mat4_scale( vec3 scale ) {
	mat4 result = mat4_scale( scale.x, scale.y, scale.z );
	return result;
}

// internal mat4
// transform( vec3 position = Vec3( 0.0f, 0.0f, 0.0f ), vec3 scale = Vec3( 1.0f, 1.0f, 1.0f ), QUATERNION orientation = Quat( Vec4_fill( 0.0f ) ) ) {
	// mat4 result = mat4_identity();
	// result *= mat4_translate( position );
	// result *= mat4_rotate( orientation );
	// result *= mat4_scale( scale );
	// return result;
// }

// internal mat4
// transform( vec2 position, vec2 scale ) {
	// mat4 result = transform( Vec3( position, 0.0f ), Vec3( scale, 1.0f ) );
	// return result;
// }

// internal mat4
// transform( vec2 position, vec2 scale, flo32 degrees ) {
	// mat4 result = mat4_identity();
	// result *= mat4_translate( Vec3( position, 0.0f ) );
	// result *= mat4_rotate( 0.0f, 0.0f, 1.0f, degrees );
	// result *= mat4_scale( Vec3( scale, 1.0f ) );
	// return result;
// }

#endif	// STD_INCLUDE_FUNC