# QSM to Lignum

This project must reside under lignum-core, that is, in the directory lignum-core/

QSMreader reads a QSM file, produced by TreeQSM program (https://github.com/InverseTampere/TreeQSM.git),<br />
transforms it to Lignum tree and stores as a Lignum xml file. <br />

The QSM file contains information of one cylinder per line. The position of the line in the file defines the number of the cylinder, for example, fifth line from the top specifies the information of cylinder #5.
Each line contains the following 17  items:
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
14. mad =  mean absolute distance (m)
15. SurfCov = surface coverage (%)
16. added = is cylinder added to fil a gap (1 = true)
17. UnmodRadius = unmodified radius (m)

Usage: ./maketree file [-conifer] [-straighten] <br />
-conifer     Stores the tree as a conifer, default is hardwood <br />
-straighten	  Stem generated from point cloud may wobble, this option sets stem go up straight.<br />


The program works in a simple way (it is based on a version that reads root information to Lignun, the cylinder (TreeSegment in Lignum vocabulary), is refered to with link or root_link in the program):
1. The data is read in to list<vector<string> > root_links;
2. The cylinders are added one by one to Lignum tree: add_link(lignum_tree, link_num, father_num, order);
3. Then tree architeture (= positions and orientations of cylinders) is set

