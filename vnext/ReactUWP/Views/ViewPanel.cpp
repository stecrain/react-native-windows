// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewPanel.h"
#include "DynamicAutomationPeer.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>

#include <XamlDirectInstance.h>

namespace winrt
{
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
} // namespace winrt

namespace winrt::react::uwp::implementation
{

const winrt::TypeName viewPanelTypeName{
    winrt::hstring{L"ViewPanel"},
    winrt::TypeKind::Metadata};

ViewPanel::ViewPanel() : Super()
{
}

winrt::AutomationPeer ViewPanel::OnCreateAutomationPeer()
{
  return winrt::make<winrt::react::uwp::implementation::DynamicAutomationPeer>(*this);
}

/*static*/ void ViewPanel::VisualPropertyChanged(winrt::DependencyObject sender, winrt::DependencyPropertyChangedEventArgs e)
{
  auto panel{sender.as<ViewPanel>()};
  if (panel.get() != nullptr)
    panel->m_propertiesChanged = true;
}

/*static*/ void ViewPanel::PositionPropertyChanged(winrt::DependencyObject sender, winrt::DependencyPropertyChangedEventArgs e)
{
  auto element{sender.as<winrt::UIElement>()};
  if (element != nullptr)
    element.InvalidateArrange();
}

/*static*/ winrt::DependencyProperty ViewPanel::ViewBackgroundProperty()
{
  static winrt::DependencyProperty s_viewBackgroundProperty =
    winrt::DependencyProperty::Register(
      L"ViewBackground",
      winrt::xaml_typename<winrt::Brush>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(
        winrt::SolidColorBrush(),
        ViewPanel::VisualPropertyChanged));

  return s_viewBackgroundProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::BorderThicknessProperty()
{
  static winrt::DependencyProperty s_borderThicknessProperty =
      winrt::DependencyProperty::Register(
          L"BorderThickness",
          winrt::xaml_typename<winrt::Thickness>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(winrt::Thickness()),
              ViewPanel::VisualPropertyChanged));

  return s_borderThicknessProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::BorderBrushProperty()
{
  static winrt::DependencyProperty s_borderBrushProperty =
      winrt::DependencyProperty::Register(
          L"BorderBrush",
          winrt::xaml_typename<winrt::Brush>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::SolidColorBrush(),
              ViewPanel::VisualPropertyChanged));

  return s_borderBrushProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::CornerRadiusProperty()
{
  static winrt::DependencyProperty s_cornerRadiusProperty =
      winrt::DependencyProperty::Register(
          L"CornerRadius",
          winrt::xaml_typename<winrt::CornerRadius>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(winrt::CornerRadius()),
              ViewPanel::VisualPropertyChanged));

  return s_cornerRadiusProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::TopProperty()
{
  static winrt::DependencyProperty s_topProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"Top",
          winrt::xaml_typename<double>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value((double)0),
              ViewPanel::PositionPropertyChanged));

  return s_topProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::LeftProperty()
{
  static winrt::DependencyProperty s_LeftProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"Left",
          winrt::xaml_typename<double>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value((double)0),
              ViewPanel::PositionPropertyChanged));

  return s_LeftProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::ClipChildrenProperty()
{
  static winrt::DependencyProperty s_clipChildrenProperty =
      winrt::DependencyProperty::Register(
          L"ClipChildren",
          winrt::xaml_typename<bool>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(false),
              ViewPanel::VisualPropertyChanged));

  return s_clipChildrenProperty;
}

