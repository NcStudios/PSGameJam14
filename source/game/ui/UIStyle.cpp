#include "UIStyle.h"

#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/window/Window.h"

#include "imgui/imgui.h"

namespace
{
constexpr auto DarkBlue = nc::Vector4{0.004f, 0.043f, 0.071f, 0.85f};
constexpr auto Orange = nc::Vector4{1.0f, .427f, 0.04f, 1.0f};
constexpr auto Red = nc::Vector4{0.733f, .251f, 0.251f, 1.0f};
}

namespace game::ui
{
void SetUIStyle()
{
    auto& style = ImGui::GetStyle();

    auto [xScale, yScale] = nc::window::GetContentScale();
    style.ScaleAllSizes(std::floor(std::max(xScale, yScale)));

    constexpr auto rounding = 8.0f;
    style.WindowRounding = rounding;
    style.ChildRounding = rounding;
    style.FrameRounding = rounding;
    style.PopupRounding = rounding;

    constexpr auto borderSize = 2.0f;
    style.WindowBorderSize = borderSize;
    style.ChildBorderSize = borderSize;
    style.FrameBorderSize = borderSize;
    style.PopupBorderSize = borderSize;

    style.WindowMenuButtonPosition = ImGuiDir_None;
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = Orange;
    colors[ImGuiCol_TextDisabled]          = nc::ui::default_scheme::Disabled;
    colors[ImGuiCol_WindowBg]              = DarkBlue;
    colors[ImGuiCol_ChildBg]               = nc::ui::default_scheme::ChildBG;
    colors[ImGuiCol_PopupBg]               = nc::ui::default_scheme::PopupBG;
    colors[ImGuiCol_Border]                = Orange;
    colors[ImGuiCol_BorderShadow]          = nc::ui::default_scheme::ScrollBarBG;
    colors[ImGuiCol_FrameBg]               = nc::ui::default_scheme::Title;
    colors[ImGuiCol_FrameBgHovered]        = nc::ui::default_scheme::Hovered;
    colors[ImGuiCol_FrameBgActive]         = nc::ui::default_scheme::Active;
    colors[ImGuiCol_TitleBg]               = nc::ui::default_scheme::Title;
    colors[ImGuiCol_TitleBgActive]         = nc::ui::default_scheme::Title;
    colors[ImGuiCol_TitleBgCollapsed]      = nc::ui::default_scheme::Title;
    colors[ImGuiCol_MenuBarBg]             = nc::ui::default_scheme::ChildBG;
    colors[ImGuiCol_ScrollbarBg]           = nc::ui::default_scheme::ScrollBarBG;
    colors[ImGuiCol_ScrollbarGrab]         = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_ScrollbarGrabHovered]  = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_ScrollbarGrabActive]   = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_CheckMark]             = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_SliderGrab]            = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_SliderGrabActive]      = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_Button]                = nc::ui::default_scheme::Button;
    colors[ImGuiCol_ButtonHovered]         = nc::ui::default_scheme::Hovered;
    colors[ImGuiCol_ButtonActive]          = nc::ui::default_scheme::Active;
    colors[ImGuiCol_Header]                = nc::ui::default_scheme::WindowBG;
    colors[ImGuiCol_HeaderHovered]         = nc::ui::default_scheme::Tabs;
    colors[ImGuiCol_HeaderActive]          = nc::ui::default_scheme::Hovered;
    colors[ImGuiCol_Separator]             = nc::ui::default_scheme::Separator;
    colors[ImGuiCol_SeparatorHovered]      = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_SeparatorActive]       = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_ResizeGrip]            = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_ResizeGripHovered]     = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_ResizeGripActive]      = nc::ui::default_scheme::AccentActive;
    colors[ImGuiCol_Tab]                   = nc::ui::default_scheme::Tabs;
    colors[ImGuiCol_TabHovered]            = nc::ui::default_scheme::Title;
    colors[ImGuiCol_TabActive]             = nc::ui::default_scheme::Title;
    colors[ImGuiCol_TabUnfocused]          = nc::ui::default_scheme::Tabs;
    colors[ImGuiCol_TabUnfocusedActive]    = nc::ui::default_scheme::Tabs;
    colors[ImGuiCol_TextSelectedBg]        = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_DragDropTarget]        = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_NavHighlight]          = nc::ui::default_scheme::Accent;
    colors[ImGuiCol_NavWindowingHighlight] = nc::ui::default_scheme::WindowHighlight;
    colors[ImGuiCol_NavWindowingDimBg]     = nc::ui::default_scheme::DimBg;
    colors[ImGuiCol_ModalWindowDimBg]      = nc::ui::default_scheme::DimBg;
    colors[ImGuiCol_PlotHistogram]         = nc::ui::default_scheme::AccentDim;
    colors[ImGuiCol_PlotHistogramHovered]  = nc::ui::default_scheme::Accent;
}
} // namespace game::ui
