#include "common.hpp"
#include <iostream>

namespace ng {
	
void printChars(char* str, int start, int end) {
	for(int i=start; i < end-start; i++) {
		std::cout << str[i];
	}
}

Rect getIntersectingRectangle(const Rect &a, const Rect &b) {
	int Ax = std::max(a.x, b.x);
	int Ay = std::max(a.y, b.y);
	int Bx = std::min(a.x+a.w,b.x+b.w);
	int By = std::min(a.y+a.h,b.y+b.h);
	if(Ax < Bx && Ay < By) {
		return {Ax,Ay,Bx-Ax,By-Ay};
	}
	return {0,0,0,0};
}


#ifdef USE_SDL
const char sdl_code_conversion_table[] = 
"................................" // 32 dots
" !\"#$%&\'()*+,-./0123456789:;<=>?@" // 33 chars
".........................." // 26 dots
"[\\]^_`abcdefghijklmnopqrstuvwxyz"
;
const char sdl_code_conversion_table_shift[] = 
"................................" // 32 dots
" !\"#$%&\"()*+<_>?)!@#$%^&*(::<+>?@" // 33 chars
".........................." // 26 dots
"{|}^_~ABCDEFGHIJKLMNOPQRSTUVWXYZ" // 32 chars
;
char SDLCodeToChar( int code, bool shift ) {
	if(code <= 122) {
		return (shift ? sdl_code_conversion_table_shift[code] : sdl_code_conversion_table[code]);
	} else {
		return ' ';
	}
}

#endif


// #ifdef USE_SFML
// const char sf_code_conversion_table[] =
// "abcdefghijklmnopqrstuvwxyz0123456789..........[];,.'/\\`=- l\b\t......+-*/....0123456789................";
// const char sf_code_conversion_table_shift[] =
// "ABCDEFGHIJKLMNOPQRSTUVWXYZ)!@#$%^&*(..........{}:<>\"?|~+_ l\b\t......+-*/....0123456789................";
// char SFMLCodeToChar( unsigned char sf_code, bool shift ) {
	// return (shift ? sf_code_conversion_table_shift[sf_code] : sf_code_conversion_table[sf_code]);
// }
// #endif

}
