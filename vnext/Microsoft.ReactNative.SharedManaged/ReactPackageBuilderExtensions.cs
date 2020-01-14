// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using System.Reflection;

namespace Microsoft.ReactNative.Managed
{
  static class ReactPackageBuilderExtensions
  {
    internal static void AddAttributedModules(this IReactPackageBuilder packageBuilder)
    {
      foreach (var type in typeof(ReactPackageBuilderExtensions).GetTypeInfo().Assembly.GetTypes())
      {
        var moduleAttribute = type.GetTypeInfo().GetCustomAttribute<ReactModuleAttribute>();
        if (moduleAttribute != null)
        {
          ReactModuleInfo moduleInfo = ReactModuleInfo.GetOrAddModuleInfo(type, moduleAttribute);
          packageBuilder.AddModule(moduleInfo.ModuleName, moduleInfo.ModuleProvider);
        }
      }
    }

    internal static void AddViewManagers(this IReactPackageBuilder packageBuilder)
    {
      foreach (var typeInfo in typeof(ReactPackageBuilderExtensions).GetTypeInfo().Assembly.DefinedTypes)
      {
        if (!typeInfo.IsAbstract && typeInfo.ImplementedInterfaces.Contains(typeof(IViewManager)))
        {
          packageBuilder.AddViewManager(typeInfo.Name, () => (IViewManager)Activator.CreateInstance(typeInfo.AsType()));
        }
      }
    }
  }
}
