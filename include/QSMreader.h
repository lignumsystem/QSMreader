#ifndef QSMREADER_H
#define QSMREADER_H
#include <Lignum.h>
#include <Turtle.h>
#include <PositionVector.h>
#include<Point.h>
#include<iterator>
#include <Kite.h>


using namespace cxxadt;
using namespace std;

class HwQSMBud;
class HwQSMSegment : public HwTreeSegment<HwQSMSegment,HwQSMBud,Triangle>
{
public:
  HwQSMSegment(Tree<HwQSMSegment,HwQSMBud>* t)
    :HwTreeSegment<HwQSMSegment,HwQSMBud,Triangle>(t){}

    int getNumber() {return number;}
    void setNumber(const int& nu) {number = nu;}

private:
    int number;
};

class HwQSMBud : public Bud<HwQSMSegment,HwQSMBud>{
public:
    HwQSMBud(Tree<HwQSMSegment,HwQSMBud>* t):Bud<HwQSMSegment,HwQSMBud>(t){}
};

class CfQSMBud;
class CfQSMSegment : public CfTreeSegment<CfQSMSegment,CfQSMBud>
{
public:
    CfQSMSegment(Tree<CfQSMSegment,CfQSMBud>* t)
        :CfTreeSegment<CfQSMSegment,CfQSMBud>(t){}

    int getNumber() {return number;}
    void setNumber(const int& nu) {number = nu;}

private:
    int number;
};

class CfQSMBud : public Bud<CfQSMSegment,CfQSMBud>{
public:
    CfQSMBud(Tree<CfQSMSegment,CfQSMBud>* t):Bud<CfQSMSegment,CfQSMBud>(t){}
};

//          Ellipse leaves
class HwQSMBud_e;
class HwQSMSegment_e : public HwTreeSegment<HwQSMSegment_e,HwQSMBud_e,Ellipse>
{
public:
  HwQSMSegment_e(Tree<HwQSMSegment_e,HwQSMBud_e>* t)
    :HwTreeSegment<HwQSMSegment_e,HwQSMBud_e,Ellipse>(t){}

    int getNumber() {return number;}
    void setNumber(const int& nu) {number = nu;}

private:
    int number;
};

class HwQSMBud_e : public Bud<HwQSMSegment_e,HwQSMBud_e>{
public:
    HwQSMBud_e(Tree<HwQSMSegment_e,HwQSMBud_e>* t):Bud<HwQSMSegment_e,HwQSMBud_e>(t){}
};


//        Kite leaves
class HwQSMBud_k;
class HwQSMSegment_k : public HwTreeSegment<HwQSMSegment_k,HwQSMBud_k,Kite>
{
public:
  HwQSMSegment_k(Tree<HwQSMSegment_k,HwQSMBud_k>* t)
    :HwTreeSegment<HwQSMSegment_k,HwQSMBud_k,Kite>(t){}
  HwQSMSegment_k(const Point& p,const PositionVector& pv,
                const LGMdouble go, const METER l, const METER r,
		 const METER rn,Tree<HwQSMSegment_k,HwQSMBud_k>* tree)
    :HwTreeSegment(p,pv,go, l, r, rn, tree) {}

    int getNumber() {return number;}
    void setNumber(const int& nu) {number = nu;}

private:
    int number;
};

class HwQSMBud_k : public Bud<HwQSMSegment_k,HwQSMBud_k>{
public:
    HwQSMBud_k(Tree<HwQSMSegment_k,HwQSMBud_k>* t):Bud<HwQSMSegment_k,HwQSMBud_k>(t){}
    HwQSMBud_k(const Point& p, const PositionVector& d, const LGMdouble omega,
	       Tree<HwQSMSegment_k,HwQSMBud_k>* tree): Bud<HwQSMSegment_k,HwQSMBud_k>(p, d, omega, tree){} 
};





template<class TS, class BUD>
class SetLink {
public:
    SetLink(const int f_num, const int m_num, const int m_o) : father_num(f_num),
        my_num(m_num), my_order(m_o) {}

