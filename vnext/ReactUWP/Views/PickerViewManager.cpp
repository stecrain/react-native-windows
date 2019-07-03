// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "PickerViewManager.h"
#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>
#include "unicode.h"

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Core.Direct.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Core::Direct;
}

namespace react { namespace uwp {

class PickerShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  PickerShadowNode();
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;
  bool IsExternalLayoutDirty() const override { return m_hasNewItems; }
  void DoExtraLayoutPrep(YGNodeRef yogaNode) override;

private:
  void RepopulateItems();
  static void OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex, folly::dynamic&& text);

  folly::dynamic m_items;
  int32_t m_selectedIndex = -1;
  bool m_hasNewItems = false;

  // FUTURE: remove when we can require RS5+
  bool m_isEditableComboboxSupported;
};

PickerShadowNode::PickerShadowNode()
  : Super()
{
  m_isEditableComboboxSupported = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.ComboBox", L"IsEditableProperty");
}

void PickerShadowNode::DoExtraLayoutPrep(YGNodeRef yogaNode)
{
  if (!m_hasNewItems)
    return;

  m_hasNewItems = false;

  auto comboBox = GetView().try_as<winrt::ComboBox>();
  comboBox.UpdateLayout();
}

void PickerShadowNode::createView()
{
  Super::createView();
  auto combobox = GetView().as<winrt::ComboBox>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  combobox.SelectionChanged([=](auto&&, auto&&)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
    {
      int32_t index = combobox.SelectedIndex();
      folly::dynamic value;
      if (index >= 0 && index < static_cast<int32_t>(m_items.size()))
        value = m_items.at(index)["value"];
      folly::dynamic text;
      if (m_isEditableComboboxSupported && index == -1)
        text = HstringToDynamic(combobox.Text());
      OnSelectionChanged(*instance, m_tag, std::move(value), index, std::move(text));
    }
  });
}

void PickerShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;

  bool updateSelectedIndex = false;
  auto combobox = GetView().as<winrt::ComboBox>();

  auto comboboxXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(combobox);

  for (auto& pair : props.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName == "editable")
    {
      if (m_isEditableComboboxSupported)
      {
        if (propertyValue.isBool())
          //combobox.IsEditable(propertyValue.asBool());
          XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            comboboxXD,
            winrt::XamlPropertyIndex::ComboBox_IsEditable,
            propertyValue.asBool()
          );
        else if (propertyValue.isNull())
          //combobox.ClearValue(winrt::ComboBox::IsEditableProperty());
          XamlDirectInstance::GetXamlDirect().ClearProperty(
            comboboxXD,
            winrt::XamlPropertyIndex::ComboBox_IsEditable
          );
      }
    }
    else if (propertyName == "text")
    {
      if (m_isEditableComboboxSupported)
      {
        if (propertyValue.isString())
          //combobox.Text(asHstring(propertyValue));
          XamlDirectInstance::GetXamlDirect().SetStringProperty(
            comboboxXD,
            winrt::XamlPropertyIndex::ComboBox_Text,
            asHstring(propertyValue)
          );
        else if (propertyValue.isNull())
          //combobox.ClearValue(winrt::ComboBox::TextProperty());
          XamlDirectInstance::GetXamlDirect().ClearProperty(
            comboboxXD,
            winrt::XamlPropertyIndex::ComboBox_Text
          );
      }
    }
    else if (propertyName == "enabled")
    {
      if (propertyValue.isBool())
        //combobox.IsEnabled(propertyValue.asBool());
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
          comboboxXD,
          winrt::XamlPropertyIndex::Control_IsEnabled,
          propertyValue.asBool()
        );
    }
    else if (propertyName == "selectedIndex")
    {
      if (propertyValue.isNumber())
      {
        auto selectedIndex = propertyValue.asDouble();
        if (selectedIndex == static_cast<int32_t>(selectedIndex))
        {
          m_selectedIndex = static_cast<int32_t>(selectedIndex);
        }
        updateSelectedIndex = true;
      }
    }
    else if (propertyName == "items")
    {
      if (propertyValue.isArray())
      {
        m_items = propertyValue;
        RepopulateItems();
      }
    }
  }

  // Update selectedIndex last, in case items and selectedIndex were both changing
  if (updateSelectedIndex)
    //combobox.SelectedIndex(m_selectedIndex);
    XamlDirectInstance::GetXamlDirect().SetInt32Property(
      comboboxXD,
      winrt::XamlPropertyIndex::Selector_SelectedIndex,
      m_selectedIndex
    );

  Super::updateProperties(std::move(props));
  m_updating = false;
}

