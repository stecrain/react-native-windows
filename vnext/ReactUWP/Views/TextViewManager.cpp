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
  auto textBlock = XamlDirectInstance::GetXamlDirect().CreateInstance(XD::XamlTypeIndex::TextBlock);
  XamlDirectInstance::GetXamlDirect().SetEnumProperty(textBlock, XD::XamlPropertyIndex::TextBlock_TextWrapping, static_cast<uint32_t>(winrt::TextWrapping::Wrap)); // Default behavior in React Native
  return textBlock;
}

void TextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto textBlock = nodeToUpdate->GetView();
  if (textBlock == nullptr)
    return;

  for (const auto& pair : reactDiffMap.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    if (TryUpdateForeground(textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdateFontProperties(textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdateTextAlignment(textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdateTextTrimming(textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdateTextDecorationLine(textBlock, propertyName, propertyValue))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(textBlock, propertyName, propertyValue, XD::XamlPropertyIndex::TextBlock_CharacterSpacing))
    {
      continue;
    }
    else if (propertyName == "numberOfLines")
    {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetInt32Property(textBlock, XD::XamlPropertyIndex::TextBlock_MaxLines, static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(textBlock, XD::XamlPropertyIndex::TextBlock_MaxLines);
    }
    else if (propertyName == "lineHeight")
    {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetInt32Property(textBlock, XD::XamlPropertyIndex::TextBlock_LineHeight, static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(textBlock, XD::XamlPropertyIndex::TextBlock_LineHeight);
    }
    else if (propertyName == "selectable")
    {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(textBlock, XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled, propertyValue.asBool());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(textBlock, XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled);
    }
    else if (propertyName == "allowFontScaling")
    {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(textBlock, XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled, propertyValue.asBool());
      else
        XamlDirectInstance::GetXamlDirect().ClearProperty(textBlock, XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled);
    }
    else if (propertyName == "selectionColor")
    {
      if (propertyValue.isNumber())
      {
        XamlDirectInstance::GetXamlDirect().SetObjectProperty(textBlock, XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor, winrt::box_value(SolidColorBrushFrom(propertyValue)));
      }
      else
        XamlDirectInstance::GetXamlDirect().ClearProperty(textBlock, XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor);
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  auto inlinesCollection = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(parent, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(inlinesCollection, static_cast<uint32_t>(index), child);
}

void TextViewManager::RemoveAllChildren(XamlView parent)
{
  auto inlinesCollection = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(parent, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().ClearCollection(inlinesCollection);
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto inlinesCollection = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(parent, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(inlinesCollection, static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

} }
