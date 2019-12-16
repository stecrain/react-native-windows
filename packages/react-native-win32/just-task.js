/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  copyTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
  cleanTask,
} = require('just-scripts');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

option('production');
option('clean');
option('ci');

task('apiExtractorVerify', apiExtractorVerifyTask());
task('apiExtractorUpdate', apiExtractorUpdateTask());

task('apiDocumenter', () => {
  require('child_process').execSync(
    'npx @microsoft/api-documenter markdown -i temp -o docs/api',
    {stdio: 'inherit'},
  );
});

task('eslint', () => {
  return eslintTask();
});
task('copyFlowFiles', () => {
  return copyTask(['src/**/*.js'], '.');
});
task('copyPngFiles', () => {
  return copyTask(['src/**/*.png'], '.');
});
task('initRNLibraries', () => {
  require('./scripts/copyRNLibraries').copyRNLibraries();
});

task('ts', () => {
  return tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
      sourceRoot: path.relative(libPath, srcPath),
    }),
    target: 'es5',
    module: 'commonjs',
  });
});
task('clean', () => {
  return cleanTask(
    ['jest', 'Libraries', 'RNTester', 'lib'].map(p =>
      path.join(process.cwd(), p),
    ),
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'eslint',
    'initRNLibraries',
    'copyFlowFiles',
    'copyPngFiles',
    // native-bundle:filtered
    // trickle
    // react-test
    'ts',
    condition('apiExtractorVerify', () => argv().ci),
    'apiExtractorUpdate',
    'apiDocumenter',
  ),
);
