#ifndef _SSHLWAPI_H_
#define _SSHLWAPI_H_
#include <shlwapi.h>
/*shlwapi.h 学习文档*/

//一、Desktop Family
//(一)、字符串
/*
	1、	字符串查找, 在字符串中搜索与指定字符匹配的第一个字符
		->(区分大小写)
			StrChrA(_In_ PCSTR pszStart, WORD wMatch);
			StrChrW
			StrCmpNCA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2, int nChar);//使用C运行时库
			StrCmpNCW
		->(不区分大小写)
			StrChrIA(_In_ PCSTR pszStart, WORD wMatch);
			StrChrIW
			StrCmpNICA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2, int nChar);
			StrCmpNICW
		->(区分大小写), cchMax:搜索的最大字符数
			StrChrNW(_In_ PCWSTR pszStart, WCHAR wMatch, UINT cchMax);
			StrChrNIW	
		->逆序查找,区分大小写
			StrRChrA(_In_ PCSTR pszStart, _In_opt_ PCSTR pszEnd, WORD wMatch);
			StrRChrW
		->逆序查找,不区分大小写
			StrRChrIA(_In_ PCSTR pszStart, _In_opt_ PCSTR pszEnd, WORD wMatch);
			StrRChrIW
	2、	字符比较
		->不区分大小写
			ChrCmpIA(WORD w1, WORD w2);
			ChrCmpIW
	3、	字符串比较, 
		->(区分大小写)比较两个字符串开头的指定数量的字符
			StrCmpNA(_In_ PCSTR psz1, _In_ PCSTR psz2, int nChar);
			StrCmpNW
		->(不区分大小写)比较两个字符串开头的指定数量的字符
			StrCmpNIA(_In_ PCSTR psz1, _In_ PCSTR psz2, int nChar);、
			StrCmpNIW
		->自行确认是否区分大小写
			StrIsIntlEqualA(BOOL fCaseSens, _In_ PCSTR pszString1, _In_ PCSTR pszString2, int nChar);
			StrIsIntlEqualW
			IntlStrEqWorkerA(BOOL fCaseSens, LPCSTR lpString1, LPCSTR lpString2, int nChar);
			IntlStrEqWorkerW
		->完全比较,区分大小写
			StrCmpW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
			StrCmpCA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2);//使用C运行时库
			StrCmpCW
		->完全比较,不区分大小写
			StrCmpIW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
			StrCmpICA(_In_ LPCSTR pszStr1, _In_ LPCSTR pszStr2);//使用C运行时库
			StrCmpICW
		->unicode, 不区分大小写
			StrCmpLogicalW(_In_ PCWSTR psz1, _In_ PCWSTR psz2);
	4、	字符串查找, 在字符串中搜索第一次出现的字符串. 
		->区分大小写
			StrStrA(_In_ PCSTR pszFirst, _In_ PCSTR pszSrch);
			StrStrW
			StrStrNW(_In_ PCWSTR pszFirst, _In_ PCWSTR pszSrch, UINT cchMax);
		->不区分大小写
			StrStrIA(_In_ PCSTR pszFirst, _In_ PCSTR pszSrch);
			StrStrIW
			StrStrNIW(_In_ PCWSTR pszFirst, _In_ PCWSTR pszSrch, UINT cchMax);
		->(区分大小写, 包含'\0')
			StrCSpnA(_In_ PCSTR pszStr, _In_ PCSTR pszSet);
			StrCSpnW
		->(不区分大小写, 包含'\0')
				StrCSpnIA(_In_ PCSTR pszStr, _In_ PCSTR pszSet);
				StrCSpnIW
		->不包含'\0'
			StrPBrkA(_In_ PCSTR psz, _In_ PCSTR pszSet);
			StrPBrkW
		->逆序查找,不区分大小写
			StrRStrIA(_In_ PCSTR pszSource, _In_opt_ PCSTR pszLast, _In_ PCSTR pszSrch);
			StrRStrIW
	5、	复制字符串
		->返回新字符串
			StrDupA(_In_ PCSTR pszSrch);
			StrDupW
			SHLocalStrDup
			SHLocalStrDupA
			SHLocalStrDupW
		->字符串psz2复制到psz1
			StrCpyW(_Out_ PWSTR psz1, _In_ PCWSTR psz2);
		->字符串pszSrc中cchMax数量的字符复制到pszDst(不要使用此函数)
			StrCpyNW(_Out_writes_(cchMax) PWSTR pszDst, _In_ PCWSTR pszSrc, int cchMax);
		->深拷贝,使用CoTaskMemAlloc分配内存, 新的字符串需要手动使用CoTaskMemFree释放
			SHStrDup
			SHStrDupA(_In_ LPCSTR psz, LPWSTR *ppwsz);
			SHStrDupW
	6、	字符串追加Append
		->psz2的末尾追加psz2的cchMax个字符
			StrNCatA(_Inout_updates_(cchMax) PSTR psz1, PCSTR psz2, int cchMax);
			StrNCatW
		->完全追加Append
			StrCatW(_Inout_ PWSTR psz1, _In_ PCWSTR psz2);
		->复制并追加
			StrCatBuffA(PSTR pszDest, _In_ PCSTR pszSrc, int cchDestBuffSize);
			StrCatBuffW
	7、	字符串位置查找 返回字符串psz中第一个不在指定字符串pszSet中出现的字符下标
			StrSpnA(_In_ PCSTR psz, _In_ PCSTR pszSet);
			StrSpnW
	8、	连接两个字符串
			StrCatChainW(PWSTR pszDst, DWORD cchDst, DWORD ichAt, PCWSTR pszSrc);
	8、	格式化字符串
			wvnsprintfA(PSTR pszDest, _In_ int cchDest, _In_ PCSTR pszFmt, _In_ va_list arglist);
			wvnsprintfW
			wnsprintfA(PSTR pszDest, _In_ int cchDest, _In_  PCSTR pszFmt, ...);
			wnsprintfW
	8、	字节数(byte大小)转内存/存储大小的字符串(B,KB,MB,GB)
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
	9、	将以毫秒为单位指定的时间间隔转换为字符串
			StrFromTimeIntervalA
			StrFromTimeIntervalW
	10、字符串转int
		->十进制的字符串转int
				StrToIntA(_In_ PCSTR pszSrc);
			StrToIntW
		->十进制或十六进制数的字符串转换为整数
			StrToIntExA(_In_ PCSTR pszString, STIF_FLAGS dwFlags, _Out_ int * piRet);
			StrToIntExW
			StrToInt64ExA(_In_ PCSTR pszString, STIF_FLAGS dwFlags, _Out_ LONGLONG * pllRet);
			StrToInt64ExW
	11、字符串移除指定的字符
			StrTrimA(_Inout_ PSTR psz, _In_ PCSTR pszTrimChars);
			StrTrimW
	12、字符判断
		->字符是否是空格
			IsCharSpace
			IsCharSpaceA(CHAR wch);
			IsCharSpaceW
*/

