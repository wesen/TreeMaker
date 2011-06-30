#!/bin/sh
# Note: type chmod u+x <script_name> to make <script_name> executable.
# This script builds both release and debug versions of wxWidgets from a fresh
# download, using the options required for my projects.
# To use it:
# 1. Put this script into the wxWidgets directory.
# 2. Type "./build-wxw.sh", which executes the script.
# 3. Use the --noconfig option for rebuilding after a minor edit (it does
#      not rebuild dependencies, so be careful)
#    Use the --nodebug option for skipping the debug version
#    Use the --norelease option for skipping the release version

for i in $*
do
  case $i in
    --noconfig)
      noconfig=1
      ;;
    --nodebug)
      nodebug=1
      ;;
    --norelease)
      norelease=1
      ;;
    *)
      echo "Unknown option $i"
      exit 1
      ;;
  esac
done

if [ ! $noconfig ]
then
  echo "Configuring makefiles..."
  autoconf
fi

# wxWidgets's options:
#  --with-gnomeprint: use the nice, PDF-enabled GNOME printing dialog if
#    available - it does _not_ require GNOME to either compile or run
#  --without-libtiff: we don't use TIFF at the moment, and each removed
#    DLL reduces binary portability problems
#  --disabled-shared, --enable-monolithic: again, for better binary
#    portability
options='--disable-shared --enable-monolithic --with-opengl --enable-std_iostreams --enable-std_string --with-gnomeprint --without-libtiff'

if [ ! $norelease ]
then
  echo "Creating source code & library subdirectory and configuring (release)..."
  if [ ! -d build-release ]; then mkdir build-release; fi
  cd build-release
  if [ ! $noconfig ]; then ../configure $options; fi

  echo "Compiling wxWidgets (release)..."
  make
  cd ..
fi
if [ ! $nodebug ]
then
  echo "Creating source code & library subdirectory and configuring (debug)..."
  if [ ! -d build-debug ]; then mkdir build-debug; fi
  cd build-debug
  if [ ! $noconfig ]; then ../configure --enable-debug $options; fi

  echo "Compiling wxWidgets (debug)..."
  make
  cd ..
fi
echo "I'm done, $USER."