void PickerShadowNode::RepopulateItems()
{
  auto combobox = GetView().as<winrt::ComboBox>();
  //auto comboBoxItems = combobox.Items();
  //comboBoxItems.Clear();

  auto comboboxXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(combobox);
  auto comboboxItemsXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(comboboxXD, winrt::XamlPropertyIndex::ItemsControl_Items);
  XamlDirectInstance::GetXamlDirect().ClearCollection(comboboxItemsXD);

  for (const auto& item : m_items)
  {
    if (item.count("label"))
    {
      std::string label = item["label"].asString();

      /*auto comboboxItem = winrt::ComboBoxItem();
      comboboxItem.Content(winrt::box_value(facebook::react::unicode::utf8ToUtf16(label)));
      if (item.count("textColor"))
        comboboxItem.Foreground(BrushFrom(item["textColor"]));
      comboBoxItems.Append(comboboxItem);*/

      auto comboboxItem = XamlDirectInstance::GetXamlDirect().CreateInstance(winrt::XamlTypeIndex::ComboBoxItem);      

      XamlDirectInstance::GetXamlDirect().SetStringProperty(
        comboboxItem,
        winrt::XamlPropertyIndex::ContentControl_Content,
        facebook::react::unicode::utf8ToUtf16(label)
      );
      if (item.count("textColor"))
      {
          auto brush = BrushFrom(item["textColor"]).as<winrt::SolidColorBrush>();
          XamlDirectInstance::GetXamlDirect().SetColorProperty(
            comboboxItem,
            winrt::XamlPropertyIndex::Control_Foreground,
            brush.Color()
          );
      }
      auto items = XamlDirectInstance::GetXamlDirect().GetObject(comboboxItemsXD).as<winrt::ItemCollection>();
      items.Append(XamlDirectInstance::GetXamlDirect().GetObject(comboboxItem).as<winrt::ComboBoxItem>());
      // out of bounds error when adding to collection, otherwise use code below instead of 2 lines above
       //XamlDirectInstance::GetXamlDirect().AddToCollection(comboboxItemsXD, comboboxItem);
    }
    m_hasNewItems = true;
  }
  if (m_selectedIndex < static_cast<int32_t>(m_items.size()))
    //combobox.SelectedIndex(m_selectedIndex);
    XamlDirectInstance::GetXamlDirect().SetInt32Property(
      comboboxXD,
      winrt::XamlPropertyIndex::Selector_SelectedIndex,
      m_selectedIndex
    );
}

/*static*/ void PickerShadowNode::OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex, folly::dynamic&& text)
{
  folly::dynamic eventData = folly::dynamic::object
    ("target", tag)
    ("value", std::move(value))
    ("itemIndex", selectedIndex)
    ("text", std::move(text));
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

PickerViewManager::PickerViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* PickerViewManager::GetName() const
{
  return "RCTPicker";
}

folly::dynamic PickerViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("editable", "boolean")
    ("enabled", "boolean")
    ("items", "array")
    ("selectedIndex", "number")
    ("text", "string")
  );

  return props;
}

facebook::react::ShadowNode* PickerViewManager::createShadow() const
{
  return new PickerShadowNode();
}

XamlView PickerViewManager::CreateViewCore(int64_t tag)
{
  auto combobox = winrt::ComboBox();
  return combobox;
}

YGMeasureFunc PickerViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

}}
