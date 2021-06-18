//TYPEDEFINE.H

//==========================================================
#ifndef _TYPEDEFINE_H
#define _TYPEDEFINE_H
//==========================================================
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;
typedef list<string> _MAPLIST;

typedef char			_SBYTE;
typedef unsigned char 	_UBYTE;
typedef unsigned int 	_UINT;
typedef unsigned long 	_ULONG;
typedef unsigned long 	_UDWORD; 
typedef unsigned long long _ULONGLONG;
typedef long long		_SLONGLONG;
typedef signed long 	_SLONG;
typedef signed int 		_SINT;
typedef float 			_FLOAT;

typedef union 
{
	_UBYTE Byte [2];
	_UINT Value;
}un_2Byte;


typedef union 
{
	_UBYTE Byte [4];
	_ULONG Value;
}un_4Byte;

//==========================================================
#endif 	//_TYPEDEFINE_H
//==========================================================

