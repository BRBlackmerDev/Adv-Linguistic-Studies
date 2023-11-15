
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

#include <android/input.h>
#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android_native_app_glue.c>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "Log.cpp"

// #define STB_TRUETYPE_IMPLEMENTATION
// #include "stb_truetype.h"

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
#include "opengl.cpp"
#include "debug_android.cpp"
#include "mesh.cpp"
#include "renderSystem.cpp"
#include "assetSystem.cpp"
#include "draw.cpp"
#include "android_platform.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "vector.cpp"
#include "vertex.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#include "renderSystem.cpp"
#include "opengl.cpp"
#include "assetSystem.cpp"
#include "draw.cpp"
#include "debug_android.cpp"

#include "advlinguisticstudies.h"
#include "advlinguisticstudies_tools.cpp"
#include "advlinguisticstudies.cpp"
#include "defaultMonoFont.cpp"

#include "android_platform.cpp"
#undef	STD_INCLUDE_FUNC