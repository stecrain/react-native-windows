'use strict';
import * as React from 'react';
import RN = require('react-native');

// removes from interface T the members of interface K
type Omit<T, K> = Pick<T, Exclude<keyof T, keyof K>>;

// All of T becomes optional except for Keys
type PartiallyRequired<T, Keys extends keyof T = keyof T> = Pick<Partial<T>, Exclude<keyof T, Keys>> & Pick<T, Keys>;

/**
 * All of TOrigin except Key from TUse
 */
export type UseFrom<TOrigin, TUse, Key extends keyof TUse> = Pick<TOrigin, Exclude<keyof TOrigin, Key>> & Pick<TUse, Key>;

export enum EventPhase {
  None,
  Capturing,
  AtTarget,
  Bubbling
}

export interface INativeKeyboardEvent {
  altKey: boolean;
  ctrlKey: boolean;
  metaKey: boolean;
  shiftKey: boolean;
  key: string;
  eventPhase: EventPhase;
}
export type IKeyboardEvent = RN.NativeSyntheticEvent<INativeKeyboardEvent>;
export type IHandledKeyboardEvent = PartiallyRequired<INativeKeyboardEvent, 'key'>;

export interface IViewWin32 {
  focus: () => void;
}

export type ARIARole =
  | 'alert'
  | 'alertdialog'
  | 'application'
  | 'button'
  | 'checkbox'
  | 'combobox'
  | 'dialog'
  | 'group'
  | 'link'
  | 'menu'
  | 'menubar'
  | 'menuitem'
  | 'none'
  | 'presentation'
  | 'progressbar'
  | 'radio'
  | 'radiogroup'
  | 'scrollbar'
  | 'search'
  | 'spinbutton'
  | 'switch'
  | 'tab'
  | 'tablist'
  | 'tabpanel'
  | 'textbox'
  | 'timer'
  | 'toolbar'
  | 'tree'
  | 'treeitem';

export type AnnotationType =
  | 'AdvanceProofingIssue'
  | 'Author'
  | 'CircularReferenceError'
  | 'Comment'
  | 'ConflictingChange'
  | 'DataValidationError'
  | 'DeletionChange'
  | 'EditingLockedChange'
  | 'Endnote'
  | 'ExternalChange'
  | 'Footer'
  | 'Footnote'
  | 'FormatChange'
  | 'FormulaError'
  | 'GrammarError'
  | 'Header'
  | 'Highlighted'
  | 'InsertionChange'
  | 'Mathematics'
  | 'MoveChange'
  | 'SpellingError'
  | 'TrackChanges'
  | 'Unknown'
  | 'UnsyncedChange';

export type AccessibilityAnnotationInfo = Readonly<{
  typeID: AnnotationType;
  typeName?: string;
  author?: string;
  dateTime?: string;
}>;

export type AccessibilityActionName =
  | RN.AccessibilityActionName
  | 'AddToSelection'
  | 'RemoveFromSelection'
  | 'Select'
  | 'Expand'
  | 'Collapse';

export type AccessibilityActionInfo = Readonly<{
  name: AccessibilityActionName;
  label?: string;
}>;

export type AccessibilityActionEvent = RN.NativeSyntheticEvent<
  Readonly<{
    actionName: string;
  }>
>;

export type AccessibilityStates = RN.AccessibilityStates | 'multiselectable' | 'required';

export type SharedAccessibilityPropsIOSandWin32 = {
  onAccessibilityTap?: () => void;
};

export type OmittedAccessibilityPropsWin32 = {
  accessibilityActions?: ReadonlyArray<RN.AccessibilityActionInfo>;
  accessibilityRole?: RN.AccessibilityRole;
  accessibilityStates?: RN.AccessibilityState[];
};

export type BasePropsWin32 = {
  /**
   * Tells a person using a screen reader the type of element they are focused on.
   *
   * Overrides the `accessibilityRole` prop on React Native to accept a subset of ARIA roles.
   *
   * Possible values for ARIARole are: alert, alertdialog, application, button, checkbox,
   * comobox, dialog, group, link, menu, menubar, menuitem, none, presentation, progressbar,
   * radio, radiogroup, scrollbar, search, spinbutton, switch, tab, tablist, tabpanel, textbox,
   * timer, toolbar, tree, and treeitem.
   */
  accessibilityRole?: RN.AccessibilityRole | ARIARole;
  accessibilityStates?: AccessibilityStates[];
  accessibilityActions?: ReadonlyArray<AccessibilityActionInfo>;
};

export type ViewWin32OmitTypes = RN.ViewPropsAndroid &
  RN.ViewPropsIOS &
  RN.AccessibilityPropsAndroid &
  Omit<RN.AccessibilityPropsIOS, SharedAccessibilityPropsIOSandWin32> &
  OmittedAccessibilityPropsWin32;

/**
 * Properties for ViewWin32 component
 */
export interface IViewWin32Props extends Omit<RN.ViewProps, ViewWin32OmitTypes>, BasePropsWin32 {
  // tslint:disable-next-line no-reserved-keywords -- type name matching facebook implementation
  type?: React.ElementType;
  children?: React.ReactNode;
  acceptsKeyboardFocus?: boolean;
  accessibilityActions?: ReadonlyArray<AccessibilityActionInfo>;
  /**
   * Tells a person using a screen reader what kind of annotation they
   * have selected. If available, it will also tell a person the author of the annotation and
   * the date and time the annotation was posted.
   *
   * Note: If typeID is 'Unknown', a typeName must be provided.
   */
  accessibilityAnnotation?: AccessibilityAnnotationInfo;
  accessibilityLevel?: number;
  accessibilityPositionInSet?: number;

  accessibilitySetSize?: number;
  animationClass?: string;

  /**
   * The onBlur event occurs when an element loses focus.  The opposite of onBlur is onFocus.  Note that in React
   * Native, unlike in the web, the onBlur event bubbles (similar to onFocusOut in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onBlur?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onBlur event occurs when an element loses focus.  The opposite of onBlur is onFocus.  Note that in React
   * Native, unlike in the web, the onBlur event bubbles (similar to onFocusOut in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onBlurCapture?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onFocus event occurs when an element gets focus. The opposite of onFocus is onBlur.  Note that in React
   * Native, unlike in the web, the onFocus event bubbles (similar to onFocusIn in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onFocus?: (ev: RN.NativeSyntheticEvent<{}>) => void;
  /**
   * The onFocus event occurs when an element gets focus. The opposite of onFocus is onBlur.  Note that in React
   * Native, unlike in the web, the onFocus event bubbles (similar to onFocusIn in the web).
   *
   * `ev.target === ev.currentTarget` when the focus is being lost from this component.
   * `ev.target !== ev.currentTarget` when the focus is being lost from a descendant.
   */
  onFocusCapture?: (ev: RN.NativeSyntheticEvent<{}>) => void;

  onKeyDown?: (args: IKeyboardEvent) => void;
  onKeyDownCapture?: (args: IKeyboardEvent) => void;
  onKeyUp?: (args: IKeyboardEvent) => void;
  onKeyUpCapture?: (args: IKeyboardEvent) => void;

  onMouseEnter?: () => void;
  onMouseLeave?: () => void;

  keyDownEvents?: IHandledKeyboardEvent[];
  keyUpEvents?: IHandledKeyboardEvent[];

  /**
   * Provides a screentip to be used on hover of the view
   */
  tooltip?: string;
}
