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
        L_FLAT,
        L_VOLUMETRIC,
    };

    enum LGClasses : uint8_t
    {
        L_PLANE,
        L_CUBE,
        L_SPHERE,
        L_ICOSPHERE,
        L_CONE,
        L_TORUS,
        L_CYLINDER,
        L_MODEL,
    };

    enum LLights : uint8_t
    {
        L_POINT_LIGHT,
        L_SPOT_LIGHT,
        L_DIRECTIONAL_LIGHT,
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

    enum ModelLoading
    {
        FAST,
        QUALITY,
        MAX_QUALITY,
    };

    enum QualityLevels
    {
        AUTO,
        LOW,
        MEDIUM,
        HIGH,
    };

    enum TextureTypes
    {
        DIFFUSE,
        NORMAL,
    };

    enum RenderingTypes
    {
        FORWARD,
        DEFERRED,
    };

    enum SpecialCharacters
    {
        SPACE = ' ',
    };

}