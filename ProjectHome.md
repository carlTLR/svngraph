# About #

[Subversion](http://subversion.apache.org/) (SVN) is an open source version control system. It scales very well and many big open source projects use SVN.

This utility takes an SVN path as input and outputs graphical representation in [dot](http://www.graphviz.org/) format. The output can then be converted to any of the supported image format or opened directly to any of the available tool.

---


# Install #
There is no package yet and the code needs to be compiled to use.
## Prerequisite ##
  * [libsvn](http://subversion.apache.org/)
  * [boost](http://www.boost.org/)
  * [cmake](http://www.cmake.org/)

## Compile ##
  * `[svngraph]$ mkdir build; cd build`
  * `[build]$ cmake ../`
  * `[build]$ make`

## Usage ##
  * `[build]$ ./bin/scmGraph http://svngraph.googlecode.com/svn > svngraph.dot`