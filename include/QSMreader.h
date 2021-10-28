#ifndef QSMREADER_H
#define QSMREADER_H
#include <Lignum.h>
#include <Turtle.h>
#include <PositionVector.h>
#include<Point.h>
#include<iterator>


using namespace cxxadt;
using namespace std;

//bool is_left = false;
class QSMBud;

class QSMSegment : public HwTreeSegment<QSMSegment,QSMBud>
{
public:
    QSMSegment(Tree<QSMSegment,QSMBud>* t)
        :HwTreeSegment<QSMSegment,QSMBud>(t){}

    int getNumber() {return number;}
    void setNumber(const int& nu) {number = nu;}

private:
    int number;
};

class QSMBud:public Bud<QSMSegment,QSMBud>{
public:
    QSMBud(Tree<QSMSegment,QSMBud>* t):Bud<QSMSegment,QSMBud>(t){}
};

class SetLink {
public:
    SetLink(const int f_num, const int m_num, const int m_o) : father_num(f_num),
        my_num(m_num), my_order(m_o) {}

    TreeCompartment<QSMSegment,QSMBud>* operator()
    (TreeCompartment<QSMSegment,QSMBud>* tc)const {
        if(Axis<QSMSegment,QSMBud>* ax =
                dynamic_cast<Axis<QSMSegment,QSMBud>*>(tc)){
            list<TreeCompartment<QSMSegment,QSMBud>*>& c_list =
                    GetTreeCompartmentList(*ax);
            list<TreeCompartment<QSMSegment,QSMBud>*>::iterator Ic;
            bool found = false;
            QSMSegment* frs;
            for(Ic = c_list.begin(); Ic != c_list.end(); Ic++) {
                if((frs =  dynamic_cast<QSMSegment*>(*Ic)))
                    if(frs->getNumber() == father_num) {
                        found = true;
                        break;             //father found
                    }
            }
            if(!found)
                return tc;

            Tree<QSMSegment,QSMBud>& t = GetTree(*frs);

            if(static_cast<int>(GetValue(*frs,LGAomega) == my_order)) {
                QSMSegment* seg = new QSMSegment(&t);  //into the same axis
                seg->setNumber(my_num);
                SetValue(*seg,LGAomega,static_cast<double>(my_order));
                BranchingPoint<QSMSegment,QSMBud> *bp =
                        new BranchingPoint<QSMSegment,QSMBud>(&t);
                Ic++;  Ic++;    //points now past next BranchingPoint
                c_list.insert(Ic, seg);
                c_list.insert(Ic, bp);
            }
            else {    //forking from the next BranchingPoint
                QSMBud *new_bud = new QSMBud(&t);
                QSMSegment* new_seg = new QSMSegment(&t);
                new_seg->setNumber(my_num);
                SetValue(*new_seg,LGAomega,static_cast<double>(my_order));
                BranchingPoint<QSMSegment,QSMBud> *new_bp =
                        new BranchingPoint<QSMSegment,QSMBud>(&t);
                Axis<QSMSegment,QSMBud>* new_ax =
                        new Axis<QSMSegment,QSMBud>(&t);
                InsertTreeCompartment(*new_ax, new_bud);
                InsertTreeCompartmentSecondLast(*new_ax,new_seg);
                InsertTreeCompartmentSecondLast(*new_ax,new_bp);
                Ic++;        //points now to next BranchingPoint
                BranchingPoint<QSMSegment,QSMBud> *bp =
                        dynamic_cast<BranchingPoint<QSMSegment,QSMBud>*>(*Ic);
                InsertAxis(*bp, new_ax);
            }
        }

        return tc;
    }
private:
    int father_num;
    int my_num;
    int my_order;
};

void add_link(Tree<QSMSegment,QSMBud>& fr_tree, const int num,
              const int father_num, const int ord)  {
    if(num == 1)  { //first segment, tree is empty
        Axis<QSMSegment,QSMBud>& main_ax = GetAxis(fr_tree);
        QSMBud *bud0 = new  QSMBud(&fr_tree);
        QSMSegment* seg0 = new QSMSegment(&fr_tree);
        BranchingPoint<QSMSegment,QSMBud> *bp0 =
                new BranchingPoint<QSMSegment,QSMBud>(&fr_tree);
        InsertTreeCompartment(main_ax,bud0);
        InsertTreeCompartmentSecondLast(main_ax,seg0);
        InsertTreeCompartmentSecondLast(main_ax,bp0);
        seg0->setNumber(num);
        SetValue(*seg0,LGAomega,static_cast<double>(ord));
        //      cout << "nolla " << num << endl;
    }
    else {
        SetLink sl(father_num, num, ord);
        ForEach(fr_tree, sl);
    }

    return;
}

