#pragma once

#if !defined(__alignas_is_defined) || __alignas_is_defined != 1
	#undef __alignas_is_defined
	#undef alignas
	#define alignas _Alignas
#endif

#if !defined(__alignof_is_defined) || __alignof_is_defined != 1
	#undef __alignof_is_defined
	#undef alignof
	#define alignof _Alignof
#endif
