// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include "DynamicAutomationProperties.h"

#include "cppwinrt/DynamicAutomationPeer.g.h"
namespace winrt::react::uwp::implementation
{

//
// DynamicAutomationPeer translates the values of the attached properties in DynamicAutomationProperties
// into the equivalent UIA AutomationPeer
//
struct DynamicAutomationPeer : DynamicAutomationPeerT<DynamicAutomationPeer>
{
  using Super = DynamicAutomationPeerT<DynamicAutomationPeer>;

  DynamicAutomationPeer() = delete;
  DynamicAutomationPeer(winrt::Windows::UI::Xaml::FrameworkElement const& owner);

  winrt::hstring GetClassNameCore() const;

  winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
  winrt::Windows::Foundation::IInspectable GetPatternCore(winrt::Windows::UI::Xaml::Automation::Peers::PatternInterface const& patternInterface) const;

  bool IsEnabledCore() const;

  // IInvokeProvider
  void Invoke() const;

  // ISelectionProvider
  bool CanSelectMultiple() const { return true; }
  bool IsSelectionRequired() const { return false; }
  winrt::com_array<winrt::Windows::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetSelection() const;

  // ISelectionItemProvider
  bool IsSelected() const;
  winrt::Windows::UI::Xaml::Automation::Provider::IRawElementProviderSimple SelectionContainer() const;
  void AddToSelection() const;
  void RemoveFromSelection() const;
  void Select() const;

private:
  winrt::react::uwp::AccessibilityRoles GetAccessibilityRole() const;
  bool GetAccessibilityState(winrt::react::uwp::AccessibilityStates state) const;
  winrt::react::uwp::AccessibilityInvokeEventHandler GetAccessibilityInvokeEventHandler() const;
};
} // namespace winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation
{
  struct DynamicAutomationPeer : DynamicAutomationPeerT<DynamicAutomationPeer, implementation::DynamicAutomationPeer>
  {
  };
}
