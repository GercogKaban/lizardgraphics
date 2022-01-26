#include "pch.h"
#include "LImagedShape.h"
#include "LRectangleBuffer.h"
#include "Lshaders.h"
#include "LApp.h"
#include "LLogger.h"
#include "LResourceManager.h"
#include "LPlane.h"

namespace LGraphics
{
    LImagedShape::LImagedShape(LApp* app, ImageResource res, std::vector<LShape*>& changed, std::vector<LGraphics::LWidget::PrimitiveUniforms>& u,
     bool& needToResetBuffer) :LShape(app),LImage(res,app->info.api)
    {
        init(app);
        shader = app->getLightningShader().get();
        app->toCreate.push(this);
        diffusePath = app->getRealTexturesPath() + "diffuse/";
        normalsPath = app->getRealTexturesPath() + "normal/";
        displacementPath = app->getRealTexturesPath() + "displacement/";
        reflexPath = app->getRealReflexPath() + "reflex/";
        setNormalMapping(changed,this,res.normals);
        setParallaxMapping(changed, this, res.parallax);
        setReflexMapping(changed, this, res.reflex);
        if (u.size() == u.capacity())
            needToResetBuffer = true;
        u.push_back(PrimitiveUniforms());
        rotateX(changed, this, 180.0f);
    }

    void LImagedShape::init(LApp* app)
    {
        LOG_CALL
            this->app = app;
        buffer = app->standartRectBuffer;
        if (app->info.api == L_OPENGL)
            shader = app->openGLLightShader.get();
        else if (app->info.api == L_VULKAN)
            shader = app->lightShader.get();
    }

    void LImagedShape::updateBufferParallel(LGClasses type, LWidget::PrimitiveUniforms* buffer, const std::vector<LShape*>& changed, size_t begin, size_t end)
    {
        for (size_t i = begin; i < end; i++)
            updateUniforms(app, type, buffer, changed[i]->id);
    }

    void LImagedShape::updateBuffer(LGClasses type, std::vector<LGraphics::LShape*>& objChanged, bool& needToResetBuffer
    ,GLuint vao, GLuint vbo, const std::vector<LWidget::PrimitiveUniforms>& u)
    {
        if (objChanged.size())
        {
            if (needToResetBuffer)
            {
                resetInstanceBuffer(vao,vbo,u);

                //objChanged.clear();
                //objChanged.resize(app->primitives[type].size());
                //for (size_t i = 0; i < app->primitives[type].size(); ++i)
                //    objChanged[i] = app->primitives[type][i];
                needToResetBuffer = false;
            }

            LWidget::PrimitiveUniforms* buffer = (LWidget::PrimitiveUniforms*)glMapNamedBufferRange(vbo, 0, sizeof(LWidget::PrimitiveUniforms) * u.capacity(),
                GL_MAP_WRITE_BIT);
            if (!buffer)
                throw std::runtime_error(LOG_HEADER + "buffer == NULL, error: " + std::to_string(glGetError()) + "\n\n");

#ifdef PARALLEL_UPDATE

            if (objChanged.size() > 1000 && app->info.freeThreads > 1)
            {
                std::thread* threads = new std::thread[app->info.freeThreads];
                for (size_t i = 0; i < app->info.freeThreads; ++i)
                {
                    const size_t interval = i * objChanged.size() / app->info.freeThreads;
                    threads[i] = std::thread(LImagedShape::updateBufferParallel, type, buffer, objChanged, interval, interval + objChanged.size() / app->info.freeThreads);
                }

                for (size_t i = 0; i < app->info.freeThreads; ++i)
                    threads[i].join();
                delete[] threads;
            }
            else
#endif
                for (const auto& obj : objChanged)
                    LImagedShape::updateUniforms(app, type, buffer, obj->id);
            objChanged.clear();
            glUnmapNamedBuffer(vbo);
        }

    }

