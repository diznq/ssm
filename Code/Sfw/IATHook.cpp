#include "StdAfx.h"
#include <windows.h>

// Library/Hook headers
#include "IATHook.h"
namespace sfw {
static void *RVA( void *base, size_t offset )
{
	return static_cast<unsigned char*>( base ) + offset;
}

static const void *RVA( const void *base, size_t offset )
{
	return static_cast<const unsigned char*>( base ) + offset;
}

/**
 * @brief Obtains address of IAT directory data in the specified module.
 * @param module The module handle.
 * @return Pointer to IAT directory data structure or NULL if some error occurred (invalid module or missing IAT).
 */
static const IMAGE_DATA_DIRECTORY *GetIATDirectoryData( const void *module )
{
	const IMAGE_DOS_HEADER *pDOSHeader = static_cast<const IMAGE_DOS_HEADER*>( module );
	if ( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return NULL;

	const IMAGE_NT_HEADERS *pPEHeader = static_cast<const IMAGE_NT_HEADERS*>( RVA( module, pDOSHeader->e_lfanew ) );
	if ( pPEHeader->Signature != IMAGE_NT_SIGNATURE )
		return NULL;

#ifdef WIN64
	const WORD optionalHeaderSignature = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
#else
	const WORD optionalHeaderSignature = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
#endif

	if ( pPEHeader->OptionalHeader.Magic != optionalHeaderSignature )
		return NULL;

	if ( pPEHeader->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_IAT )
		return NULL;

	const IMAGE_DATA_DIRECTORY *pIATData = &pPEHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
	if ( pIATData->VirtualAddress == 0 || pIATData->Size == 0 )
		return NULL;

	return pIATData;
}

/**
 * @brief Sets value of one entry in IAT (performs IAT hook).
 * @param pEntry Pointer to the IAT entry.
 * @param pNewFunc Address of the new function.
 * @return 0 if no error occurred, otherwise -1.
 */
static int ReplaceIATEntry( void **pEntry, void *pNewFunc )
{
	DWORD oldProtection;

	if ( VirtualProtect( pEntry, sizeof (void*), PAGE_EXECUTE_READWRITE, &oldProtection ) == 0 )
		return -1;

	(*pEntry) = pNewFunc;

	if ( VirtualProtect( pEntry, sizeof (void*), oldProtection, &oldProtection ) == 0 )
		return -1;

	return 0;
}

int IATHookByAddress( void *module, const void *pFunc, void *pNewFunc )
{
	const IMAGE_DATA_DIRECTORY *pIATData = GetIATDirectoryData( module );
	if ( pIATData == NULL )
	{
		SetLastError( ERROR_INVALID_MODULETYPE );
		return -1;
	}

	// the import address table
	void **pIAT = static_cast<void**>( RVA( module, pIATData->VirtualAddress ) );

	// number of functions in the table
	const size_t entryCount = pIATData->Size / sizeof (void*);

	bool found = false;
	for ( size_t i = 0; i < entryCount; i++ )
	{
		if ( pIAT[i] == pFunc )
		{
			found = true;

			// hook the function
			if ( ReplaceIATEntry( &pIAT[i], pNewFunc ) < 0 )
			{
				return -1;
			}
		}
	}

	if ( ! found )
	{
		SetLastError( ERROR_PROC_NOT_FOUND );
		return -1;
	}

	return 0;
}

}