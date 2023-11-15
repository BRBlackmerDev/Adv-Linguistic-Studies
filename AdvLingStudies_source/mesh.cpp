
#ifdef	STD_INCLUDE_DECL

enum MESH_DRAW_TYPE {
	meshDrawType_null,
	
	meshDrawType_triangles,
	meshDrawType_triangleFan,
	meshDrawType_lineLoop,
	
	meshDrawType_count,
};

struct MESH {
	MESH_DRAW_TYPE drawType;
	
	int32 vertex_count;
	vec2 * vertex;
};

#endif	// STD_INCLUDE_DECL

#ifdef	STD_INCLUDE_FUNC
#endif	// STD_INCLUDE_FUNC