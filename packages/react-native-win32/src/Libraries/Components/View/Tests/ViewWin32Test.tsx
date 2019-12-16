'use strict';
import * as React from 'react';
import { StyleSheet, Text, TouchableHighlight } from 'react-native';
import { ViewWin32 } from '../ViewWin32';
import { IKeyboardEvent, IHandledKeyboardEvent } from '../ViewWin32.Props';
import { IRNTesterPage } from 'src/RNTester/RNTester.types';

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black'
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'row',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around'
  },
  keyEnterVisualizer: {
    margin: 5,
    alignItems: 'center',
    minWidth: 100,
    minHeight: 30
  },
  blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 }
});

interface IFocusableComponentState {
  hasFocus: boolean;
}

class FocusMoverTestComponent extends React.Component<{}, IFocusableComponentState> {
  private _focusTarget: ViewWin32 = null;
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false
    };
  }
  public render() {
    return (
      <ViewWin32 style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-around', marginVertical: 5 }}>
        <TouchableHighlight onPress={this._onPress}>
          <ViewWin32 style={styles.blackbox} />
        </TouchableHighlight>
        <ViewWin32
          ref={this._setRef}
          acceptsKeyboardFocus
          style={this.state.hasFocus ? { backgroundColor: '#aee8fcff' } : { backgroundColor: '#00000000' }}
          onFocus={this._onFocus}
          onBlur={this._onBlur}
        >
          <Text>{this.state.hasFocus ? 'Focus: Yes' : 'Focus: No'}</Text>
        </ViewWin32>
      </ViewWin32>
    );
  }

  private _setRef = (ref: ViewWin32) => {
    this._focusTarget = ref;
  };

  private _onPress = () => {
    if (this._focusTarget !== undefined) {
      this._focusTarget.focus();
    }
  };

  private _onFocus = () => {
    this.setState({
      hasFocus: true
    });
  };

  private _onBlur = () => {
    this.setState({
      hasFocus: false
    });
  };
}

interface IKeyboardableComponentState {
  lastKeyDown: string;
  lastKeyUp: string;
}

const handledNativeKeyboardEvents: IHandledKeyboardEvent[] = [
  { key: 'ArrowDown' },
  { key: 'ArrowUp' },
  { key: 'ArrowLeft' },
  { key: 'ArrowRight' },
  { key: 'Tab' }
];

class KeyboardTestComponent extends React.Component<{}, IFocusableComponentState & IKeyboardableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false,
      lastKeyDown: null,
      lastKeyUp: null
    };
  }

  public render() {
    return (
      <ViewWin32 keyDownEvents={handledNativeKeyboardEvents} keyUpEvents={handledNativeKeyboardEvents}>
        <ViewWin32
          style={this.state.hasFocus ? [styles.keyComponentRoot, styles.border] : styles.keyComponentRoot}
          acceptsKeyboardFocus
          onKeyUp={this._onKeyUp}
          onKeyDown={this._onKeyDown}
          onFocus={this._onFocus}
          onBlur={this._onBlur}
        >
          <ViewWin32 style={styles.keyEnterVisualizer}>
            <Text>OnKeyDown</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '}</Text>
          </ViewWin32>
          <ViewWin32 style={styles.keyEnterVisualizer}>
            <Text>OnKeyUp</Text>
            <Text>----</Text>
            <Text>{this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '}</Text>
          </ViewWin32>
        </ViewWin32>
      </ViewWin32>
    );
  }

  private _onFocus = () => {
    this.setState({
      hasFocus: true
    });
  };

  private _onBlur = () => {
    this.setState({
      hasFocus: false
    });
  };

  private _onKeyUp = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyUp: ev.nativeEvent.key, lastKeyDown: null });
  };

  private _onKeyDown = (ev: IKeyboardEvent) => {
    this.setState({ lastKeyDown: ev.nativeEvent.key, lastKeyUp: null });
  };
}

interface IHoverComponentProps {
  color: string;
}
class HoverTestComponent extends React.Component<IHoverComponentProps, IFocusableComponentState> {
  public constructor(props) {
    super(props);
    this.state = {
      hasFocus: false
    };
  }

  public render() {
    return (
      <ViewWin32
        onMouseEnter={this._onMouseEnter}
        onMouseLeave={this._onMouseLeave}
        style={this.state.hasFocus ? [styles.blackbox, { backgroundColor: this.props.color }] : styles.blackbox}
      />
    );
  }
  private _onMouseLeave = () => {
    this.setState({ hasFocus: false });
  };
  private _onMouseEnter = () => {
    this.setState({ hasFocus: true });
  };
}

class HoverExample extends React.Component {
  public render() {
    return (
      <ViewWin32 style={{ flexDirection: 'row', marginHorizontal: 75, justifyContent: 'space-around' }}>
        <HoverTestComponent color="red" />
        <HoverTestComponent color="orange" />
        <HoverTestComponent color="yellow" />
        <HoverTestComponent color="green" />
        <HoverTestComponent color="blue" />
        <HoverTestComponent color="indigo" />
        <HoverTestComponent color="violet" />
      </ViewWin32>
    );
  }
}

const ToolTipExample: React.FunctionComponent<{}> = () => {
  return (
    <ViewWin32
      style={{
        backgroundColor: 'red',
        height: 100,
        width: 100
      }}
      tooltip="Example tooltip"
    />
  );
};

const ViewWin32TestPage: IRNTesterPage = {
  title: '<ViewWin32>',
  displayName: 'ViewWin32 Example',
  description: 'All the stock View props plus Win32 specific ones',
  examples: [
    {
      title: 'focus() method example',
      description: 'Each of these black boxes moves focus to the ViewWin32 on the right',
      render(): JSX.Element {
        return (
          <ViewWin32>
            <FocusMoverTestComponent />
            <FocusMoverTestComponent />
            <FocusMoverTestComponent />
          </ViewWin32>
        );
      }
    },
    {
      title: 'KeyboardEvents example',
      description: 'Native keyboarding has been prevented',
      render(): JSX.Element {
        return <KeyboardTestComponent />;
      }
    },
    {
      title: 'Hover example',
      description: 'Hover a rainbow',
      render(): JSX.Element {
        return <HoverExample />;
      }
    },
    {
      title: 'Tooltip example',
      description: 'Displays a tooltip on hover',
      render(): JSX.Element {
        return <ToolTipExample />;
      }
    }
  ]
};

export = ViewWin32TestPage;
