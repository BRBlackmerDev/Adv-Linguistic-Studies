
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // type definitions
#include <stdlib.h> // rand
#include <time.h> // time
#include <cstdio> // sprintf
#include <intrin.h>
#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "assetID_generated.h"
#include "advlinguisticstudies_assetID.h"
#include "typeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "vector.cpp"
#include "vertex.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#include "opengl_extensions.cpp"
#include "mesh.cpp"
#include "renderSystem.cpp"
#include "assetSystem.cpp"
#include "draw.cpp"
#include "debug_win32.cpp"
#include "profile.cpp"
#include "win32_platform.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "vector.cpp"
#include "vertex.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#include "opengl_extensions.cpp"
#include "profile.cpp"
#include "renderSystem.cpp"
#include "opengl.cpp"
#include "mesh.cpp"
#include "assetSystem.cpp"
#include "draw.cpp"
#include "debug_win32.cpp"

#include "advlinguisticstudies.h"
#include "advlinguisticstudies_tools.cpp"

#include "advlinguisticstudies.cpp"
#include "defaultMonoFont.cpp"

#include "win32_platform.cpp"
#undef	STD_INCLUDE_FUNC