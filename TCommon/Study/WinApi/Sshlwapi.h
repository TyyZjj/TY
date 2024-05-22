#ifndef _SSHLWAPI_H_
#define _SSHLWAPI_H_
#include <shlwapi.h>
/*shlwapi.h ѧϰ�ĵ�*/

//һ��Desktop Family
//(һ)���ַ���
/*
	1��	�ַ�������, ���ַ�����������ָ���ַ�ƥ��ĵ�һ���ַ�
		->(���ִ�Сд)
			StrChrA(_In_ PCSTR pszStart, WORD wMatch);
			StrChrW
			StrCmpNCA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2, int nChar);//ʹ��C����ʱ��
			StrCmpNCW
		->(�����ִ�Сд)
			StrChrIA(_In_ PCSTR pszStart, WORD wMatch);
			StrChrIW
			StrCmpNICA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2, int nChar);
			StrCmpNICW
		->(���ִ�Сд), cchMax:����������ַ���
			StrChrNW(_In_ PCWSTR pszStart, WCHAR wMatch, UINT cchMax);
			StrChrNIW	
		->�������,���ִ�Сд
			StrRChrA(_In_ PCSTR pszStart, _In_opt_ PCSTR pszEnd, WORD wMatch);
			StrRChrW
		->�������,�����ִ�Сд
			StrRChrIA(_In_ PCSTR pszStart, _In_opt_ PCSTR pszEnd, WORD wMatch);
			StrRChrIW
	2��	�ַ��Ƚ�
		->�����ִ�Сд
			ChrCmpIA(WORD w1, WORD w2);
			ChrCmpIW
	3��	�ַ����Ƚ�, 
		->(���ִ�Сд)�Ƚ������ַ�����ͷ��ָ���������ַ�
			StrCmpNA(_In_ PCSTR psz1, _In_ PCSTR psz2, int nChar);
			StrCmpNW
		->(�����ִ�Сд)�Ƚ������ַ�����ͷ��ָ���������ַ�
			StrCmpNIA(_In_ PCSTR psz1, _In_ PCSTR psz2, int nChar);��
			StrCmpNIW
		->����ȷ���Ƿ����ִ�Сд
			StrIsIntlEqualA(BOOL fCaseSens, _In_ PCSTR pszString1, _In_ PCSTR pszString2, int nChar);
			StrIsIntlEqualW
			IntlStrEqWorkerA(BOOL fCaseSens, LPCSTR lpString1, LPCSTR lpString2, int nChar);
			IntlStrEqWorkerW
		->��ȫ�Ƚ�,���ִ�Сд
			StrCmpW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
			StrCmpCA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2);//ʹ��C����ʱ��
			StrCmpCW
		->��ȫ�Ƚ�,�����ִ�Сд
			StrCmpIW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
			StrCmpICA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2);//ʹ��C����ʱ��
			StrCmpICW
		->unicode, �����ִ�Сд
			StrCmpLogicalW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
	4��	�ַ�������, ���ַ�����������һ�γ��ֵ��ַ���. 
		->���ִ�Сд
			StrStrA(_In_ PCSTR pszFirst, _In_ PCSTR pszSrch);
			StrStrW
			StrStrNW(_In_ PCWSTR pszFirst, _In_ PCWSTR pszSrch, UINT cchMax);
		->�����ִ�Сд
			StrStrIA(_In_ PCSTR pszFirst, _In_ PCSTR pszSrch);
			StrStrIW
			StrStrNIW(_In_ PCWSTR pszFirst, _In_ PCWSTR pszSrch, UINT cchMax);
		->(���ִ�Сд, ����'\0')
			StrCSpnA(_In_ PCSTR pszStr, _In_ PCSTR pszSet);
			StrCSpnW
		->(�����ִ�Сд, ����'\0')
				StrCSpnIA(_In_ PCSTR pszStr, _In_ PCSTR pszSet);
				StrCSpnIW
		->������'\0'
			StrPBrkA(_In_ PCSTR psz, _In_ PCSTR pszSet);
			StrPBrkW
		->�������,�����ִ�Сд
			StrRStrIA(_In_ PCSTR pszSource, _In_opt_ PCSTR pszLast, _In_ PCSTR pszSrch);
			StrRStrIW
	5��	�����ַ���
		->�������ַ���
			StrDupA(_In_ PCSTR pszSrch);
			StrDupW
			SHLocalStrDup
			SHLocalStrDupA
			SHLocalStrDupW
		->�ַ���psz2���Ƶ�psz1
			StrCpyW(_Out_ PWSTR psz1, _In_ PCWSTR psz2);
		->�ַ���pszSrc��cchMax�������ַ����Ƶ�pszDst(��Ҫʹ�ô˺���)
			StrCpyNW(_Out_writes_(cchMax) PWSTR pszDst, _In_ PCWSTR pszSrc, int cchMax);
		->���,ʹ��CoTaskMemAlloc�����ڴ�, �µ��ַ�����Ҫ�ֶ�ʹ��CoTaskMemFree�ͷ�
			SHStrDup
			SHStrDupA(_In_ LPCSTR psz, LPWSTR *ppwsz);
			SHStrDupW
	6��	�ַ���׷��Append
		->psz2��ĩβ׷��psz2��cchMax���ַ�
			StrNCatA(_Inout_updates_(cchMax) PSTR psz1, PCSTR psz2, int cchMax);
			StrNCatW
		->��ȫ׷��Append
			StrCatW(_Inout_ PWSTR psz1, _In_ PCWSTR psz2);
		->���Ʋ�׷��
			StrCatBuffA(PSTR pszDest, _In_ PCSTR pszSrc, int cchDestBuffSize);
			StrCatBuffW
	7��	�ַ���λ�ò��� �����ַ���psz�е�һ������ָ���ַ���pszSet�г��ֵ��ַ��±�
			StrSpnA(_In_ PCSTR psz, _In_ PCSTR pszSet);
			StrSpnW
	8��	���������ַ���
			StrCatChainW(PWSTR pszDst, DWORD cchDst, DWORD ichAt, PCWSTR pszSrc);
	8��	��ʽ���ַ���
			wvnsprintfA(PSTR pszDest, _In_ int cchDest, _In_ PCSTR pszFmt, _In_ va_list arglist);
			wvnsprintfW
			wnsprintfA(PSTR pszDest, _In_ int cchDest, _In_  PCSTR pszFmt, ...);
			wnsprintfW
	8��	�ֽ���(byte��С)ת�ڴ�/�洢��С���ַ���(B,KB,MB,GB)
			StrFormatByteSizeEx(ULONGLONG ull, 
				SFBS_FLAGS flags, 
				_Out_writes_(cchBuf) PWSTR pszBuf, 
				_In_range_(>,0) UINT cchBuf);
			enum tagSFBS_FLAGS
			{
				SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT     = 0x0001,   // round to the nearest displayed digit.	2147483647 -> "2.00 GB"
				SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS  = 0x0002,   // discard undisplayed digits.				2147483647 -> "1.99 GB"
			};
			StrFormatByteSizeA
			StrFormatByteSize64A
			StrFormatByteSizeW
			StrFormatKBSizeW
			StrFormatKBSizeA
	9��	���Ժ���Ϊ��λָ����ʱ����ת��Ϊ�ַ���
			StrFromTimeIntervalA
			StrFromTimeIntervalW
	10���ַ���תint
		->ʮ���Ƶ��ַ���תint
				StrToIntA(_In_ PCSTR pszSrc);
			StrToIntW
		->ʮ���ƻ�ʮ�����������ַ���ת��Ϊ����
			StrToIntExA(_In_ PCSTR pszString, STIF_FLAGS dwFlags, _Out_ int * piRet);
			StrToIntExW
			StrToInt64ExA(_In_ PCSTR pszString, STIF_FLAGS dwFlags, _Out_ LONGLONG * pllRet);
			StrToInt64ExW
	11���ַ����Ƴ�ָ�����ַ�
			StrTrimA(_Inout_ PSTR psz, _In_ PCSTR pszTrimChars);
			StrTrimW
	12���ַ��ж�
		->�ַ��Ƿ��ǿո�
			IsCharSpace
			IsCharSpaceA(CHAR wch);
			IsCharSpaceW
*/

