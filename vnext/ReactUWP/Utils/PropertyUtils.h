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
#include <XamlDirectInstance.h>

#include <Views/ShadowNodeBase.h>

#include <Views/ViewPanel.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Documents;
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

static inline void UpdatePadding(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, ShadowEdges edge, double margin, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_padding[edge] = margin;
  //winrt::Thickness thickness = GetThickness(node->m_padding, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  //element.Padding(thickness);
  winrt::Thickness thickness = GetThickness(node->m_padding, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(thickness));
}

static inline void SetBorderThickness(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, ShadowEdges edge, double margin, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_border[edge] = margin;
  /*winrt::Thickness thickness = GetThickness(node->m_border, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.BorderThickness(thickness);*/
  winrt::Thickness thickness = GetThickness(node->m_border, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(thickness));
}

static inline void SetBorderBrush(XD::IXamlDirectObject elementXD, const winrt::Windows::UI::Xaml::Media::Brush& brush, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  //element.BorderBrush(brush);
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(brush));
}

static inline bool TryUpdateBackgroundBrush(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "backgroundColor")
  {
    if (propertyValue.isNumber())
      //element.Background(BrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      //element.ClearValue(T::BackgroundProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);

    return true;
  }

  return false;
}

static inline void UpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, ShadowCorners corner, double newValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_cornerRadius[corner] = newValue;
  /*winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.CornerRadius(cornerRadius);*/
  winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(cornerRadius));
}

static inline bool TryUpdateForeground(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "color")
  {
    if (propertyValue.isNumber())
      //element.Foreground(BrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      //element.ClearValue(T::ForegroundProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);

    return true;
  }

  return false;
}

static inline bool TryUpdateBorderProperties(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  bool isBorderProperty = true;

  if (propertyName == "borderColor")
  {
    if (propertyValue.isNumber())
      //element.BorderBrush(BrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(BrushFrom(propertyValue)));
    else if (propertyValue.isNull())
      //element.ClearValue(T::BorderBrushProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
  }
  else if (propertyName == "borderLeftWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::Left, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::Top, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderRightWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::Right, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::Bottom, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderStartWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::Start, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderEndWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::End, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, elementXD, ShadowEdges::AllEdges, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    isBorderProperty = false;
  }

  return isBorderProperty;
}

static inline bool TryUpdatePadding(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex = XD::XamlPropertyIndex::Control_Padding)
{
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Left, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingTop")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Top, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingRight")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Right, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingBottom")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Bottom, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingStart")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Start, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingEnd")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::End, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingHorizontal")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Horizontal, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingVertical")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::Vertical, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "padding")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, elementXD, ShadowEdges::AllEdges, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

static inline bool TryUpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "borderTopLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::TopLeft, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::TopRight, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  if (propertyName == "borderTopStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::TopStart, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::TopEnd, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::BottomRight, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::BottomLeft, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::BottomStart, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::BottomEnd, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, elementXD, ShadowCorners::AllCorners, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    return false;
  }

  return true;
}

static inline bool TryUpdateFontProperties(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  bool isFontProperty = true;
  auto element = XamlDirectInstance::GetXamlDirect().GetObject(elementXD);

  if (propertyName == "fontSize")
  {
    if (propertyValue.isNumber())
      //element.FontSize(propertyValue.asDouble());
      XamlDirectInstance::GetXamlDirect().SetDoubleProperty(elementXD, xamlDirectPropIndex, propertyValue.asDouble());
    else if (propertyValue.isNull())
      //element.ClearValue(T::FontSizeProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
  }
  else if (propertyName == "fontFamily")
  {
    if (propertyValue.isString())
      //element.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue)));
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(
        elementXD,
        xamlDirectPropIndex,
        winrt::box_value(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue)))
      );
    else if (propertyValue.isNull())





      //element.ClearValue(T::FontFamilyProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
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

      //element.FontWeight(fontWeight);
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(fontWeight));
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::FontWeightProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }

  }
  else if (propertyName == "fontStyle")
  {
    auto fontStylePropXD = XD::XamlPropertyIndex::Control_FontStyle;
    if (typeid(element) == typeid(winrt::TextBlock))
      fontStylePropXD = XD::XamlPropertyIndex::TextBlock_FontStyle;
    else if (typeid(element) == typeid(winrt::TextElement))
      fontStylePropXD = XD::XamlPropertyIndex::TextElement_FontStyle;

    if (propertyValue.isString())
    {
      /*element.FontStyle((propertyValue.getString() == "italic")
        ? winrt::Windows::UI::Text::FontStyle::Italic
        : winrt::Windows::UI::Text::FontStyle::Normal);*/
      auto fontStyle = (propertyValue.getString() == "italic")
        ? winrt::Windows::UI::Text::FontStyle::Italic
        : winrt::Windows::UI::Text::FontStyle::Normal;
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, fontStylePropXD, static_cast<uint32_t>(fontStyle));
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::FontStyleProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, fontStylePropXD);
    }

  }
  else
  {
    isFontProperty = false;
  }

  return isFontProperty;
}

