// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Utils/ValueUtils.h>

#include <folly/dynamic.h>
#include <stdint.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <Views/ShadowNodeBase.h>

namespace winrt {
using namespace Windows::UI::Xaml;
}

namespace react { namespace uwp {

struct ShadowNodeBase;

static double DefaultOrOverride(double defaultValue, double x) {
  return x != c_UndefinedEdge ? x : defaultValue;
};

inline winrt::Windows::UI::Xaml::Thickness GetThickness(double thicknesses[ShadowEdges::CountEdges], bool isRTL)
{
  const double defaultWidth = std::max<double>(0, thicknesses[ShadowEdges::AllEdges]);
  double startWidth = DefaultOrOverride(thicknesses[ShadowEdges::Left], thicknesses[ShadowEdges::Start]);
  double endWidth = DefaultOrOverride(thicknesses[ShadowEdges::Right], thicknesses[ShadowEdges::End]);
  if (isRTL)
    std::swap(startWidth, endWidth);

  // Compute each edge.  Most specific setting wins, so fill from broad to narrow: all, horiz/vert, start/end, left/right
  winrt::Windows::UI::Xaml::Thickness thickness = { defaultWidth, defaultWidth, defaultWidth, defaultWidth };

  if (thicknesses[ShadowEdges::Horizontal] != c_UndefinedEdge)
    thickness.Left = thickness.Right = thicknesses[ShadowEdges::Horizontal];
  if (thicknesses[ShadowEdges::Vertical] != c_UndefinedEdge)
    thickness.Top = thickness.Bottom = thicknesses[ShadowEdges::Vertical];

  if (startWidth != c_UndefinedEdge)
    thickness.Left = startWidth;
  if (endWidth != c_UndefinedEdge)
    thickness.Right = endWidth;
  if (thicknesses[ShadowEdges::Top] != c_UndefinedEdge)
    thickness.Top = thicknesses[ShadowEdges::Top];
  if (thicknesses[ShadowEdges::Bottom] != c_UndefinedEdge)
    thickness.Bottom = thicknesses[ShadowEdges::Bottom];

  return thickness;
}

inline winrt::Windows::UI::Xaml::CornerRadius GetCornerRadius(double cornerRadii[ShadowCorners::CountCorners], bool isRTL)
{
  winrt::Windows::UI::Xaml::CornerRadius cornerRadius;
  const double defaultRadius = std::max<double>(0, cornerRadii[ShadowCorners::AllCorners]);
  double topStartRadius = DefaultOrOverride(cornerRadii[ShadowCorners::TopLeft], cornerRadii[ShadowCorners::TopStart]);
  double topEndRadius = DefaultOrOverride(cornerRadii[ShadowCorners::TopRight], cornerRadii[ShadowCorners::TopEnd]);
  double bottomStartRadius = DefaultOrOverride(cornerRadii[ShadowCorners::BottomLeft], cornerRadii[ShadowCorners::BottomStart]);
  double bottomEndRadius = DefaultOrOverride(cornerRadii[ShadowCorners::BottomRight], cornerRadii[ShadowCorners::BottomEnd]);
  if (isRTL)
  {
    std::swap(topStartRadius, topEndRadius);
    std::swap(bottomStartRadius, bottomEndRadius);
  }

  cornerRadius.TopLeft = DefaultOrOverride(defaultRadius, topStartRadius);
  cornerRadius.TopRight = DefaultOrOverride(defaultRadius, topEndRadius);
  cornerRadius.BottomLeft = DefaultOrOverride(defaultRadius, bottomStartRadius);
  cornerRadius.BottomRight = DefaultOrOverride(defaultRadius, bottomEndRadius);

  return cornerRadius;
}

inline bool IsFlowRTL(const XD::IXamlDirectObject& element)
{
  return XamlDirectInstance::GetXamlDirect().GetEnumProperty(element, XD::XamlPropertyIndex::FrameworkElement_FlowDirection) == static_cast<uint32_t>(winrt::FlowDirection::RightToLeft);
}

inline void UpdatePadding(ShadowNodeBase* node, const XD::IXamlDirectObject& element, ShadowEdges edge, double margin)
{
  node->m_padding[edge] = margin;
  winrt::Thickness thickness = GetThickness(node->m_padding, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_Padding, winrt::box_value(thickness));
}

inline void SetBorderThickness(ShadowNodeBase* node, XD::IXamlDirectObject& element, ShadowEdges edge, double margin)
{
  node->m_border[edge] = margin;
  winrt::Thickness thickness = GetThickness(node->m_border, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_BorderThickness, winrt::box_value(thickness));
}

inline void SetBorderBrush(const XD::IXamlDirectObject& element, const winrt::Windows::UI::Xaml::Media::Brush& brush)
{
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_BorderBrush, winrt::box_value(brush));
}

inline bool TryUpdateBackgroundBrush(XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "backgroundColor")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_Background, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_Background);

    return true;
  }

  return false;
}

