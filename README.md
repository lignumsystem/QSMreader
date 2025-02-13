# QSM to Lignum

QSMreader reads a QSM file, produced by TreeQSM program (https://github.com/InverseTampere/TreeQSM.git),<br />
transforms it to Lignum tree and stores as a Lignum xml file. <br />

The QSM file contains information of one cylinder per line. The position of the line in the file defines the number of the cylinder, for example, fifth line from the top specifies the information of cylinder #5.
Each line must contain the following 14 items:
1. radius (m)
2. length (m)
3. start_point_x
4. start_point_y
5. start_point_z
6. axis_direction_x
7. axis_direction_y
8. axis_direction_z
9. Number of parent cylinder
10. Number of next cylinder (= 0, if no next cylinder)
11. Index of the branch the cylinder belongs to
12. Order of the branch (0 = stem, 1 = branch forking off from stem, etc.)
13. Number of the cylinder in the branch
14. added = is cylinder added to fill a gap (1 = true)

If there are more than 14 items the rest are ignored.

Usage: ./maketree file [-conifer] [-straighten] <br />
-conifer     Stores the tree as a conifer, default is hardwood <br />
-straighten	  Stem generated from point cloud may wobble, this option sets stem go up straight.<br />


The program works in a simple way (it is based on a version that reads root information to Lignun, the cylinder (TreeSegment in Lignum vocabulary), is refered to with link or root_link in the program):
1. The data is read in to list<vector<string> > root_links;
2. The cylinders are added one by one to Lignum tree: add_link(lignum_tree, link_num, father_num, order);
3. Then tree architeture (= positions and orientations of cylinders) is set

# COMPILATION

>[!IMPORTANT]
>Qt4 seems to be ever more difficult to maintain in MacPorts. 

Apple Silicon processors (M1, M2, etc.), macOS Ventura and later require Qt5  with XMLTree. *qt-workbench* cannot be compiled 
and used with Qt5 because of the backward incompatibilites between the Qt4 and Qt5 versions. To install Qt5 Toolkit 
from MacPorts type:

    sudo port install qt5

## CMake build system

It  seems  that  [CMake](https://cmake.org) is  becoming  increasingly
popular (perhaps  de facto standard) cross-platform  build system for
various programming languages  including C/C++. This is  true also for
Qt system that seems  to be giving up its `qmake`  tool for CMake (Qt6
onwards).

The Lignum core system as well as LignumForest, Crowndensity, and
QSMreader projects have now CMake project files to organise and
compile Lignum software.  Qt `qmake` can be still used but it is
highly recommended to start to use CMake.

One of the main benefits of CMake is its ability to use separate build
trees from source file trees, i.e.  the software compilation takes
under a single separate build directory located outside the source
trees.  The second benefit is the ability to generate build processes
for traditional Unix Makefile systems as well as for several
Integrated Development Environments (IDE) including Xcode and
Microsoft Visual Studio from the same set of *CMakeLists.txt*
configuration files.

## QSMreader: CMake for macOS and Unix/Linux Makefile build system

To create Makefile build system with CMake first create the
build tree  directory and  then with `cmake`  the Unix  Makefile build
system itself. To build the Lignum core system:

        cd lignum-core
        mkdir build
        cd build 
        cmake .. 
        make install
        
See also *lignum-core* [README](https://github.com/lignumsystem/lignum-core/blob/master/README.md).

To create QSMreader Makefile build system for debug and compile `maketree` binary 
type:

    cd QSMreader
    mkdir debug
    cd  debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make install 

For QSMreader Makefile build system for Release (optimised, no debug information) type:

    cd QSMreader
    mkdir release
    cd release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make install

In both cases `make install` will move `maketree` to QSMreader directory
        
Command line options and their  short documentation can be obtained by
running `./maketree`  without any  command line parameters.


>[!IMPORTANT]
>It is important to type `make install` to also move `maketree` to
>directory above to be used by the scripts to run simulatations.
>Typing just `make` the `maketree` program remains in the compilation directory.

>[!IMPORTANT]
>To let Unix Makefile build system keep up with file dependencies
>correctly  (for example  changes made  in  c++adt in  the Lignum  core
>system) clean  first the build  tree from previous software  build.

To recompile `maketree` type:

        make clean
        make install
        
By default Unix Makefile build system tracks only changes made
in the current QSMreader project.

>[!IMPORTANT]
>To remove all CMake  configurations and compilation work just
>remove the build  tree directory (i.e. *debug*,  *release* or *xcode*)
>and recreate the build tree directory.

>[!NOTE]
>CMake  projects   are   configured  with   *CMakeLists.txt*
>files. For  this CMake  has an  extensive set  of CMake  variables and
>built-in functions that can be set in CMakeLists.txt files or given in
>command line.

The best way to  learn CMake is by  studying examples.
lignum-core and QSMreader provide  CMakeLists.txt file examples how
to create libraries, find and integrate external libraries (Qt, HDF5)
create and use external binaries (`l2c` to compile L-system files) and
setup the final product with its dependenices.

## QSMreader: CMake for Xcode

For Xcode IDE create the Xcode project file:

    mkdir xcode
    cd xcode
    cmake .. -G Xcode

Open  Xcode  IDE  from  Terminal. Alternatively open  the  Xcode  project  file
`maketree.xcodeproj` from XCode:
     
         open maketree.xcodeproj

Build the `maketree` Product in  Xcode for debugging.  It will appear
in *xcode/Debug*  directory:

        Xcode -> Product (in the menu bar) -> Build For -> Running/Testing/Profiling

See  also that: 

        Xcode -> Product (in the menu bar) -> Scheme 

is set  to `maketree` to allow Run: 

        Xcode -> Product (in the menu bar) -> Run
        
to debug the program. Xcode IDE itself tracks file dependencies.

Set command  line parameters for  `maketree` in Xcode:

        Xcode -> Product (in the menu  bar) -> Scheme ->  Edit Scheme -> Arguments.

Divide the command line into practical parts for debugging from `Arguments -> '+'`.
## CMake for QSMreader dependency graph

CMake allows to generate `graphviz` output file to show all library and executable dependencies of the project.
Then with `dot` create image file with desired file format. For example in the *release* directory type:
        
        mkdir graphviz
        cmake ..   --graphviz=graphviz/QSMreader.dot
        dot -Tpdf -Kneato -Goverlap=prism  graphviz/QSMreader.dot  -o  QSMreader.pdf
        
The output file *QSMreader.pdf* contains the visual presentation of the target dependenices including
external binaries and required link libraries. The option `-T` understands many well known image file formats.


## QSMreader compilation with qmake

To compile QSMreader (and lignum-core) type:

    cd QSMreader
    qmake  QSMreader.pro
    make

To compile with optimization on (faster, no debug) type:

    qmake  "CONFIG+=release" QSMreader.pro
    make

To remove all compilation work type `make distclean`.


