/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {Button, AppRegistry, StyleSheet, Text, View} from 'react-native';
const RNTesterActions = require('react-native/RNTester/js/RNTesterActions');
const RNTesterExampleContainer = require('react-native/RNTester/js/RNTesterExampleContainer');
const RNTesterExampleList = require('react-native/RNTester/js/RNTesterExampleList');
import RNTesterList from './RNTesterList.win32';
import RNTesterNavigationReducer from './RNTesterNavigationReducer.win32';
import {RNTesterAction} from './RNTesterActions.win32';

const styles = StyleSheet.create({
  headerContainer: {
    borderBottomWidth: StyleSheet.hairlineWidth,
    borderBottomColor: '#96969A',
    backgroundColor: '#F5F5F6',
  },
  header: {
    height: 40,
    flexDirection: 'row',
  },
  headerLeft: {},
  headerCenter: {
    flex: 1,
    position: 'absolute',
    top: 7,
    left: 0,
    right: 0,
  },
  title: {
    fontSize: 19,
    fontWeight: '600',
    textAlign: 'center',
  },
  exampleContainer: {
    flex: 1,
  },
});

interface IRNTesterNavigationState {
  openExample?: string;
}

interface IRNTesterAppProps {
  exampleFromAppetizeParams: string;
}

const Header = ({onBack, title}: {onBack?: () => void; title: string}) => (
  <View style={styles.headerContainer}>
    <View style={styles.header}>
      <View style={styles.headerCenter}>
        <Text style={styles.title}>{title}</Text>
      </View>
      {onBack && (
        <View style={styles.headerLeft}>
          <Button title="Back" onPress={onBack} accessibilityLabel="Back" />
        </View>
      )}
    </View>
  </View>
);

class RNTesterApp extends React.Component<
  IRNTesterAppProps,
  IRNTesterNavigationState
> {
  public render() {
    if (!this.state) {
      return <Text>null state</Text>;
    }
    if (this.state.openExample) {
      const Component = RNTesterList.Modules[this.state.openExample];
      if (Component.external) {
        return <Component onExampleExit={this._handleBack} />;
      } else {
        return (
          <View style={styles.exampleContainer}>
            <Header onBack={this._handleBack} title={Component.title} />
            <RNTesterExampleContainer module={Component} />
          </View>
        );
      }
    }
    return (
      <View style={styles.exampleContainer}>
        <Header title="RNTester" />
        <RNTesterExampleList
          onNavigate={this._handleAction}
          list={RNTesterList}
        />
      </View>
    );
  }

  /*
  UNSAFE_componentWillMount() {
    BackHandler.addEventListener('hardwareBackPress', this._handleBack);
  }
  */

  public componentDidMount() {
    /*
    Linking.getInitialURL().then((url) => {
      AsyncStorage.getItem(APP_STATE_KEY, (err, storedString) => {
        const exampleAction = URIActionMap(this.props.exampleFromAppetizeParams);
        const urlAction = URIActionMap(url);
        const launchAction = exampleAction || urlAction;
     if (err || !storedString) {
      */
    const initialAction: RNTesterAction = /*launchAction ||*/ {
      type: 'InitialAction',
    };
    this.setState(RNTesterNavigationReducer(undefined, initialAction));
    return;
    /*
        }
        const storedState = JSON.parse(storedString);
        if (launchAction) {
          this.setState(RNTesterNavigationReducer(storedState, launchAction));
          return;
        }
        this.setState(storedState);
      });
    });

    Linking.addEventListener('url', (url) => {
      this._handleAction(URIActionMap(url));
    });
    */
  }

  private _handleBack = () => {
    this._handleAction(RNTesterActions.Back());
  };

  private _handleAction = (action?: RNTesterAction) => {
    if (!action) {
      return;
    }
    const newState = RNTesterNavigationReducer(this.state, action);
    if (this.state !== newState) {
      this.setState(newState);
    }
  };
}

AppRegistry.registerComponent('RNTesterApp', () => RNTesterApp);

export = RNTesterApp;