class PrintN {
public:
    TreeCompartment<QSMSegment,QSMBud>* operator()
    (TreeCompartment<QSMSegment,QSMBud>* tc)const {
        if(QSMSegment* frs = dynamic_cast<QSMSegment*>(tc)){
            cout << frs->getNumber() << endl;
        }
        return tc;
    }
};



class SetArchitecture {
public:
  SetArchitecture(vector<vector<double> >& dat,vector< PositionVector> &direction, vector<Point> &position) : data(dat), Dir(direction), Pos(position){}
    TreeCompartment<QSMSegment,QSMBud>* operator()
    (TreeCompartment<QSMSegment,QSMBud>* tc)const {

        if(QSMSegment* frs = dynamic_cast<QSMSegment*>(tc)){
            int number = frs->getNumber()-1;
            double len = (data[number])[0];
            double rad = (data[number])[1];
            PositionVector  DirVec = Dir[number];
            Point PosVec = Pos[number];

            SetValue(*frs,LGAR, rad);
            SetValue(*frs,LGAL, len);

            SetPoint(*frs, Point(0,0,0));  //Point(0,0,0) is only for compilation, must be position
            SetDirection(*frs, PositionVector(0,0,0));  //PositionVector(0,0,0) is only for compilation, must be
            //direction
            SetDirection(*frs, DirVec);
            SetPoint(*frs, PosVec);
        }
        else if(QSMBud *bu = dynamic_cast<QSMBud*>(tc)){
            //CHECK THAT BUD IS AT THE END OF THE LAST TREESEGMENT IN BRANCH!!!!!
            SetPoint(*bu, Point(0,0,0));     //use the same position and direction to set these
            SetDirection(*bu, PositionVector(0,0,0));
            //Point p = (Point)PositionVector(0,0,0);
//            PositionVector  DirVec = Dir[0];
//            Point PosVec = Pos[0];
//            SetDirection(*bu, DirVec);
//            SetPoint(*bu, PosVec);


        }
        else if(BranchingPoint<QSMSegment,QSMBud> *bp =
                dynamic_cast<BranchingPoint<QSMSegment,QSMBud>*>(tc)){
            //        PositionVector direction; //= PositionVector(xDir, yDir,zDir);
            //        Point Position; //= (Point)PositionVector(xPos,yPos,zPos);
            SetPoint(*bp, Point(0,0,0));  //use the same position and direction to set these
            SetDirection(*bp, PositionVector(0,0,0));

//            PositionVector  DirVec = Dir[0];
//            Point PosVec = Pos[0];

//            //PositionVector direction = PositionVector(xDir, yDir,zDir);
//            //Point Position = (Point)PositionVector(xPos,yPos,zPos);
//            SetDirection(*bp, DirVec);
//            SetPoint(*bp, PosVec);
        }
        else {}

        return tc;
    }
private:
    vector<vector<double> > data;
    vector< PositionVector> Dir;
    vector<Point> Pos;
};

template <class TS, class BUD>
class ConnectTreeDir{
     public:
    ConnectTreeDir(){}
    PositionVector& operator()(PositionVector& direction, TreeCompartment<QSMSegment,QSMBud>* tc)const
         {
           if(QSMBud *bu = dynamic_cast<QSMBud*>(tc)) {
             SetDirection(*bu,direction);
             PositionVector direction = GetDirection(*bu);
          }
           else if(BranchingPoint<QSMSegment,QSMBud> *bp =
                   dynamic_cast<BranchingPoint<QSMSegment,QSMBud>*>(tc))
           {
               SetDirection(*bp,direction);
               PositionVector direction = GetDirection(*bp);
           }
           else{}
           return direction;
         }
     };

//Adds order of compartments by one
class IncrementOrderByOne {
 public:
  TreeCompartment<QSMSegment,QSMBud>* operator()
    (TreeCompartment<QSMSegment,QSMBud>* tc)const {
    if(QSMSegment *ts = dynamic_cast<QSMSegment*>(tc)){
      SetValue(*ts, LGAomega, GetValue(*ts,LGAomega)+1.0);
    }
    else if(QSMBud *bu = dynamic_cast<QSMBud*>(tc)){
      SetValue(*bu, LGAomega, GetValue(*bu,LGAomega)+1.0);
    }
    else if(BranchingPoint<QSMSegment,QSMBud> *bp =
	    dynamic_cast<BranchingPoint<QSMSegment,QSMBud>*>(tc)){
      SetValue(*bp, LGAomega, GetValue(*bp,LGAomega)+1.0);
    }
    else {;}
      
    return tc;
  }

};

