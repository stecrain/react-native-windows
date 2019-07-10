// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <XamlDirectInstance.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
}

namespace react { namespace uwp {

class TextShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  TextShadowNode() = default;
  bool ImplementsPadding() override { return true; }
};

TextViewManager::TextViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

facebook::react::ShadowNode* TextViewManager::createShadow() const
{
  return new TextShadowNode();
}

const char* TextViewManager::GetName() const
{
  return "RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t tag)
{
  auto textBlock = winrt::TextBlock();
  XamlDirectInstance::GetXamlDirect().SetEnumProperty(
    XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(textBlock),
    XD::XamlPropertyIndex::TextBlock_TextWrapping,
    static_cast<int32_t>(winrt::TextWrapping::Wrap)
  );
  return textBlock;
}

void TextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto textBlock = nodeToUpdate->GetView().as<winrt::TextBlock>();
  if (textBlock == nullptr)
    return;

  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(textBlock);

  for (const auto& pair : reactDiffMap.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    auto fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontSize;
    if (propertyName == "fontFamily")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontFamily;
    }
    else if (propertyName == "fontWeight")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontWeight;
    }
    else if (propertyName == "fontStyle")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontStyle;
    }

    if (TryUpdateForeground(textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_Foreground))
    {
      continue;
    }
    else if (TryUpdateFontProperties(textBlockXD, propertyName, propertyValue, fontPropIdx))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_Padding))
    {
      continue;
    }
    else if (TryUpdateTextAlignment(textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_TextAlignment))
    {
      continue;
    }
    else if (TryUpdateTextTrimming(textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_TextTrimming))
    {
      continue;
    }
    else if (TryUpdateTextDecorationLine(textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_TextDecorations))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(textBlockXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_CharacterSpacing))
    {
      continue;
    }
    else if (propertyName == "numberOfLines")
    {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetInt32Property(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_MaxLines,
          static_cast<int32_t>(propertyValue.asDouble())
        );
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_MaxLines
        );
    }
    else if (propertyName == "lineHeight")
    {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetDoubleProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_LineHeight,
          propertyValue.asDouble()
        );
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_LineHeight
        );
    }
    else if (propertyName == "selectable")
    {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled,
          propertyValue.asBool()
        );
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled
        );
    }
    else if (propertyName == "allowFontScaling")
    {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled,
          propertyValue.asBool()
        );
      else
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled
        );
    }
    else if (propertyName == "selectionColor")
    {
      if (propertyValue.isNumber())
      {
        XamlDirectInstance::GetXamlDirect().SetColorProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor,
          SolidColorBrushFrom(propertyValue).Color()
        );
      }
      else
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor
        );
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto childInlineXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(child.as<winrt::Inline>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
    textBlockXD,
    XD::XamlPropertyIndex::TextBlock_Inlines
  );
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(
    textBlockInlinesXD,
    static_cast<uint32_t>(index), childInlineXD
  );
}

void TextViewManager::RemoveAllChildren(XamlView parent)
{
  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
    textBlockXD,
    XD::XamlPropertyIndex::TextBlock_Inlines
  );
  XamlDirectInstance::GetXamlDirect().ClearCollection(textBlockInlinesXD);
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
    textBlockXD,
    XD::XamlPropertyIndex::TextBlock_Inlines
  );
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(
    textBlockInlinesXD,
    static_cast<uint32_t>(index)
  );
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

} }
