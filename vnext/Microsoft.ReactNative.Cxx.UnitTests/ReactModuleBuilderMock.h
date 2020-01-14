// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "JSValue.h"

namespace winrt::Microsoft::ReactNative {

struct ReactModuleBuilderMock {
  template <class... TArgs>
  void Call0(std::wstring const &methodName, TArgs &&... args) noexcept;

  template <class TResult, class... TArgs>
  void Call1(std::wstring const &methodName, std::function<void(TResult)> const &resolve, TArgs &&... args) noexcept;

  template <class TResult, class TError, class... TArgs>
  void Call2(
      std::wstring const &methodName,
      std::function<void(TResult)> const &resolve,
      std::function<void(TError)> const &reject,
      TArgs &&... args) noexcept;

  template <class TResult, class... TArgs>
  void CallSync(std::wstring const &methodName, TResult &result, TArgs &&... args) noexcept;

  JSValueObject GetConstants() noexcept;

  template <class T>
  void SetEventHandler(std::wstring const &eventName, std::function<void(T)> const &eventHandler) noexcept;

  bool IsResolveCallbackCalled() const noexcept;
  void IsResolveCallbackCalled(bool value) noexcept;
  bool IsRejectCallbackCalled() const noexcept;
  void IsRejectCallbackCalled(bool value) noexcept;

 public: // IReactModuleBuilder
  void SetEventEmitterName(hstring const &name) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;
  void AddConstantProvider(ConstantProvider const &constantProvider) noexcept;
  void AddEventHandlerSetter(hstring const &name, ReactEventHandlerSetter const &eventHandlerSetter) noexcept;

 private:
  MethodDelegate GetMethod0(std::wstring const &methodName) const noexcept;
  MethodDelegate GetMethod1(std::wstring const &methodName) const noexcept;
  MethodDelegate GetMethod2(std::wstring const &methodName) const noexcept;
  SyncMethodDelegate GetSyncMethod(std::wstring const &methodName) const noexcept;

  static IJSValueWriter ArgWriter(JSValue &jsValue) noexcept;
  template <class... TArgs>
  static IJSValueReader ArgReader(TArgs &&... args) noexcept;
  static IJSValueReader CreateArgReader(std::function<void(IJSValueWriter const &)> const &argWriter) noexcept;

  template <class T>
  MethodResultCallback ResolveCallback(JSValue const &jsValue, std::function<void(T)> const &resolve) noexcept;
  template <class T>
  MethodResultCallback RejectCallback(JSValue const &jsValue, std::function<void(T)> const &reject) noexcept;

 private:
  std::wstring m_eventEmitterName;
  std::map<std::wstring, std::tuple<MethodReturnType, MethodDelegate>> m_methods;
  std::map<std::wstring, SyncMethodDelegate> m_syncMethods;
  std::vector<ConstantProvider> m_constProviders;
  std::map<std::wstring, std::function<void(IJSValueReader &)>> m_eventHandlers;
  bool m_isResolveCallbackCalled;
  bool m_isRejectCallbackCalled;
};

struct ReactModuleBuilderImpl : implements<ReactModuleBuilderImpl, IReactModuleBuilder> {
  ReactModuleBuilderImpl(ReactModuleBuilderMock &mock) noexcept;

 public: // IReactModuleBuilder
  void SetEventEmitterName(hstring const &name) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;
  void AddConstantProvider(ConstantProvider const &constantProvider) noexcept;
  void AddEventHandlerSetter(hstring const &name, ReactEventHandlerSetter const &eventHandlerSetter) noexcept;