// class NeedleAdd {
//  public:
//  NeedleAdd(const int nn, const double rfi, const LGMdouble hcb) : ns(nn), rf(rfi), Hcb(hcb) {}
//   TreeCompartment<QSMSegment,QSMBud>* operator()
//     (TreeCompartment<QSMSegment,QSMBud>* tc)const {
//     if(QSMSegment* ts = dynamic_cast<QSMSegment*>(tc)){
//       Point top = GetEndPoint(*ts);
//       if(ts->getNumber() == ns && top.getZ() > Hcb) {
// 	//Nyt on käytetty P. Stenberg and S. Palmroth and B. Bond and 
// 	//D. Sprugel and H. Smolander Tree Physiology  21  805-814  (2001)
// 	//16 cm2/ cm needle area packing ja specifi needele area = 152 cm2/g
// 	//in current shoots & Muukkosen vaitoskirjasta neulasia jaljella
// 	//(fm.fun in /LignumForest), josta etta n. 75% needle area packing
// 	//kaikissa versoissa: 0.75*16/152 = 0.0789 = 0.08
// 	LGMdouble r = GetValue(*ts, LGAR);
// 	LGMdouble l = GetValue(*ts, LGAL);
// 	LGMdouble sf = 28.0;         //Area (m2) / Needle mass (kg C)
//                                      //nominal value, Stenberg. et al
// 	//show bit larger 2*15.2 = 30.4 
// 	LGMdouble Wf = 0.08 * 100.0 * l;    //g
// 	Wf *= 1.0e-3;                     //g -> kg
// 	Wf /= 2.0;                        //biomass -> C
// 	SetValue(*ts, LGARf, rf);
// 	SetValue(*ts,LGAWf, Wf);
// 	SetValue(*ts, LGAsf, sf);  //Af is calculated with sf if needed
//       }
//     }
//     return tc;
//   }
//  private:
//   int ns;             //Make needles to this TreeSegment
//   double rf;          //Foliage radius
//   LGMdouble Hcb;      //Needles only to TreeSegment that is in branch starting above CB
// };

// void add_needles(Tree<QSMSegment,QSMBud>& fr_tree, const int nseg, const double rf, const LGMdouble Hc){
  
//   ForEach(fr_tree, NeedleAdd(nseg, rf, Hc));
//   return;
// }

// class DeleteNeedles {
//  public:
//   TreeCompartment<QSMSegment,QSMBud>* operator()
//     (TreeCompartment<QSMSegment,QSMBud>* tc)const {
//     if(QSMSegment* ts = dynamic_cast<QSMSegment*>(tc)){
// 	SetValue(*ts, LGARf, 0.0);
// 	SetValue(*ts,LGAWf, 0.0);
// 	SetValue(*ts, LGAsf, 0.0);
//     }
//     return tc;
//   }
//  private:
//   int ns;
// };


  //This function corrects the wobbling of the stem caused by digitizing on different sides of the stem.
  //Algorithm: 1) Open the main Axis of the tree (=the stem),
  //2) Take x and y coordinates of the first segment
  //3) Move all the other tree compartments of the main Axis so that they have the x and y of the first
  //   segment, set also their directions up (= direction = (0,0,1)).
 template<class TS, class BUD>
  void StraightenStem(Tree<TS,BUD>& tree) {
    Axis<TS,BUD>& axis = GetAxis(tree);
    TreeSegment<TS,BUD>* ts0 = GetFirstTreeSegment(axis);
    LGMdouble x0 = GetPoint(*ts0).getX();
    LGMdouble y0 = GetPoint(*ts0).getY();
    Point pxy0(x0, y0, 0.0);

    list<TreeCompartment<TS,BUD>*>& a_cmpl = GetTreeCompartmentList(axis);
    typename list<TreeCompartment<TS,BUD>*>::iterator a_i;
    PositionVector direction(0.0, 0.0, 1.0); //all Segments (an other parts) of the stem are upright
 
    for (a_i = a_cmpl.begin(); a_i != a_cmpl.end(); a_i++) {
      Point pxy(GetPoint(**a_i).getX(), GetPoint(**a_i).getY(), 0.0);
      Point move = pxy0 - pxy;
      SetPoint(**a_i, GetPoint(**a_i)+move);
      SetDirection(**a_i, direction);
    }

  }





#endif
