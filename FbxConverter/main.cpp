#include "pch.h"
#include <iostream>
#include "FbxUtil.h"
#include "MeshViewer.h"

int main(int argc, char** argv)
{	
	if (argc == 1)
	{
		char* filename = argv[0];
		filename = PathFindFileNameA( filename );
		std::cout << "Fbx mesh to binary converter tool." << std::endl;
		std::cout << "Usage: " << filename << " [file|directory]" << std::endl;

		return 0;
	}

	if (argc >= 2)
	{
		if ((argv[1][0] == '\\' || argv[1][0] == '/') &&
			(argv[1][1] == 'v' || argv[1][1] == 'V'))
		{
			// Run mesh viewer
			MeshViewer meshViewer;

			meshViewer.Start();

			return 0;
		}
	}

	if (argc >= 2)
		FbxUtil::Convert(argv[1], FbxUtil::FbxType::kMesh);

	if (argc >= 3)
		FbxUtil::Convert(argv[2], FbxUtil::FbxType::kAnimation);
	
	return 0;
}