/*!
@file
@brief Файл, демонстрирующий применение классов LizardGraphicss
*/

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"
#include "LTextEdit.h"
#include "additional.h"
#include <cmath>

using namespace LGraphics;

LWidgetI* prevActiveWidget = nullptr;

ivect2 getPosByCoords(LIButton* board, szvect2 screenSize, GLFWwindow* wnd, fvect3 coords)
{
    auto mouseGl = pointOnScreenToGlCoords(fvect2(screenSize), { coords.x ,coords.y});
    auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
    auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
    dist.y -= 0.172f;
    return { (int)floor((firstPosition.x + dist.x / 2 - mouseGl.x) / dist.x) + 1 ,(int)floor((firstPosition.y + dist.y / 2 - mouseGl.y) / dist.y) + 1 };
}

//fvect3 getCoordsByPosition(LIButton* board, fvect3 move)
//{
//    return fvect3();
//}

fvect3 getCoordsByPosition(LIButton* board, size_t x, size_t y)
{
    auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
    auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
    dist.y -= 0.172f;
    return { firstPosition.x - (y - 1)* dist.x, firstPosition.y - (x-1)*dist.y, firstPosition.z - dist.z };
}

void deleteByPos(LApp* app, LIButton* board, size_t x, size_t y)
{
    auto obj = app->getObjects();
    for (size_t i = 0; i < obj->size(); ++i)
    {
        if (std::abs((*obj)[i]->getMove().x - getCoordsByPosition(board, y, x).x) < 0.01 &&
            std::abs((*obj)[i]->getMove().y - getCoordsByPosition(board, y, x).y) < 0.01 &&
            (*obj)[i]->getScale() != fvect3{ 1.2f, 1.7f, 1.0f })
        {
            prevActiveWidget = nullptr;
            app->setActiveWidget(nullptr);
            (*obj).erase((*obj).begin() + i);
            delete (*obj)[i];
        }
    }
}


void moveToPos(LIButton* board, LWidgetI* checker, size_t x, size_t y)
{
    checker->move(getCoordsByPosition(board, x, y));
}