    TreeCompartment<TS,BUD>* operator()
    (TreeCompartment<TS,BUD>* tc)const {
        if(Axis<TS,BUD>* ax =
                dynamic_cast<Axis<TS,BUD>*>(tc)){
            list<TreeCompartment<TS,BUD>*>& c_list =
                    GetTreeCompartmentList(*ax);
            typename list<TreeCompartment<TS,BUD>*>::iterator Ic;
            bool found = false;
            TS* frs;
            for(Ic = c_list.begin(); Ic != c_list.end(); Ic++) {
                if((frs =  dynamic_cast<TS*>(*Ic)))
                    if(frs->getNumber() == father_num) {
                        found = true;
                        break;             //father found
                    }
            }
            if(!found)
                return tc;

            Tree<TS,BUD>& t = GetTree(*frs);

            if(static_cast<int>(GetValue(*frs,LGAomega) == my_order)) {
                TS* seg = new TS(&t);  //into the same axis
                seg->setNumber(my_num);
                SetValue(*seg,LGAomega,static_cast<double>(my_order));
                BranchingPoint<TS,BUD> *bp =
                        new BranchingPoint<TS,BUD>(&t);
                Ic++;  Ic++;    //points now past next BranchingPoint
                c_list.insert(Ic, seg);
                c_list.insert(Ic, bp);
            }
            else {    //forking from the next BranchingPoint
                BUD *new_bud = new BUD(&t);
                TS* new_seg = new TS(&t);
                new_seg->setNumber(my_num);
                SetValue(*new_seg,LGAomega,static_cast<double>(my_order));
                BranchingPoint<TS,BUD> *new_bp =
                        new BranchingPoint<TS,BUD>(&t);
                Axis<TS,BUD>* new_ax =
                        new Axis<TS,BUD>(&t);
                InsertTreeCompartment(*new_ax, new_bud);
                InsertTreeCompartmentSecondLast(*new_ax,new_seg);
                InsertTreeCompartmentSecondLast(*new_ax,new_bp);
                Ic++;        //points now to next BranchingPoint
                BranchingPoint<TS,BUD> *bp =
                        dynamic_cast<BranchingPoint<TS,BUD>*>(*Ic);
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

template<class TS, class BUD>
void add_link(Tree<TS,BUD>& fr_tree, const int num,
              const int father_num, const int ord)  {
    if(num == 1)  { //first segment, tree is empty
        Axis<TS,BUD>& main_ax = GetAxis(fr_tree);
        BUD *bud0 = new  BUD(&fr_tree);
        TS* seg0 = new TS(&fr_tree);
        BranchingPoint<TS,BUD> *bp0 =
                new BranchingPoint<TS,BUD>(&fr_tree);
        InsertTreeCompartment(main_ax,bud0);
        InsertTreeCompartmentSecondLast(main_ax,seg0);
        InsertTreeCompartmentSecondLast(main_ax,bp0);
        seg0->setNumber(num);
        SetValue(*seg0,LGAomega,static_cast<double>(ord));
    }
    else {
      SetLink<TS,BUD> sl(father_num, num, ord);
        ForEach(fr_tree, sl);
    }

    return;
}

template<class TS, class BUD>
class SetArchitecture {
public:
  SetArchitecture(vector<vector<double> >& dat,vector< PositionVector> &direction, vector<Point> &position) : data(dat), Dir(direction), Pos(position){}
    TreeCompartment<TS,BUD>* operator()
    (TreeCompartment<TS,BUD>* tc)const {

        if(TS* frs = dynamic_cast<TS*>(tc)){
            int number = frs->getNumber()-1;
            double len = (data[number])[0];
            double rad = (data[number])[1];
            PositionVector  DirVec = Dir[number];
            Point PosVec = Pos[number];

            SetValue(*frs,LGAR, rad);
            SetValue(*frs,LGAL, len);

            // SetPoint(*frs, Point(0,0,0));  //Point(0,0,0) is only for compilation, must be position
            // SetDirection(*frs, PositionVector(0,0,0));  //PositionVector(0,0,0) is only for compilation, must be
            // //direction
            SetDirection(*frs, DirVec);
            SetPoint(*frs, PosVec);
        }
        else if(BUD *bu = dynamic_cast<BUD*>(tc)){
            SetPoint(*bu, Point(0,0,0));     //use the same position and direction to set these
            SetDirection(*bu, PositionVector(0,0,0));
        }
        else if(BranchingPoint<TS,BUD> *bp =
                dynamic_cast<BranchingPoint<TS,BUD>*>(tc)){
            SetPoint(*bp, Point(0,0,0));  //use the same position and direction to set these
            SetDirection(*bp, PositionVector(0,0,0));
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
class PropagateDirToBPBud{
public:
  PropagateDirToBPBud(){}
  PositionVector& operator()(PositionVector& direction, TreeCompartment<TS,BUD>* tc)const
  {
    if(BUD *bu = dynamic_cast<BUD*>(tc)) {
      SetDirection(*bu,direction);
    }
    else if(BranchingPoint<TS,BUD> *bp =
	    dynamic_cast<BranchingPoint<TS,BUD>*>(tc)) {
      SetDirection(*bp,direction);
    }
    else if(TS* ts = dynamic_cast<TS*>(tc)) {
      direction = GetDirection(*ts);
    }
    return direction;
  }
};

//Adds order of compartments by one
template <class TS, class BUD>
class IncrementOrderByOne {
 public:
  TreeCompartment<TS,BUD>* operator()
    (TreeCompartment<TS,BUD>* tc)const {
    if(TS *ts = dynamic_cast<TS*>(tc)){
      SetValue(*ts, LGAomega, GetValue(*ts,LGAomega)+1.0);
    }
    else if(BUD *bu = dynamic_cast<BUD*>(tc)){
      SetValue(*bu, LGAomega, GetValue(*bu,LGAomega)+1.0);
    }
    else if(BranchingPoint<TS,BUD> *bp =
	    dynamic_cast<BranchingPoint<TS,BUD>*>(tc)){
      SetValue(*bp, LGAomega, GetValue(*bp,LGAomega)+1.0);
    }
    else {;}
      
    return tc;
  }

};

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

//Sets coordinates so that the position of next BranchingPoint or
// Bud = base of previous TreeSegment + length * direction
// Coordinates of the TreeSegments are not changed
//Run this with propagate up
template <class TS, class BUD>
class ConnectTreeQSM{
public:
  Point& 
  operator()(Point& p, TreeCompartment<TS,BUD>* tc)const
  {
    if(TS* ts = dynamic_cast<TS*>(tc)) {
      p = GetPoint(*ts);
      LGMdouble l = GetValue(*ts, LGAL);
      PositionVector direction = GetDirection(*ts);
      Point end = p + Point(l*direction);
      p = end;
    }
    else {
      SetPoint(*tc, p);
    }

    return p;
  }
};


#endif
