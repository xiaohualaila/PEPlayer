//
// Created by SVR00003 on 2017/9/16.
//

#ifndef XRSDK_XRAPICONFIG_H
#define XRSDK_XRAPICONFIG_H

#if defined( XRAPI_ENABLE_EXPORT )
#define XRAPI_EXPORT __attribute__( ( __visibility__( "default" ) ) )
#else
#define XRAPI_EXPORT
#endif

#define XRAPI_DEPRECATED __attribute__( ( deprecated ) )

#if defined( __x86_64__ ) || defined( __aarch64__ ) || defined( _WIN64 )
#define XRAPI_64_BIT
#else
#define XRAPI_32_BIT
#endif

#if defined( __cplusplus ) && __cplusplus >= 201103L
	#define XRAPI_STATIC_ASSERT( exp )					static_assert( exp, #exp )
#endif

#if !defined( XRAPI_STATIC_ASSERT ) && defined( __clang__ )
	#if __has_feature( cxx_static_assert ) || __has_extension( cxx_static_assert )
		#define XRAPI_STATIC_ASSERT( exp )				static_assert( exp )
	#endif
#endif

#if !defined( XRAPI_STATIC_ASSERT )
	#if defined( __COUNTER__ )
		#define XRAPI_STATIC_ASSERT( exp )				XRAPI_STATIC_ASSERT_ID( exp, __COUNTER__ )
	#else
		#define XRAPI_STATIC_ASSERT( exp )				XRAPI_STATIC_ASSERT_ID( exp, __LINE__ )
	#endif
	#define XRAPI_STATIC_ASSERT_ID( exp, id )			XRAPI_STATIC_ASSERT_ID_EXPANDED( exp, id )
	#define XRAPI_STATIC_ASSERT_ID_EXPANDED( exp, id )	typedef char assert_failed_##id[(exp) ? 1 : -1]
#endif

#if defined( __COUNTER__ )
	#define XRAPI_PADDING( bytes )						XRAPI_PADDING_ID( bytes, __COUNTER__ )
#else
	#define XRAPI_PADDING( bytes )						XRAPI_PADDING_ID( bytes, __LINE__ )
#endif
#define XRAPI_PADDING_ID( bytes, id )					XRAPI_PADDING_ID_EXPANDED( bytes, id )
#define XRAPI_PADDING_ID_EXPANDED( bytes, id )			unsigned char dead##id[(bytes)]

#define XRAPI_ASSERT_TYPE_SIZE( type, bytes	)			XRAPI_STATIC_ASSERT( sizeof( type ) == (bytes) )

#if defined( XRAPI_64_BIT )
	#define XRAPI_PADDING_32_BIT( bytes )
	#if defined( __COUNTER__ )
		#define XRAPI_PADDING_64_BIT( bytes )				XRAPI_PADDING_ID( bytes, __COUNTER__ )
	#else
		#define XRAPI_PADDING_64_BIT( bytes )				XRAPI_PADDING_ID( bytes, __LINE__ )
	#endif
	#define XRAPI_ASSERT_TYPE_SIZE_32_BIT( type, bytes	)
	#define XRAPI_ASSERT_TYPE_SIZE_64_BIT( type, bytes	)	XRAPI_STATIC_ASSERT( sizeof( type ) == (bytes) )
#else
	#define XRAPI_ASSERT_TYPE_SIZE( type, bytes )			XRAPI_STATIC_ASSERT( sizeof( type ) == (bytes) )
	#if defined( __COUNTER__ )
		#define XRAPI_PADDING_32_BIT( bytes )				XRAPI_PADDING_ID( bytes, __COUNTER__ )
	#else
		#define XRAPI_PADDING_32_BIT( bytes )				XRAPI_PADDING_ID( bytes, __LINE__ )
	#endif
	#define XRAPI_PADDING_64_BIT( bytes )
	#define XRAPI_ASSERT_TYPE_SIZE_32_BIT( type, bytes	)	XRAPI_STATIC_ASSERT( sizeof( type ) == (bytes) )
	#define XRAPI_ASSERT_TYPE_SIZE_64_BIT( type, bytes	)
#endif

#endif //XRSDK_XRAPICONFIG_H