int main()
{   
    LGraphics::LApp app;
    LGraphics::LBaseComponent c;

    auto board = new LGraphics::LIButton(&app, nullptr, "d64_01.png", &c);

    board->setClickEventFunction([&]() 
    {
        if (prevActiveWidget && prevActiveWidget != board)
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(app.getWindowHandler(), &mouse_x, &mouse_y);
            auto pos = getPosByCoords(board, app.getWindowSize(), app.getWindowHandler(), {(float)mouse_x, (float)mouse_y, 1.0f});
            if (pos.x >= 1 && pos.x <= 8 && pos.y >= 1 && pos.y <= 8 && (pos.x + pos.y) % 2 == 0)
            {
                moveToPos(board, prevActiveWidget, pos.y, pos.x);
                app.setActiveWidget(prevActiveWidget);
            }
        }
    }
    );
    board->scale(1.2f, 1.7f, 1.0f);
    board->turnOffColor();

    const size_t checkersCount = 12;
    LIButton* white[checkersCount];
    for (size_t i = 0; i < checkersCount; ++i)
        white[i] = new LGraphics::LIButton(&app, nullptr, "white.png", &c);

    LIButton* black[checkersCount];
    for (size_t i = 0; i < checkersCount; ++i)
        black[i] = new LGraphics::LIButton(&app, nullptr, "black.png", &c);

    size_t counter = 0;

    for (size_t i = 1; i <= 3; ++i)
    {
        for (size_t j = 1; j <= 8; ++j)
        {
            if ((i + j) % 2 == 0)
            {
                auto coords = getCoordsByPosition(board, i, j);
                white[counter]->scale(0.09f, 0.16f, 1.0f);
                white[counter]->turnOffColor();

                white[counter]->setClickEventFunction(
                    [&]()
                {
                    prevActiveWidget = app.getActiveWidget();
                    //double mouse_x, mouse_y;
                    //glfwGetCursorPos(app.getWindowHandler(), &mouse_x, &mouse_y);
                    //auto pos = getPosByCoords(board, app.getWindowSize(), app.getWindowHandler(), { (float)mouse_x, (float)mouse_y, 1.0f });
                    //deleteByPos(&app, board, pos.x, pos.y);
                });

                white[counter]->setAnimation([&]()
                {
                    static bool fading = true;
                    if (app.getActiveWidget()->getTransparency() <= 0.25f)
                        fading = false;
                    else if (app.getActiveWidget()->getTransparency() >= 0.85f)
                        fading = true;
                    if (fading)
                        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
                    else if (!fading)
                        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
                });

                white[counter]->setBreakingAnimation([&]()
                {
                    app.getActiveWidget()->transparency(1.0f);
                });

                moveToPos(board, white[counter],i,j);
                ++counter;
            }
        }
    }

    counter = 0;
    for (size_t i = 8; i >= 6; i--)
    {
        for (size_t j = 1; j <= 8; ++j)
        {
            if ((i + j) % 2 == 0)
            {
                auto coords = getCoordsByPosition(board, i, j);
                black[counter]->scale(0.09f, 0.16f, 1.0f);
                black[counter]->turnOffColor();
                black[counter]->setClickEventFunction(
                    [&]()
                {
                    prevActiveWidget = app.getActiveWidget();
                });

                black[counter]->setAnimation([&]()
                {
                    static bool fading = true;
                    if (app.getActiveWidget()->getTransparency() <= 0.25f)
                        fading = false;
                    else if (app.getActiveWidget()->getTransparency() >= 0.85f)
                        fading = true;
                    if (fading)
                        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
                    else if (!fading)
                        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
                });

                black[counter]->setBreakingAnimation([&]()
                {
                    app.getActiveWidget()->transparency(1.0f);
                });

                moveToPos(board, black[counter], i, j);
                ++counter;
            }
        }
    }

    //auto white = new LGraphics::LIRectangle(&app, nullptr, "white.png", &c);
    //white->scale(0.09f, 0.16f, 1.0f);
    //white->turnOffColor();
    //white->move(fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f,board->getBottomLeftCorner().z});

    //auto black = new LGraphics::LIRectangle(&app, nullptr, "black.png", &c);
    //black->scale(0.09f, 0.16f, 1.0f);
    //black->turnOffColor();
    //black->move(fvect3{ white->getMove().x - dist.x,white->getMove().y,white->getMove().z });

    //auto e = new LIButton(&app, nullptr, "image.png",&c);
    //auto d = new LIButton(&app, nullptr, "image.png", &c);

    //app.setWindowedMode();
    //auto textEdit = new LTextEdit(&app, "Hello, world!\nHello, world!\nHello, world!\n", nullptr, "test.png", &c);
    //textEdit->turnOffTexture();
    //textEdit->color(255, 255, 255);
    //textEdit->move(app.getWindowSize().x/2, app.getWindowSize().y / 2);
    //textEdit->setVerticalScroller(new LGraphics::LVerticalScroller(&app, nullptr, "", &c));
    ////textEdit->setHorizontalScroller(new LGraphics::LHorizontalScroller(&app, nullptr, "", &c));
    ////textEdit->scale({ 1.5f,1.8f,1.0f });
    ////textEdit->transparency(0.85f);
    ////textEdit->move(700, 500);

    //e->scale({ 0.1f,0.1f,1.0f });
    //e->move(100,50);
    //e->color(255, 0, 0);

    //d->scale({ 0.1f,0.1f,1.0f });
    //d->move(700, 50);
    //d->color(0, 0, 0);
    //d->setLabel("aue");
    //d->setLabelColor(255, 255, 255);
    //d->turnOffTexture();

    //e->setClickEventFunction([&]() {textEdit->scale({ 1.5f,0.5f,1.0f }); });
    //d->setClickEventFunction([&]() {textEdit->move(app.getWindowSize().x / 3, app.getWindowSize().y / 2); });

    app.loop();
    return 0;
}