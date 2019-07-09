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

static inline void UpdatePadding(ShadowNodeBase* node, XD::IXamlDirectObject element, ShadowEdges edge, double margin, XD::XamlPropertyIndex propertyIndex)
{
  node->m_padding[edge] = margin;
  winrt::Thickness thickness = GetThickness(node->m_padding, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetThicknessProperty(element, propertyIndex, thickness);
}

static inline void SetBorderThickness(ShadowNodeBase* node, XD::IXamlDirectObject element, ShadowEdges edge, double margin, XD::XamlPropertyIndex propertyIndex)
{
  node->m_border[edge] = margin;
  winrt::Thickness thickness = GetThickness(node->m_border, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetThicknessProperty(element, propertyIndex, thickness);
}

static inline void SetBorderBrush(XD::IXamlDirectObject element, const winrt::Windows::UI::Xaml::Media::Brush& brush, XD::XamlPropertyIndex propertyIndex)
{
  const auto propertyValueXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(brush);
  XamlDirectInstance::GetXamlDirect().SetXamlDirectObjectProperty(element, propertyIndex, propertyValueXD);
}

static inline bool TryUpdateBackgroundBrush(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "backgroundColor")
  {
    if (propertyValue.isNumber())
    {
      const auto propertyValueXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(BrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetXamlDirectObjectProperty(element, propertyIndex, propertyValueXD);
    }
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);

    return true;
  }

  return false;
}

static inline void UpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject element, ShadowCorners corner, double newValue, XD::XamlPropertyIndex propertyIndex)
{
  node->m_cornerRadius[corner] = newValue;
  winrt::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius, IsFlowRTL(element));
  XamlDirectInstance::GetXamlDirect().SetCornerRadiusProperty(element, propertyIndex, cornerRadius);
}

static inline bool TryUpdateForeground(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "color")
  {
    if (propertyValue.isNumber())
    {
      const auto propertyValueXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(SolidColorBrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetXamlDirectObjectProperty(element, propertyIndex, propertyValueXD);
    }
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);

    return true;
  }

  return false;
}

static inline bool TryUpdateBorderProperties(ShadowNodeBase* node, XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  bool isBorderProperty = true;

  if (propertyName == "borderColor")
  {
    if (propertyValue.isNumber())
    {
      const auto propertyValueXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(SolidColorBrushFrom(propertyValue));
      XamlDirectInstance::GetXamlDirect().SetXamlDirectObjectProperty(element, propertyIndex, propertyValueXD);
    }
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
  }
  else if (propertyName == "borderLeftWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Left, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderTopWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Top, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderRightWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Right, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderBottomWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Bottom, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderStartWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::Start, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderEndWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::End, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderWidth")
  {
    if (propertyValue.isNumber())
      SetBorderThickness(node, element, ShadowEdges::AllEdges, propertyValue.asDouble(), propertyIndex);
  }
  else
  {
    isBorderProperty = false;
  }

  return isBorderProperty;
}

static inline bool TryUpdatePadding(ShadowNodeBase* node, XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Left, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingTop")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Top, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingRight")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Right, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingBottom")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Bottom, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingStart")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Start, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingEnd")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::End, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingHorizontal")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Horizontal, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "paddingVertical")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Vertical, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "padding")
  {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::AllEdges, propertyValue.asDouble(), propertyIndex);
  }
  else
  {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

static inline bool TryUpdateCornerRadius(ShadowNodeBase* node, XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "borderTopLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopLeft, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderTopRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopRight, propertyValue.asDouble(), propertyIndex);
  }
  if (propertyName == "borderTopStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopStart, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderTopEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::TopEnd, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderBottomRightRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomRight, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderBottomLeftRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomLeft, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderBottomStartRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomStart, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderBottomEndRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::BottomEnd, propertyValue.asDouble(), propertyIndex);
  }
  else if (propertyName == "borderRadius")
  {
    if (propertyValue.isNumber())
      UpdateCornerRadius(node, element, ShadowCorners::AllCorners, propertyValue.asDouble(), propertyIndex);
  }
  else
  {
    return false;
  }

  return true;
}

