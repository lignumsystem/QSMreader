#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <math.h>
#include <XMLTree.h>
#include <QSMreader.h>
#include <PositionVector.h>
#include <Point.h>
#include <string>
#include <algorithm>
#include<iterator>

using namespace cxxadt;
using namespace std;


bool is_csv = false;

void process_line(string& line, vector<string>& items) {
  istringstream l(line);

  for(int i = 0; i < 14; i++) {
    if(is_csv) {
      std::getline(l,items[i], ',');
    } else {
      l >> items[i];
    }
  }
}

int main(int argc, char** argv)
{

  if(argc < 2){
    cout << "Reads the QSM file <file>, transforms it to Lignum tree and stores as a Lignum xml file" << endl;
    cout << "with name *.xml where * stands for:  possible extension dropped off." << endl << endl;
    cout << "Usage: ./maketree <file> [-conifer] [-ellipse] [-kite] [-straighten] [-csv] [-leafFile <file>]" << endl;
    cout << "-conifer     Stores the tree as a conifer, default is hardwood" << endl; 
    cout << "-straighten  Stem generated from point cloud may wobble, this option sets stem go up straight."<<endl;
    cout << "-csv         Default of the QSM file is Space-delimited Text, this reads Comma-separated Values." << endl;
    cout << "-ellipse     Triangle is the default leaf of deciduous, this makes leaves ellipses." << endl;
    cout << "-kite        Triangle is the default leaf of deciduous, this makes leaves kite shaped." << endl;
    cout << "-leafFile <file>   Deciduous tree may have leaves given in a separate file" << endl;
    cout << "                   Read it and then add a segment (lenghth = 0.1, diameter = 0)" << endl;
    cout << "                   at top of the tree and attach all leaves to it. The leaf data is for" << endl;
    cout << "                   kite shaped leaves (at least for time being), they can be stored also as ellipses" << endl;
    cout << "                   or triangles. <file> is read in as a comma separated file." << endl;
    cout << endl;
    cout << "Each line of the input file contains the information about one QSM cylinder. Program assumes there are" << endl;
    cout << "at least 14 items in a line. If there are more (in some cases additional information items have been" << endl;
    cout << "stored) they are ignored. The necessary 14 items are:" << endl;
    cout << " 1. radius (m)" << endl;
    cout << " 2. length (m)" << endl;
    cout << " 3. start_point_x" << endl;
    cout << " 4. start_point_y" << endl;
    cout << " 5. start_point_z" << endl;
    cout << " 6. axis_direction_x" << endl;
    cout << " 7. axis_direction_y" << endl;
    cout << " 8. axis_direction_z" << endl;
    cout << " 9. Number of parent cylinder" << endl;
    cout << " 10. Number of next cylinder (= 0, if no next cylinder)" << endl;
    cout << " 11. Index of the branch the cylinder belongs to" << endl;
    cout << " 12. Order of the branch (0 = stem, 1 = branch forking off from stem, etc.)" << endl;
    cout << " 13. Number of the cylinder in the branch" << endl;
    cout << " 14. If added to cover a volume void of points" << endl;
    cout << endl;
    cout << "In addition to these 14 values, the number of the line of cylinder items denotes the number" << endl;
    cout << "of the particular cylinder." << endl << endl;
    cout << endl;
    exit(0);
  }

  string line;
  int lineNumber = 0;


  is_csv = false;
  if(CheckCommandLine(argc,argv,"-csv")) {
    is_csv = true;
  }
  
  //read in data file of cylinderinformation

// 14 values are read from a line. They are
// 1. radius (m)
// 2. length (m)
// 3. start_point_x
// 4. start_point_y
// 5. start_point_z
// 6. axis_direction_x
// 7. axis_direction_y
// 8. axis_direction_z
// 9. Number of parent cylinder
// 10. Number of next cylinder (= 0, if no next cylinder)
// 11. Index of the branch the cylinder belongs to
// 12. Order of the branch (0 = stem, 1 = branch forking off from stem, etc.)
// 13. Number of the cylinder in the branch
// 14. If added to cover a volume void of points

  ifstream input_file(argv[1]);
  if (!input_file) {
    cout << "Did not find input file " << argv[1] << endl;
    exit(1);
  } else {
    cout << "Input file:  " << argv[1] << endl;
  }

  //  //root link = cylinder in QSM file

  vector<string> items(15);               //This is for items on one line
  list<vector<string> > root_links;       //This contains all information of the input file

  //  //Read in input file

  getline(input_file,line);                 //header

  getline(input_file,line);       //first segment to start
 
  lineNumber++;
  process_line(line, items)/*, needle_items)*/;
  string lineNumberStr = std::to_string(lineNumber);
  items[14] =  lineNumberStr;
  root_links.push_back(items);


  //This is the position of the tree (start of first cylinder), used later
  Point p0(atof(items[2].c_str()), atof(items[3].c_str()), atof(items[4].c_str()));
  cout << " p0 " << p0 << endl;
  bool last = false;
  while(input_file.good()){
    getline(input_file,line);

    if(input_file.eof()) {
      last = true;
      // break;
    }
    process_line(line, items);
    lineNumber++;
    string lineNumberStr = std::to_string(lineNumber);
    items[14] =  lineNumberStr;
    root_links.push_back(items);
  }  //end of reading input file


  //  //Make tree

  bool is_hw = true;
  bool is_hw_triangle = true;     //default leaf shape
  bool is_hw_ellipse = false;
  bool is_hw_kite = false;
  if(CheckCommandLine(argc,argv,"-conifer")) {
    is_hw = false;
    is_hw_ellipse = false;
    is_hw_triangle = false;
    is_hw_kite = false;
  } else {         // is_hw, the defaul leaf of a hardwood is Triangle
    is_hw = true;
    is_hw_triangle = true;
    if(CheckCommandLine(argc,argv,"-ellipse") ) {
      is_hw_ellipse = true;
      is_hw_triangle = false;
      is_hw_kite = false;
    }
    if(CheckCommandLine(argc,argv,"-kite") ) {
      is_hw_kite = true;
      is_hw_triangle = false;
      is_hw_ellipse = false;
    }
  }

  Tree<HwQSMSegment,HwQSMBud> lignum_tree_hw(Point(0,0,0), PositionVector(0,0,1.0));
  Tree<CfQSMSegment,CfQSMBud> lignum_tree_cf(Point(0,0,0), PositionVector(0,0,1.0));
  Tree<HwQSMSegment_e,HwQSMBud_e> lignum_tree_hw_e(Point(0,0,0), PositionVector(0,0,1.0));
  Tree<HwQSMSegment_k,HwQSMBud_k> lignum_tree_hw_k(Point(0,0,0), PositionVector(0,0,1.0));

  //Add cylinders one by one
  //link_num = number of link starting with 0. If == 0, add_link adds first TreeSegment to the tree,
  //otherwise just adds after parent link.   link_number = row number in the file - 1.

  list<vector<string> >::iterator rI;
  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
    int link_num = atoi(((*rI)[14]).c_str());
    int father_num = atoi(((*rI)[8]).c_str());
    int order = atoi(((*rI)[11]).c_str());
    if(is_hw) {
      if(is_hw_triangle) {
	add_link<HwQSMSegment,HwQSMBud>(lignum_tree_hw, link_num, father_num, order);
      } else if(is_hw_ellipse) {
	add_link<HwQSMSegment_e,HwQSMBud_e>(lignum_tree_hw_e, link_num, father_num, order);
      } else {
	add_link<HwQSMSegment_k,HwQSMBud_k>(lignum_tree_hw_k, link_num, father_num, order);
      }
    } else {     //conifer
      add_link<CfQSMSegment,CfQSMBud>(lignum_tree_cf, link_num, father_num, order);
    }
  }


  //  //Then set the positions, directions, radii etc of cylinders
  vector<vector<double> > struct_data;  //length, rad  by segment number
  vector< PositionVector> directionVector; // This stores the direction to a vector
  vector< Point> pointVector;              // positions from the file are read in to this vector.
  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
    double len =  atof(((*rI)[1]).c_str());  //changed the file name so the length is at 2
    double rad =  atof(((*rI)[0]).c_str());   //You need to change dameter --> Radius

    PositionVector direction = PositionVector(atof(((*rI)[5]).c_str()), 
					      atof(((*rI)[6]).c_str()),atof(((*rI)[7]).c_str()));
    Point position = (Point)PositionVector(atof(((*rI)[2]).c_str()),
					   atof(((*rI)[3]).c_str()),atof(((*rI)[4]).c_str()));
    std::vector<double> v(2);
    v[0] = len; v[1] = rad;
    struct_data.push_back(v);
    directionVector.push_back(direction);
    pointVector.push_back(position);
  }

  if(is_hw) {
    if(is_hw_triangle) {
      SetArchitecture<HwQSMSegment,HwQSMBud> SA(struct_data,directionVector,pointVector);
      ForEach(lignum_tree_hw, SA);
      PropagateUp(lignum_tree_hw, p0, ConnectTreeQSM<HwQSMSegment,HwQSMBud>());
      Axis<HwQSMSegment,HwQSMBud>& ax =  GetAxis(lignum_tree_hw);
      TreeCompartment<HwQSMSegment,HwQSMBud>* tc = GetFirstTreeCompartment(ax);
      PositionVector Dir = GetDirection(*tc);
      PropagateUp(lignum_tree_hw, Dir, PropagateDirToBPBud<HwQSMSegment,HwQSMBud>());
      int iniGo = 0;
      PropagateUp(lignum_tree_hw,iniGo,SetGraveliusOrder<HwQSMSegment,HwQSMBud>());
      SetPoint(lignum_tree_hw,p0);
    } else if(is_hw_ellipse) {
      SetArchitecture<HwQSMSegment_e,HwQSMBud_e> SA(struct_data,directionVector,pointVector);
      ForEach(lignum_tree_hw_e, SA);
      PropagateUp(lignum_tree_hw_e, p0, ConnectTreeQSM<HwQSMSegment_e,HwQSMBud_e>());
      Axis<HwQSMSegment_e,HwQSMBud_e>& ax =  GetAxis(lignum_tree_hw_e);
      TreeCompartment<HwQSMSegment_e,HwQSMBud_e>* tc = GetFirstTreeCompartment(ax);
      PositionVector Dir = GetDirection(*tc);
      PropagateUp(lignum_tree_hw_e, Dir, PropagateDirToBPBud<HwQSMSegment_e,HwQSMBud_e>());
      int iniGo = 0;
      PropagateUp(lignum_tree_hw_e,iniGo,SetGraveliusOrder<HwQSMSegment_e,HwQSMBud_e>());
      SetPoint(lignum_tree_hw_e,p0);
    } else {    //is kite leaf
      SetArchitecture<HwQSMSegment_k,HwQSMBud_k> SA(struct_data,directionVector,pointVector);
      ForEach(lignum_tree_hw_k, SA);
      PropagateUp(lignum_tree_hw_k, p0, ConnectTreeQSM<HwQSMSegment_k,HwQSMBud_k>());
      Axis<HwQSMSegment_k,HwQSMBud_k>& ax =  GetAxis(lignum_tree_hw_k);
      TreeCompartment<HwQSMSegment_k,HwQSMBud_k>* tc = GetFirstTreeCompartment(ax);
      PositionVector Dir = GetDirection(*tc);
      PropagateUp(lignum_tree_hw_k, Dir, PropagateDirToBPBud<HwQSMSegment_k,HwQSMBud_k>());
      int iniGo = 0;
      PropagateUp(lignum_tree_hw_k,iniGo,SetGraveliusOrder<HwQSMSegment_k,HwQSMBud_k>());
      SetPoint(lignum_tree_hw_k,p0);
    }
  } else {      //conifer
    SetArchitecture<CfQSMSegment,CfQSMBud> SA(struct_data,directionVector,pointVector);
    ForEach(lignum_tree_cf, SA);
    PropagateUp(lignum_tree_cf, p0, ConnectTreeQSM<CfQSMSegment,CfQSMBud>());
    Axis<CfQSMSegment,CfQSMBud>& ax =  GetAxis(lignum_tree_cf);
    TreeCompartment<CfQSMSegment,CfQSMBud>* tc = GetFirstTreeCompartment(ax);
    PositionVector Dir = GetDirection(*tc);
    PropagateUp(lignum_tree_cf, Dir, PropagateDirToBPBud<CfQSMSegment,CfQSMBud>());
    int iniGo = 0;
    PropagateUp(lignum_tree_cf,iniGo,SetGraveliusOrder<CfQSMSegment,CfQSMBud>());
  }

  if(CheckCommandLine(argc,argv,"-straighten")) {
    //The stem wobbles. It is straightend here
    //(compartments of the main axis are moved so that their x, y coordinates are the same as
    //those of the first segment. Direction is set up = (0,0,1)
    //After that Connecttree sets the locations of above Compartment = location of the top of
    //the compartment below.
    //This solves also the problem of branches hanging in the air.
    if(is_hw) {
      StraightenStem<HwQSMSegment,HwQSMBud>(lignum_tree_hw); 
      Axis<HwQSMSegment,HwQSMBud>& ax =  GetAxis(lignum_tree_hw);
      TreeCompartment<HwQSMSegment,HwQSMBud>* tc = GetFirstTreeCompartment(ax);
      Point p0 = GetPoint(*tc);
      PropagateUp(lignum_tree_hw, p0, ConnectTree<HwQSMSegment,HwQSMBud>());
    } else {
      StraightenStem<CfQSMSegment,CfQSMBud>(lignum_tree_cf); 
      Axis<CfQSMSegment,CfQSMBud>& ax =  GetAxis(lignum_tree_cf);
      TreeCompartment<CfQSMSegment,CfQSMBud>* tc = GetFirstTreeCompartment(ax);
      Point p0 = GetPoint(*tc);
      PropagateUp(lignum_tree_cf, p0, ConnectTree<CfQSMSegment,CfQSMBud>());
    }
  }

 

  //Deciduous tree may have leaves given in a separate file
  //Read it and then add a segment (lenghth = 0.1, diameter = 0)
  //at top of the tree and attach all leaves to it.
  //The leaves are kite shaped (at least for time being), they
  //are stored as ellipses having the same length and width, a, and b
  //as the leaf. The degree of filling (LGAdof) is equal 2/pi

  string leaf_file;
  if(ParseCommandLine(argc,argv,"-leafFile", leaf_file) ) {
    ifstream input_file;
    input_file.open(leaf_file);

    if (!input_file.is_open()) {
      cout << "Did not find input file " << leaf_file << endl;
      exit(1);
    } else {
      cout << "Leaf file:  " << leaf_file << endl;
    }
  
    list<vector<Point> > leaf_vertices; 

    string line;
    getline(input_file,line);   //header
    
    while (getline(input_file,line))
      {
	istringstream iss(line);
	string lineStream;
	string::size_type sz;

	vector <double> row;

	while (getline(iss, lineStream, ','))
	  {  
	    row.push_back(stod(lineStream,&sz)); // convert to double
	  }

	vector<Point> vertices;
	int col = 1;
	for(int i = 0; i < 4; i++) {
	  Point v;
	  v.setX(row[col]); v.setY(row[col+1]); v.setZ(row[col+2]);
	  vertices.push_back(v);
	  col += 3;
	}
	leaf_vertices.push_back(vertices);
      }
       
    cout << "Data of " << leaf_vertices.size() << " leaves read in." << endl;
    
     
    if(is_hw_triangle) {      //Triangle leaf
      Axis<HwQSMSegment,HwQSMBud>& stem_axis =  GetAxis(lignum_tree_hw);
      Bud<HwQSMSegment,HwQSMBud>* last_b = GetTerminatingBud(stem_axis);

      BranchingPoint<HwQSMSegment,HwQSMBud> *new_bp =
	new BranchingPoint<HwQSMSegment,HwQSMBud>(&lignum_tree_hw);
      HwQSMSegment* new_seg =
	new HwQSMSegment(&lignum_tree_hw);    

      InsertTreeCompartmentSecondLast(stem_axis, new_seg); 
      InsertTreeCompartmentSecondLast(stem_axis, new_bp); 
    
      Point pos = GetPoint(*last_b);
      SetPoint(*new_seg, pos);
      SetValue(*new_seg, LGAomega, 1.0);
      SetValue(*new_seg, LGAL, 0.1);
      SetValue(*new_seg, LGAR, 0.00001);
      pos = pos + Point(0.0,0.0,0.1);   //End of the leaf TreSsegment
      SetPoint(*new_bp, pos);
      SetValue(*new_bp, LGAomega, 1.0);
      SetPoint(*last_b, pos);

      //Leaves to the addional TreeSegment new_seg
      //NOTE that Ellipse is the Shape of the leaf in HwQSMSegment by default.

    
      typename list<vector<Point> >::iterator I;
      for(I=leaf_vertices.begin(); I != leaf_vertices.end(); I++) {
	//It is assumed that the vertices are in the vector vertices in the
	//order base, left corner, apex, right corner
	Point l_pos =(*I)[0];
	Point leaf_axis = (*I)[2] - l_pos;
	Petiole* leaf_pet = new Petiole(l_pos - 0.01*leaf_axis, l_pos);   //Petiole length 1% of leaf length

	//For the time being the shape of the leaf is ellipse that is the default
	//for HwTreeSegment. The ellipse is "drawn" around the four vertices of
	//the leaves (kite-shaped) in the data. It is assumed that all four vertices
	//are in the same plane (no check for it). The degree of filling (LGAdof)
	//shows the proportion of true leaf area to the area of the ellipse and is
	//equal to 2/pi.


	Triangle* leaf_shape = new Triangle((*I)[1],(*I)[3],(*I)[2]);   //leftcorner, rightcorner, apexcorner --
	//This is only for compilation for time being

	// cout << leaf_axis.getLength()/2.0 << endl;
	// exit(0);

	//   const Point&  getCenterPoint() const { return center;};
	// const PositionVector& getNormal()const { return normal;};
	// double getSemimajorAxis()const{ return semimajoraxis;};
	// Point  getSemimajorAxisPoint()const;
	// double getSemiminorAxis()const{return semiminoraxis;};
	// Point  getSemiminorAxisPoint()const;


	// Petiole(const Point& begin, const Point& end);

	// Ellipse(const Point& petiole_end,
	// 	      const PositionVector& petiole_dir,
	// 	      const PositionVector& leaf_normal,
	// 	      const double& semimajoraxis,  const double& semiminoraxis);
	//BroadLeaf(const SHAPE& shape, const Petiole& petiole);

	BroadLeaf<Triangle>* new_leaf = new BroadLeaf<Triangle>(*leaf_shape, *leaf_pet);	
	new_seg->addLeaf(new_leaf);

	SetValue<Triangle>(*new_leaf, LGAsf, 28.0);             //This value is a guess
	SetValue<Triangle>(*new_leaf, LGAdof, 1.0);


	// // // S& GetShape(const BroadLeaf<S>& bl);

	// Ellipse koe = GetShape(new_leaf);
	// cout << "Normal " << koe.getNormal() << endl;
	// exit(0);	
      }
	
    } else if(is_hw_ellipse) {   //Ellipse leaf
      Axis<HwQSMSegment_e,HwQSMBud_e>& stem_axis =  GetAxis(lignum_tree_hw_e);
      Bud<HwQSMSegment_e,HwQSMBud_e>* last_b = GetTerminatingBud(stem_axis);

      BranchingPoint<HwQSMSegment_e,HwQSMBud_e> *new_bp =
	new BranchingPoint<HwQSMSegment_e,HwQSMBud_e>(&lignum_tree_hw_e);
      HwQSMSegment_e* new_seg =
	new HwQSMSegment_e(&lignum_tree_hw_e);    

      InsertTreeCompartmentSecondLast<HwQSMSegment_e,HwQSMBud_e>(stem_axis, new_seg); 
      InsertTreeCompartmentSecondLast<HwQSMSegment_e,HwQSMBud_e>(stem_axis, new_bp); 
    
      Point pos = GetPoint(*last_b);
      SetPoint(*new_seg, pos);
      SetValue(*new_seg, LGAomega, 1.0);
      SetValue(*new_seg, LGAL, 0.1);
      SetValue(*new_seg, LGAR, 0.00001);
      pos = pos + Point(0.0,0.0,0.1);   //End of the leaf TreSsegment
      SetPoint(*new_bp, pos);
      SetValue(*new_bp, LGAomega, 1.0);
      SetPoint(*last_b, pos);

      typename list<vector<Point> >::iterator I;
      for(I=leaf_vertices.begin(); I != leaf_vertices.end(); I++) {
	//It is assumed that the vertices are in the vector vertices in the
	//order base, left corner, apex, right corner
	Point l_pos =(*I)[0];
	Point leaf_axis = (*I)[2] - l_pos;
	Point leaf_diag = (*I)[3] - (*I)[1];
	Petiole* leaf_pet = new Petiole(l_pos - 0.01*leaf_axis, l_pos);   //Petiole length 1% of leaf length

	//For the time being the shape of the leaf is ellipse that is the default
	//for HwTreeSegment. The ellipse is "drawn" around the four vertices of
	//the leaves (kite-shaped) in the data. It is assumed that all four vertices
	//are in the same plane (no check for it). The degree of filling (LGAdof)
	//shows the proportion of true leaf area to the area of the ellipse and is
	//equal to 2/pi.

	//leaf normal
	PositionVector l_normal = Cross(PositionVector(leaf_axis),
					PositionVector(leaf_diag));
	l_normal.normalize();
	double norm_z = l_normal.getZ();
	if(norm_z < 0.0) l_normal = (-1.0)*l_normal;  //Normal points up
	PositionVector pet_dir = PositionVector(leaf_axis);
	pet_dir.normalize();
	Ellipse* leaf_shape = new Ellipse(l_pos + 0.01*leaf_axis, pet_dir ,
					  l_normal,leaf_axis.getLength()/2.0,leaf_diag.getLength()/2.0);

	BroadLeaf<Ellipse>* new_leaf = new BroadLeaf<Ellipse>(*leaf_shape, *leaf_pet);
	new_seg->addLeaf(new_leaf);
	SetValue(*new_leaf, LGAdof, 2.0/PI_VALUE);
	SetValue(*new_leaf, LGAsf, 28.0);             //This value is a guess
      }
    } else {         //is Kite leaf
      Axis<HwQSMSegment_k,HwQSMBud_k>& stem_axis =  GetAxis(lignum_tree_hw_k);
      Bud<HwQSMSegment_k,HwQSMBud_k>* last_b = GetTerminatingBud(stem_axis);

      BranchingPoint<HwQSMSegment_k,HwQSMBud_k> *new_bp =
	new BranchingPoint<HwQSMSegment_k,HwQSMBud_k>(&lignum_tree_hw_k);
      HwQSMSegment_k* new_seg =
	new HwQSMSegment_k(&lignum_tree_hw_k);    

      InsertTreeCompartmentSecondLast<HwQSMSegment_k,HwQSMBud_k>(stem_axis, new_seg); 
      InsertTreeCompartmentSecondLast<HwQSMSegment_k,HwQSMBud_k>(stem_axis, new_bp); 
    
      Point pos = GetPoint(*last_b);
      SetPoint(*new_seg, pos);
      SetValue(*new_seg, LGAomega, 1.0);
      SetValue(*new_seg, LGAL, 0.1);
      SetValue(*new_seg, LGAR, 0.00001);
      pos = pos + Point(0.0,0.0,0.1);   //End of the leaf TreSsegment
      SetPoint(*new_bp, pos);
      SetValue(*new_bp, LGAomega, 1.0);
      SetPoint(*last_b, pos);
    
      //Leaves to the addional TreeSegment new_seg
      //NOTE that Ellipse is the Shape of the leaf in HwQSMSegment by default.

      int counter = 0;
      typename list<vector<Point> >::iterator I;
      for(I=leaf_vertices.begin(); I != leaf_vertices.end(); I++) {
	//It is assumed that the vertices are in the vector vertices in the
	//order base, right corner, apex, left corner
	Point l_pos =(*I)[0];
	Point leaf_axis = (*I)[2] - l_pos;
	Point leaf_diag = (*I)[3] - (*I)[1];

	Petiole* leaf_pet = new Petiole(l_pos - 0.01*leaf_axis, l_pos);   //Petiole length 1% of leaf length

	//leaf normal
	PositionVector l_normal = Cross(PositionVector(leaf_axis),
					PositionVector(leaf_diag));
	l_normal.normalize();
	double norm_z = l_normal.getZ();
	if(norm_z < 0.0) l_normal = (-1.0)*l_normal;  //Normal points up
	PositionVector pet_dir = PositionVector(leaf_axis);
	pet_dir.normalize();
	
	//Kite constructor: base, left, right, apex
	Kite* leaf_shape = new Kite((*I)[0], (*I)[3], (*I)[1], (*I)[2]);

	BroadLeaf<Kite>* new_leaf = new BroadLeaf<Kite>(*leaf_shape, *leaf_pet);
	new_seg->addLeaf(new_leaf);
	SetValue(*new_leaf, LGAdof, 1.0);
	SetValue(*new_leaf, LGAsf, 28.0);             //This value is a guess
      }
    }
     
  } //if(-leafFile ...

  //Write xml file
  std::string outputFile = argv[1];
  string f_txt;
  if(is_csv) {
    f_txt = ".csv";
  } else {
    f_txt = ".txt";
  }
  std::size_t found = outputFile.find(f_txt);

  if(found != string::npos) { // found .txt or .csv
    outputFile.replace(found, f_txt.length(), ".xml");
  } else { //just append .xml
    outputFile.append(".xml");
  }
  
  cout << "Name of the xml file: " << outputFile << endl;

  if(is_hw) {
    if(is_hw_triangle) {
      XMLDomTreeWriter<HwQSMSegment,HwQSMBud,Triangle> writer;
      writer.writeTreeToXML(lignum_tree_hw, outputFile);
      cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_hw,LGADbh) << " Dbase: "
	   << 100*GetValue(lignum_tree_hw,LGADbase) << " H: " << GetValue(lignum_tree_hw,LGAH) << endl;
    } else if(is_hw_ellipse){
      XMLDomTreeWriter<HwQSMSegment_e,HwQSMBud_e,Ellipse> writer;
      writer.writeTreeToXML(lignum_tree_hw_e, outputFile);
      cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_hw_e,LGADbh) << " Dbase: "
	   << 100*GetValue(lignum_tree_hw_e,LGADbase) << " H: " << GetValue(lignum_tree_hw_e,LGAH) << endl;
    } else if(is_hw_kite) {
      XMLDomTreeWriter<HwQSMSegment_k,HwQSMBud_k,Kite> writer;
      writer.writeTreeToXML(lignum_tree_hw_k, outputFile);
      cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_hw_k,LGADbh) << " Dbase: "
	   << 100*GetValue(lignum_tree_hw_k,LGADbase) << " H: " << GetValue(lignum_tree_hw_k,LGAH) << endl;
    }
  } else {   //is conifer
    XMLDomTreeWriter<CfQSMSegment,CfQSMBud> writer;
    writer.writeTreeToXML(lignum_tree_cf, outputFile);
    cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_cf,LGADbh) << " Dbase: "
	 << 100*GetValue(lignum_tree_cf,LGADbase) << " H: " << GetValue(lignum_tree_cf,LGAH) << endl;
  }

  exit(0);

}



