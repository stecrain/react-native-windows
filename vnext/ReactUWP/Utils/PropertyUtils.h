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

template <class T>
void UpdatePadding(ShadowNodeBase* node, const T& element, ShadowEdges edge, double margin, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_padding[edge] = margin;
  //winrt::Thickness thickness = GetThickness(node->m_padding, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  //element.Padding(thickness);
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
  winrt::Thickness thickness = GetThickness(node->m_padding, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(thickness));
}

template <class T>
void SetBorderThickness(ShadowNodeBase* node, const T& element, ShadowEdges edge, double margin, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_border[edge] = margin;
  /*winrt::Thickness thickness = GetThickness(node->m_border, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.BorderThickness(thickness);*/
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
  winrt::Thickness thickness = GetThickness(node->m_border, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(thickness));
}

template <class T>
void SetBorderBrush(const T& element, const winrt::Windows::UI::Xaml::Media::Brush& brush, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  //element.BorderBrush(brush);
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(brush));
}

template <class T>
bool TryUpdateBackgroundBrush(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "backgroundColor")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
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

template <class T>
void UpdateCornerRadius(ShadowNodeBase* node, const T& element, ShadowCorners corner, double newValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  node->m_cornerRadius[corner] = newValue;
  /*winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.CornerRadius(cornerRadius);*/
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
  winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, IsFlowRTL(elementXD));
  XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, xamlDirectPropIndex, winrt::box_value(cornerRadius));
}

template <class T>
bool TryUpdateForeground(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "color")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
bool TryUpdateBorderProperties(ShadowNodeBase* node, const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  bool isBorderProperty = true;

  if (propertyName == "borderColor")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
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
      SetBorderThickness(node, element, ShadowEdges::Left, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Top, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderRightWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Right, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Bottom, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderStartWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Start, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderEndWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::End, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::AllEdges, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    isBorderProperty = false;
  }

  return isBorderProperty;
}

template <class T>
bool TryUpdatePadding(ShadowNodeBase* node, const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Left, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingTop")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Top, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingRight")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Right, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingBottom")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Bottom, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingStart")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Start, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingEnd")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::End, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingHorizontal")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Horizontal, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "paddingVertical")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Vertical, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "padding")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::AllEdges, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

template <class T>
bool TryUpdateCornerRadius(ShadowNodeBase* node, const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "borderTopLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopLeft, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopRight, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  if (propertyName == "borderTopStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopStart, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderTopEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopEnd, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomRight, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomLeft, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomStart, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderBottomEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomEnd, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else if (propertyName == "borderRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::AllCorners, propertyValue.asDouble(), xamlDirectPropIndex);
  }
  else
  {
    return false;
  }

  return true;
}

template <class T>
bool TryUpdateFontProperties(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue)
{
  bool isFontProperty = true;
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

  if (propertyName == "fontSize")
  {
    auto fontSizePropXD = XD::XamlPropertyIndex::Control_FontSize;
    if (typeid(element) == typeid(winrt::TextBlock))
      fontSizePropXD = XD::XamlPropertyIndex::TextBlock_FontSize;
    else if (typeid(element) == typeid(winrt::TextElement))
      fontSizePropXD = XD::XamlPropertyIndex::TextElement_FontSize;

    if (propertyValue.isNumber())
      //element.FontSize(propertyValue.asDouble());
      XamlDirectInstance::GetXamlDirect().SetDoubleProperty(elementXD, fontSizePropXD, propertyValue.asDouble());
    else if (propertyValue.isNull())
      //element.ClearValue(T::FontSizeProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, fontSizePropXD);
  }
  else if (propertyName == "fontFamily")
  {
    auto fontFamilyPropXD = XD::XamlPropertyIndex::Control_FontFamily;
    if (typeid(element) == typeid(winrt::TextBlock))
      fontFamilyPropXD = XD::XamlPropertyIndex::TextBlock_FontFamily;
    else if (typeid(element) == typeid(winrt::TextElement))
      fontFamilyPropXD = XD::XamlPropertyIndex::TextElement_FontFamily;

    if (propertyValue.isString())
      //element.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue)));
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, fontFamilyPropXD, winrt::box_value(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue))));
    else if (propertyValue.isNull())
      //element.ClearValue(T::FontFamilyProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, fontFamilyPropXD);
  }
  else if (propertyName == "fontWeight")
  {
    auto fontWeightPropXD = XD::XamlPropertyIndex::Control_FontWeight;
    if (typeid(element) == typeid(winrt::TextBlock))
      fontWeightPropXD = XD::XamlPropertyIndex::TextBlock_FontWeight;
    else if (typeid(element) == typeid(winrt::TextElement))
      fontWeightPropXD = XD::XamlPropertyIndex::TextElement_FontWeight;

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
      XamlDirectInstance::GetXamlDirect().SetObjectProperty(elementXD, fontWeightPropXD, winrt::box_value(fontWeight));
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::FontWeightProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(elementXD, fontWeightPropXD);
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

template <class T>
void SetTextAlignment(const T& element, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
bool TryUpdateTextAlignment(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

  if (propertyName == "textAlign")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextAlignment(element, value, xamlDirectPropIndex);
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

template <class T>
void SetTextTrimming(const T& element, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
bool TryUpdateTextTrimming(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "ellipsizeMode")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextTrimming(element, value, xamlDirectPropIndex);
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

template <class T>
bool TryUpdateTextDecorationLine(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "textDecorationLine")
  {
    // FUTURE: remove when SDK target minVer >= 10.0.15063.0
    static bool isTextDecorationsSupported = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.TextBlock", L"TextDecorations");
    if (!isTextDecorationsSupported)
      return true;

    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
void SetFlowDirection(const T& element, const std::string& value, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
bool TryUpdateFlowDirection(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if ((propertyName == "writingDirection") || (propertyName == "direction"))
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetFlowDirection(element, value, xamlDirectPropIndex);
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

template <class T>
bool TryUpdateCharacterSpacing(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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

template <class T>
bool TryUpdateOrientation(const T& element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex xamlDirectPropIndex)
{
  if (propertyName == "orientation")
  {
    auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);

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
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<int32_t>(Orientation::Horizontal));
      else if (valueString == "vertical")
        //element.Orientation(Orientation::Vertical);
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(elementXD, xamlDirectPropIndex, static_cast<int32_t>(Orientation::Vertical));
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
