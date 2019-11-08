#ifndef __ENGINE_DEFINE_H__

#define BEGIN(NAME)		\
namespace NAME			\
{

#define END				\
}

#define USING(NAME)	\
using namespace NAME;

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MIN_STR 64
#define MID_STR 128
#define MAX_STR 256

#define ERR_MSG(message)	\
::MessageBox(nullptr, message, L"System Error!", MB_OK)

#define NULL_CHECK(ptr) \
if(nullptr == (ptr)) { __asm{int 3}; return; }

#define NULL_CHECK_MSG(ptr, message) \
if(nullptr == (ptr)) { ERR_MSG(message); __asm{int 3}; return; }

#define NULL_CHECK_RETURN(ptr, return_val) \
if(nullptr == (ptr)) { __asm{int 3}; return return_val; }

#define NULL_CHECK_MSG_RETURN(ptr, message, return_val) \
if(nullptr == (ptr)) { ERR_MSG(message); __asm{int 3}; return return_val;}

#define FAILED_CHECK(hr) \
if(FAILED(hr)) { __asm{int 3}; return; }

#define FAILED_CHECK_MSG(hr, message) \
if(FAILED(hr)) { ERR_MSG(message); __asm{int 3}; return; }

#define FAILED_CHECK_RETURN(hr, return_val) \
if(FAILED(hr)) { __asm{int 3}; return return_val; }

#define FAILED_CHECK_MSG_RETURN(hr, message, return_val) \
if(FAILED(hr)) { ERR_MSG(message); __asm{int 3}; return return_val;}

#define NO_COPY(ClassName)						\
private:										\
	ClassName(const ClassName& _Obj);			\
	ClassName& operator=(const ClassName& _Obj);

#define DECLARE_SINGLETON(ClassName)			\
		NO_COPY(ClassName)						\
public:											\
	static ClassName* GetInstance()				\
	{											\
		if(nullptr == m_pInstance)				\
			m_pInstance = new ClassName;		\
		return m_pInstance;						\
	}											\
	void DestroyInstance()						\
	{											\
		if(m_pInstance)							\
		{										\
			delete m_pInstance;					\
			m_pInstance = nullptr;				\
		}										\
	}											\
private:										\
	static ClassName*	m_pInstance;			

#define IMPLEMENT_SINGLETON(ClassName)			\
ClassName* ClassName::m_pInstance = nullptr;

#define __ENGINE_DEFINE_H__
#endif