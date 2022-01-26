#include "LShape.h"
#include "pch.h"
#include "LApp.h"

namespace LGraphics
{
    void LShape::color(const glm::vec3 val)
    {
        LOG_CALL
        color_ = val;
        if (app->getAppInfo().api == L_VULKAN)
            setUpdateUniformsFlag();
    }

    void LShape::color(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        LOG_CALL
        color_ = { (float)r / (float)UINT8_MAX, (float)g / (float)UINT8_MAX,(float)b / (float)UINT8_MAX };
        setUpdateUniformsFlag();
    }

    void LShape::transparency(const float val)
    {
        LOG_CALL
        transparency_ = val;
        setUpdateUniformsFlag();
    }

    void LShape::scale(const glm::vec3 val)
    {
        LOG_CALL
        LShape::scale(val.x, val.y, val.z);
    }

    void LShape::scale(const float x, const float y, const float z)
    {
        LOG_CALL
        auto scaleDif = glm::vec3(x, y, z) - scale_;
        scaleWithoutAlign({ x,y,z });
        setUpdateUniformsFlag();
    }

    void LShape::scaleWithoutAlign(const glm::vec3 val)
    {
        LOG_CALL
        scale_ = val;
    }

    void LShape::move(const glm::vec3 val)
    {
        LOG_CALL
        LShape::move(val.x, val.y, val.z);
    }

    void LShape::move(const float x, const float y, const float z)
    {
        LOG_CALL
        auto moveDif = glm::vec3(x,y,z) - move_;
        move_ = { x,y,z };
        setUpdateUniformsFlag();
    }

    void LShape::move(const size_t x, const size_t y)
    {
        LOG_CALL
        auto coords = pointOnScreenToGlCoords(glm::vec2(app->getWindowSize()), { (float)x,(float)y });
        LShape::move(coords.x, coords.y, 0.0f);
    }   

    void LShape::move(const glm::vec<2,size_t> v)
    {
        LOG_CALL
        auto coords = pointOnScreenToGlCoords(glm::vec2(app->getWindowSize()), { (float)v.x, (float)v.y });
        LShape::move(coords.x, coords.y, 0.0f);
    }

    void LShape::setModel(const glm::mat4& model)
    {
        LOG_CALL
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(model, scale_, rotate_q, move_, skew, perspective);
    }

    float& LShape::getTransparencyRef()
    {
        LOG_CALL
        setUpdateUniformsFlag();
        return transparency_;
    }

    glm::vec3& LShape::getColorRef()
    {
        LOG_CALL
        setUpdateUniformsFlag();
        return color_;
    }

    glm::vec3& LShape::getScaleRef()
    {
        LOG_CALL
        setUpdateUniformsFlag();
        return scale_;
    }

    glm::vec3& LShape::getMoveRef()
    {
        LOG_CALL
        setUpdateUniformsFlag();
        return move_;
    }

    LShape::LShape(LApp* app /*, ImageResource res*/)
        :LWidget(app)
    {
    }

    void LShape::refreshModel()
    {
        LOG_CALL
        model = calculateModelMatrix();
    }

