// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  public interface IManagedNativeModule
  {
    string Name { get; }
    void Initialize();
  }

  public sealed class ManagedNativeModule : INativeModule
  {
    public IManagedNativeModule NativeModule { get; private set; }

    internal TypeInfo TypeInfo { get; private set; }

    public ManagedNativeModule(IManagedNativeModule nativeModule)
    {
      NativeModule = nativeModule ?? throw new ArgumentNullException(nameof(nativeModule));
      TypeInfo = nativeModule.GetType().GetTypeInfo();
    }

    // INativeModule

    public string Name => NativeModule.Name;

    public void Initialize() => NativeModule.Initialize();

    public IReadOnlyDictionary<string, object> Constants
    {
      get
      {
        if (null == _constants)
        {
          var constants = new Dictionary<string, object>();

          foreach (var fieldInfo in TypeInfo.DeclaredFields)
          {
            var constantAttribute = ReflectionHelpers.GetCustomAttributeData<NativeModuleConstantAttribute>(fieldInfo);
            if (null != constantAttribute)
            {
              constants.Add(ReflectionHelpers.GetStringAttributeValueByName(constantAttribute, "Name") ?? fieldInfo.Name, fieldInfo.GetValue(NativeModule));
            }
          }

          foreach (var propertyInfo in TypeInfo.DeclaredProperties)
          {
            var constantAttribute = ReflectionHelpers.GetCustomAttributeData<NativeModuleConstantAttribute>(propertyInfo);
            if (null != constantAttribute)
            {
              constants.Add(ReflectionHelpers.GetStringAttributeValueByName(constantAttribute, "Name") ?? propertyInfo.Name, propertyInfo.GetMethod.Invoke(NativeModule, null));
            }
          }

          _constants = constants;
        }
        return _constants;
      }
    }
    private IReadOnlyDictionary<string, object> _constants;

    public IReadOnlyList<Bridge.MethodInfo> Methods
    {
      get
      {
        if (null == _methods)
        {
          var methods = new List<Bridge.MethodInfo>();

          foreach (var methodInfo in TypeInfo.DeclaredMethods)
          {
            var methodAttribute = ReflectionHelpers.GetCustomAttributeData<NativeModuleMethodAttribute>(methodInfo);

            if (null != methodAttribute)
            {
              string methodName = GetName(methodAttribute) ?? methodInfo.Name;

              var returnType = GetReturnType(methodAttribute);

              Method method = null;

              if (!returnType.HasValue) // Determine the ReturnType using reflection
              {
                if (methodInfo.ReturnType == typeof(void))
                {
                  // Assume void
                  returnType = ReturnType.Void;
                  method = (args, callback, ___) =>
                  {
                    var nativeArguments = ParseNativeArguments(args, methodInfo.GetParameters());
                    methodInfo.Invoke(NativeModule, nativeArguments);
                  };
                }
                else
                {
                  // Assume callback
                  returnType = ReturnType.Callback;
                  method = (args, callback, ___) =>
                  {
                    var nativeArguments = ParseNativeArguments(args, methodInfo.GetParameters());
                    var result = methodInfo.Invoke(NativeModule, nativeArguments);
                    callback(new object[] { result });
                  };
                }
              }
              else if (returnType.Value == ReturnType.Void) // ReturnType is Void
              {
                method = (args, __, ___) =>
                {
                  var nativeArguments = ParseNativeArguments(args, methodInfo.GetParameters());
                  methodInfo.Invoke(NativeModule, nativeArguments);
                };
              }
              else if (returnType.Value == ReturnType.Callback) // ReturnType is Callback
              {
                // Callback
                method = (args, callback, ___) =>
                {
                  var nativeArguments = ParseNativeArguments(args, methodInfo.GetParameters());
                  nativeArguments[nativeArguments.Length - 1] = callback;
                  methodInfo.Invoke(NativeModule, nativeArguments);
                };
              }
              else if (returnType.Value == ReturnType.Promise) // ReturnType is Promise
              {
                // Promise
                method = (args, resolve, reject) =>
                {
                  var nativeArguments = ParseNativeArguments(args, methodInfo.GetParameters());
                  nativeArguments[nativeArguments.Length - 2] = resolve;
                  nativeArguments[nativeArguments.Length - 1] = reject;
                  methodInfo.Invoke(NativeModule, nativeArguments);
                };
              }

              methods.Add(new Bridge.MethodInfo(methodName, returnType.Value, method));
            }
          }

          _methods = methods;
        }
        return _methods;
      }
    }
    private IReadOnlyList<Bridge.MethodInfo> _methods;

    private static string GetName(CustomAttributeData customAttributeData)
    {
      return ReflectionHelpers.GetStringAttributeValueByName(customAttributeData, "Name");
    }

    private static ReturnType? GetReturnType(CustomAttributeData customAttributeData)
    {
      return ReflectionHelpers.GetAttributeValueByName<ReturnType>(customAttributeData, "ReturnType");
    }

    private static object[] ParseNativeArguments(IReadOnlyList<object> input, ParameterInfo[] targetParameters)
    {
      var outputArray = new object[targetParameters.Length];
      Array.Copy(input.ToArray(), outputArray, input.Count);
      return outputArray;
    }
  }
}
