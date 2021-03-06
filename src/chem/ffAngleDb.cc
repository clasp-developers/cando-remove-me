/*
    File: ffAngleDb.cc
*/
/*
Open Source License
Copyright (c) 2016, Christian E. Schafmeister
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 
This is an open source license for the CANDO software from Temple University, but it is not the only one. Contact Temple University at mailto:techtransfer@temple.edu if you would like a different license.
*/
/* -^- */
       
#define	DEBUG_LEVEL_NONE


//
// (C) 2004 Christian E. Schafmeister
//


/*
 *	ffAngle.cc
 *
 *	Maintains a database of stretch types
 */
#include <clasp/core/foundation.h>
#include <clasp/core/str.h>
#include <cando/chem/ffAngleDb.h>
#include <cando/chem/forceField.h>
#include <cando/chem/ffStretchDb.h>
#include <cando/chem/units.h>
//#include "core/archiveNode.h"
#include <clasp/core/wrappers.h>


namespace chem
{



void FFAngle_O::fields(core::Record_sp node)
{
  this->Base::fields(node);
  node->field(INTERN_(kw,type1),this->_Type1);
  node->field(INTERN_(kw,type2),this->_Type2);
  node->field(INTERN_(kw,type3),this->_Type3);
  node->field(INTERN_(kw,angRad), this->_AngRad);
  node->field(INTERN_(kw,k2),this->_K2__kJPerRadianSquared);
}

void	FFAngleDb_O::fields(core::Record_sp node)
{
    this->Base::fields(node);
    node->field(INTERN_(kw,angleFunction),this->_AngleFunction);
    node->field(INTERN_(kw,angles),this->_Terms );
    node->field(INTERN_(kw,map),this->_Lookup );
    node->field(INTERN_(kw,zConstants),this->_ZConstants);
    node->field(INTERN_(kw,cConstants),this->_CConstants);
}

#if 0 //[
void
adapt::QDomNode_sp	FFAngle_O::asXml()
{
    adapt::QDomNode_sp	node;
    node = adapt::QDomNode_O::create(XmlTag_FFAngle());
    node->addAttributeString("Type1",this->_Type1 );
    node->addAttributeString("Type2",this->_Type2 );
    node->addAttributeString("Type3",this->_Type3 );
    node->addAttributeDoubleScientific("AngRad",this->_AngRad);
    node->addAttributeDoubleScientific("K2",this->_K2);
    return node;
}

    void	FFAngle_O::parseFromXml( adapt::QDomNode_sp node )
{
    this->_Type1 = node->getAttributeString("Type1");
    this->_Type2 = node->getAttributeString("Type2");
    this->_Type3 = node->getAttributeString("Type3");
    this->_AngRad = node->getAttributeDouble("AngRad");
    this->_K2 = node->getAttributeDouble("K2");
}
#endif //]




void	FFAngleDb_O::initialize()
{
    this->Base::initialize();
    this->_Terms.clear();
    this->_Lookup.clear();
    this->_ZConstants.clear();
    this->_CConstants.clear();
}




void FFAngle_O::setK2_kCalPerRadianSquared(double val)
{
    this->_K2__kJPerRadianSquared = kCalPerRadianSquared_to_kJPerRadianSquared(val);
}


void FFAngle_O::setK2_kJPerRadianSquared(double val)
{
    this->_K2__kJPerRadianSquared = val;
}


double FFAngle_O::getK2_kCalPerRadianSquared() const
{
    return kJPerRadianSquared_to_kCalPerRadianSquared(this->_K2__kJPerRadianSquared);
}


double FFAngle_O::getK2_kJPerRadianSquared() const
{
    return this->_K2__kJPerRadianSquared;
}