static inline void SetTextAlignment(XD::IXamlDirectObject elementXD, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (value == "left")
    //element.TextAlignment(winrt::TextAlignment::Left);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextAlignment::Left));
  else if (value == "right")
    //element.TextAlignment(winrt::TextAlignment::Right);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextAlignment::Right));
  else if (value == "center")
    //element.TextAlignment(winrt::TextAlignment::Center);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextAlignment::Center));
  else if (value == "justify")
    //element.TextAlignment(winrt::TextAlignment::Justify);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextAlignment::Justify));
  else
    //element.TextAlignment(winrt::TextAlignment::DetectFromContent);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextAlignment::DetectFromContent));
}

static inline bool TryUpdateTextAlignment(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "textAlign")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextAlignment(elementXD, value, xamlDirectPropIndex);
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::TextAlignmentProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }

    return true;
  }

  return false;
}

static inline void SetTextTrimming(XD::IXamlDirectObject elementXD, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (value == "clip")
    //element.TextTrimming(winrt::TextTrimming::Clip);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextTrimming::Clip));
  else if (value == "head" || value == "middle" || value == "tail")
  {
    // "head" and "middle" not supported by UWP, but "tail"
    // behavior is the most similar
    //element.TextTrimming(winrt::TextTrimming::CharacterEllipsis);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextTrimming::CharacterEllipsis));
  }
  else
    //element.TextTrimming(winrt::TextTrimming::None);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::TextTrimming::None));
}

static inline bool TryUpdateTextTrimming(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "ellipsizeMode")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextTrimming(elementXD, value, xamlDirectPropIndex);
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::TextTrimmingProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }

    return true;
  }

  return false;
}

static inline bool TryUpdateTextDecorationLine(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
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

      //element.TextDecorations(decorations);
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(decorations));
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::TextDecorationsProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }

    return true;
  }

  return false;
}

static inline void SetFlowDirection(XD::IXamlDirectObject elementXD, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (value == "rtl")
    //element.FlowDirection(winrt::FlowDirection::RightToLeft);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::FlowDirection::RightToLeft));
  else if (value =="ltr")
    //element.FlowDirection(winrt::FlowDirection::LeftToRight);
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<uint32_t>(winrt::FlowDirection::LeftToRight));
  else // 'auto', 'inherit'
    //element.ClearValue(winrt::FrameworkElement::FlowDirectionProperty());
    XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
}

static inline bool TryUpdateFlowDirection(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if ((propertyName == "writingDirection") || (propertyName == "direction"))
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetFlowDirection(elementXD, value, xamlDirectPropIndex);
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::FlowDirectionProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }

    return true;
  }

  return false;
}

static inline bool TryUpdateCharacterSpacing(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing")
  {
    if (propertyValue.isNumber())
      //element.CharacterSpacing(static_cast<int32_t>(propertyValue.asDouble()));
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<int32_t>(propertyValue.asDouble()));
    else if (propertyValue.isNull())
      //element.ClearValue(T::CharacterSpacingProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);

    return true;
  }

  return false;
}

static inline bool TryUpdateOrientation(XD::IXamlDirectObject elementXD, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "orientation")
  {
    if (propertyValue.isNull())
    {
      //element.ClearValue(T::OrientationProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, xamlDirectPropIndex);
    }
    else if (propertyValue.isString())
    {
      const std::string& valueString = propertyValue.getString();
      if (valueString == "horizontal")
        //element.Orientation(Orientation::Horizontal);
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<int32_t>(winrt::Orientation::Horizontal));
      else if (valueString == "vertical")
        //element.Orientation(Orientation::Vertical);
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<int32_t>(winrt::Orientation::Vertical));
    }

    return true;
  }

  return false;
}

inline bool TryUpdateMouseEvents(ShadowNodeBase* node, const std::string& propertyName, const folly::dynamic& propertyValue)
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
