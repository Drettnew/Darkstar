#pragma once
#include <Windows.h>
#include <vector>
#include "ModelAsset.h"

struct Material
{
	char name[MAX_PATH]; // Material name.
	float Ka[3];	// Ambient.
	float Kd[3];	// Diffuse (color).
	float Ks[3];	// Specular color.
	float Tf[3];	// Transmission filter.
	float Tr;		// Transparency (also called d).
	float Ns;		// Specular power (shininess).
	float Ni;		// Index of refraction (optical density).
	int illum;		// Illumination model (See remark at bottom of file).
	char map_Ka[MAX_PATH]; // Ambient texture file name.
	char map_Kd[MAX_PATH]; // Diffuse texture file name.
	char map_Ks[MAX_PATH]; // Specular color texture file name.
	char map_Ns[MAX_PATH]; // Specular power texture file name.
	char map_Tr[MAX_PATH]; // Transparency texture file name.
	char map_Disp[MAX_PATH]; // Displacement map.
	char map_Bump[MAX_PATH]; // Bump map.
	char map_Refl[MAX_PATH]; // Reflection map.

	Material()
	{
		// Set material defaults (using mtl file specs).
		strncpy(name, "default", MAX_PATH);
		Ka[0] = 0.2f; Ka[1] = 0.2f; Ka[2] = 0.2f;
		Kd[0] = 0.8f; Kd[1] = 0.8f; Kd[2] = 0.8f;
		//Ks[0] = 1.0f; Ks[1] = 1.0f; Ks[2] = 1.0f;
		Ks[0] = 0.f; Ks[1] = 0.f; Ks[2] = 0.f;
		Tf[0] = 1.0f; Tf[1] = 1.0f; Tf[2] = 1.0f;
		Tr = 1.f; // Fully opaque.
				  //Ns = 0.f;
		Ns = 32.f;
		Ni = 1.f;
		illum = 2; // No default specified (?).
		map_Ka[0] = 0;
		map_Kd[0] = 0;
		map_Ks[0] = 0;
		map_Ns[0] = 0;
		map_Tr[0] = 0;
		map_Disp[0] = 0;
		map_Bump[0] = 0;
		map_Refl[0] = 0;
	}

	bool LoadMtlLib(LPCTSTR fileName, std::vector<Material*>& materials);
	bool LoadObj(LPCTSTR filename);
};