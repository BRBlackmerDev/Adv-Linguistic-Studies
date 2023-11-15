
#ifdef 	STD_INCLUDE_DECL

struct RENDER_TEXTURE {
	uint32 bufferID;
	int32 width;
	int32 height;
};

struct RENDER_FONT {
	FONT   font;
	uint32 texture_bufferID;
};

struct RENDER_MONOSPACE_FONT {
	MONOSPACE_FONT	font;
	uint32	      texture_bufferID;
};

struct RENDER_OBJECT {
	MESH_DRAW_TYPE drawType;
	
	uint32 vertex_index;
	uint32 vertex_count;
	
	uint32 texture_bufferID;
	vec4 color;
};

struct ASSET_SYSTEM;
#define RENDER_SYSTEM__MAX_OBJECT_COUNT	( 8192 )
struct RENDER_SYSTEM {
	ASSET_SYSTEM * assetSystem;
	
	boo32          taskIsActive;
	RENDER_OBJECT  tempObject;
	
	int32 			object_count;
	RENDER_OBJECT 	object[ RENDER_SYSTEM__MAX_OBJECT_COUNT ];
	
	#define RENDER_SYSTEM__MAX_VERTEX_COUNT  262144
	int32 				vertex_count;
	VERTEX_TEXMESH_2D vertex[ RENDER_SYSTEM__MAX_VERTEX_COUNT ];
};

#endif 	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

internal RENDER_OBJECT
RenderObject( MESH_DRAW_TYPE drawType, uint32 vertex_index, uint32 vertex_count, uint32 texture_bufferID, vec4 color ) {
	RENDER_OBJECT result = { drawType, vertex_index, vertex_count, texture_bufferID, color };
	return result;
}

internal RENDER_TEXTURE
RenderTexture( uint32 bufferID, int32 width, int32 height ) {
	RENDER_TEXTURE result = { bufferID, width, height };
	return result;
}

internal void
addVertex( RENDER_SYSTEM * renderSystem, VERTEX_TEXMESH_2D vertex ) {
	if( renderSystem->vertex_count < RENDER_SYSTEM__MAX_VERTEX_COUNT ) {
		renderSystem->vertex[ renderSystem->vertex_count++ ] = vertex;
	} else {
		CONSOLE_STRING( "ERROR. VertexBuffer is full. Unable to add vertex to RenderSystem." );
	}
}

internal void
beginTask( RENDER_SYSTEM * renderSystem, MESH_DRAW_TYPE drawType, uint32 texture_bufferID, vec4 modColor ) {
	Assert( !renderSystem->taskIsActive );
	Assert( renderSystem->object_count < RENDER_SYSTEM__MAX_OBJECT_COUNT );
	
	renderSystem->taskIsActive = true;
	
	RENDER_OBJECT tempObject = {};
	tempObject.drawType         = drawType;
	tempObject.vertex_index     = renderSystem->vertex_count;
	tempObject.texture_bufferID = texture_bufferID;
	tempObject.color            = modColor;
	renderSystem->tempObject = tempObject;
}

internal void
addQuad( RENDER_SYSTEM * renderSystem, rect vert_bound, rect tex_bound ) {
	Assert( renderSystem->taskIsActive );
	
	VERTEX_TEXMESH_2D vertexA = VertexTexMesh2D( vert_bound.left,  vert_bound.bottom,   tex_bound.left,  tex_bound.bottom );
	VERTEX_TEXMESH_2D vertexB = VertexTexMesh2D( vert_bound.right, vert_bound.bottom,   tex_bound.right, tex_bound.bottom );
	VERTEX_TEXMESH_2D vertexC = VertexTexMesh2D( vert_bound.right, vert_bound.top,      tex_bound.right, tex_bound.top );
	VERTEX_TEXMESH_2D vertexD = VertexTexMesh2D( vert_bound.left,  vert_bound.top,      tex_bound.left,  tex_bound.top );
	
	switch( renderSystem->tempObject.drawType ) {
		case meshDrawType_triangles: {
			Assert( ( renderSystem->vertex_count + 6 ) < RENDER_SYSTEM__MAX_VERTEX_COUNT );
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexA;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexB;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexC;
			
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexA;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexC;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexD;
			
			renderSystem->tempObject.vertex_count += 6;
		} break;
		
		case meshDrawType_lineLoop:
		case meshDrawType_triangleFan: {
			Assert( ( renderSystem->vertex_count + 4 ) < RENDER_SYSTEM__MAX_VERTEX_COUNT );
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexA;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexB;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexC;
			renderSystem->vertex[ renderSystem->vertex_count++ ] = vertexD;
			
			renderSystem->tempObject.vertex_count += 4;
		} break;
		
		default: {
			InvalidCodePath;
		};
	}
}

internal void
endTask( RENDER_SYSTEM * renderSystem ) {
	Assert( renderSystem->taskIsActive );
	
	renderSystem->taskIsActive = false;
	if( renderSystem->tempObject.vertex_count > 0 ) {
		renderSystem->object[ renderSystem->object_count++ ] = renderSystem->tempObject;
	}
}

#endif 	// STD_INCLUDE_FUNC