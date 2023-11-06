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
    cout << "Usage: ./maketree <file> [-conifer] [-straighten] [-csv]" << endl;
    cout << "-conifer     Stores the tree as a conifer, default is hardwood" << endl; 
    cout << "-straighten  Stem generated from point cloud may wobble, this option sets stem go up straight."<<endl;
    cout << "-csv         Default of the QSM file is Space-delimited Text, this reads Comma-separated Values." << endl;
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
    cout << "of the particular cylinder." << endl;
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
  if(CheckCommandLine(argc,argv,"-conifer")) {
      is_hw = false;
    }

  Tree<HwQSMSegment,HwQSMBud> lignum_tree_hw(Point(0,0,0), PositionVector(0,0,1.0));
  Tree<CfQSMSegment,CfQSMBud> lignum_tree_cf(Point(0,0,0), PositionVector(0,0,1.0));

  //Add cylinders one by one
  //link_num = number of link starting with 0. If == 0, add_link adds first TreeSegment to the tree,
  //otherwise just adds after parent link.   link_number = row number in the file - 1.

  list<vector<string> >::iterator rI;
  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
    int link_num = atoi(((*rI)[14]).c_str());
    int father_num = atoi(((*rI)[8]).c_str());
    int order = atoi(((*rI)[11]).c_str());
    if(is_hw) {
    add_link<HwQSMSegment,HwQSMBud>(lignum_tree_hw, link_num, father_num, order);
    } else {
    add_link<CfQSMSegment,CfQSMBud>(lignum_tree_cf, link_num, father_num, order);
    }
  }

  //  //Then set the positions, directions, radii etc of cylinders
  vector<vector<double> > struct_data;  //length, rad, angle by segment number
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
      SetArchitecture<HwQSMSegment,HwQSMBud> SA(struct_data,directionVector,pointVector);
      ForEach(lignum_tree_hw, SA);
      Point p0(0.0,0.0,0.0);	  
      PropagateUp(lignum_tree_hw, p0, ConnectTree<HwQSMSegment,HwQSMBud>());
      Axis<HwQSMSegment,HwQSMBud>& ax =  GetAxis(lignum_tree_hw);
      TreeCompartment<HwQSMSegment,HwQSMBud>* tc = GetFirstTreeCompartment(ax);
      PositionVector Dir = GetDirection(*tc);
      PropagateUp(lignum_tree_hw, Dir, ConnectTreeDir<HwQSMSegment,HwQSMBud>());
      int iniGo = 0;
      PropagateUp(lignum_tree_hw,iniGo,SetGraveliusOrder<HwQSMSegment,HwQSMBud>());
    } else {
      SetArchitecture<CfQSMSegment,CfQSMBud> SA(struct_data,directionVector,pointVector);
      ForEach(lignum_tree_cf, SA);
      Point p0(0.0,0.0,0.0);	  
      PropagateUp(lignum_tree_cf, p0, ConnectTree<CfQSMSegment,CfQSMBud>());
      Axis<CfQSMSegment,CfQSMBud>& ax =  GetAxis(lignum_tree_cf);
      TreeCompartment<CfQSMSegment,CfQSMBud>* tc = GetFirstTreeCompartment(ax);
      PositionVector Dir = GetDirection(*tc);
      PropagateUp(lignum_tree_cf, Dir, ConnectTreeDir<CfQSMSegment,CfQSMBud>());
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


    //Write xml file



  std::string outputFile = argv[1];
  string f_txt;
  if(is_csv) {
    f_txt = ".csv";
  } else {
    f_txt = ".txt";
  }
  std::size_t found = outputFile.find(f_txt);

  if(found != string::npos) { // found .txt
    outputFile.replace(found, f_txt.length(), ".xml");
  } else { //just append .xml
    outputFile.append(".xml");
  }

  cout << "Name of the xml file: " << outputFile << endl;

    if(is_hw) {
      XMLDomTreeWriter<HwQSMSegment,HwQSMBud> writer;
      writer.writeTreeToXML(lignum_tree_hw, outputFile);
      cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_hw,LGADbh) << " Dbase: "
	   << 100*GetValue(lignum_tree_hw,LGADbase) << " H: " << GetValue(lignum_tree_hw,LGAH) << endl;
    } else {
      XMLDomTreeWriter<CfQSMSegment,CfQSMBud> writer;
      writer.writeTreeToXML(lignum_tree_cf, outputFile);
      cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree_cf,LGADbh) << " Dbase: "
	   << 100*GetValue(lignum_tree_cf,LGADbase) << " H: " << GetValue(lignum_tree_cf,LGAH) << endl;
    }

  exit(0);
}



