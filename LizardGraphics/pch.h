// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H

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

#include "vectors.h"

#ifdef OPENGL
#include "include/GLEW/glew.h"
#endif

#ifndef NDEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

//#define GLM_EXTERNAL_TEMPLATE
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "ObjectPool.h"
#include "textures.h"

//#include <stdio.h>          // printf, fprintf
//#include <stdlib.h>         // abort

#ifdef VULKAN
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#endif

#include "imgui_impl_vulkan.h"
#include "additional.h"

#endif //PCH_H
