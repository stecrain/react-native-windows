// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

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

VirtualTextViewManager::VirtualTextViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* VirtualTextViewManager::GetName() const
{
  return "RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t tag)
{
  return winrt::Span();
}

void VirtualTextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto span = nodeToUpdate->GetView().as<winrt::Span>();
  if (span == nullptr)
    return;

  auto spanXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(span.as<winrt::TextElement>());

  for (const auto& pair : reactDiffMap.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    auto fontPropIdx = XD::XamlPropertyIndex::TextElement_FontSize;
    if (propertyName == "fontFamily")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextElement_FontFamily;
    }
    else if (propertyName == "fontWeight")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextElement_FontWeight;
    }
    else if (propertyName == "fontStyle")
    {
      fontPropIdx = XD::XamlPropertyIndex::TextElement_FontStyle;
    }

    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    if (TryUpdateForeground(spanXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextElement_Foreground))
    {
      continue;
    }
    else if (TryUpdateFontProperties(spanXD, propertyName, propertyValue, fontPropIdx))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(spanXD, propertyName, propertyValue, XD::XamlPropertyIndex::TextElement_CharacterSpacing))
    {
      continue;
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void VirtualTextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto span(parent.as<winrt::Span>());
  auto spanXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(span);
  auto inlines = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(spanXD, XD::XamlPropertyIndex::Span_Inlines);
  auto childXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(child);
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(inlines, static_cast<uint32_t>(index), childXD);
}

void VirtualTextViewManager::RemoveAllChildren(XamlView parent)
{
  auto span(parent.as<winrt::Span>());
  auto spanXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(span);
  auto inlines = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(spanXD, XD::XamlPropertyIndex::Span_Inlines);
  XamlDirectInstance::GetXamlDirect().ClearCollection(inlines);
}

void VirtualTextViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto span(parent.as<winrt::Span>());
  auto spanXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(span);
  auto inlines = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(spanXD, XD::XamlPropertyIndex::Span_Inlines);
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(inlines, static_cast<uint32_t>(index));
}

bool VirtualTextViewManager::RequiresYogaNode() const
{
  return false;
}

} }
