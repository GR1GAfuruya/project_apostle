
#include "generic_frequent_entities.h"

std::unique_ptr<ResourceManager> GenericFrequentEntities::resource_manager = nullptr;
std::unique_ptr<DebugFigures>    GenericFrequentEntities::debug_figure = nullptr;
//std::unique_ptr<Mouse>           GenericFrequentEntities::mouse = nullptr;
//std::unique_ptr<GamePad>         GenericFrequentEntities::game_pad = nullptr;

void GenericFrequentEntities::entities_initialize()
{
    Graphics& graphics = Graphics::instance();
    // resource_manager
    resource_manager = std::make_unique<ResourceManager>();
    // debug_figure
    debug_figure = std::make_unique<DebugFigures>(graphics.get_device().Get());
}

void GenericFrequentEntities::entities_uninitialize()
{
    // ���̃N���X�͊e���͈̂���������Ȃ����f�X�g���N�^�͌p�������񐔕��Ă΂��̂�
    // �����Ŗ����I�ɉ�����Ȃ��Ɠ��ڈȍ~�̃f�X�g���N�^�ŗ����Ă��܂��B
    if (resource_manager != nullptr) { resource_manager.reset(nullptr); }
    if (debug_figure != nullptr) { debug_figure.reset(nullptr); }
}