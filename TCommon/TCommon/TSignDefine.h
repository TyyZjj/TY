#ifndef _TSIGNDEFINE_H_
#define _TSIGNDEFINE_H_

#ifndef TSTR
#define TSTR(SIGN) QString(SIGN)
#endif

#ifndef SIGN_COMMA//逗号
#define SIGN_COMMA					','
#endif
#ifndef SIGN_POINT//点号
#define SIGN_POINT					'.'
#endif
#ifndef SIGN_SPACE//空格
#define SIGN_SPACE					' '
#endif
#ifndef SIGN_HORIZONTAL_BAR
#define SIGN_HORIZONTAL_BAR			'-'
#endif
#ifndef SIGN_ADD
#define SIGN_ADD					'+'
#endif
#ifndef SIGN_ASTERISK
#define	SIGN_ASTERISK				'*'
#endif
#ifndef SIGN_SLASH
#define SIGN_SLASH					'/'
#endif
#ifndef SIGN_BACKSLASH
#define	SIGN_BACKSLASH				'\\'
#endif
#ifndef SIGN_COLON//冒号
#define SIGN_COLON					':'
#endif
#ifndef SIGN_SEMICOLON//分号
#define SIGN_SEMICOLON				';'
#endif
#ifndef SIGN_LOWER_HORIZONTAL_BAR//下划线
#define SIGN_LOWER_HORIZONTAL_BAR	'_'
#endif
#ifndef SIGN_EQUAL
#define SIGN_EQUAL					'='
#endif
#ifndef SIGN_LESS
#define SIGN_LESS					'<'
#endif
#ifndef SIGN_GREATER
#define SIGN_GREATER				'>'
#endif
#ifndef SIGN_TAB
#define SIGN_TAB					'	'
#endif
#ifndef SIGN_AND
#define	SIGN_AND					'&'
#endif
#ifndef SIGN_QUESTION 
#define	SIGN_QUESTION				'?'
#endif
#ifndef SIGN_EXCLAMATION			
#define	SIGN_EXCLAMATION			'!'
#endif
#ifndef SIGN_POUND
#define	SIGN_POUND					'#'
#endif
#ifndef SIGN_USD			
#define	SIGN_USD					'$'
#endif
#ifndef SIGN_CARET
#define	SIGN_CARET					'^'
#endif
#ifndef SIGN_INTERVAL
#define	SIGN_INTERVAL				'`'
#endif
#ifndef SIGN_TILDE
#define	SIGN_TILDE					'~'
#endif
#ifndef SIGN_LEFT_PARENTHESES
#define	SIGN_LEFT_PARENTHESES		'('
#endif
#ifndef SIGN_RIGHT_PARENTHESES
#define	SIGN_RIGHT_PARENTHESES		')'
#endif
#ifndef SIGN_LEFT_SQUARE_BRACKETS
#define	SIGN_LEFT_SQUARE_BRACKETS	'['
#endif
#ifndef SIGN_RIGHT_SQUARE_BRACKETS
#define	SIGN_RIGHT_SQUARE_BRACKETS	']'
#endif
#ifndef SIGN_LEFT_BRACES
#define SIGN_LEFT_BRACES			'{'
#endif
#ifndef SIGN_RIGHT_BRACES
#define	SIGN_RIGHT_BRACES			'}'
#endif
#ifndef SIGN_SINGLE_QUOTATION
#define	SIGN_SINGLE_QUOTATION		'\''
#endif
#ifndef SIGN_DOUBLE_QUOTATION
#define	SIGN_DOUBLE_QUOTATION		'"'
#endif
#ifndef SIGN_VERTICAL
#define	SIGN_VERTICAL				'|'
#endif
#ifndef SIGN_AT
#define	SIGN_AT						'@'
#endif
#ifndef SIGN_CARRIAGE_RETURN
#define	SIGN_CARRIAGE_RETURN		'\r'
#endif
#ifndef SIGN_LINE_FEED
#define	SIGN_LINE_FEED				'\n'
#endif
#ifndef LINE_FEED_UNIX
#define LINE_FEED_UNIX				SIGN_LINE_FEED
#endif
#ifndef LINE_FEED_MAC
#define LINE_FEED_MAC				SIGN_CARRIAGE_RETURN
#endif
#ifndef LINE_FEED_WIN
#define LINE_FEED_WIN				"\r\n"
#endif
#ifndef LINE_FEED
#ifdef WIN32
#define LINE_FEED					LINE_FEED_WIN
#else
#define LINE_FEED					LINE_FEED_UNIX
#endif
#endif








#endif


