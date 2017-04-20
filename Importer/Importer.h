#pragma once

//Win32 Includes
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//Basic Util Includes
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

//Other includes /*
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdint.h>

#include <stdio.h>
#include "Wincon.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>

#ifdef IMPORTER_EXPORT
#define IMPORTER_API __declspec(dllexport)   
#else  
#define IMPORTER_API __declspec(dllimport)   
#endif

namespace Importer
{
	

};