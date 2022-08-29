#include "ini.hpp"

namespace GView::Type::INI::Plugins
{
using namespace GView::View::LexicalViewer;
using namespace AppCUI::Controls;
using namespace AppCUI::Utils;

constexpr int BUTTON_OK_ID     = 123;
constexpr int BUTTON_CANCEL_ID = 124;



class SelectCaseDialog : public Window
{
    Reference<ComboBox> comboSections, comboKeys;

  public:
    SelectCaseDialog() : Window("Case-ing", "d:c,w:70,h:10", WindowFlags::ProcessReturn)
    {
        Factory::Label::Create(this, "&Sections", "x:1,y:1,w:10");
        Factory::Label::Create(this, "&Keys", "x:1,y:3,w:10");
        comboSections = Factory::ComboBox::Create(this, "x:15,y:1,w:52", "Do nothing,Upper case,Lower case");
        comboSections->SetCurentItemIndex(0);
        comboSections->SetHotKey('S');
        comboKeys = Factory::ComboBox::Create(this, "x:15,y:3,w:52", "Do nothing,Upper case,Lower case");
        comboKeys->SetCurentItemIndex(0);
        comboKeys->SetHotKey('K');
        Factory::Button::Create(this, "&Run", "l:21,b:0,w:13", BUTTON_OK_ID);
        Factory::Button::Create(this, "&Cancel", "l:36,b:0,w:13", BUTTON_CANCEL_ID);
        comboSections->SetFocus();
    }
    bool OnEvent(Reference<Controls::Control> control, Controls::Event eventType, int controlID) override
    {
        if ((eventType == Event::WindowAccept) || ((eventType == Event::ButtonClicked) && (controlID == BUTTON_OK_ID)))
        {
            this->Exit(Dialogs::Result::Ok);
            return true;
        }
        if ((eventType == Event::WindowClose) || ((eventType == Event::ButtonClicked) && (controlID == BUTTON_CANCEL_ID)))
        {
            this->Exit(Dialogs::Result::Cancel);
            return true;
        }
        return false;
    }
    CaseFormat GetCaseFormatForSections()
    {
        return static_cast<CaseFormat>(comboSections->GetCurrentItemIndex());
    }
    CaseFormat GetCaseFormatForKeys()
    {
        return static_cast<CaseFormat>(comboKeys->GetCurrentItemIndex());
    }
};

std::string_view Casing::GetName()
{
    return "Change case-ing";
}
std::string_view Casing::GetDescription()
{
    return "Change case-ing for keys and sections";
}
bool Casing::CanBeAppliedOn(const PluginData& data)
{
    return true;
}
void Casing::ChangeCaseForToken(Token& tok, CaseFormat format, bool isSection)
{
    // need to be implemented
}

PluginAfterActionRequest Casing::Execute(PluginData& data)
{
    SelectCaseDialog dlg;
    if (dlg.Show() != (int) Dialogs::Result::Ok)
        return PluginAfterActionRequest::None;
    auto sectionAction = dlg.GetCaseFormatForSections();
    auto keysAction    = dlg.GetCaseFormatForKeys();
    if ((sectionAction == CaseFormat::None) && (keysAction == CaseFormat::None))
        return PluginAfterActionRequest::None;

    auto len = data.tokens.Len();
    for (auto index = 0U; index < len; index++)
    {
        auto tok  = data.tokens[index];
        auto type = tok.GetTypeID(TokenType::Invalid);
        if ((type == TokenType::Section) && (sectionAction != CaseFormat::None))
            ChangeCaseForToken(tok, sectionAction, true);
        if ((type == TokenType::Key) && (keysAction != CaseFormat::None))
            ChangeCaseForToken(tok, keysAction, false);
    }
    return PluginAfterActionRequest::Refresh;
}
} // namespace GView::Type::INI::Plugins