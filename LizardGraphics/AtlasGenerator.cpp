#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "AtlasGenerator.h"
#include "include/stb/stb_image_write.h"


void Atlas::saveAtlas(const std::string& atlasInfoName)
{
	manager.saveAtlas(atlasData, filename.substr(0, filename.size() - 4),
		filename.substr(filename.size() - 3, 3), atlasInfoName);
}

void Atlas::AtlasManager::saveAtlas(const AtlasData& data,
	const std::string& atlasName,
	const std::string& atlasExtension,
	const std::string& atlasInfoPath)
{
	std::string atlasInfoPathCopy = atlasInfoPath + atlasInfoExtension;
	if (isExists(atlasInfoPath + atlasInfoExtension))
		atlasInfoPathCopy = getUniqueFileName(atlasInfoPath, atlasInfoExtension);
	std::cout << atlasInfoPathCopy << std::endl;


	if (atlasExtension == "png")
		stbi_write_png((atlasName + '.' + atlasExtension).data(), data.atlasSize.first, data.atlasSize.second, STBI_rgb_alpha, data.pixels.data(), 0);
	else if (atlasExtension == "jpg")
		stbi_write_jpg((atlasName + '.' + atlasExtension).data(), data.atlasSize.first, data.atlasSize.second, STBI_rgb_alpha, data.pixels.data(), 100);
	else if (atlasExtension == "tga")
		stbi_write_tga((atlasName + '.' + atlasExtension).data(), data.atlasSize.first, data.atlasSize.second, STBI_rgb_alpha, data.pixels.data());
	else if (atlasExtension == "bmp")
		stbi_write_bmp((atlasName + '.' + atlasExtension).data(), data.atlasSize.first, data.atlasSize.second, STBI_rgb_alpha, data.pixels.data());
	else
		throw std::runtime_error("wrong file format, please choose one from following: png/jpg/tga/bmp");

	std::ofstream out(atlasInfoPathCopy, std::ios::binary);
	if (!out.is_open())
		throw std::runtime_error("can't open file: " + atlasInfoPathCopy);

	out << atlasName + '.' + atlasExtension << std::endl;

	const auto atlasSize = std::to_string(data.atlasSize.first);
	out << atlasSize << std::endl;

	for (const auto& p : data.texturePaths)
		out << p + ' ';
	out << std::endl;

	for (const auto& d : data.textureDims)
	{
		const auto d1 = std::to_string(d.first);
		const auto d2 = std::to_string(d.second);
		out << d1 + ',' + d2 + ' ';
	}
	out << std::endl;

	for (const auto& o : data.textureOffsets)
	{
		const auto o1 = std::to_string(o.first);
		const auto o2 = std::to_string(o.second);
		out << o1 + ',' + o2 + ' ';
	}
	out.close();

	std::ofstream atlasCacheOut(atlasCachePath, std::ios::app | std::ios::binary);
	if (!atlasCacheOut.is_open())
		throw std::runtime_error("error, can't open file " + atlasCachePath.generic_string());
	atlasCacheOut << atlasInfoPathCopy << std::endl;
	atlasCacheOut.close();
}