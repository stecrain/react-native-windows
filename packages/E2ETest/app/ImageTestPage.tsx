/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { StyleSheet, View, Image, Button } from 'react-native'
import React, { useState } from 'react';
import { SHOW_IMAGE_BORDER, IMAGE_CONTAINER, TREE_DUMP_RESULT } from './Consts';
import { TreeDumpControl } from './TreeDumpControl'

const styles = StyleSheet.create({
  container: {
    height:300,
    width:500,
    backgroundColor: 'yellow',
    alignItems:'center',
  },
  containerWithBorder: {
    height:300,
    width:500,
    borderRadius: 10.0,
    borderWidth:10,
    borderColor: '#00ff0055',
    backgroundColor: 'yellow',
    alignItems:'center',
  },
  imageWithBorder: {
    height: '100%',
    width: '100%',
    borderRadius: 10.0,
    borderWidth:10,
    borderColor: '#0000ff55',
    backgroundColor: 'red',
  },
  image: {
    height: '100%',
    width: '100%',
    backgroundColor: 'red',
  },  
  treeDumpControl: {
    height: 150,
    width: 500,
    margin: 10,
  },
});

export function ImageTestPage() {
  const [imageWithBorder, setImageBorder] = useState(false);
  const [clickCount, setClickCount] = useState(0);
  const onPressBorder = () => {
    var previousImageBorderState = imageWithBorder;
    setImageBorder(!previousImageBorderState);
    var previousClickCount = clickCount;
    setClickCount(previousClickCount+1);
 }
  return(
  <View>
    <View testID={IMAGE_CONTAINER} style={imageWithBorder?styles.containerWithBorder:styles.container}>
      <Image
        style={imageWithBorder?styles.imageWithBorder:styles.image}
        resizeMode={'center'}
        source={{uri: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAzCAYAAAA6oTAqAAAAEXRFWHRTb2Z0d2FyZQBwbmdjcnVzaEB1SfMAAABQSURBVGje7dSxCQBACARB+2/ab8BEeQNhFi6WSYzYLYudDQYGBgYGBgYGBgYGBgYGBgZmcvDqYGBgmhivGQYGBgYGBgYGBgYGBgYGBgbmQw+P/eMrC5UTVAAAAABJRU5ErkJggg=='}}        
      />      
    </View >
    <Button title= {imageWithBorder?"Hide Border":"Show Border"} 
          onPress={onPressBorder} 
          testID={SHOW_IMAGE_BORDER}/> 
    <TreeDumpControl style={styles.treeDumpControl} dumpID={imageWithBorder?'ImageWithBorder':(clickCount == 0 ? 'ImageWithoutBorder':'ImageWithoutBorder-Subsequent')} uiaID={IMAGE_CONTAINER} testID={TREE_DUMP_RESULT} />
  </View>);
}