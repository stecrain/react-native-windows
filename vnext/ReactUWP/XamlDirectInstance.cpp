#include "pch.h"
#include <XamlDirectInstance.h>

namespace XD {
  using namespace winrt::Windows::UI::Xaml::Core::Direct;
}

XD::IXamlDirect XamlDirectInstance::m_xamlDirectInstance = NULL;

XD::IXamlDirect XamlDirectInstance::GetXamlDirect()
{
  if (m_xamlDirectInstance == NULL)
  {
    m_xamlDirectInstance = XD::XamlDirect::GetDefault();
  }
  return m_xamlDirectInstance;
}
