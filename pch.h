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

#include "vectors.h"
#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"

#include "include/SOIL2/SOIL2.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <ft2build.h>
#include "additional.h"

#endif //PCH_H
