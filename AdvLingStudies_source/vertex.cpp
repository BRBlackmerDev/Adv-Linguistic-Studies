
#ifdef	STD_INCLUDE_DECL

//----------
// vertex declarations
//----------

struct VERTEX_TEXMESH_2D {
	vec2 position;
	vec2 texCoord;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// vertex functions
//----------

internal VERTEX_TEXMESH_2D
VertexTexMesh2D( flo32 x, flo32 y, flo32 u, flo32 v ) {
	VERTEX_TEXMESH_2D result = { x, y, u, v };
	return result;
}

internal VERTEX_TEXMESH_2D
VertexTexMesh2D( vec2 a, flo32 u, flo32 v ) {
	VERTEX_TEXMESH_2D result = { a.x, a.y, u, v };
	return result;
}

internal VERTEX_TEXMESH_2D
VertexTexMesh2D( flo32 x, flo32 y, vec2 uv ) {
	VERTEX_TEXMESH_2D result = { x, y, uv.x, uv.y };
	return result;
}

#endif	// STD_INCLUDE_FUNC