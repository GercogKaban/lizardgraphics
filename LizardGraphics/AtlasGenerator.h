#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <queue>
#include <stdexcept>
#include <string>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <sstream>
#include <iterator>

#include "include/stb/stb_image.h"

//#define ALLOW_SPLIT
//#define COUT_PRINT

namespace fs = std::filesystem;

struct AtlasData
{
	using Dimensions = std::pair<size_t, size_t>;
	using DimensionsF = std::pair<float, float>;

	struct Texture
	{
		DimensionsF offset;
		DimensionsF size;
		std::string path;
	};

	std::vector<uint8_t> pixels;
	bool succesfullyCreated = false;

	std::string atlasPath;
	Dimensions atlasSize;
	std::vector<std::string> texturePaths;
	std::vector<Dimensions> textureDims;
	std::vector<Dimensions> textureOffsets;
};

class Atlas
{
	std::string filename;
	size_t sizeThreshold = 32768;
	struct Image
	{
		uint8_t* pixels;
		std::string path;
		int texWidth, texHeight, channels;
		static friend bool operator>(const Image& a, const Image& b)
		{
			return a.texHeight > b.texHeight;
		}
	};

public:

	Atlas(const std::string& filePath)
	{
		//std::string copy = std::string(filename);
		const auto base = filePath.substr(0, filePath.size() - 4);
		const auto fileEnd = filePath.substr(filePath.size() - 4,4);
		if (manager.isExists(filePath))
		    this->filename = AtlasManager::getUniqueFileName(base, fileEnd);
		else
			this->filename = filePath;
	}

	template <typename T, typename... Args>
	auto makeAtlas(T path, Args... paths)
	{
		std::array<std::string, sizeof...(Args) + 1> arr = { path, paths... };
		return makeAtlas(arr);
	}

	template <typename T, typename... Args>
	auto makeAtlas(T arr)
	{
		size_t pixelsSum = 0;
		std::vector<Image> images;
		images.resize(arr.size());
		for (size_t i = 0; i < arr.size(); ++i)
		{
			int texWidth, texHeight;
			auto& img = images[i];
			img.pixels = (uint8_t*)stbi_load(arr[i].data(), &img.texWidth, &img.texHeight, &img.channels, STBI_rgb_alpha);
			img.path = arr[i].data();
			pixelsSum += img.texWidth * img.texHeight;
			if (!img.pixels)
				throw std::runtime_error("failed to load texture image!");
		}

		std::sort(images.begin(), images.end(), [](const Image& a, const Image& b) {return a > b; });
		auto textureDims = findOptimalAtlasDimension(images);	
		while (textureDims <= sizeThreshold)
		{
			tryToCreateAtlas(images.data(), images.size(), textureDims);
			if (atlasData.succesfullyCreated)
				break;
			else
			    textureDims *= 2;
		}
		atlasData.atlasSize = { textureDims ,textureDims };
		if (textureDims > sizeThreshold)
		{
#ifdef COUT_PRINT
			std::cout << "can't place textures!\n";
#endif

#ifndef ALLOW_SPLIT
			throw std::runtime_error("can't place textures!\n");
#endif
		}

		atlasData.pixels.resize(textureDims * textureDims * STBI_rgb_alpha);
		for (size_t i = 0; i < arr.size(); ++i)
		{
			auto offset = atlasData.textureOffsets[i];
			const auto& img = images[i];
			for (size_t j = 0; j < img.texWidth; ++j)
				for (size_t k = 0; k < img.texHeight; ++k)
				{
					const auto pixel = (k * img.texWidth + j) * STBI_rgb_alpha;
					const auto pixel1 = ((offset.second + k) * textureDims + offset.first + j) * 4;
					atlasData.pixels[pixel1] = img.pixels[pixel];
					atlasData.pixels[pixel1 + 1] = img.pixels[pixel + 1];
					atlasData.pixels[pixel1 + 2] = img.pixels[pixel + 2];
					atlasData.pixels[pixel1 + 3] = img.pixels[pixel + 3];
				}
		}
		return std::ref(atlasData);
	}
	
	void saveAtlas(const std::string& atlasInfoName = "atlas_info");

	const auto& getAtlasData() const
	{
		return atlasData;
	}

	void loadAtlas(const std::string& path)
	{
		manager.loadAtlas(path,atlasData);
	}

	size_t findOptimalAtlasDimension(const std::vector<Image>& images) const
	{
		return findOptimalAtlasDimension(images.data(), images.size());
	}

	template <typename T>
	size_t findOptimalAtlasDimension(T* images,size_t size) const
	{
		size_t squareSum = 0;
		for (size_t j = 0; j < size; j++)
			squareSum += images[j].texHeight * images[j].texWidth;
		int squarePowerTwo = std::log2(getPowerTwoAlign(squareSum)) / 2;
		return 2 << squarePowerTwo - 1;
	}

	const std::vector<std::string>& getCacheAtlases() const { return manager.getCacheAtlases(); }

	void setSizeThreshold(size_t val) { sizeThreshold = val; }

	void setFileName(const std::string& fileName) { this->filename = fileName; }
	const std::string& getOutPath() const { return filename; }

private:

	class AtlasManager
	{
	public:

		AtlasManager(const std::string& atlasCacheName = "atlas",
			const std::string& atlasCacheExtension = ".ATLASC",
			const std::string& atalsInfoExtension = ".ATLAS")
		{
			if (!isExistsWithExt(fs::current_path(), atlasCacheExtension, atlasCachePath))
				atlasCachePath = std::filesystem::path(atlasCacheName + atlasCacheExtension);
			else
			{
				std::ifstream atlas(atlasCacheName + atlasCacheExtension);
				if (!atlas.is_open())
					throw std::runtime_error("can't open file: " + atlasCacheName + atlasCacheExtension);

				std::string line;
				while (std::getline(atlas, line))
					atlases.push_back(line);
			}

			this->atlasInfoExtension = atlasInfoExtension;
		}

