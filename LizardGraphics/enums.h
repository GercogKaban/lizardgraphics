#pragma once
#include <iostream>

namespace LGraphics
{
    enum LStates : uint8_t
    {
        L_FALSE,
        L_TRUE,
    };

    enum LProjections : uint8_t
    {
        L_PERSPECTIVE,
        L_ORTHOGRAPHIC,
    };

    enum LTypes : uint8_t
    {
        L_PRIMITIVE,
        L_MODEL,
    };

    enum RenderingAPI : uint8_t
    {
        L_VULKAN,
        L_OPENGL,
    };

    enum Materials : uint8_t
    {
        NO_MATERIAL,
        EMERALD,
        JADE,
        OBSIDIAN,
        PEARL,
        RUBY,
        TURQUOISE,
        BRASS,
        BRONZE,
        CHROME,
        COPPER,
        GOLD,
        SILVER,
        BLACK_PLASTIC,
        CYAN_PLASTIC,
        GREEN_PLASTIC,
        RED_PLASTIC,
        WHITE_PLASTIC,
        YELLOW_PLASTIC,
        BLACK_RUBBER,
        CYAN_RUBBER,
        GREEN_RUBBER,
        RED_RUBBER,
        WHITE_RUBBER,
        YELLOW_RUBBER,
    };

}