/* eslint-env node */
module.exports = {
  extends: ['eslint:recommended', 'plugin:@typescript-eslint/recommended-type-checked', 'plugin:svelte/recommended'],
  parser: '@typescript-eslint/parser',
  parserOptions: {
    extraFileExtensions: ['.svelte'],
    tsconfigRootDir: __dirname,
    project: './tsconfig.json',
  },
  plugins: ['@typescript-eslint'],
  overrides: [
    {
      files: ['*.svelte'],
      parser: 'svelte-eslint-parser',
      parserOptions: {
        parser: '@typescript-eslint/parser'
      }
    }
  ],
  env: {
    browser: true,
    node: true,
    jasmine: true
  },
  root: true,
};
