// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;
using SampleLibraryCS;

namespace SampleApp
{
    sealed class MainReactNativeHost : ReactNativeHost
    {
        public MainReactNativeHost()
        {
            this.InstanceSettings.UseWebDebugger = false; // Disabled temporarily because of issue #2877
            this.InstanceSettings.UseLiveReload = true; // true by default in debug builds already
            this.InstanceSettings.UseJsi = true;
        }

        protected override string MainComponentName => "SampleApp";
        protected override string JavaScriptMainModuleName => "index.windows";
        protected override IReadOnlyList<IReactPackage> Packages
        {
            get
            {
                return new IReactPackage[] { new SampleLibraryPackage() };
            }
        }

        protected override IReadOnlyList<IReactPackageProvider> PackageProviders
        {
            get
            {
                return new IReactPackageProvider[] {
                    new ReactPackageProvider(),
                    new SampleLibraryCPP.SampleLibraryCppPackage(),
                    new SampleLibraryCS.CsStringsPackageProvider()
                };
            }
        }
    }
}
