/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

const ReactNativeStyleAttributes = require('./ReactNativeStyleAttributes');

const ReactNativeViewAttributes = {};

ReactNativeViewAttributes.UIView = {
  pointerEvents: true,
  accessible: true,
  accessibilityActions: true,
  accessibilityLabel: true,
  accessibilityLiveRegion: true,
  accessibilityRole: true,
  accessibilityStates: true,
  accessibilityHint: true,
  acceptsKeyboardFocus: true, // TODO(macOS ISS#2323203)
  enableFocusRing: true, // TODO(macOS ISS#2323203)
  importantForAccessibility: true,
  nativeID: true,
  testID: true,
  textStyle: true, // [Win32] Once we flush out our JS theming story this property will no longer be needed
  tooltip: true, // [Win32]
  tabIndex: true, // TODO(win ISS#2323203)
  renderToHardwareTextureAndroid: true,
  shouldRasterizeIOS: true,
  onLayout: true,
  onAccessibilityAction: true,
  onAccessibilityTap: true,
  onMagicTap: true,
  onAccessibilityEscape: true,
  collapsable: true,
  needsOffscreenAlphaCompositing: true,
  onMouseEnter: true, // [TODO(macOS ISS#2323203)
  onMouseLeave: true,
  onDragEnter: true,
  onDragLeave: true,
  onDrop: true,
  draggedTypes: true, // ]TODO(macOS ISS#2323203)
  style: ReactNativeStyleAttributes,
};

ReactNativeViewAttributes.RCTView = {
  ...ReactNativeViewAttributes.UIView,

  // This is a special performance property exposed by RCTView and useful for
  // scrolling content when there are many subviews, most of which are offscreen.
  // For this property to be effective, it must be applied to a view that contains
  // many subviews that extend outside its bound. The subviews must also have
  // overflow: hidden, as should the containing view (or one of its superviews).
  removeClippedSubviews: true,
};

module.exports = ReactNativeViewAttributes;
