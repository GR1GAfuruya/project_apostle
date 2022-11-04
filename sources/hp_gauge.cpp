#include "hp_gauge.h"
#include "Operators.h"
#include "user.h"
#include "imgui_include.h"

void HpGauge::update(Graphics& graphics, float elapsed_time)
{
  
    //アニメーションなどを行う　＊現在は処理なし
}

void HpGauge::render(ID3D11DeviceContext* dc)
{
    //--back--//
    back->begin(dc);
    back->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
    back->end(dc);
    //--body--//
    body->begin(dc);
    body->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos,
        { gauge.texsize.x * hp_percent, gauge.texsize.y });
    body->end(dc);
    //--frame--//
    if (frame)
    {
        frame->begin(dc);
        frame->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
        frame->end(dc);
    }
}