//(二)、参见IShellFolder::GetDisplayNameOf
/*
	1、		
			StrRetToStr
			StrRetToStrA(STRRET *pstr, PCUITEMID_CHILD pidl, LPSTR *ppsz);
			StrRetToStrW
			StrRetToBuf
			StrRetToBufA(STRRET *pstr, PCUITEMID_CHILD pidl,LPSTR pszBuf, UINT cchBuf);
			StrRetToBufW
			StrRetToBSTR
*/

//(三)、文件、路径
/*
	1、
		->路径添加反斜杠
			PathAddBackslash
			PathAddBackslashA(LPSTR pszPath);
			PathAddBackslashW
		->添加文件扩展名
			PathAddExtension
			PathAddExtensionA(LPSTR pszPath, LPCSTR pszExt);
			PathAddExtensionW
		->路径附加到另一路径的末尾
			PathAppendA(LPSTR pszPath, LPCSTR pszMore);
			PathAppendW
		->通过给定驱动器号返回驱动器根目录
			PathBuildRoot
			PathBuildRootA(LPSTR pszRoot, int iDrive);//iDrive:[0,25]
			PathBuildRootW
		->返回规范路径('.'/'..'等类似的文件, 返回真实的路径)
			PathCanonicalizeA(LPSTR pszBuf, LPCSTR pszPath);
			PathCanonicalizeW
		->路径拼接
			PathCombine
			PathCombineA(LPSTR pszDest, LPCSTR pszDir, LPCSTR pszFile);
			PathCombineW
		->路径变更为缩略路径 "C:\\path1\\path2\\sample.txt" -> "C:\path1\...\sample.txt"
			PathCompactPathA(HDC hDC, LPSTR pszPath, UINT dx);
			PathCompactPathW
			PathCompactPathExA(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);\
			PathCompactPathExW
		->比较两个路径, 确定是否有相同的前缀(有相同的父文件夹?)
			PathCommonPrefixA(LPCSTR pszFile1, LPCSTR pszFile2, LPSTR achPath);
			PathCommonPrefixW
		->文件是否存在
			PathFileExists
			PathFileExistsA(LPCSTR pszPath);
			PathFileExistsW
		->文件扩展名(文件格式)
			PathFindExtension
			PathFindExtensionA(LPCSTR pszPath);
			PathFindExtensionW
			PathIsContentTypeA(_In_ LPCSTR pszPath, _In_ LPCSTR pszContentType);
			PathIsContentTypeW
		->返回文件名
			PathFindFileName
			PathFindFileNameA(LPCSTR pszPath);
			PathFindFileNameW
		->解析路径并返回该路径中第一个反斜杠后面的部分
			PathFindNextComponent
			PathFindNextComponentA(LPCSTR pszPath);
			PathFindNextComponentW
		->ppszOtherDirs等一堆路径中是否存在pszPath文件(吐槽:Windows的API 94这么难用)
			PathFindOnPathA(LPSTR pszPath, PZPCSTR ppszOtherDirs);
			PathFindOnPathW
		->确定给定文件名是否是以下后缀之一
			PathFindSuffixArray
			PathFindSuffixArrayA(LPCSTR pszPath, const LPCSTR *apszSuffix, int iArraySize);
			PathFindSuffixArrayW
		->解析命令行(查找给定路径中的命令行参数)
			PathGetArgs
			PathGetArgsA(LPCSTR pszPath);	//"test.exe temp.txt sample.doc" -> "temp.txt sample.doc"
			PathGetArgsW					//"test.exe 1 2 3" -> "1 2 3"
		->解析文件名是否是长格式
			PathIsLFNFileSpec
			PathIsLFNFileSpecA(LPCSTR pszName);
			PathIsLFNFileSpecW
		->确定与路径相关的字符类型(不要用)
			PathGetCharTypeA(UCHAR ch);
			PathGetCharTypeW
		->获取驱动器号. 在路径中搜索"A"到"Z"范围内的驱动器号, 并返回相应的驱动器号
			PathGetDriveNumber
			PathGetDriveNumberA(_In_ LPCSTR pszPath);//"E:\\2022-myl" -> 4
			PathGetDriveNumberW
		->路径是否是文件夹
			PathIsDirectory
			PathIsDirectoryA(_In_ LPCSTR pszPath);
			PathIsDirectoryW
		->文件夹是否为空
			PathIsDirectoryEmpty
			PathIsDirectoryEmptyA(_In_ LPCSTR pszPath);
			PathIsDirectoryEmptyW
		->在路径中搜索任何路径分隔字符如:":"或" "
			PathIsFileSpec
			PathIsFileSpecA(_In_ LPCSTR pszPath);
			PathIsFileSpecW
		->查路径以确定它是否以pszPrefix传递的前缀值开头
			PathIsPrefix
			PathIsPrefixA(_In_ LPCSTR pszPrefix, _In_ LPCSTR pszPath);
			PathIsPrefixW
		->是否是相对路径
			PathIsRelative
			PathIsRelativeA(_In_ LPCSTR pszPath);
			PathIsRelativeW
		->是否是根目录
			PathIsRoot
			PathIsRootA(_In_ LPCSTR pszPath);
			PathIsRootW
		->是否是有效的Universal Naming Convention(UNC)路径, 而不是基于驱动器的路径
			PathIsUNC
			PathIsUNCA(_In_ LPCSTR pszPath);
			PathIsUNCW
			PathIsUNCServer
			PathIsUNCServerA(_In_ LPCSTR pszPath);
			PathIsUNCServerW
			PathIsUNCServerShare
			PathIsUNCServerShareA(_In_ LPCSTR pszPath);
			PathIsUNCServerShareW
		->是否是网络路径/网络资源
			PathIsNetworkPath
			PathIsNetworkPathA(_In_ LPCSTR pszPath);
			PathIsNetworkPathW
		->URL
			PathIsURL
			PathIsURLA(_In_ LPCSTR pszPath);
			PathIsURLW
		->路径大小写
			PathMakePrettyA//全大写路径转换为全小写字符，以使路径具有一致的外观
			PathMakePrettyW
		->使用Microsoft MS-DOS通配符匹配类型搜索字符串
			PathMatchSpecA
			PathMatchSpecW
		->根据一个或多个文件名模式匹配路径中的文件名
			PathMatchSpecExA
			PathMatchSpecExW
*/	


//(四)URL
/*

*/

/*
二、Desktop or OneCore Family
	1、SHLoadIndirectString(PWSTR pszOutBuf, UINT cchOutBuf, void **ppvReserved);
*/


#endif