//(��)���μ�IShellFolder::GetDisplayNameOf
/*
	1��		
			StrRetToStr
			StrRetToStrA(STRRET *pstr, PCUITEMID_CHILD pidl, LPSTR *ppsz);
			StrRetToStrW
			StrRetToBuf
			StrRetToBufA(STRRET *pstr, PCUITEMID_CHILD pidl,LPSTR pszBuf, UINT cchBuf);
			StrRetToBufW
			StrRetToBSTR
*/

//(��)���ļ���·��
/*
	1��
		->·����ӷ�б��
			PathAddBackslash
			PathAddBackslashA(LPSTR pszPath);
			PathAddBackslashW
		->����ļ���չ��
			PathAddExtension
			PathAddExtensionA(LPSTR pszPath, LPCSTR pszExt);
			PathAddExtensionW
		->·�����ӵ���һ·����ĩβ
			PathAppendA(LPSTR pszPath, LPCSTR pszMore);
			PathAppendW
		->ͨ�������������ŷ�����������Ŀ¼
			PathBuildRoot
			PathBuildRootA(LPSTR pszRoot, int iDrive);//iDrive:[0,25]
			PathBuildRootW
		->���ع淶·��('.'/'..'�����Ƶ��ļ�, ������ʵ��·��)
			PathCanonicalizeA(LPSTR pszBuf, LPCSTR pszPath);
			PathCanonicalizeW
		->·��ƴ��
			PathCombine
			PathCombineA(LPSTR pszDest, LPCSTR pszDir, LPCSTR pszFile);
			PathCombineW
		->·�����Ϊ����·�� "C:\\path1\\path2\\sample.txt" -> "C:\path1\...\sample.txt"
			PathCompactPathA(HDC hDC, LPSTR pszPath, UINT dx);
			PathCompactPathW
			PathCompactPathExA(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);\
			PathCompactPathExW
		->�Ƚ�����·��, ȷ���Ƿ�����ͬ��ǰ׺(����ͬ�ĸ��ļ���?)
			PathCommonPrefixA(LPCSTR pszFile1, LPCSTR pszFile2, LPSTR achPath);
			PathCommonPrefixW
		->�ļ��Ƿ����
			PathFileExists
			PathFileExistsA(LPCSTR pszPath);
			PathFileExistsW
		->�ļ���չ��(�ļ���ʽ)
			PathFindExtension
			PathFindExtensionA(LPCSTR pszPath);
			PathFindExtensionW
			PathIsContentTypeA(_In_ LPCSTR pszPath, _In_ LPCSTR pszContentType);
			PathIsContentTypeW
		->�����ļ���
			PathFindFileName
			PathFindFileNameA(LPCSTR pszPath);
			PathFindFileNameW
		->����·�������ظ�·���е�һ����б�ܺ���Ĳ���
			PathFindNextComponent
			PathFindNextComponentA(LPCSTR pszPath);
			PathFindNextComponentW
		->ppszOtherDirs��һ��·�����Ƿ����pszPath�ļ�(�²�:Windows��API 94��ô����)
			PathFindOnPathA(LPSTR pszPath, PZPCSTR ppszOtherDirs);
			PathFindOnPathW
		->ȷ�������ļ����Ƿ������º�׺֮һ
			PathFindSuffixArray
			PathFindSuffixArrayA(LPCSTR pszPath, const LPCSTR *apszSuffix, int iArraySize);
			PathFindSuffixArrayW
		->����������(���Ҹ���·���е������в���)
			PathGetArgs
			PathGetArgsA(LPCSTR pszPath);	//"test.exe temp.txt sample.doc" -> "temp.txt sample.doc"
			PathGetArgsW					//"test.exe 1 2 3" -> "1 2 3"
		->�����ļ����Ƿ��ǳ���ʽ
			PathIsLFNFileSpec
			PathIsLFNFileSpecA(LPCSTR pszName);
			PathIsLFNFileSpecW
		->ȷ����·����ص��ַ�����(��Ҫ��)
			PathGetCharTypeA(UCHAR ch);
			PathGetCharTypeW
		->��ȡ��������. ��·��������"A"��"Z"��Χ�ڵ���������, ��������Ӧ����������
			PathGetDriveNumber
			PathGetDriveNumberA(_In_ LPCSTR pszPath);//"E:\\2022-myl" -> 4
			PathGetDriveNumberW
		->·���Ƿ����ļ���
			PathIsDirectory
			PathIsDirectoryA(_In_ LPCSTR pszPath);
			PathIsDirectoryW
		->�ļ����Ƿ�Ϊ��
			PathIsDirectoryEmpty
			PathIsDirectoryEmptyA(_In_ LPCSTR pszPath);
			PathIsDirectoryEmptyW
		->��·���������κ�·���ָ��ַ���:":"��" "
			PathIsFileSpec
			PathIsFileSpecA(_In_ LPCSTR pszPath);
			PathIsFileSpecW
		->��·����ȷ�����Ƿ���pszPrefix���ݵ�ǰ׺ֵ��ͷ
			PathIsPrefix
			PathIsPrefixA(_In_ LPCSTR pszPrefix, _In_ LPCSTR pszPath);
			PathIsPrefixW
		->�Ƿ������·��
			PathIsRelative
			PathIsRelativeA(_In_ LPCSTR pszPath);
			PathIsRelativeW
		->�Ƿ��Ǹ�Ŀ¼
			PathIsRoot
			PathIsRootA(_In_ LPCSTR pszPath);
			PathIsRootW
		->�Ƿ�����Ч��Universal Naming Convention(UNC)·��, �����ǻ�����������·��
			PathIsUNC
			PathIsUNCA(_In_ LPCSTR pszPath);
			PathIsUNCW
			PathIsUNCServer
			PathIsUNCServerA(_In_ LPCSTR pszPath);
			PathIsUNCServerW
			PathIsUNCServerShare
			PathIsUNCServerShareA(_In_ LPCSTR pszPath);
			PathIsUNCServerShareW
		->�Ƿ�������·��/������Դ
			PathIsNetworkPath
			PathIsNetworkPathA(_In_ LPCSTR pszPath);
			PathIsNetworkPathW
		->URL
			PathIsURL
			PathIsURLA(_In_ LPCSTR pszPath);
			PathIsURLW
		->·����Сд
			PathMakePrettyA//ȫ��д·��ת��ΪȫСд�ַ�����ʹ·������һ�µ����
			PathMakePrettyW
		->ʹ��Microsoft MS-DOSͨ���ƥ�����������ַ���
			PathMatchSpecA
			PathMatchSpecW
		->����һ�������ļ���ģʽƥ��·���е��ļ���
			PathMatchSpecExA
			PathMatchSpecExW
*/	


//(��)URL
/*

*/

/*
����Desktop or OneCore Family
	1��SHLoadIndirectString(PWSTR pszOutBuf, UINT cchOutBuf, void **ppvReserved);
*/


#endif