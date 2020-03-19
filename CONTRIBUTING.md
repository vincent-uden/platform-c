# Contributing to Platform-C
The contributing guidelines are still in early development and are subject to change at any time.

## Table of contents

- General Notes
- C Styleguide
  - Language Verision
  - Source Code Style

## General Notes
Don't include temporary files in commits if it can be avoided. This includes but is not limited to Vim's .swp files, emacs temporary files, Visual Studio's .sln files and temporary files only used for debugging. Vim's .swp files are already in the .gitignore, feel free to add other files to the list if necessary. 

## C Styleguide
The styleguide is a general suggestion to follow, not a law. It's open to changes at any time if better alternatives would appear to what's written below.

### Language verision
The compiler uses C99 with the `-pedantic` flag. This (intentionally) limits the language use to only features included in the C99 standard as implemented in GCC.

### Source Code Style
Here is a short example which should demonstrate what code should look like.
```C
/* A demo funtion used to demonstrate style guidelines */
int func(int x, char* str) {
  // Loop that does something
  for ( int i = 0; i < 10; i++ ) {
    if ( x == 2 ) {
      return 1;
    } else {
      break;
    }
  }
  
  switch ( *str ) {
  case 'a':
    return 2;
    break;
  default:
    return 3;
    break;
  }
  
  return 0;
}
```
- Curly braces go on the same line as function definitions, if statements, loops, etc. This is to not waste an entire line on just a curly brace.  
- The parenthesis in for loops, if statements, switches, but __not__ function definitions or calls should open and close with a space for padding. This is to increase readability for nested parenthesis (which don't occur in function definitions, and functions calls this complicated are usually split onto several lines anyways).
  - `if ( 5 * (3 + 5) ) {` instead of `if (5 * (3 + 5)) {`
- Case statements use the same level of indentation as the switch statement. This saves screen space.
- Permanent comments used for documentation should use the `/* Comment */` syntax.
- Temporary comments used while debugging or for TODOs use the `// Comment` syntax. This is to easily be able to find and remove temporary comments with grep/sed.
