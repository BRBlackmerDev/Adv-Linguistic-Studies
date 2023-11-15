
#ifdef	STD_INCLUDE_DECL

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// shader functions
//----------

struct CREATE_SHADER_RESULT {
	boo32 isValid;
	GLuint program;
};

internal CREATE_SHADER_RESULT
createShader( const char * shader_vertexCode, const char * shader_fragmentCode ) {
	CREATE_SHADER_RESULT result;
	
	int32 string_length = 0;
	char buffer[ 1024 ] = {};
	
	result.program = glCreateProgram();
	if( !result.program ) {
		CONSOLE_STRING( "ERROR: Unable to create OpenGL program" );								 
		InvalidCodePath;
	}
	
	GLuint shader_vertex = glCreateShader( GL_VERTEX_SHADER );
	if( shader_vertex ) {
		glShaderSource( shader_vertex, 1, ( const GLchar ** )&shader_vertexCode, 0 );
		glCompileShader( shader_vertex );
		
		GLint shader_status = 0;
		glGetShaderiv( shader_vertex, GL_COMPILE_STATUS, &shader_status );
		if( shader_status != GL_TRUE ) {
			glGetShaderInfoLog( shader_vertex, 1024, &string_length, buffer );
			CONSOLE_STRING( "ERROR: Unable to compile vertex shader" );
			CONSOLE_STRING( buffer );
			InvalidCodePath;
		} else {
			glAttachShader( result.program, shader_vertex );
		}
	} else {
		InvalidCodePath;
	}
	
	GLuint shader_fragment = glCreateShader( GL_FRAGMENT_SHADER );
	if( shader_fragment ) {
		glShaderSource( shader_fragment, 1, ( const GLchar ** )&shader_fragmentCode, 0 );
		glCompileShader( shader_fragment );
		
		GLint shader_status = 0;
		glGetShaderiv( shader_fragment, GL_COMPILE_STATUS, &shader_status );
		if( shader_status != GL_TRUE ) {
			glGetShaderInfoLog( shader_fragment, 1024, &string_length, buffer );
			CONSOLE_STRING( "ERROR: Unable to compile fragment shader" );
			CONSOLE_STRING( buffer );
			InvalidCodePath;
		} else {
			glAttachShader( result.program, shader_fragment );
		}
	} else {
		InvalidCodePath;
	}
	
	glLinkProgram( result.program );
	GLint shader_programStatus = 0;
	glGetProgramiv( result.program, GL_LINK_STATUS, &shader_programStatus );
	if( shader_programStatus == GL_TRUE ) {
		result.isValid = true;
		glDeleteShader( shader_vertex );
		glDeleteShader( shader_fragment );
	} else {
		glGetProgramInfoLog( result.program, 1024, &string_length, buffer );
		CONSOLE_STRING( "ERROR: Unable to link shader program" );
		CONSOLE_STRING( buffer );
		InvalidCodePath;
	}
	
	return result;
}

#endif	// STD_INCLUDE_FUNC