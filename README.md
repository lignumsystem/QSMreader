#QSM to Lignum

This project must reside under lignum-core, that is, in the directory lignum-core/

QSMreader reads a QSM file, tansforms it to Lignum tree and stores as a Lignum xml file. <br />
STORES THE TREE AT MOMENT ONLY AS HARDWOOD TREE (applies HwTreeSegment) <br />

The QSM file contains information of one cylinder per line. The position of the line in the file defines the number of the cylinder, for example, fifth line from the top specifies the information of cylinder #5.
Each line contains the following 14 (tab separated) items:
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
14. If the cylinder has been added to fill a gap, (not based on point cloud), 1 = true
14. mad
15. SurfCov
16. added
17. UnmodRadius (m)

Usage: ./maketree file [-straighten] [-mm] [-addNeedles]  [-CR value ] <br />
-straighten	  Stem generated from point cloud may wobble, this option sets stem go up straight.<br />
-mm		  If measures are in mm instead of meter <br />
THESE TWO OPTIONS ARE NOT IN USE AT THE MOMENT! <br />
-addNeedles	  Needles are added (to Scots Pine) as explained in R. Sievanen, P. Raumonen, J. Perttunen, <br />
		  E. Nikinmaa, and P. Kaitaniemi. A study of crown development mechanisms using a shoot-based <br />
		  tree model and segmented terrestrial laser scanning data. Annals of Botany, 122(3):423–434, 2018. <br />
-CR value	  Input of crown ratio for height of crown base - makes sense only in case of -addNeedles <br />

The program works in a simple way (it is based on a version that reads root information to Lignun, the cylinder, that is,
TreeSegment in Lignum vocabulary, is refered to with link or root_link in the program):
1. The data is read in to list<vector<string> > root_links;
2. The cylinders are added one by one to Lignum tree: add_link(lignum_tree, link_num, father_num, order);
3. Then tree architeture (= positions and orientations of cylinders) is set

