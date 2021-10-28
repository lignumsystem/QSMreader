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

// 5.2.2016
// HUOM: nyt on Pasin tiedostaossa ainakin testimielessa kaksi saraketta lisaa:
// sylinterin muokkaamaton lapimitta ja tieto onko neulasia (0/1)
// Nama luetaan samoissa loopeissa kuin ylla lisatietorakenteisiin:
//    pair<string,string> needle_info;
//    list<pair<string, string> >needle_information;       //needles or not, first string = number of segment,
// josta sitten arkkitehtuurin asettamisen jalkeen asetetaan neulaset paikalleen:
// nille segmenteille, joilla niita on: add_needles(lignum_tree, nseg);

//Jos ajaa 14 sarakkeen tiedostolla, ohjelma luultavasto kaatuu.


using namespace cxxadt;
using namespace std;

void process_line(string& line, vector<string>& items, vector<string>& n_items) {
  istringstream l(line);
  string dummy;
  for(int i = 0; i < 14; i++) {
    l >> items[i];
  }
  l >> dummy;   //Original radius
  for(int i = 0; i < 3; i++) {
    l >> n_items[i];
  }
}

int main(int argc, char** argv)
{

  if(argc < 2){
    cout << "Usage: ./maketree <file> [-straighten] [-mm] [-addNeedles]  [-CR <value>]" << endl;
    cout << "Reads the QSM file <file>, tansforms it to Lignum tree and stores as Lignum xml file." << endl;
    cout << "STORES THE TREE AT MOMENT ONLY AS HARDWOOD TREE (applies HwTreeSegment)" << endl;
    cout << "-straighten  Stem generated from point cloud may wobble, this option sets stem go up straight."<<endl;
    cout << "-mm	  If measures are in mm instead of meter" << endl;
    cout << "NOT IN USE -addNeedles  Needles are added (to Scots Pine) as explained in R. Sievanen, P. Raumonen,"<<endl;
    cout << "             J. Perttunen, E. Nikinmaa, and P. Kaitaniemi. A study of crown development" << endl;
    cout << "             mechanisms using a shoot-based tree model and segmented terrestrial laser" << endl;
    cout << "             scanning data. Annals of Botany, 122(3):423–434, 2018." << endl;
    cout << "-CR <value>  Input of crown ratio for height of crown base" << endl;
    cout << "                      - makes sense only in case of -addNeedles" << endl;

    exit(0);
  }

  string line;
  int lineNumber = 0;

  //read in data file of cylinderinformation
  //Modify that the name of file can be read from command line

  // ifstream input_file("kaikki.dat"); // this is the original
  // cout<<"here"<<endl;
  cout<<"arg "<<argv[1]<<endl;
  ifstream input_file(argv[1]);


  if (!input_file) {
    cout << "Did not find input file cyl_data_branch.dat" << endl;
    exit(1);
  }


  //  //root link = cylinder in Pasi Raumonen's file

  vector<string> items(15);               //This is for items on one line
  list<vector<string> > root_links;       //This contains all information of the input file
  // // needle_items sisaltaa 4 kpl indikaattoreita  neulasia/ei (1/0) arvioituna
  // // 1. Pasin 1. saanto (15%)
  // // 2. Pasin 2. saanto (30%)
  // // 3. Pekan 1. saanto
  // // 4. Pekan 2. saanto
  vector<string>  needle_items(4);
  // pair<string,vector<string> > needle_info;
  // list<pair<string, vector<string> > > needle_information;       //needles or not, first string = number of segment

  //  //Read in input file

 getline(input_file,line);                 //header

  getline(input_file,line);       //first segment to start
  lineNumber++;
  process_line(line, items, needle_items);
  string lineNumberStr = static_cast<ostringstream*>( &(ostringstream() << lineNumber) )->str();
  items[14] =  lineNumberStr;
  root_links.push_back(items);

  // needle_info.first = items[14];
  // needle_info.second = needle_items;
  // needle_information.push_back(needle_info);

  bool last = false;

  while(input_file.good()){
    getline(input_file,line);

    if(input_file.eof()) {
      last = true;
      // break;
    }
    process_line(line, items, needle_items);
    lineNumber++;
    string lineNumberStr = static_cast<ostringstream*>( &(ostringstream() << lineNumber) )->str();
    items[14] =  lineNumberStr;
    root_links.push_back(items);

    // needle_info.first = items[14];
    // needle_info.second = needle_items;

//       cout << needle_items[0] << " " << needle_items[1] << " " << needle_items[2] << " " << needle_items[3] << endl;
//     if(needle_items[0] == "1") exit(0);

//    needle_information.push_back(needle_info);

  }  //end of reading input file

  //    ////CODE TO CHECK IF THE FIRST LINE IS WRITTEN IN TO root_links or NOT
  //    for(r2I = root_links.begin(); r2I != root_links.end(); r2I++) {
  //        cout<<"rI "<<*r2I<<endl;
  //        exit(0);
  //}
  //***************************************************************************
  //  //Add the cylinders to Lignum tree one by one on the basis of parent information


  // Sizes of cylinders may be in meters or millimeters

  LGMdouble mm_to_m = 1.0;
  if(CheckCommandLine(argc,argv,"-mm")) {
    mm_to_m = 0.001;
  }
  //  //Make tree

  Tree<QSMSegment,QSMBud> lignum_tree(Point(0,0,0), PositionVector(0,0,1.0));

  //Add cylinders one by one
  //link_num = number of link starting with 0. If == 0, add_link adds first TreeSegment to the tree,
  //otherwise just adds after parent link.   link_number = row number in the file - 1.

  list<vector<string> >::iterator rI;
  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
    int link_num = atoi(((*rI)[14]).c_str());
    int father_num = atoi(((*rI)[8]).c_str());
    int order = atoi(((*rI)[11]).c_str());  
    add_link(lignum_tree, link_num, father_num, order);
  }

  //  //Then set the positions, directions, radii etc of cylinders
  vector<vector<double> > struct_data;  //length, rad, angle by segment number
  vector< PositionVector> directionVector; // This stores the direction to a vector
  vector< Point> pointVector;              // positions from the file are read in to this vector.
  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
    double len = mm_to_m * atof(((*rI)[1]).c_str());  //changed the file name so the length is at 2
    double rad = mm_to_m * atof(((*rI)[0]).c_str());   //You need to change dameter --> Radius

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


  SetArchitecture SA(struct_data,directionVector,pointVector);   // make the object for SetArchitecture
  ForEach(lignum_tree, SA);

  Point p0(0.0,0.0,0.0);	  
  PropagateUp(lignum_tree, p0, ConnectTree<QSMSegment,QSMBud>());

  Axis<QSMSegment,QSMBud>& ax =  GetAxis(lignum_tree);
  TreeCompartment<QSMSegment,QSMBud>* tc = GetFirstTreeCompartment(ax);
  PositionVector Dir = GetDirection(*tc);

  PropagateUp(lignum_tree, Dir, ConnectTreeDir<QSMSegment,QSMBud>());


  int iniGo = 0;
  PropagateUp(lignum_tree,iniGo,SetGraveliusOrder<QSMSegment,QSMBud>());


  if(CheckCommandLine(argc,argv,"-straighten")) {
    //The stem wobbles. It is straightend here
    //(compartments of the main axis are moved so that their x, y coordinates are the same as
    //those of the first segment. Direction is set up = (0,0,1)
    //After that Connecttree sets the locations of above Compartment = location of the top of
    //the compartment below.
    //This solves also the problem of branches hanging in the air. 
    StraightenStem<QSMSegment,QSMBud>(lignum_tree); 

    Axis<QSMSegment,QSMBud>& ax =  GetAxis(lignum_tree);
    TreeCompartment<QSMSegment,QSMBud>* tc = GetFirstTreeCompartment(ax);
    Point p0 = GetPoint(*tc);
    PropagateUp(lignum_tree, p0, ConnectTree<QSMSegment,QSMBud>());
  }

  //Neulaset Kaikilla variaatioilla: Pasin 1 ja saanto ja Pekka K:n 1 ja 2 saanto.
  //Kaikista kirjoitetaan oma puutiedosto