inline void UpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject& element, ShadowCorners corner, double newValue)
{
  node->m_cornerRadius[corner] = newValue;
  winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_CornerRadius, winrt::box_value(cornerRadius));
}

inline bool TryUpdateForeground(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "color")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_Foreground, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_Foreground);

    return true;
  }

  return false;
}

inline bool TryUpdateBorderProperties(ShadowNodeBase* node, XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  bool isBorderProperty = true;

  if (propertyName == "borderColor")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_BorderBrush, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_BorderBrush);
  }
  else if (propertyName == "borderLeftWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Left, propertyValue.asDouble());
  }
  else if (propertyName == "borderTopWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Top, propertyValue.asDouble());
  }
  else if (propertyName == "borderRightWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Right, propertyValue.asDouble());
  }
  else if (propertyName == "borderBottomWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Bottom, propertyValue.asDouble());
  }
  else if (propertyName == "borderStartWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Start, propertyValue.asDouble());
  }
  else if (propertyName == "borderEndWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::End, propertyValue.asDouble());
  }
  else if (propertyName == "borderWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::AllEdges, propertyValue.asDouble());
  }
  else
  {
    isBorderProperty = false;
  }

  return isBorderProperty;
}

inline bool TryUpdatePadding(ShadowNodeBase* node, const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Left, propertyValue.asDouble());
  }
  else if (propertyName == "paddingTop")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Top, propertyValue.asDouble());
  }
  else if (propertyName == "paddingRight")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Right, propertyValue.asDouble());
  }
  else if (propertyName == "paddingBottom")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Bottom, propertyValue.asDouble());
  }
  else if (propertyName == "paddingStart")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Start, propertyValue.asDouble());
  }
  else if (propertyName == "paddingEnd")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::End, propertyValue.asDouble());
  }
  else if (propertyName == "paddingHorizontal")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Horizontal, propertyValue.asDouble());
  }
  else if (propertyName == "paddingVertical")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Vertical, propertyValue.asDouble());
  }
  else if (propertyName == "padding")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::AllEdges, propertyValue.asDouble());
  }
  else
  {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

bool TryUpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "borderTopLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopLeft, propertyValue.asDouble());
  }
  else if (propertyName == "borderTopRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopRight, propertyValue.asDouble());
  }
  if (propertyName == "borderTopStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopStart, propertyValue.asDouble());
  }
  else if (propertyName == "borderTopEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopEnd, propertyValue.asDouble());
  }
  else if (propertyName == "borderBottomRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomRight, propertyValue.asDouble());
  }
  else if (propertyName == "borderBottomLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomLeft, propertyValue.asDouble());
  }
  else if (propertyName == "borderBottomStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomStart, propertyValue.asDouble());
  }
  else if (propertyName == "borderBottomEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomEnd, propertyValue.asDouble());
  }
  else if (propertyName == "borderRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::AllCorners, propertyValue.asDouble());
  }
  else
  {
    return false;
  }

  return true;
}

inline bool TryUpdateFontProperties(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  bool isFontProperty = true;

  if (propertyName == "fontSize")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, XD::XamlPropertyIndex::Control_FontSize, propertyValue.asDouble());
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_FontSize);
  }
  else if (propertyName == "fontFamily")
  {
    if (propertyValue.isString())
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_FontFamily, winrt::box_value(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue))));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_FontFamily);
  }
  else if (propertyName == "fontWeight")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      winrt::Windows::UI::Text::FontWeight fontWeight;
      if (value == "normal")
        fontWeight = winrt::Windows::UI::Text::FontWeights::Normal();
      else if (value == "bold")
        fontWeight = winrt::Windows::UI::Text::FontWeights::Bold();
      else if (value == "100")
        fontWeight.Weight = 100;
      else if (value == "200")
        fontWeight.Weight = 200;
      else if (value == "300")
        fontWeight.Weight = 300;
      else if (value == "400")
        fontWeight.Weight = 400;
      else if (value == "500")
        fontWeight.Weight = 500;
      else if (value == "600")
        fontWeight.Weight = 600;
      else if (value == "700")
        fontWeight.Weight = 700;
      else if (value == "800")
        fontWeight.Weight = 800;
      else if (value == "900")
        fontWeight.Weight = 900;
      else
        fontWeight = winrt::Windows::UI::Text::FontWeights::Normal();

      XamlDirectInstance::GetXamlDirect().SetObjectProperty(element, XD::XamlPropertyIndex::Control_FontWeight, winrt::box_value(fontWeight));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_FontWeight);
    }

  }
  else if (propertyName == "fontStyle")
  {
    if (propertyValue.isString())
    {
      auto fontStyle = (propertyValue.getString() == "italic")
        ? winrt::Windows::UI::Text::FontStyle::Italic
        : winrt::Windows::UI::Text::FontStyle::Normal;
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::Control_FontStyle, static_cast<uint32_t>(fontStyle));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::Control_FontStyle);
    }

  }
  else
  {
    isFontProperty = false;
  }

  return isFontProperty;
}