		const std::vector<std::string>& getCacheAtlases() const { return atlases; }

		static bool isExistsWithExt(const std::filesystem::path& dirPath, const std::string& ext,
			std::filesystem::path& filePathRes)
		{
			for (auto& f : std::filesystem::directory_iterator(dirPath))
				if (f.is_regular_file())
					if (f.path().extension() == ext)
					{
						filePathRes = f.path();
						return true;
					}
			return false;
		}

		static bool isExists(const std::string& path)
		{
			return std::filesystem::exists(path);
		}

		static std::string getUniqueFileName(const std::string& base, const std::string& ext)
		{
			size_t num = 1;
			while (true)
			{
				if (!isExists(base + std::to_string(num) + ext))
					break;
				else
					num++;
			}
			return base + std::to_string(num) + ext;
		}

		std::filesystem::path getAtlasCachePath() const
		{
			return atlasCachePath;
		}

		std::string getAtlasExtension() const
		{
			return atlasExtension;
		}

		std::string getAtlasCacheExtension() const
		{
			return atlasCachePath.extension().generic_string();
		}

		void saveAtlas(const AtlasData& data,
			const std::string& atlasName,
			const std::string& atlasExtension = "png",
			const std::string& atlasInfoPath = "atlas_info");

	    void loadAtlas(const std::string& path, AtlasData& data)
		{
			//AtlasData data;
			std::ifstream in(path);
			if (!in.is_open())
				throw std::runtime_error("error, can't open file " + path);

			std::string line;

			std::getline(in, line);
			data.atlasPath = line;

			std::getline(in, line);
			data.atlasSize = {std::stoi(line),std::stoi(line)};

			std::getline(in, line);
			data.texturePaths = splitByToken(line);

			std::getline(in, line);
			data.textureDims = fromTokenToPair(splitByToken(line));

			std::getline(in, line);
			data.textureOffsets = fromTokenToPair(splitByToken(line));
		}

	private:

		std::vector<std::string> atlases;

		std::vector<std::string> splitByToken(const std::string& str)
		{
			std::istringstream iss(str);
			std::vector<std::string> tokens;
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(tokens));
			return tokens;
		}

		std::vector<AtlasData::Dimensions> fromTokenToPair(const std::vector<std::string>& tokens)
		{
			std::vector<AtlasData::Dimensions> ret;
			for (const auto& t : tokens)
			{
				std::string num1, num2;
				size_t i = 0;
				while (t[i] != ',')
					num1 += t[i++];
				i++;
				while (i< t.size())
					num2 += t[i++];
				ret.push_back(std::make_pair(std::stoi(num1),std::stoi(num2)));
			}
			return ret;
		}

		std::filesystem::path atlasCachePath;
		std::string atlasExtension;
		std::string atlasInfoExtension;

	} manager;

	size_t getPowerTwoAlign(size_t size) const
	{
		size_t i = 2;
		while (size > i)
			i <<= 2;
		return i;
	}

	void tryToCreateAtlas(Image* images, size_t imgCount, size_t textureDims) //std::vector<AtlasData::Texture>& ret)
	{
		struct InsertPoint
		{
			AtlasData::Dimensions offset;
			size_t parentHeight;
		};

		std::deque<InsertPoint> insertPoints;
		size_t lastHeight = 0;
		bool firstInRow = true;
		AtlasData ret;
		ret.texturePaths.reserve(imgCount);
		ret.textureDims.reserve(imgCount);
		ret.textureOffsets.reserve(imgCount);
		insertPoints.push_back({ { 0,0 }, textureDims });
		for (size_t i = 0; i < imgCount; ++i)
		{		
			auto prevRetSize = ret.texturePaths.size();
			for (size_t j = 0; j < insertPoints.size(); ++j)
			{
				const auto& img = images[i];
				const auto insertPoint = insertPoints[j];
				if (insertPoint.offset.first + img.texWidth <= textureDims &&
					insertPoint.offset.second + img.texHeight <= insertPoint.parentHeight)
				{
					insertPoints.erase(insertPoints.begin() + j);
					firstInRow = insertPoint.offset.first == 0;

					// right point
					if (insertPoint.offset.first + img.texWidth <= textureDims)
						insertPoints.push_back({ { insertPoint.offset.first + img.texWidth,insertPoint.offset.second },
							(size_t)img.texHeight + insertPoint.offset.second });

					// down point
					if (firstInRow || insertPoint.offset.second + img.texHeight < insertPoint.parentHeight
						|| img.texHeight != lastHeight)
					{
						insertPoints.push_back({ { insertPoint.offset.first, insertPoint.offset.second + img.texHeight },insertPoint.parentHeight});
						firstInRow = false;
					}

					ret.textureOffsets.push_back({insertPoint.offset.first, insertPoint.offset.second });
					ret.textureDims.push_back({ img.texWidth, img.texHeight });
					ret.texturePaths.push_back(img.path);

					lastHeight = img.texHeight;
					break;
				}
				if (prevRetSize == ret.texturePaths.size() && !insertPoints.size() ||
					insertPoints.size() == j + 1)
				{
					atlasData = ret;
					return;
				}
			}
		}
		ret.succesfullyCreated = true;
		atlasData = ret;
	}

	//AtlasData::DimensionsF normalizeVal(AtlasData::Dimensions val, AtlasData::Dimensions maxVal) const
	//{
	//	return { (float)val.first / (float)maxVal.first, (float)val.second / (float)maxVal.second };
	//}

	AtlasData atlasData;
};