//   vector<string> rule(4);
//   rule[0] = "Pa1"; rule[1] = "Pa2"; rule[2] = "Pe1"; rule[3] = "Pe2";
//   LGMdouble crown_ratio = 1.0;   //Jos ei annettu -CR neulasia kaikkialle
//   string clarg;
//   //If crown ratio has bee specifies, add needles only above crown base
//   if(ParseCommandLine(argc,argv,"-CR", clarg)) {
//     crown_ratio = atof(clarg.c_str());
//     if(crown_ratio < 0.0 || crown_ratio > 1.0) {
//       cout << "Incorrect value of crown ratio " << crown_ratio << endl;
//       exit(0);
//     }
//   }
  
//   LGMdouble H = GetValue(lignum_tree, LGAH);
//   LGMdouble Hcb = (1.0 - crown_ratio) * H;


//   if(CheckCommandLine(argc,argv,"-addNeedles")) {

//       list<pair<string, vector<string> > >::iterator nI;
//       for(nI = needle_information.begin(); nI != needle_information.end(); nI++) {
// 	if(atoi((((*nI).second)[0]).c_str()) == 1) {   //has needles
// 	  double rf = atof((((*nI).second)[2]).c_str());  //this is mean foliage radius
// 	  int nseg = atoi(((*nI).first).c_str());
// // 	  cout << atoi((((*nI).second)[0]).c_str()) << " " << nseg << " " << rf << endl;
// // 	  exit(0);
// 	  add_needles(lignum_tree, nseg, rf, Hcb);
// 	}
//       }

//       XMLDomTreeWriter<QSMSegment,QSMBud> writer;
//       std::string outputFile = argv[1];

//       // A gernalised solution for the outputfile naming.
//       // iterator to find the .position to the filename
//       std::string::iterator beg = std::find(outputFile.begin(), outputFile.end(), '.'); 
//       // iterator to find all the characters from . till the end of file
//       std::string::iterator end = std::find(beg, outputFile.end(), ' ');
//       string end_string = "-" + rule[1] + ".xml"; 
//       outputFile.replace(beg, end, end_string);  // any type of extension is replaced with .xml

//       writer.writeTreeToXML(lignum_tree, outputFile);

//       cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree,LGADbh) << " Dbase: "
// 	   << 100*GetValue(lignum_tree,LGADbase) << " H: " << GetValue(lignum_tree,LGAH) << endl;

//       ForEach(lignum_tree, DeleteNeedles());
//  }

  XMLDomTreeWriter<QSMSegment,QSMBud> writer;

  std::string outputFile = argv[1];
  std::size_t found = outputFile.find(".txt");

  if(found != string::npos) { // found .txt
    outputFile.replace(found, std::string(".txt").length(), ".xml");
  } else { //just append .xml
    outputFile.append(".xml");
  }

  writer.writeTreeToXML(lignum_tree, outputFile);

  cout << outputFile << " Dbh: " << 100*GetValue(lignum_tree,LGADbh) << " Dbase: "
	 << 100*GetValue(lignum_tree,LGADbase) << " H: " << GetValue(lignum_tree,LGAH) << endl;

  exit(0);
}



