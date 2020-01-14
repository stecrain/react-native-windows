/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

const requireNativeComponent = require('../../ReactNative/requireNativeComponent');

import type {ViewProps} from '../View/ViewPropTypes';
import type {ViewStyleProp} from '../../StyleSheet/StyleSheet';
import type {NativeComponent} from '../../Renderer/shims/ReactNative';
import type {NativeOrDynamicColorType} from '../../Color/NativeOrDynamicColorType'; // ]TODO(macOS ISS#2323203)

type NativeProps = $ReadOnly<{|
  ...ViewProps,

  /**
   * Whether the indicator should hide when not animating (true by default).
   *
   * See http://facebook.github.io/react-native/docs/activityindicator.html#hideswhenstopped
   */
  hidesWhenStopped?: ?boolean,

  /**
   * Whether to show the indicator (true, the default) or hide it (false).
   *
   * See http://facebook.github.io/react-native/docs/activityindicator.html#animating
   */
  animating?: ?boolean,

  /**
   * The foreground color of the spinner (default is gray).
   *
   * See http://facebook.github.io/react-native/docs/activityindicator.html#color
   */
  color?: ?(string | NativeOrDynamicColorType), // TODO(macOS ISS#2323203)

  /**
   * Size of the indicator (default is 'small').
   * Passing a number to the size prop is only supported on Android.
   *
   * See http://facebook.github.io/react-native/docs/activityindicator.html#size
   */
  size?: ?('small' | 'large'),

  style?: ?ViewStyleProp,
  styleAttr?: ?string,
  indeterminate?: ?boolean,
|}>;

type ActivityIndicatorNativeType = Class<NativeComponent<NativeProps>>;

module.exports = ((requireNativeComponent(
  'RCTActivityIndicatorView',
): any): ActivityIndicatorNativeType);
