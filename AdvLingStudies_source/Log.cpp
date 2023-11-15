
class Log {
	public:
		static void error( const char* pMessage, ... );
		static void warn( const char* pMessage, ... );
		static void info( const char* pMessage, ... );
		static void debug( const char* pMessage, ... );
};

#ifndef NDEBUG
	#define packt_Log_debug(...) Log::debug(__VA_ARGS__)
#else
	#define packt_Log_debug(...)
#endif

void Log::info( const char * pMessage, ... ) {
	va_list varArgs;
	va_start( varArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_INFO, "PACKT", pMessage, varArgs );
	__android_log_print( ANDROID_LOG_INFO, "PACKT", "\n");
	va_end( varArgs );
}

void Log::error( const char * pMessage, ... ) {
	va_list varArgs;
	va_start( varArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_ERROR, "PACKT", pMessage, varArgs );
	__android_log_print( ANDROID_LOG_ERROR, "PACKT", "\n");
	va_end( varArgs );
}

void Log::warn( const char * pMessage, ... ) {
	va_list varArgs;
	va_start( varArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_WARN, "PACKT", pMessage, varArgs );
	__android_log_print( ANDROID_LOG_WARN, "PACKT", "\n");
	va_end( varArgs );
}

void Log::debug( const char * pMessage, ... ) {
	va_list varArgs;
	va_start( varArgs, pMessage );
	__android_log_vprint( ANDROID_LOG_DEBUG, "PACKT", pMessage, varArgs );
	__android_log_print( ANDROID_LOG_DEBUG, "PACKT", "\n");
	va_end( varArgs );
}