    glm::mat4 LGraphics::LShape::calculateModelMatrix() const
    {
        LOG_CALL
#if _DEBUG
#include "../Optimized/optimized.h"
        return _calculateModelMatrix(move_, glm::mat4_cast(rotate_q), scale_);
#else
        glm::mat4 model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3(move_.x, move_.y, move_.z));
        model_ *= glm::mat4_cast(rotate_q);
        model_ = glm::scale(model_, glm::vec3(scale_.x, scale_.y, scale_.z));
        return model_;
#endif
    }

    glm::vec3 LShape::getMove() const
    {
        LOG_CALL
        return move_;
    }

    void LShape::turnOffColor()
    {
        LOG_CALL
        color_ = rgbToFloat(1, 1, 1);
    }

    void LShape::setGlobalUniformsGBuffer(GLuint shaderProgram)
    {
        LOG_CALL
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();

        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "reflexMap"), 2);

        glUniform1i(glGetUniformLocation(shaderProgram, "environment"), 6);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        setFogUniforms(shaderProgram);
    }

    void LShape::setGlobalUniformsDeferred(GLuint shaderProgram)
    {
        LOG_CALL
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();
        if (app->currentLight)
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"),
                app->currentLight->getLightspaceMatrix().size(), GL_FALSE,
                glm::value_ptr(app->currentLight->getLightspaceMatrix()[0]));

        glUniform1i(glGetUniformLocation(shaderProgram, "drawingReflex"), app->drawingReflex);
        glUniform1i(glGetUniformLocation(shaderProgram, "selfShading"), false);
        glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);

        glUniform1i(glGetUniformLocation(shaderProgram, "gPosition"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "gNormal"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "gAlbedo"), 2);

        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowCubeMap"), 5);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        //glUniform1f(glGetUniformLocation(shaderProgram, "heightScale"), app->heightScale);

        setLightUniforms(shaderProgram);
        setFogUniforms(shaderProgram);

        if (dynamic_cast<LPointLight*>(app->currentLight))
        {
            glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),
                app->currentLight->position.x, app->currentLight->position.y, app->currentLight->position.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "farPlane"), app->currentLight->farPlane);
        }
        glUniform1i(glGetUniformLocation(shaderProgram, "pcf"), app->pcf);
    }

    void LShape::setGlobalUniformsPicking(GLuint shaderProgram)
    {
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        const auto& currentLight = app->currentLight;
        if (dynamic_cast<LPointLight*>(currentLight))
        {
            glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),
                currentLight->position.x, currentLight->position.y, currentLight->position.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "farPlane"), currentLight->farPlane);
        }
    }

    void LShape::setGlobalUniformsShadows(GLuint shaderProgram)
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"),
            app->currentLight->getLightspaceMatrix().size(), GL_FALSE,
            glm::value_ptr(app->currentLight->getLightspaceMatrix()[0]));
    }

    void LShape::setGlobalUniformsReflex(GLuint shaderProgram)
    {
        setGlobalUniforms(shaderProgram);
        const auto proj = app->getProjectionMatrix();

        float aspect = (float)1024 / (float)1024;
        float near_ = 0.01f;
        float far_ = 100.0f;

        const std::array<glm::mat4, 6> faceTransforms = {
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))),
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))),
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))),
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))),
        (proj * glm::lookAt(app->reflexPos, app->reflexPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)))
        };
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "faceTransforms"), faceTransforms.size(), GL_FALSE, glm::value_ptr(faceTransforms[0]));
    }

    void LShape::setGlobalUniformsPostProcessing(GLuint shaderProgram)
    {
        const auto projView = app->projection * app->view;
        const auto invProjView = glm::inverse(projView);

        glUniform1i(glGetUniformLocation(shaderProgram, "colorMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "depthMap"), 2);

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projView"), 
        //    1, GL_FALSE, glm::value_ptr(projView));
        //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "invProjView"),
        //    1, GL_FALSE, glm::value_ptr(invProjView));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"),
            1, GL_FALSE, glm::value_ptr(app->projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(app->view));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "invProj"),
            1, GL_FALSE, glm::value_ptr(glm::inverse(app->projection)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "invView"),
            1, GL_FALSE, glm::value_ptr(glm::inverse(app->view)));
        setFogUniforms(shaderProgram);
    }

    void LGraphics::LShape::setGlobalUniforms(GLuint shaderProgram)
    {
        LOG_CALL
        const auto proj = app->getProjectionMatrix();
        const auto view = app->getViewMatrix();
        if (app->currentLight)
            setGlobalUniformsShadows(shaderProgram);

        glUniform1i(glGetUniformLocation(shaderProgram, "drawingReflex"), app->drawingReflex);
        glUniform1i(glGetUniformLocation(shaderProgram, "selfShading"), false);
        glUniform2i(glGetUniformLocation(shaderProgram, "screenSize"), (int)app->info.wndWidth, (int)app->info.wndHeight);

        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "parallaxMap"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "reflexMap"), 3);

        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 4);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowCubeMap"), 5);
        glUniform1i(glGetUniformLocation(shaderProgram, "environment"), 6);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "heightScale"), app->heightScale);

        setLightUniforms(shaderProgram);
        setFogUniforms(shaderProgram);
    }

    void LShape::setLightUniforms(GLuint shaderProgram)
    {
        char res[32];
        // setting point lights
        FOR(i, 0, app->lights[L_POINT_LIGHT].size())
        {
            sprintf(res, "pointSources[%zu]", i);

            const auto& l = *(LPointLight*)app->lights[L_POINT_LIGHT][i];

            glUniform1i(glGetUniformLocation(shaderProgram, (std::string(res) + ".calculateShadow").data()), l.calculateShadow);
            glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".position").data()),
                l.position.x, l.position.y, l.position.z);
            glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".ambient").data()),
                l.ambient.x, l.ambient.y, l.ambient.z);
            glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".diffuse").data()),
                l.diffuse.x, l.diffuse.y, l.diffuse.z);
            glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".specular").data()),
                l.specular.x, l.specular.y, l.specular.z);

            glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".constant").data()), l.constant);
            glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".linear").data()), l.linear);
            glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".quadratic").data()), l.quadratic);
        }

        // setting spot lights
        FOR(i, 0, app->lights[L_SPOT_LIGHT].size())
        {
            sprintf(res, "spotSources[%zu]", i);

            const auto& l = *(LSpotLight*)app->lights[L_SPOT_LIGHT][i];

            glUniform1i(glGetUniformLocation(shaderProgram, (std::string(res) + ".calculateShadow").data()), l.calculateShadow);

            //if (l.calculateShadow)
            {
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".position").data()),
                    l.position.x, l.position.y, l.position.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".direction").data()),
                    l.direction.x, l.direction.y, l.direction.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".ambient").data()),
                    l.ambient.x, l.ambient.y, l.ambient.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".diffuse").data()),
                    l.diffuse.x, l.diffuse.y, l.diffuse.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".specular").data()),
                    l.specular.x, l.specular.y, l.specular.z);

                glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".cutOff").data()), l.cutOff);
                glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".outerCutOff").data()), l.outerCutOff);
                glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".constant").data()), l.constant);
                glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".linear").data()), l.linear);
                glUniform1f(glGetUniformLocation(shaderProgram, (std::string(res) + ".quadratic").data()), l.quadratic);
            }
        }

        // setting directional lights
        FOR(i, 0, app->lights[L_DIRECTIONAL_LIGHT].size())
        {
            sprintf(res, "dirSources[%zu]", i);

            const auto& l = *(LDirectionalLight*)app->lights[L_DIRECTIONAL_LIGHT][i];

            glUniform1i(glGetUniformLocation(shaderProgram, (std::string(res) + ".calculateShadow").data()), l.calculateShadow);

            //if (l.calculateShadow)
            {
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".position").data()),
                    l.position.x, l.position.y, l.position.z);
                //glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".direction").data()),
                    //l.direction.x, l.direction.y, l.direction.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".ambient").data()),
                    l.ambient.x, l.ambient.y, l.ambient.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".diffuse").data()),
                    l.diffuse.x, l.diffuse.y, l.diffuse.z);
                glUniform3f(glGetUniformLocation(shaderProgram, (std::string(res) + ".specular").data()),
                    l.specular.x, l.specular.y, l.specular.z);
            }
        }

        glUniform1i(glGetUniformLocation(shaderProgram, "pointSourcesCount"), app->lights[L_POINT_LIGHT].size());
        glUniform1i(glGetUniformLocation(shaderProgram, "spotSourcesCount"), app->lights[L_SPOT_LIGHT].size());
        glUniform1i(glGetUniformLocation(shaderProgram, "dirSourcesCount"), app->lights[L_DIRECTIONAL_LIGHT].size());

    }

    void LShape::setFogUniforms(GLuint shaderProgram)
    {
        if (app->fog.isEnabled)
        {
            glUniform3f(glGetUniformLocation(shaderProgram, "fog.color"), app->fog.color.x, app->fog.color.y, app->fog.color.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "fog.density"), app->fog.density);
            glUniform1i(glGetUniformLocation(shaderProgram, "fog.isEnabled"), true);
        }
    }

    glm::vec3 LShape::getMiddlePoint() const
    {
        //const std::pair<float, float> scaledRangeX = { scale_.x * rangeX.first ,  scale_.x * rangeX.second  };
        //const std::pair<float, float> scaledRangeY = { scale_.y * rangeY.first ,  scale_.y * rangeY.second  };
        //const std::pair<float, float> scaledRangeZ = { scale_.z * rangeZ.first ,  scale_.z * rangeZ.second};
        return { 
            move_.x + (scale_.x * rangeX.second - scale_.x * rangeX.first),
            move_.y + (scale_.y * rangeY.second - scale_.y * rangeY.first) ,
            move_.z + (scale_.z * rangeZ.second - scale_.z * rangeZ.first) };
    }

    LShape::~LShape()
    {
        //delete buffer;
        //delete shader;
    }
}