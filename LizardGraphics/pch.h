﻿// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#pragma once

//#define GLEW_STATIC

//#define WIN32_LEAN_AND_MEAN  

#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <iostream>
#include <functional>
#include <chrono>
#include <cmath>
#include <thread>
#include <optional>
#include <set>
#include <array>
#include <map>
#include <tuple>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define VK_VERSION_1_0
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <csignal>
#endif

#include "vulkan/vulkan.h"

#ifndef NDEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_EXTERNAL_TEMPLATE
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "include/glm/gtc/quaternion.hpp"
#include "include/glm/gtx/matrix_decompose.hpp"

#include "include/stb/stb_image.h"

#include "ObjectPool.h"
#include "textures.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include "vulkan_wrapper.h"
#endif

#include "additional.h"

//#endif //PCH_H
