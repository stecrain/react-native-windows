// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "TestRunner.h"
#include "Unicode.h"

#include <DevSettings.h>
#include <InstanceManager.h>
#include <cxxreact/Instance.h>
#include "TestInstance.h"
#include "TestModule.h"

#include <objbase.h>
#include <locale>

#include <boost/exception/all.hpp>

using namespace facebook::react;
using namespace facebook::xplat::module;
using namespace folly;

using std::make_shared;
using std::make_tuple;
using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;
using std::wstring;

namespace Microsoft::React::Test {

void TestRunner::AwaitEvent(HANDLE &event, TestResult &result) {
  DWORD done = ::WaitForSingleObject(event, INFINITE);

  if (WAIT_OBJECT_0 != done)
    result.Status = TestStatus::Failed;
}

TestRunner::TestRunner() {}

TestResult TestRunner::RunTest(string &&bundlePath, string &&appName, NativeLoggingHook &&loggingCallback) {
  // Set up
  HANDLE functionCalled = CreateEvent(
      /* lpEventAttributes */ NULL,
      /* bManualReset      */ FALSE,
      /* bInitialState     */ FALSE,
      /* lpName            */ nullptr);
  assert(functionCalled != NULL);
  TestResult result{TestStatus::Pending, wstring()};

  try {
    // TODO: Defer MessageQueueThread creation to variant implementations
    // (Win32, WinRT).
    vector<tuple<string, CxxModule::Provider>> modules{
        make_tuple(TestModule::name, [this, &result, &functionCalled]() -> unique_ptr<CxxModule> {
          return make_unique<TestModule>([this, &result, &functionCalled](bool success) {
            if (TestStatus::Pending != result.Status)
              return;

            result.Status = success ? TestStatus::Passed : TestStatus::Failed;
            result.Message = L"markTestSucceeded(false)";
            ::SetEvent(functionCalled);
          });
        })};

    // Note, further configuration should be done in each Windows variant's
    // TestRunner implementation.
    shared_ptr<DevSettings> devSettings = make_shared<DevSettings>();
    devSettings->useWebDebugger = false; // WebSocketJSExecutor can't register native log hooks.
    devSettings->debugHost = "localhost:8081";
    devSettings->liveReloadCallback = []() {}; // Enables ChakraExecutor
    devSettings->errorCallback = [&result](string message) {
      result.Message = Microsoft::Common::Unicode::Utf8ToUtf16(message);
      result.Status = TestStatus::Failed;
    };
    devSettings->loggingCallback = std::move(loggingCallback);

    // React instance scope
    {
      shared_ptr<ITestInstance> instance =
          GetInstance(std::move(bundlePath), std::move(modules), std::move(devSettings));

      InitializeLogging([&result, &functionCalled](RCTLogLevel logLevel, const char *message) {
        if (TestStatus::Pending != result.Status)
          return;

        switch (logLevel) {
          case RCTLogLevel::Error:
          case RCTLogLevel::Fatal:
            result.Message = Microsoft::Common::Unicode::Utf8ToUtf16(message);
            result.Status = TestStatus::Failed;
            ::SetEvent(functionCalled);
            break;

          case RCTLogLevel::Info:
          case RCTLogLevel::Trace:
          case RCTLogLevel::Warning:
          default:
            break;
        }
      });

      instance->AttachMeasuredRootView(move(appName));

      // Failure on instance creation.
      if (TestStatus::Failed == result.Status)
        return result;

      AwaitEvent(functionCalled, result);

      instance->DetachRootView();
    }

    // Tear down
    CloseHandle(functionCalled);
    functionCalled = NULL;
  } catch (...) {
    result.Message =
        Microsoft::Common::Unicode::Utf8ToUtf16(boost::current_exception_diagnostic_information(/*verbose*/ true));
    result.Status = TestStatus::Failed;
  }
  return result;
}

} // namespace Microsoft::React::Test
