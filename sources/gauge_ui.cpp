#include "gauge_ui.h"
#include "Operators.h"
#include "user.h"
#include "imgui_include.h"

GaugeUi::GaugeUi(const wchar_t* back_filename, const wchar_t* body_filename, const wchar_t* frame_filename)
{
    Graphics& graphics = Graphics::instance();
    back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), back_filename, 1);
    body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), body_filename, 2);
    if (frame_filename)
    {
        frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), frame_filename, 1);
    }

    now_percent = 1.0f;
    old_percent = 1.0f;
    diff_color = { 2.0f,2.0f, 1.0f, 1.0f };

    gauge.texsize = { static_cast<float>(back->get_texture2d_desc().Width), static_cast<float>(back->get_texture2d_desc().Height) };

}

void GaugeUi::update(float elapsed_time)
{
    //ゲージパーセントが減った場合の差分ゲージの処理
    if (now_percent < old_percent)
    {
        const float min_rate = 0.5f;
        const float max_rate = 1.0f;
        const float diff_rate = lerp(min_rate, max_rate, (1 - (now_percent / old_percent))) * elapsed_time;
        old_percent = lerp(old_percent, now_percent, diff_rate);
    }
}

void GaugeUi::render(ID3D11DeviceContext* dc)
{
    gauge.angle = 90;
    //--back--//
    back->begin(dc);
    back->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
    back->end(dc);
    //--body--//
    body->begin(dc);
    //ゲージ差分
    body->render(dc, gauge.position, gauge.scale, gauge.pivot, diff_color, gauge.angle, gauge.texpos,
        { gauge.texsize.x * old_percent, gauge.texsize.y });
    //ゲージ本体
    body->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos,
        { gauge.texsize.x * now_percent, gauge.texsize.y });
    body->end(dc);
    //--frame--//
    if (frame)
    {
        frame->begin(dc);
        frame->render(dc, gauge.position, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
        frame->end(dc);
    }
}