    core::Symbol_sp angleKey(core::Symbol_sp t1
                             , core::Symbol_sp t2
                             , core::Symbol_sp t3)
    {
        return chemkw_intern(t1->symbolName()->get()+"-"
                           + t2->symbolName()->get()+"-"
                           + t3->symbolName()->get());
    }



void    FFAngleDb_O::add(FFAngle_sp ang)
{
    core::Symbol_sp  key;
    this->_Terms.push_back(ang);
    key = angleKey(ang->_Type1,ang->_Type2,ang->_Type3);
    this->_Lookup.set(key,ang);
    key = angleKey(ang->_Type3,ang->_Type2,ang->_Type1);
    this->_Lookup.set(key,ang);
}


FFAngle_sp FFAngleDb_O::findTerm(chem::Atom_sp a1, chem::Atom_sp a2, chem::Atom_sp a3 )
{ 
FFAngle_sp       match;
core::Symbol_sp          key;
core::Symbol_sp		t1, t2, t3;
    t1 = a1->getType();
    t2 = a2->getType();
    t3 = a3->getType();
    key = angleKey(t1,t2,t3);
    if ( this->_Lookup.count(key) != 0 ) {
        return this->_Lookup.get(key);
    }
    key = angleKey(t3,t2,t1);
    if ( this->_Lookup.count(key) != 0 ) {
        return this->_Lookup.get(key);
    }
    if ( t1->symbolName()->get() > t3->symbolName()->get() ) {
	core::Symbol_sp tt = t1;
	t1 = t3;
	t3 = tt;
    }
    match = this->estimateTerm(a1,a2,a3);
    return match;
}


#if 0 //[
bool    FFAngleDb_O::hasTerm(string t1, string t2, string t3 )
{
FFAngle_sp	match;
string          key;
    key = angleKey(t1,t2,t3);
    if ( this->_Lookup.count(key) != 0 ) {
        return true;
    }
    key = angleKey(t3,t2,t1);
    if ( this->_Lookup.count(key) != 0 ) {
        return true;
    }
    return false;
}


void    FFAngleDb_O::cantFind(string t1, string t2, string t3 )
{
    stringstream ss;
    ss << "PARAMETER_PROBLEM Can't find angle term for ("<<t1<<")-("<<t2<<")-("<<t3<<")";
    ss << " atoms("<<a1->description()<<") - (" << a2->description()<<")";
    printf( "%s\n", ss.str().c_str() );
    //SIMPLE_ERROR(BF("%s")%ss.str());
}
#endif //]



void	FFAngle_O::initialize()
{
    this->Base::initialize();
    this->_Type1 = _Nil<core::Symbol_O>();
    this->_Type2 = _Nil<core::Symbol_O>();
    this->_Type3 = _Nil<core::Symbol_O>();
    this->_AngRad = 0.0;
    this->_K2__kJPerRadianSquared = 0.0;
//    this->_K3 = 0.0;
//    this->_K4 = 0.0;
//    this->_Ub_k = 0.0;
//    this->_Ub_len = 0.0;
}


/*! Estimate the angle term according to Wang et al. J. Comput. Chem 25, 1157-1174 (2004) */
FFAngle_sp	FFAngleDb_O::estimateTerm(chem::Atom_sp a1, chem::Atom_sp a2, chem::Atom_sp a3 )
{
FFStretch_sp	r12,r32;
FFAngle_sp	ff_121;
FFAngle_sp	ff_323;
double		d, z1, c2, z3, angRad, k;
core::Symbol_sp		element1, element2, element3;
core::Symbol_sp		t1, t2, t3;
ForceField_sp	ff;
    ASSERTNOTNULL(this->_ForceField);
    ff = this->_ForceField;
    LOG(BF("status") );
    t1 = a1->getType();
    t2 = a2->getType();
    t3 = a3->getType();
    LOG(BF("status") );
    // To avoid an std::endless loop of estimating terms just go straight to guess if t1==t3
    if ( t1==t3 ) goto GUESS;
    LOG(BF("status") );
    LOG(BF("status") );
    ff_121 = ff->_Angles->findTerm(a1,a2,a1);
    ff_323 = ff->_Angles->findTerm(a3,a2,a3);
    LOG(BF("status") );
    if ( ff_121.nilp() ) goto GUESS;
    LOG(BF("status") );
    if ( ff_323.nilp() ) goto GUESS;
    LOG(BF("status") );
    angRad = (ff_121->_AngRad + ff_323->_AngRad)/2.0;
    element1 = a1->getElementAsSymbol();
    element2 = a2->getElementAsSymbol();
    element3 = a3->getElementAsSymbol();
    LOG(BF("status") );
    if ( ff->_Angles->_ZConstants.count(element1) == 0 ) goto GUESS;
    LOG(BF("status") );
    if ( ff->_Angles->_CConstants.count(element2) == 0 ) goto GUESS;
    LOG(BF("status") );
    if ( ff->_Angles->_ZConstants.count(element3) == 0 ) goto GUESS;
    LOG(BF("status") );
    z1 = ff->_Angles->_ZConstants.get(element1)->get();
    c2 = ff->_Angles->_CConstants.get(element2)->get();
    z3 = ff->_Angles->_ZConstants.get(element3)->get();
    r12 = ff->_Stretches->findTerm(a1,a2);
    r32 = ff->_Stretches->findTerm(a3,a2);
    LOG(BF("status") );
    if ( r12.nilp() ) goto GUESS;
    LOG(BF("status") );
    if ( r32.nilp() ) goto GUESS;
    LOG(BF("status") );
    d = (r12->getR0_Angstrom()-r32->getR0_Angstrom())/(r12->getR0_Angstrom()+r32->getR0_Angstrom());
    d = d*d;
    k = 143.9*z1*c2*z3*(1.0/((r12->getR0_Angstrom()+r32->getR0_Angstrom())*(angRad*angRad)))*exp(-2.0*d);
    {
      FFAngle_sp angle = FFAngle_O::create();
      angle->_Level = estimated;
      angle->_Type1 = t1;
      angle->_Type2 = t2;
      angle->_Type3 = t3;
      angle->_AngRad = angRad;
      angle->setK2_kCalPerRadianSquared(k);
      return angle;
    }
GUESS:
    {
      FFAngle_sp angle = FFAngle_O::create();
      angle->_Level = rough;
      angle->_Type1 = t1;
      angle->_Type2 = t2;
      angle->_Type3 = t3;
      if ( a2->getHybridization() == hybridization_sp3 ) {
	angle->_AngRad = 109.5*0.0174533;
      } else if ( a2->getHybridization() == hybridization_sp2 ) {
	angle->_AngRad = 120.0*0.0174533;
      } else if ( a2->getHybridization() == hybridization_sp ) {
	angle->_AngRad = 180.0*0.0174533;
      } else {
	LOG(BF("Unknown hybridization(%s) for: %s") % a2->getHybridization().c_str() % a2->description().c_str() );
	angle->_AngRad = 109.5*0.0174533;
      }
		// See Gaff paper for where I get these force constants
      if ( a1->getElement() == element_H && a3->getElement() == element_H ) {
	angle->_K2__kJPerRadianSquared = kCalPerRadianSquared_to_kJPerRadianSquared(32.5);
      } else if ( a1->getElement() == element_H || a3->getElement() == element_H ) {
	angle->_K2__kJPerRadianSquared = kCalPerRadianSquared_to_kJPerRadianSquared(50.0);
      } else {
	angle->_K2__kJPerRadianSquared = kCalPerRadianSquared_to_kJPerRadianSquared(70.0);
      }
      return angle;
    }
}


string	FFAngle_O::levelDescription()
{
stringstream	desc;
    desc << this->FFParameter_O::levelDescription();
    desc << " angle between types "<<this->_Type1<< " "
		<< this->_Type2 << " "
		<< this->_Type3;
    return desc.str();
}




};