 private:
  ReactModuleBuilderMock &m_mock;
};

//===========================================================================
// ReactModuleBuilderMock inline implementation
//===========================================================================

template <class... TArgs>
inline void ReactModuleBuilderMock::Call0(std::wstring const &methodName, TArgs &&... args) noexcept {
  if (auto method = GetMethod0(methodName)) {
    JSValue jsValue;
    method(ArgReader(std::forward<TArgs>(args)...), ArgWriter(jsValue), nullptr, nullptr);
  }
}

template <class TResult, class... TArgs>
inline void ReactModuleBuilderMock::Call1(
    std::wstring const &methodName,
    std::function<void(TResult)> const &resolve,
    TArgs &&... args) noexcept {
  if (auto method = GetMethod1(methodName)) {
    JSValue jsValue;
    method(ArgReader(std::forward<TArgs>(args)...), ArgWriter(jsValue), ResolveCallback(jsValue, resolve), nullptr);
  }
}

template <class TResult, class TError, class... TArgs>
inline void ReactModuleBuilderMock::Call2(
    std::wstring const &methodName,
    std::function<void(TResult)> const &resolve,
    std::function<void(TError)> const &reject,
    TArgs &&... args) noexcept {
  if (auto method = GetMethod2(methodName)) {
    JSValue jsValue;
    method(
        ArgReader(std::forward<TArgs>(args)...),
        ArgWriter(jsValue),
        ResolveCallback(jsValue, resolve),
        RejectCallback(jsValue, reject));
  }
}

template <class TResult, class... TArgs>
inline void
ReactModuleBuilderMock::CallSync(std::wstring const &methodName, TResult &result, TArgs &&... args) noexcept {
  if (auto method = GetSyncMethod(methodName)) {
    JSValue jsValue;
    method(ArgReader(std::forward<TArgs>(args)...), ArgWriter(jsValue));
    ReadArgs(MakeJSValueTreeReader(jsValue), result);
  }
}

template <class T>
inline void ReactModuleBuilderMock::SetEventHandler(
    std::wstring const &eventName,
    std::function<void(T)> const &eventHandler) noexcept {
  m_eventHandlers.emplace(eventName, [eventHandler](IJSValueReader const &reader) noexcept {
    std::remove_const_t<std::remove_reference_t<T>> arg;
    ReadArgs(reader, /*out*/ arg);
    eventHandler(arg);
  });
}

template <class... TArgs>
inline /*static*/ IJSValueReader ReactModuleBuilderMock::ArgReader(TArgs &&... args) noexcept {
  return CreateArgReader([&args...](IJSValueWriter const &writer) mutable noexcept {
    WriteArgs(writer, std::forward<TArgs>(args)...);
  });
}

template <class T>
inline MethodResultCallback ReactModuleBuilderMock::ResolveCallback(
    JSValue const &jsValue,
    std::function<void(T)> const &resolve) noexcept {
  return [ this, &jsValue, &resolve ](IJSValueWriter const & /*writer*/) noexcept {
    std::remove_const_t<std::remove_reference_t<T>> arg;
    ReadArgs(MakeJSValueTreeReader(jsValue), arg);
    resolve(arg);
    m_isResolveCallbackCalled = true;
  };
}

template <class T>
inline MethodResultCallback ReactModuleBuilderMock::RejectCallback(
    JSValue const &jsValue,
    std::function<void(T)> const &reject) noexcept {
  return [ this, &jsValue, &reject ](IJSValueWriter const & /*writer*/) noexcept {
    std::remove_const_t<std::remove_reference_t<T>> arg;
    ReadArgs(MakeJSValueTreeReader(jsValue), arg);
    reject(arg);
    m_isRejectCallbackCalled = true;
  };
}

inline bool ReactModuleBuilderMock::IsResolveCallbackCalled() const noexcept {
  return m_isResolveCallbackCalled;
}

inline void ReactModuleBuilderMock::IsResolveCallbackCalled(bool value) noexcept {
  m_isResolveCallbackCalled = value;
}

inline bool ReactModuleBuilderMock::IsRejectCallbackCalled() const noexcept {
  return m_isRejectCallbackCalled;
}

inline void ReactModuleBuilderMock::IsRejectCallbackCalled(bool value) noexcept {
  m_isRejectCallbackCalled = value;
}

//===========================================================================
// ReactModuleBuilderImpl inline implementation
//===========================================================================

inline ReactModuleBuilderImpl::ReactModuleBuilderImpl(ReactModuleBuilderMock &mock) noexcept : m_mock{mock} {}

inline void ReactModuleBuilderImpl::SetEventEmitterName(hstring const &name) noexcept {
  m_mock.SetEventEmitterName(name);
}

inline void ReactModuleBuilderImpl::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  m_mock.AddMethod(name, returnType, method);
}

inline void ReactModuleBuilderImpl::AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
  m_mock.AddSyncMethod(name, method);
}

inline void ReactModuleBuilderImpl::AddConstantProvider(ConstantProvider const &constantProvider) noexcept {
  m_mock.AddConstantProvider(constantProvider);
}

inline void ReactModuleBuilderImpl::AddEventHandlerSetter(
    hstring const &name,
    ReactEventHandlerSetter const &eventHandlerSetter) noexcept {
  m_mock.AddEventHandlerSetter(name, eventHandlerSetter);
}

} // namespace winrt::Microsoft::ReactNative
