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
#include <winrt/Windows.UI.Xaml.Core.Direct.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Core::Direct;
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
  textBlock.TextWrapping(winrt::TextWrapping::Wrap); // Default behavior in React Native
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

    winrt::XamlPropertyIndex fontPropIdx;

    if (propertyName == "fontSize")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontSize;
    }
    else if (propertyName == "fontFamily")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontFamily;
    }
    else if (propertyName == "fontWeight")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextBlock_FontWeight;
    }

    if (TryUpdateForeground(textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_Foreground))
    {
      continue;
    }
    else if (TryUpdateFontProperties(textBlockXD, propertyName, propertyValue, fontPropIdx))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_Padding))
    {
      continue;
    }
    else if (TryUpdateTextAlignment(textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_TextAlignment))
    {
      continue;
    }
    else if (TryUpdateTextTrimming(textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_TextTrimming))
    {
      continue;
    }
    else if (TryUpdateTextDecorationLine(textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_TextDecorations))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(textBlockXD, propertyName, propertyValue, winrt::XamlPropertyIndex::TextBlock_CharacterSpacing))
    {
      continue;
    }
    else if (propertyName == "numberOfLines")
    {
      if (propertyValue.isNumber())
        //textBlock.MaxLines(static_cast<int32_t>(propertyValue.asDouble()));
        XamlDirectInstance::GetXamlDirect().SetInt32Property(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_MaxLines,
          static_cast<int32_t>(propertyValue.asDouble())
        );
      else if (propertyValue.isNull())
        //textBlock.ClearValue(winrt::TextBlock::MaxLinesProperty());
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_MaxLines
        );
    }
    else if (propertyName == "lineHeight")
    {
      if (propertyValue.isNumber())
        // why does casting to int work here when line height should be a double??
        //textBlock.LineHeight(static_cast<int32_t>(propertyValue.asDouble()));
        XamlDirectInstance::GetXamlDirect().SetDoubleProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_LineHeight,
          propertyValue.asDouble()
        );
      else if (propertyValue.isNull())
        //textBlock.ClearValue(winrt::TextBlock::LineHeightProperty());
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_LineHeight
        );
    }
    else if (propertyName == "selectable")
    {
      if (propertyValue.isBool())
        //textBlock.IsTextSelectionEnabled(propertyValue.asBool());
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled,
          propertyValue.asBool()
        );
      else if (propertyValue.isNull())
        //textBlock.ClearValue(winrt::TextBlock::IsTextSelectionEnabledProperty());
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled
        );
    }
    else if (propertyName == "allowFontScaling")
    {
      if (propertyValue.isBool())
        //textBlock.IsTextScaleFactorEnabled(propertyValue.asBool());
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled,
          propertyValue.asBool()
        );
      else
        //textBlock.ClearValue(winrt::TextBlock::IsTextScaleFactorEnabledProperty());
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled
        );
    }
    else if (propertyName == "selectionColor")
    {
      if (propertyValue.isNumber())
      {
        auto solidColorBrush = SolidColorBrushFrom(propertyValue);
        //textBlock.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
        XamlDirectInstance::GetXamlDirect().SetColorProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_SelectionHighlightColor,
          solidColorBrush.Color()
        );
      }
      else
        //textBlock.ClearValue(winrt::TextBlock::SelectionHighlightColorProperty());
        XamlDirectInstance::GetXamlDirect().ClearProperty(
          textBlockXD,
          winrt::XamlPropertyIndex::TextBlock_SelectionHighlightColor
        );
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
 /* auto textBlock(parent.as<winrt::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);*/

  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto childInlineXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(child.as<winrt::Inline>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(textBlockXD, winrt::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(textBlockInlinesXD, static_cast<uint32_t>(index), childInlineXD);
}

void TextViewManager::RemoveAllChildren(XamlView parent)
{
 /* auto textBlock(parent.as<winrt::TextBlock>());
  textBlock.Inlines().Clear();*/

  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(textBlockXD, winrt::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().ClearCollection(textBlockInlinesXD);
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  /*auto textBlock(parent.as<winrt::TextBlock>());
  return textBlock.Inlines().RemoveAt(static_cast<uint32_t>(index));*/

  auto textBlockXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(parent.as<winrt::TextBlock>());
  auto textBlockInlinesXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(textBlockXD, winrt::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(textBlockInlinesXD, static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

} }
