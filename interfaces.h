#include "LizardGraphics/LApp.h"
#include "LizardGraphics/LWRectangle.h"
#include "LizardGraphics/LModel.h"
#include "LizardGraphics/LTimer.h"
#include "LizardGraphics/LLogger.h"
#include "LizardGraphics/FileSystemDialog.h"

namespace LGraphics
{
    class ImGuiInterface
    {
    public:
        ImGuiInterface(LApp* app)
            :poolSize(app->info.poolSize)
        {
            this->app = app;
            pressed = new bool[poolSize];
            sliderRanges = new SliderRanges[poolSize];
            std::fill(pressed, pressed + poolSize, false);
            currentPath = std::filesystem::current_path().generic_string();
            vectors.insert(std::make_pair(MOVE, "move_"));
            vectors.insert(std::make_pair(SCALE, "scale_"));
        }

        ~ImGuiInterface()
        {
            delete[] pressed;
            delete[] sliderRanges;
        }

    private:

        LApp* app;

        static const size_t pathSize = 128;
        static constexpr size_t coordsCount = 3;
        const size_t poolSize;

        std::unordered_map<uint8_t, std::string> vectors;
        enum VectorTypes : uint8_t
        {
            MOVE,
            SCALE,
        };

        struct SliderRanges
        {
            static const size_t vectorTypes = 2;
            static const size_t rangeEndsCount = 2;
            float ranges[coordsCount * vectorTypes* rangeEndsCount];
            bool inited = false;
        };

        std::string currentPath;

        char* fileSystemArr = nullptr;
        char texturePath_[pathSize] = "";
        char modelPath_[pathSize] = "";
        char createObjName[32] = "";
        char objName[32] = "";
        bool lockCamOnObject = false;
        bool* pressed;
        bool deleteDialog = false;
        bool firstDialog = false;
        SliderRanges* sliderRanges;