    void LImagedShape::drawInstanced(LGClasses type, std::vector<LShape*>& changed, bool& needToResetBuffer, 
        GLuint vao, GLuint vbo,std::vector<LWidget::PrimitiveUniforms>& uniforms, size_t indCount)
    {
        if (!uniforms.size()) return;
        LShaders::OpenGLShader* shader;
        shader = (LShaders::OpenGLShader*)app->getStandartShader();
        //shader = app->tesselation? (LShaders::OpenGLShader*)app->getStandartShaderTes() : (LShaders::OpenGLShader*)app->getStandartShader();
        if (app->drawingInShadow)
        {
            shader = dynamic_cast<LPointLight*>(app->currentLight)
                ? (LShaders::OpenGLShader*)app->shadowCubeMapShader.get()
                : (LShaders::OpenGLShader*)app->shadowMapShader.get();
        }
        else if (app->drawingReflex)
            shader = ((LShaders::OpenGLShader*)app->reflexPrimitiveShader.get());
        else if (app->drawingPicking)
            shader = ((LShaders::OpenGLShader*)app->pickingPrimitiveShader.get());

        GLuint shaderProgram = shader->getShaderProgram();
        shader->use();

        if (app->drawingInShadow)
            setGlobalUniformsShadows(shaderProgram);
        else if (app->drawingReflex)
            setGlobalUniformsReflex(shaderProgram);
        else if (app->drawingPicking)
            setGlobalUniformsPicking(shaderProgram);
        else if (app->deferredRendering)
            setGlobalUniformsGBuffer(shaderProgram);
        else
            setGlobalUniforms(shaderProgram);

        updateBuffer(type,changed,needToResetBuffer,vao,vbo,uniforms);
        if (!app->drawingInShadow)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, app->megatexture.id);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, app->megatexture.idNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, app->megatexture.idParallax);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, app->megatexture.idReflex);

            if (dynamic_cast<LPointLight*>(app->currentLight))
            {
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_CUBE_MAP, app->currentDepthMap);
            }
            else
            {
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, app->currentDepthMap);
            }
        }

        glBindVertexArray(vao);
        //if (app->tesselation)
        //{
        //    glPatchParameteri(GL_PATCH_VERTICES, 3);
        //    glDrawElementsInstanced(GL_PATCHES, indCount, GL_UNSIGNED_INT, 0, uniforms.size());
        //}
        //else
            glDrawElementsInstanced(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0, uniforms.size());
    }

    void LImagedShape::updateUniforms(LApp* app, LGClasses primitive, LWidget::PrimitiveUniforms* buffer, size_t id)
    {
        auto path = app->primitives[primitive][id]->getDiffusePath();
        auto pathNormal = app->primitives[primitive][id]->getNormalsPath();
        auto pathDisplacement = app->primitives[primitive][id]->getDisplacementPath();
        auto pathReflex = app->primitives[primitive][id]->getReflexPath();

        const auto subtexture = app->megatexture.subtextures[path];
        const auto subtextureNormal = app->megatexture.subtexturesNormal[pathNormal];
        const auto subtextureParallax = app->megatexture.subtexturesParallax[pathDisplacement];
        const auto subtextureReflex = app->megatexture.subtexturesReflex[pathReflex];

        auto& obj = buffer[id];
        obj.model = app->primitives[primitive][id]->calculateModelMatrix();

        obj.diffuseCoords = { subtexture.first,subtexture.second };
        obj.normalCoords = { subtextureNormal.first,subtextureNormal.second };
        obj.heightCoords = { subtextureParallax.first,subtextureParallax.second };
        obj.reflexCoords = { subtextureReflex.first,subtextureReflex.second };

        obj.mapping = { app->primitives[primitive][id]->getNormalMapping(),app->primitives[primitive][id]->getParallaxMapping(),
        app->primitives[primitive][id]->getReflexMapping() };
        obj.inverseModel = glm::mat3(glm::transpose(glm::inverse(obj.model)));
        obj.ids = { id,primitive };
    }

    void LGraphics::LImagedShape::resetInstanceBuffer(GLuint vao, GLuint vbo, const std::vector<LWidget::PrimitiveUniforms>& uniforms)
    {
        const size_t vec4Size = sizeof(glm::vec4);
        const size_t vec3Size = sizeof(glm::vec3);
        const size_t vec2Size = sizeof(glm::vec2);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LWidget::PrimitiveUniforms) * uniforms.capacity(), uniforms.data(), GL_STATIC_DRAW);

        glBindVertexArray(vao);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)0);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(vec4Size));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(2 * vec4Size));
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)(3 * vec4Size));
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, diffuseCoords));
        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, normalCoords));
        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, heightCoords));
        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, reflexCoords));
        glEnableVertexAttribArray(13);
        glVertexAttribIPointer(13, 3, GL_INT, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, mapping));
        glEnableVertexAttribArray(14);
        glVertexAttribPointer(14, 3, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms), 
            (void*)offsetof(LWidget::PrimitiveUniforms, inverseModel));
        glEnableVertexAttribArray(15);
        glVertexAttribPointer(15, 3, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms),
            (void*)(offsetof(LWidget::PrimitiveUniforms, inverseModel) + vec3Size));
        glEnableVertexAttribArray(16);
        glVertexAttribPointer(16, 3, GL_FLOAT, GL_FALSE, sizeof(LWidget::PrimitiveUniforms),
            (void*)(offsetof(LWidget::PrimitiveUniforms, inverseModel) + 2 * vec3Size));
        glVertexAttribIPointer(17, 2, GL_INT, sizeof(LWidget::PrimitiveUniforms), (void*)offsetof(LWidget::PrimitiveUniforms, ids));
        glEnableVertexAttribArray(17);

        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);
        glVertexAttribDivisor(9, 1);
        glVertexAttribDivisor(10, 1);
        glVertexAttribDivisor(11, 1);
        glVertexAttribDivisor(12, 1);
        glVertexAttribDivisor(13, 1);
        glVertexAttribDivisor(14, 1);
        glVertexAttribDivisor(15, 1);
        glVertexAttribDivisor(16, 1);
        glVertexAttribDivisor(17, 1);
        glBindVertexArray(0);
        app->checkError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void LGraphics::LImagedShape::setModel(std::vector<LShape*>& changed, LShape* shape, const glm::mat4& model)
    {
        LShape::setModel(model);
        changed.push_back(shape);
    }

    void LGraphics::LImagedShape::scale(std::vector<LShape*>& changed, LShape* shape, const float x, const float y, const float z)
    {
        LShape::scale(x, y, z);
        changed.push_back(shape);
    }

    void LGraphics::LImagedShape::move(std::vector<LShape*>& changed, LShape* shape, const float x, const float y, const float z)
    {
        LShape::move(x, y, z);
        changed.push_back(shape);
    }

    void LGraphics::LImagedShape::rotateX(std::vector<LShape*>& changed, LShape* shape, float angle)
    {
        LWidget::rotateX(angle);
        changed.push_back(shape);
    }

    void LGraphics::LImagedShape::rotateY(std::vector<LShape*>& changed, LShape* shape, float angle)
    {
        LWidget::rotateY(angle);
        changed.push_back(shape);
    }

    void LGraphics::LImagedShape::rotateZ(std::vector<LShape*>& changed, LShape* shape, float angle)
    {
        LWidget::rotateZ(angle);
        changed.push_back(shape);
    }

    void LImagedShape::setNormalMapping(std::vector<LShape*>& changed, LShape* shape, bool normalMapping)
    {
        LImage::setNormalMapping(normalMapping);
        changed.push_back(shape);
    }

    void LImagedShape::setParallaxMapping(std::vector<LShape*>& changed, LShape* shape, bool parallaxMapping)
    {
        LImage::setParallaxMapping(parallaxMapping);
        changed.push_back(shape);
    }

    void LImagedShape::setReflexMapping(std::vector<LShape*>& changed, LShape* shape, bool reflexMapping)
    {
        LImage::setReflexMapping(reflexMapping);
        changed.push_back(shape);
    }
}

