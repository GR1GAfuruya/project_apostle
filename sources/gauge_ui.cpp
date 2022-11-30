#include "gauge_ui.h"
#include "Operators.h"
#include "user.h"
#include "imgui_include.h"

GaugeUi::GaugeUi(Graphics& graphics, const wchar_t* back_filename, const wchar_t* body_filename, const wchar_t* frame_filename)
{
    back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), back_filename, 1);
    body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), body_filename, 1);
    if (frame_filename)
    {
        body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), frame_filename, 1);
    }

    percent = 1.0f;
    gauge.texsize = { static_cast<float>(back->get_texture2d_desc().Width), static_cast<float>(back->get_texture2d_desc().Height) };

}

void GaugeUi::update(Graphics& graphics, float elapsed_time)
{
  
    //アニメーションなどを行う　＊現在は処理なし
}

void GaugeUi::render(ID3D11DeviceContext* dc)
{
   
    //--back--//
    back->begin(dc);
    back->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
    back->end(dc);
    //--body--//
    body->begin(dc);
    body->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos,
        { gauge.texsize.x * percent, gauge.texsize.y });
    body->end(dc);
    //--frame--//
    if (frame)
    {
        frame->begin(dc);
        frame->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
        frame->end(dc);
    }
}