        bool radioRectangle = false, radioModel = false, fileSystem = false;

public:
        void imguiInterface()
        {
            //LOG_CALL
            if (!app->drawUI_)
                return;
            ImGui::Begin("fps", 0,
                ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_::ImGuiWindowFlags_NoMouseInputs);

            ImGui::TextColored({ 0.0f / 255.0f,0.0f / 255.0f,255.0f / 255.0f,1 }, "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::TextColored({ 0.0f,0.0f,1.0f,1.0f }, "camera pos: x = %.3f, y = %.3f, z = %.3f",app->cameraPos.x, app->cameraPos.y, app->cameraPos.z);
            ImGui::TextColored({ 0.0f,0.0f,1.0f,1.0f }, "camera front: x = %.3f, y = %.3f, z = %.3f", app->cameraFront.x, app->cameraFront.y, app->cameraFront.z);
            ImGui::TextColored({ 0.0f / 255.0f,0.0f / 255.0f,255.0f / 255.0f,1 }, "renderer: %s", app->info.api == L_OPENGL ? "OpenGL" : "Vulkan");
            ImGui::End();

            auto fileDialog = [&](char* arr)
            {
                fileSystem = true;
                fileSystemArr = arr;
            };

            //auto drawFileDialog = [&]
            //{

            //}

            ImGui::Begin("create");
            if (ImGui::RadioButton("LWRectangle", radioRectangle))
            {
                radioRectangle = true;
                radioModel = false;
            }
            if (ImGui::RadioButton("LModel", radioModel))
            {
                radioModel = true;
                radioRectangle = false;
            }

            if (radioRectangle)
            {
                ImGui::Text("texture path:");
                ImGui::SameLine();
                ImGui::PushID(0);
                ImGui::InputText("", texturePath_, sizeof(texturePath_));
                ImGui::PopID();
                ImGui::SameLine();
                if (ImGui::Button("browse"))
                    fileDialog(texturePath_);
                ImGui::Text("object name: ");
                ImGui::SameLine();
                ImGui::PushID(1);
                ImGui::InputText("", createObjName, sizeof(createObjName));
                ImGui::PopID();
            }

            else if (radioModel)
            {
                ImGui::PushID(0);
                ImGui::Text("model path:  ");
                ImGui::SameLine();
                ImGui::InputText("", modelPath_, sizeof(modelPath_));
                ImGui::SameLine();
                if (ImGui::Button("browse"))
                    fileDialog(modelPath_);
                ImGui::PopID();

                ImGui::PushID(1);
                ImGui::Text("texture path:");
                ImGui::SameLine();
                ImGui::InputText("", texturePath_, sizeof(texturePath_));
                ImGui::SameLine();
                if (ImGui::Button("browse"))
                    fileDialog(texturePath_);
                ImGui::PopID();

                ImGui::Text("object name: ");
                ImGui::SameLine();
                ImGui::InputText("", createObjName, sizeof(createObjName));
            }

            if (radioRectangle || radioModel)
            {
                if (ImGui::Button("create"))
                {
                    if (radioModel)
                    {
                        //auto m = new LModel(app, modelPath_, texturePath_);
                        //m->setName(createObjName);
                    }
                    else if (radioRectangle)
                    {
                        auto r = new LWRectangle(app, texturePath_);
                        r->setName(createObjName);
                    }
                    else
                    {

                    }
                }
            }

            ImGui::End();
           
            if (fileSystem)
            {
                ImGui::Begin("filesystem");
                if (strlen(fileSystemArr))
                {
                    std::string path(fileSystemArr);
                    currentPath = path.substr(0, path.find_last_of("/"));
                    fileSystemArr[0] = '\0';
                }

                currentPath = FileSystemDialog::startDialog(currentPath.data());
                if (!std::filesystem::is_directory(currentPath))
                {
                    memcpy(fileSystemArr, currentPath.data(), currentPath.size());
                    currentPath = std::filesystem::current_path().generic_string();
                    fileSystem = false;
                }
                ImGui::End();
            }

              
            ImGui::Begin("Objects redactor", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
            if (ImGui::RadioButton("lock camera on object", lockCamOnObject))
                lockCamOnObject = !lockCamOnObject;
            ImGui::BeginChild("Scrolling");

            auto drawObjectUI = [&](int i, LWidget* obj, std::string name_)
            {
                //const auto& obj = app->getModels()[i];
                std::string name = strlen(obj->getName().data()) ? obj->getName() : name_ + std::to_string(i);
                auto pressed_ = ImGui::Button(name.data());

                if (pressed_ && lockCamOnObject)
                {
                    app->setCameraPos({ obj->getMove().x, obj->getMove().y + 10.0f, obj->getMove().z + 5.0f });
                    app->setCameraFront(glm::normalize(obj->getMove() - app->getCameraPos()));
                }

                pressed[i] = pressed_ ? !pressed[i] : pressed[i];
                ImGui::SameLine();
                if (ImGui::Button("X") || deleteDialog)
                {
                    if (!firstDialog)
                    {
                        if (deleteDialog) firstDialog = true;
                        deleteDialog = true;
                        ImGui::Begin("Delete object", 0, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize
                        | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
                        ImGui::Text("Are you sure you want to delete this object?");
                        if (ImGui::Button("Yes"))
                        {
                            sliderRanges[i].inited = false;
                            if (obj->getObjectType() == std::string("LModel"))
                                app->addObjectToDelete(obj, L_MODEL);
                            else if (obj->getObjectType() == std::string("LWRectangle"))
                                app->addObjectToDelete(obj, L_PRIMITIVE);
                            deleteDialog = false;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No"))
                            deleteDialog = false;
                        ImGui::End();
                    }
                }

                constexpr float range = 0.01f;
                constexpr float halfRange = range / 2.0f;

                if (pressed[i])
                {
                    auto& degrees = obj->getRotateDegrees();
                    auto ranges = sliderRanges[i];

                    //app->lockFrontViewCamera();

                    // support functions

                    auto initRanges = [&](const glm::vec3* vec, VectorTypes vectorType)
                    {

                        const auto leftEndOffset = vectorType * coordsCount * SliderRanges::rangeEndsCount;
                        const auto rightEndOffset = leftEndOffset + coordsCount;

                        ranges.ranges[leftEndOffset] = vec->x - range;
                        ranges.ranges[rightEndOffset] = vec->x + range;
                        ranges.ranges[leftEndOffset + 1] = vec->y - range;
                        ranges.ranges[rightEndOffset + 1] = vec->y + range;
                        ranges.ranges[leftEndOffset + 2] = vec->z - range;
                        ranges.ranges[rightEndOffset + 2] = vec->z + range;
                    };


                    auto checkRanges = [&](const glm::vec3& vec, VectorTypes vectorType, size_t coordNum)
                    {
                        const auto leftOffset = vectorType * coordsCount * SliderRanges::rangeEndsCount;
                        const auto rightOffset = leftOffset + coordsCount;

                        if (vec[coordNum] <= ranges.ranges[leftOffset + coordNum])
                        {
                            ranges.ranges[leftOffset + coordNum] -= halfRange;
                            ranges.ranges[rightOffset + coordNum] -= halfRange;
                        }
                        else if (vec[coordNum] >= ranges.ranges[rightOffset + coordNum])
                        {
                            ranges.ranges[leftOffset + coordNum] += halfRange;
                            ranges.ranges[rightOffset + coordNum] += halfRange;
                        }
                    };

                    auto vectorGui = [&](glm::vec3& vec, VectorTypes type, size_t coordNum)
                    {
                        const auto offset = type * coordsCount * SliderRanges::rangeEndsCount + coordNum;
                        ImGui::PushID(coordsCount * type + coordNum);
                        if (ImGui::Button("<"))
                            vec[coordNum] -= range;
                        ImGui::SameLine();
                        ImGui::SliderFloat("", &vec[coordNum], ranges.ranges[offset], ranges.ranges[offset + 3]);
                        ImGui::SameLine();
                        if (ImGui::Button(">"))
                            vec[coordNum] += range;
                        ImGui::SameLine();
                        if (coordNum == 0) ImGui::Text((vectors[type] + 'x').data());
                        else if (coordNum == 1) ImGui::Text((vectors[type] + 'y').data());
                        else if (coordNum == 2) ImGui::Text((vectors[type] + 'z').data());
                        ImGui::PopID();
                    };

                    auto doLogick = [&](std::array<std::pair<glm::vec3*, VectorTypes>, SliderRanges::vectorTypes>& vectors)
                    {
                        for (auto& i : vectors)
                        {
                            if (!ranges.inited)
                            {
                                initRanges(i.first, i.second);
                            }
                            for (size_t j = 0; j < coordsCount; j++)
                            {
                                checkRanges(*i.first, i.second, j);
                                vectorGui(*i.first, i.second, j);
                            }

                        }
                        ranges.inited = true;
                    };

                    //////////////////////////////////////////////////////////////////////


                    std::array<std::pair<glm::vec3*, VectorTypes>, SliderRanges::vectorTypes> vectors_
                    {
                        std::make_pair(&obj->getMoveRef(),MOVE),
                        std::make_pair(&obj->getScaleRef(),SCALE),
                    };

                    doLogick(vectors_);

                    ImGui::PushID(INT_MAX - 1 - i);
                    constexpr float range = 1.0f;
                    if (ImGui::Button("<"))
                        obj->rotateX(-range);
                    ImGui::SameLine();
                    if (ImGui::SliderFloat("", &degrees.x, -360.0f, 360.0f))
                        obj->rotateX(degrees.x - obj->getRotateDegrees().x);
                    ImGui::SameLine();
                    if (ImGui::Button(">"))
                        obj->rotateX(range);
                    ImGui::SameLine();
                    ImGui::Text("rotate_x");
                    ImGui::PopID();

                    ImGui::PushID(INT_MAX - 2 - i);
                    if (ImGui::Button("<"))
                        obj->rotateY(-range);
                    ImGui::SameLine();
                    if (ImGui::SliderFloat("", &degrees.y, -360.0f, 360.0f))
                        obj->rotateY(degrees.y - obj->getRotateDegrees().y);
                    ImGui::SameLine();
                    if (ImGui::Button(">"))
                        obj->rotateY(range);
                    ImGui::SameLine();
                    ImGui::Text("rotate_y");
                    ImGui::PopID();

                    ImGui::PushID(INT_MAX - 3 - i);
                    if (ImGui::Button("<"))
                        obj->rotateZ(-range);
                    ImGui::SameLine();
                    if (ImGui::SliderFloat("", &degrees.z, -360.0f, 360.0f))
                        obj->rotateZ(degrees.z - obj->getRotateDegrees().z);
                    ImGui::SameLine();
                    if (ImGui::Button(">"))
                        obj->rotateZ(range);
                    ImGui::SameLine();
                    ImGui::Text("rotate_z");
                    ImGui::PopID();

                    ImGui::InputText("", objName, sizeof(objName));
                    ImGui::SameLine();
                    ImGui::Text("name");
                    obj->setName(objName);
                }
            };
            for (size_t i = 0; i < app->getModels().size(); ++i)
            {
                ImGui::PushID(i);
                drawObjectUI(i, app->getModels()[i], "model");
                ImGui::PopID();
            }

            for (size_t i = 0; i < app->getPrimitives().size(); ++i)
            {
                const size_t id = i + app->getModels().size();
                ImGui::PushID(id);
                drawObjectUI(id, app->getPrimitives()[i], "rect");
                ImGui::PopID();
            }
            firstDialog = false;
            ImGui::EndChild();
            ImGui::End();
        }
    };
    
}
