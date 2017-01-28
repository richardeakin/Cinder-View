
#if defined( CINDER_DLL )
	#if defined( CINDER_UI_EXPORTS )
		#define CI_UI_API __declspec(dllexport)
	#else
		#define CI_UI_API __declspec(dllimport)
	#endif
	// "needs to have dll-interface to be used by clients of class" warning
	//#pragma warning (disable: 4251)
	// "non dll-interface class 'std::exception' used as base for dll-interface class" (Mostly for cinder::Exception)
	//#pragma warning (disable: 4275)
#else
	#define CI_UI_API
#endif