static inline bool TryUpdateFontProperties(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  bool isFontProperty = true;

  if (propertyName == "fontSize")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetDoubleProperty(element, propertyIndex, propertyValue.asDouble());
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
  }
  else if (propertyName == "fontFamily")
  {
    if (propertyValue.isString())
      XamlDirectInstance::GetXamlDirect().SetStringProperty(element, propertyIndex, asHstring(propertyValue));
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
  }
  else if (propertyName == "fontWeight")
  {
    if (propertyValue.isString())
    {
      XamlDirectInstance::GetXamlDirect().SetStringProperty(element, propertyIndex, asHstring(propertyValue));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

  }
  else if (propertyName == "fontStyle")
  {
    if (propertyValue.isString())
    {
      auto fontStyle = (propertyValue.getString() == "italic")
        ? winrt::Windows::UI::Text::FontStyle::Italic
        : winrt::Windows::UI::Text::FontStyle::Normal;
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(fontStyle));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

  }
  else
  {
    isFontProperty = false;
  }

  return isFontProperty;
}

static inline void SetTextAlignment(XD::IXamlDirectObject element, const std::string& value, XD::XamlPropertyIndex propertyIndex)
{
  if (value == "left")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextAlignment::Left));
  else if (value == "right")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextAlignment::Right));
  else if (value == "center")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextAlignment::Center));
  else if (value == "justify")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextAlignment::Justify));
  else
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextAlignment::DetectFromContent));
}

static inline bool TryUpdateTextAlignment(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "textAlign")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextAlignment(element, value, propertyIndex);
    }
    else if (propertyValue.isNull())
    {
      //element.ClearValue(T::TextAlignmentProperty());
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

    return true;
  }

  return false;
}

static inline void SetTextTrimming(XD::IXamlDirectObject element, const std::string& value, XD::XamlPropertyIndex propertyIndex)
{
  if (value == "clip")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextTrimming::Clip));
  else if (value == "head" || value == "middle" || value == "tail")
  {
    // "head" and "middle" not supported by UWP, but "tail"
    // behavior is the most similar
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextTrimming::CharacterEllipsis));
  }
  else
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(winrt::TextTrimming::None));
}

static inline bool TryUpdateTextTrimming(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "ellipsizeMode")
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetTextTrimming(element, value, propertyIndex);
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

    return true;
  }

  return false;
}

static inline bool TryUpdateTextDecorationLine(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
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

      XamlDirectInstance::GetXamlDirect().SetEnumProperty(element, propertyIndex, static_cast<uint32_t>(decorations));
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

    return true;
  }

  return false;
}

static inline void SetFlowDirection(XD::IXamlDirectObject element, const std::string& value, XD::XamlPropertyIndex propertyIndex)
{
  if (value == "rtl")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(
      element,
      propertyIndex,
      static_cast<uint32_t>(winrt::FlowDirection::RightToLeft)
    );
  else if (value =="ltr")
    XamlDirectInstance::GetXamlDirect().SetEnumProperty(
      element,
      propertyIndex,
      static_cast<uint32_t>(winrt::FlowDirection::LeftToRight)
    );
  else // 'auto', 'inherit'
    XamlDirectInstance::GetXamlDirect().ClearProperty(
      element,
      propertyIndex
    );
}

static inline bool TryUpdateFlowDirection(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if ((propertyName == "writingDirection") || (propertyName == "direction"))
  {
    if (propertyValue.isString())
    {
      const std::string& value = propertyValue.getString();
      SetFlowDirection(element, value, propertyIndex);
    }
    else if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }

    return true;
  }

  return false;
}

static inline bool TryUpdateCharacterSpacing(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing")
  {
    if (propertyValue.isNumber())
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(
        element,
        propertyIndex,
        static_cast<int32_t>(propertyValue.asDouble())
      );
    else if (propertyValue.isNull())
      XamlDirectInstance::GetXamlDirect().ClearProperty(
        element,
        propertyIndex
      );

    return true;
  }

  return false;
}

static inline bool TryUpdateOrientation(XD::IXamlDirectObject element, const std::string& propertyName, const folly::dynamic& propertyValue, XD::XamlPropertyIndex propertyIndex)
{
  if (propertyName == "orientation")
  {
    if (propertyValue.isNull())
    {
      XamlDirectInstance::GetXamlDirect().ClearProperty(element, propertyIndex);
    }
    else if (propertyValue.isString())
    {
      const std::string& valueString = propertyValue.getString();
      if (valueString == "horizontal")
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(
          element,
          propertyIndex,
          static_cast<int32_t>(winrt::Orientation::Horizontal)
        );
      else if (valueString == "vertical")
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(
          element,
          propertyIndex,
          static_cast<int32_t>(winrt::Orientation::Vertical)
        );
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