inline void SetTextAlignment(const XD::IXamlDirectObject& element, const std::string& value)
{
  if (value == "left")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment, static_cast<uint32_t>(winrt::TextAlignment::Left));
  else if (value == "right")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment, static_cast<uint32_t>(winrt::TextAlignment::Right));
  else if (value == "center")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment, static_cast<uint32_t>(winrt::TextAlignment::Center));
  else if (value == "justify")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment, static_cast<uint32_t>(winrt::TextAlignment::Justify));
  else
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment, static_cast<uint32_t>(winrt::TextAlignment::DetectFromContent));
}

inline bool TryUpdateTextAlignment(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "textAlign")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextAlignment(element, value);
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::TextBox_TextAlignment);
    }

    return true;
  }

  return false;
}

inline void SetTextTrimming(const XD::IXamlDirectObject& element, const std::string& value)
{
  if (value == "clip")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBlock_TextTrimming, static_cast<uint32_t>(winrt::TextTrimming::Clip));
  else if (value == "head" || value == "middle" || value == "tail")
  {
    // "head" and "middle" not supported by UWP, but "tail"
    // behavior is the most similar
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBlock_TextTrimming, static_cast<uint32_t>(winrt::TextTrimming::CharacterEllipsis));
  }
  else
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBlock_TextTrimming, static_cast<uint32_t>(winrt::TextTrimming::None));
}

inline bool TryUpdateTextTrimming(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "ellipsizeMode")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextTrimming(element, value);
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::TextBlock_TextTrimming);
    }

    return true;
  }

  return false;
}

inline bool TryUpdateTextDecorationLine(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "textDecorationLine")
  {
    // FUTURE: remove when SDK target minVer >= 10.0.15063.0
    static bool isTextDecorationsSupported = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.TextBlock", L"TextDecorations");
    if (!isTextDecorationsSupported)
      return true;

    if (propertyValue.isString())
    {
      using winrt::Windows::UI::Text::TextDecorations;

      const std::string& value = propertyValue.getString();
      TextDecorations decorations = TextDecorations::None;
      if (value == "none")
        decorations = TextDecorations::None;
      else if (value == "underline")
        decorations = TextDecorations::Underline;
      else if (value == "line-through")
        decorations = TextDecorations::Strikethrough;
      else if (value == "underline line-through")
        decorations = TextDecorations::Underline | TextDecorations::Strikethrough;

      XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::TextBlock_TextDecorations, static_cast<uint32_t>(decorations));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::TextBlock_TextDecorations);
    }

    return true;
  }

  return false;
}

inline void SetFlowDirection(const XD::IXamlDirectObject& element, const std::string& value)
{
  if (value == "rtl")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::FrameworkElement_FlowDirection, static_cast<uint32_t>(winrt::FlowDirection::RightToLeft));
  else if (value == "ltr")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, XD::XamlPropertyIndex::FrameworkElement_FlowDirection, static_cast<uint32_t>(winrt::FlowDirection::LeftToRight));
  else // 'auto', 'inherit'
    XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_FlowDirection);
}

inline bool TryUpdateFlowDirection(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if ((propertyName == "writingDirection") || (propertyName == "direction"))
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetFlowDirection(element, value);
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, XD::XamlPropertyIndex::FrameworkElement_FlowDirection);
    }
    
    return true;
  }

  return false;
}

inline bool TryUpdateCharacterSpacing(const XD::IXamlDirectObject& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex prop)
{
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, prop, static_cast<int32_t>(propertyValue.asDouble()));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, prop);

    return true;
  }

  return false;
}

inline inline bool TryUpdateMouseEvents(ShadowNodeBase* node, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  if (propertyName == "onMouseEnter")
    node->m_onMouseEnter = !propertyValue.isNull() && propertyValue.asBool();
  else if (propertyName == "onMouseLeave")
    node->m_onMouseLeave = !propertyValue.isNull() && propertyValue.asBool();
  else if (propertyName == "onMouseMove")
    node->m_onMouseMove = !propertyValue.isNull() && propertyValue.asBool();
  else
    return false;

  return true;
}

} }
