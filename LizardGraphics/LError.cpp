#pragma once
#include "LError.h"
#include "pch.h"

namespace LGraphics
{
    std::vector<std::string> LError::errors;
    size_t LError::lastErrorNum = 0;
    LApp* LError::app_;
}