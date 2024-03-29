#pragma once
#include "debug_figures.h"
#include "game_pad.h"
#include "graphics.h"
#include "mouse.h"
#include "resource_manager.h"
#include "debug_figures.h"

class GenericFrequentEntities
{
public:
    //--------<constructor/destructor>--------//
    GenericFrequentEntities() {}
    virtual ~GenericFrequentEntities() {}
protected:
    //--------< 関数 >--------//
    /*呼び出しはFrameworkのみ*/
    void entities_initialize();
    /*呼び出しはFrameworkのみ*/
    void entities_uninitialize();
    //--------< 変数 >--------//
    static std::unique_ptr<ResourceManager> resource_manager;
    static std::unique_ptr<DebugFigures> debug_figure;
    //static std::unique_ptr<Mouse> mouse;
   // static std::unique_ptr<GamePad> game_pad;
};