/*static*/ void ViewPanel::SetTop(winrt::Windows::UI::Xaml::UIElement const& element, double value)
{
  element.SetValue(TopProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
}

/*static*/ void ViewPanel::SetLeft(winrt::Windows::UI::Xaml::UIElement const& element, double value)
{
  element.SetValue(LeftProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
}

winrt::Size ViewPanel::MeasureOverride(winrt::Size availableSize)
{
  // All children are given as much size as they'd like
  winrt::Size childConstraint(INFINITY, INFINITY);

  for (winrt::UIElement child : Children())
    child.Measure(childConstraint);

  // ViewPanels never choose their size, that is completely up to the parent - so return no size
  return winrt::Size(0, 0);
}

winrt::Size ViewPanel::ArrangeOverride(winrt::Size finalSize)
{
  for (winrt::UIElement child : Children())
  {
    double childHeight = 0.0;
    double childWidth = 0.0;

    // A Border or inner ViewPanel should take up the same space as this panel
    if (child == m_border)
    {
      childWidth = finalSize.Width;
      childHeight = finalSize.Height;
    }
    else
    {
      // We expect elements to have been arranged by yoga which means their Width & Height are set
      winrt::FrameworkElement fe = child.try_as<winrt::FrameworkElement>();
      if (fe != nullptr)
      {
        childWidth = fe.Width();
        childHeight = fe.Height();
      }
      // But we fall back to the measured size otherwise
      else
      {
        childWidth = child.DesiredSize().Width;
        childHeight = child.DesiredSize().Height;
      }
    }

    // Guard against negative values
    childWidth = std::max<double>(0.0f, childWidth);
    childHeight = std::max<double>(0.0f, childHeight);

    child.Arrange(winrt::Rect((float)ViewPanel::GetLeft(child), (float)ViewPanel::GetTop(child), (float)childWidth, (float)childHeight));
  }

  UpdateClip(finalSize);

  return finalSize;
}

void ViewPanel::InsertAt(uint32_t const index, winrt::UIElement const &value) const
{
  const auto childrenXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(Children());
  const auto valueXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(value);
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(childrenXD, index, valueXD);
}

void ViewPanel::RemoveAt(uint32_t const index) const
{
  const auto childrenXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(Children());
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(childrenXD, index);
}

void ViewPanel::Remove(winrt::UIElement element) const
{
  const auto childrenXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(Children());
  const auto elementXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(element);
  XamlDirectInstance::GetXamlDirect().RemoveFromCollection(childrenXD, elementXD);
}

void ViewPanel::Clear() const
{
  const auto childrenXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(Children());
  XamlDirectInstance::GetXamlDirect().ClearCollection(childrenXD);
}

void ViewPanel::ViewBackground(winrt::Brush const &value)
{
  SetValue(ViewBackgroundProperty(), winrt::box_value(value));
}

void ViewPanel::BorderThickness(winrt::Thickness const &value)
{
  SetValue(BorderThicknessProperty(), winrt::box_value(value));
}

void ViewPanel::BorderBrush(winrt::Brush const &value)
{
  SetValue(BorderBrushProperty(), winrt::box_value(value));
}

void ViewPanel::CornerRadius(winrt::CornerRadius const &value)
{
  SetValue(CornerRadiusProperty(), winrt::box_value(value));
}

void ViewPanel::ClipChildren(bool value)
{
  SetValue(ClipChildrenProperty(), winrt::box_value(value));
}

void ViewPanel::FinalizeProperties()
{
  if (!m_propertiesChanged)
    return;

  // There are 3 different solutions
  //
  //  A) No Border
  //      -- No need for any border properties so just contain real children
  //      >> Background applied to ViewPanel
  //
  //  B) Inner Border (child of this panel along with real children)
  //      -- Border created and made a child of this panel, alongside standard children
  //      >> Border* properties applied to Border, Background applied to ViewPanel
  //
  //  C) Outer Border
  //      -- Border created but handed out to view manager to make parent of this ViewPanel
  //      >> Border* properties and Background applied to Border

  const auto unsetValue = winrt::DependencyProperty::UnsetValue();

  bool hasBackground = ReadLocalValue(ViewBackgroundProperty()) != unsetValue;
  bool hasBorderBrush = ReadLocalValue(BorderBrushProperty()) != unsetValue;
  bool hasBorderThickness = ReadLocalValue(BorderThicknessProperty()) != unsetValue;
  bool hasCornerRadius = ReadLocalValue(CornerRadiusProperty()) != unsetValue;
  bool displayBorder = hasBorderBrush && hasBorderThickness;

  // Determine which scenario our current properties have put us into
  enum Scenario { OuterBorder, InnerBorder, NoBorder } scenario;
  if (hasCornerRadius)
  {
    scenario = Scenario::OuterBorder;
    m_hasOuterBorder = true;
  }
  else if (!displayBorder)
  {
    scenario = Scenario::NoBorder;
    m_hasOuterBorder = false;
  }
  else if (ClipChildren())
  {
    scenario = Scenario::OuterBorder;
    m_hasOuterBorder = true;
  }
  else
  {
    scenario = Scenario::InnerBorder;
    m_hasOuterBorder = false;
  }

  auto m_borderXD = XamlDirectInstance::GetXamlDirect().CreateInstance(XD::XamlTypeIndex::Border);

  // Border element
  if (scenario != Scenario::NoBorder)
  {
    // Ensure Border is created
    if (m_border == nullptr)
    {
      // Add border as the top child if using as inner border
      if (scenario == Scenario::InnerBorder)
      {
        const auto childrenXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(Children());
        XamlDirectInstance::GetXamlDirect().AddToCollection(childrenXD, m_borderXD);
      }
    }
    else
    {
      m_borderXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(m_border);
    }


    // TODO: Can Binding be used here?
    if (hasBorderBrush)
    {
      XamlDirectInstance::GetXamlDirect().SetColorProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_BorderBrush,
        BorderBrush().as<winrt::SolidColorBrush>().Color()
      );
    }
    else
      XamlDirectInstance::GetXamlDirect().ClearProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_BorderBrush
      );

    if (hasBorderThickness)
      XamlDirectInstance::GetXamlDirect().SetThicknessProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_BorderThickness,
        BorderThickness()
      );
    else
      XamlDirectInstance::GetXamlDirect().ClearProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_BorderThickness
      );

    if (hasCornerRadius)
      XamlDirectInstance::GetXamlDirect().SetCornerRadiusProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_CornerRadius,
        CornerRadius()
      );
    else
      XamlDirectInstance::GetXamlDirect().ClearProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_CornerRadius
      );
  }
  else if (m_border != nullptr)
  {
    // Remove the Border element
    Remove(m_border);
    m_border = nullptr;
  }

  if (scenario == Scenario::OuterBorder)
  {
    m_borderXD = XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(m_border);

    if (hasBackground)
    {
      XamlDirectInstance::GetXamlDirect().SetColorProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_Background,
        ViewBackground().as<winrt::SolidColorBrush>().Color()
      );
    }
    else
      XamlDirectInstance::GetXamlDirect().ClearProperty(
        m_borderXD,
        XD::XamlPropertyIndex::Border_Background
      );

    ClearValue(winrt::Panel::BackgroundProperty());
  }
  else
  {
    // Set any background on this Panel
    if (hasBackground)
      SetValue(winrt::Panel::BackgroundProperty(), ViewBackground());
    else
      ClearValue(winrt::Panel::BackgroundProperty());
  }

  m_propertiesChanged = false;
}

winrt::Border ViewPanel::GetOuterBorder()
{
  if (m_hasOuterBorder && (m_border != nullptr))
    return m_border;
  else
    return winrt::Border(nullptr);
}

void ViewPanel::UpdateClip(winrt::Size& finalSize)
{
  // When an outer Border is used it will handle the clipping, otherwise this panel must do so
  if (!m_hasOuterBorder && ClipChildren())
  {
    winrt::RectangleGeometry clipGeometry;
    clipGeometry.Rect(winrt::Rect(0, 0, static_cast<float>(finalSize.Width), static_cast<float>(finalSize.Height)));

    Clip(clipGeometry);
  }
  else
  {
    Clip(nullptr);
  }
}

} // namespace winrt::react::uwp::implementation
