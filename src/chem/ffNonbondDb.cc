/*
    File: ffNonbondDb.cc
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
       
//
// (C) 2004 Christian E. Schafmeister
//

#define TURN_DEBUG_OFF

/*
 *	ffNonbonds.cc
 *
 *	Maintains databases and structures to store types and type assignement
 *	rules.
 */
#include <clasp/core/foundation.h>
#include <clasp/core/hashTableEq.h>
#include <cando/chem/ffNonbondDb.h>
#include <cando/chem/loop.h>
#include <cando/chem/units.h>
#include <clasp/core/wrappers.h>

namespace chem {
void FFNonbondCrossTermTable_O::fillUsingFFNonbondDb( FFNonbondDb_sp db )
{
  uint numberOfTypes = db->numberOfTypes();
  this->_NumberOfTypes = numberOfTypes;
  for ( uint it1 = 0; it1 < numberOfTypes; it1++ )
  {
    for ( uint it2 = 0; it2 < numberOfTypes; it2++ )
    {
      FFNonbondCrossTerm term;
      term._Type1Index = it1;
      term._Type2Index = it2;
      FFNonbond_sp ffNonbond1 = db->getFFNonbondUsingTypeIndex(it1);
      FFNonbond_sp ffNonbond2 = db->getFFNonbondUsingTypeIndex(it2);
      term._RStar = ffNonbond1->getRadius_Angstroms() + ffNonbond2->getRadius_Angstroms();
      double epsilonij = sqrt(ffNonbond1->getEpsilon_kCal()*ffNonbond2->getEpsilon_kCal());
      term._A = epsilonij*pow(term._RStar,12.0);
      term._C = 2.0*epsilonij*pow(term._RStar,6.0);
      this->_CrossTerms.push_back(term);
    }
  }
}

uint FFNonbondCrossTermTable_O::typeMajorIndex(uint typeIndex)
{
  ASSERT_lt(typeIndex,this->_NumberOfTypes);
  return typeIndex*this->_NumberOfTypes;
}

FFNonbondCrossTerm& FFNonbondCrossTermTable_O::nonbondCrossTermFromTypes(uint type1, uint type2 )
{
  ASSERT_lt(type1,this->_NumberOfTypes);
  ASSERT_lt(type2,this->_NumberOfTypes);
  uint idx = type1*this->_NumberOfTypes + type2;
  ASSERT_lt(idx,this->_CrossTerms.size());
  return this->_CrossTerms[idx];
}

FFNonbondCrossTerm& FFNonbondCrossTermTable_O::nonbondCrossTerm(uint idx)
{_OF();
  ASSERT_lt(idx,this->_CrossTerms.size());
  return this->_CrossTerms[idx];
}

uint FFNonbondCrossTermTable_O::nonbondCrossTermIndexFromTypes(uint type1, uint type2 )
{_OF();
  ASSERT_lt(type1,this->_NumberOfTypes);
  ASSERT_lt(type2,this->_NumberOfTypes);
  uint idx = type1*this->_NumberOfTypes + type2;
  return idx;
}

core::NullTerminatedEnumAssociation daEnum[] = {
    { "daDonor", daDonor },
    { "daAcceptor", daAcceptor },
    { "daNeither", daNeither },
    { "", -1 }
};

core::NullTerminatedEnumAssociation dielectricEnum[] = {
    { "edConstant", edConstant},
    { "edDistance", edDistance},
    { "", -1 }
};

core::NullTerminatedEnumAssociation vmrEnum[] = {
    { "vmrAverage", vmrAverage},
    { "vmrGeometric", vmrGeometric},
    { "vmrMmff94", vmrMmff94},
    { "", -1 }
};

core::NullTerminatedEnumAssociation vmwEnum[] = {
    { "vmwAverage", vmwAverage},
    { "vmwMmff94", vmwMmff94},
    { "", -1 }
};



SYMBOL_EXPORT_SC_(ChemPkg,STARDonorAcceptorEnumConverterSTAR);
SYMBOL_EXPORT_SC_(ChemPkg,STAREleDielectricEnumConverterSTAR);
SYMBOL_EXPORT_SC_(ChemPkg,STARVdwMixRadiusEnumConverterSTAR);
SYMBOL_EXPORT_SC_(ChemPkg,STARVdwMixWellEnumConverterSTAR);



SYMBOL_EXPORT_SC_(KeywordPkg,daDonor);
SYMBOL_EXPORT_SC_(KeywordPkg,daAcceptor);
SYMBOL_EXPORT_SC_(KeywordPkg,daNeither);
SYMBOL_EXPORT_SC_(KeywordPkg,edConstant);
SYMBOL_EXPORT_SC_(KeywordPkg,edDistance);
SYMBOL_EXPORT_SC_(KeywordPkg,vmrAverage);
SYMBOL_EXPORT_SC_(KeywordPkg,vmrGeometric);
SYMBOL_EXPORT_SC_(KeywordPkg,vmrMmff94);
SYMBOL_EXPORT_SC_(KeywordPkg,vmwAverage);
SYMBOL_EXPORT_SC_(KeywordPkg,vmwMmff94);

CL_BEGIN_ENUM(DonorAcceptorEnum,_sym_STARDonorAcceptorEnumConverterSTAR,"DonorAcceptorEnum");
CL_VALUE_ENUM( kw::_sym_daDonor, daDonor );
CL_VALUE_ENUM( kw::_sym_daAcceptor, daAcceptor );
CL_VALUE_ENUM( kw::_sym_daNeither, daNeither );
CL_END_ENUM(_sym_STARDonorAcceptorEnumConverterSTAR);

CL_BEGIN_ENUM(EleDielectricEnum,_sym_STAREleDielectricEnumConverterSTAR,"EleDielectricEnum");
CL_VALUE_ENUM( kw::_sym_edConstant, edConstant);
CL_VALUE_ENUM( kw::_sym_edDistance, edDistance);
CL_END_ENUM(_sym_STAREleDielectricEnumConverterSTAR);

CL_BEGIN_ENUM(VdwMixRadiusEnum,_sym_STARVdwMixRadiusEnumConverterSTAR,"VdwMixRadiusEnum");
CL_VALUE_ENUM( kw::_sym_vmrAverage, vmrAverage);
CL_VALUE_ENUM( kw::_sym_vmrGeometric, vmrGeometric);
CL_VALUE_ENUM( kw::_sym_vmrMmff94, vmrMmff94);
CL_END_ENUM(_sym_STARVdwMixRadiusEnumConverterSTAR);

CL_BEGIN_ENUM(VdwMixWellEnum,_sym_STARVdwMixWellEnumConverterSTAR,"VdwMixWellEnum");
CL_VALUE_ENUM( kw::_sym_vmwAverage, vmwAverage);
CL_VALUE_ENUM( kw::_sym_vmwMmff94, vmwMmff94);
CL_END_ENUM(_sym_STARVdwMixWellEnumConverterSTAR);

void    FFNonbondDb_O::initialize()
{
  this->Base::initialize();
  this->_Terms.clear();
  this->_Lookup = core::HashTableEq_O::create_default();
  this->_EleDielectricValue = 0.0;
  this->_EleBuffer = 0.0;
  this->_EleScale14 = 0.0;
  this->_EleChargeFcn = _Nil<core::T_O>();
  this->_VdwScale14 = 0.0;
  this->_VdwScaleBufferA = 0.0;
  this->_VdwScaleBufferB = 0.0;
  this->_EleDielectricCode = edConstant;
  this->_VdwMixRadius = vmrAverage;
  this->_VdwMixWell = vmwAverage;
}


void	FFNonbond_O::fields(core::Record_sp node)
{
  this->Base::fields(node);
  node->field(INTERN_(kw,type),this->_Type );
  node->field(INTERN_(kw,radius),this->_Radius_Nanometers);
  node->field(INTERN_(kw,well),this->_Epsilon_kJ);
  node->field_if_not_default(INTERN_(kw,apol),this->_Apol,0.0);
  node->field_if_not_default(INTERN_(kw,neff),this->_Neff,0.0);
  node->field(INTERN_(kw,mass),this->_Mass);
  node->field_if_not_default(INTERN_(kw,pol),this->_Polarizability,0.0);
  node->field_if_not_default(INTERN_(kw,initCharge),this->_InitialCharge,0.0);
  node->field_if_not_default(INTERN_(kw,fcadj),this->_Fcadj,0.0);
  node->field_if_not_default(INTERN_(kw,pbci),this->_Pbci,0.0);
  node->field(INTERN_(kw,da),this->_DonorAcceptor );
}

void	FFNonbondDb_O::fields(core::Record_sp node)
{
  this->Base::fields(node);
  node->field_if_not_default(INTERN_(kw,eleDielectricValue), this->_EleDielectricValue, 0.0);
  node->field_if_not_default(INTERN_(kw,eleBuffer), this->_EleBuffer, 0.0);
  node->field_if_not_default(INTERN_(kw,eleScale14), this->_EleScale14, 0.0);
  node->field_if_not_nil(INTERN_(kw,eleChargeFcn), this->_EleChargeFcn);
  node->field_if_not_default(INTERN_(kw,vdwScale14), this->_VdwScale14, 0.0);
  node->field_if_not_default(INTERN_(kw,vdwScaleBufferA), this->_VdwScaleBufferA, 0.0);
  node->field_if_not_default(INTERN_(kw,vdwScaleBufferB), this->_VdwScaleBufferB, 0.0);
  node->field(INTERN_(kw,nonbonds),this->_Terms );
  node->field(INTERN_(kw,map),this->_Lookup );
  node->field( INTERN_(kw,eleDielectricCode), this->_EleDielectricCode);
  node->field( INTERN_(kw,vdwMixRadius), this->_VdwMixRadius);
  node->field( INTERN_(kw,vdwMixWell), this->_VdwMixWell);
}


void    FFNonbondDb_O::add(FFNonbond_sp nb)
{_OF();
  if ( this->_Lookup->contains(nb->getType()) ) {  // This was negated
    SIMPLE_ERROR(BF("Adding nonbonded( %s ) to database but it's already there") % nb->getType() );
  }
  uint index = this->_Terms.size();
  this->_Terms.push_back(nb);
  this->_Lookup->setf_gethash(nb->getType(),core::clasp_make_fixnum(index));
}

bool    FFNonbondDb_O::hasType(core::Symbol_sp type)
{
  if ( this->_Lookup->contains(type) ) return true;
  return false;
}

gc::Nilable<FFNonbond_sp>   FFNonbondDb_O::findType(core::Symbol_sp type)
{
  if ( this->_Lookup->contains(type) ) {
//    printf("%s:%d:%s FFNonbondDb_O::findType type: %s\n", __FILE__, __LINE__, __FUNCTION__, _rep_(type).c_str());
    core::T_sp val = this->_Lookup->gethash(type);
    uint index = core::clasp_to_fixnum(val);
    return this->_Terms[index];
  }
  return _Nil<core::T_O>();
}

CL_LISPIFY_NAME("findTypeIndex");
CL_DEFMETHOD uint FFNonbondDb_O::findTypeIndex(core::Symbol_sp type)
{
  if ( this->_Lookup->contains(type) ) {
    uint index = core::clasp_to_fixnum(this->_Lookup->gethash(type));
    return index;
  }
  return UndefinedUnsignedInt;
}

uint FFNonbondDb_O::findTypeIndexOrThrow(core::Symbol_sp type)
{_OF();
  uint ti = this->findTypeIndex(type);
  if ( ti == UndefinedUnsignedInt )
  {
    SIMPLE_ERROR(BF("Unknown type %s") % _rep_(type));
  }
  return ti;
}
uint FFNonbondDb_O::findTypeMajorIndex(core::Symbol_sp type)
{
  if ( this->_Lookup->contains(type)) {
    core::T_sp val = this->_Lookup->gethash(type);
    return core::clasp_to_fixnum(val)*this->_Terms.size();
  }
  return UndefinedUnsignedInt;
}

FFNonbond_sp FFNonbondDb_O::getFFNonbondUsingTypeIndex(uint typeIdx)
{_OF();
  return this->_Terms[typeIdx];
}



CL_LISPIFY_NAME("numberOfTypes");
CL_DEFMETHOD uint FFNonbondDb_O::numberOfTypes()
{
  return this->_Terms.size();
}




void FFNonbond_O::initialize()
{
  this->Base::initialize();
  this->_Type = _Nil<core::Symbol_O>();
  this->_Radius_Nanometers = 0.0;
  this->_Epsilon_kJ = 0.0; // Depth of the VDW well
  this->_Apol = 0.0;
  this->_Neff = 0.0;
  this->_Mass = 0.0;
  this->_Polarizability = 0.0;
  this->_DonorAcceptor = daNeither;
  this->_InitialCharge = 0.0;
  this->_Fcadj = 0.0;
  this->_Pbci = 0.0;
}


void FFNonbond_O::setRadius_Nanometers(double n)
{
  this->_Radius_Nanometers = n;
}

void FFNonbond_O::setRadius_Angstroms(double n)
{
  this->_Radius_Nanometers = n/10.0;
}

double FFNonbond_O::getRadius_Nanometers() const
{
  return this->_Radius_Nanometers;
}

double FFNonbond_O::getRadius_Angstroms() const
{
  return this->_Radius_Nanometers*10.0;
}



void FFNonbond_O::setEpsilon_kJ(double kj)
{
  this->_Epsilon_kJ = kj;
}

void FFNonbond_O::setEpsilon_kCal(double kcal)
{
  this->_Epsilon_kJ = kCal_to_kJ(kcal);
}


double FFNonbond_O::getEpsilon_kCal() const
{
  return kJ_to_kCal(this->_Epsilon_kJ);
}

double FFNonbond_O::getEpsilon_kJ() const
{
  return this->_Epsilon_kJ;
}


#if 0 //[
adapt::QDomNode_sp	FFNonbond_O::asXml()
{
  adapt::QDomNode_sp	node;
  node = adapt::QDomNode_O::create(env,"FFNonbond");
  node->addAttributeString("type",this->_Type);
  node->addAttributeDoubleScientific("Radius",this->_Radius);
  node->addAttributeDoubleScientific("Well",this->_Well);
  return node;
}

void	FFNonbond_O::parseFromXml(adapt::QDomNode_sp node)
{
  this->_Type = node->getAttributeString("type");
  this->_Radius = node->getAttributeDouble("Radius");
  this->_Well = node->getAttributeDouble("Well");
}
#endif //]


string	FFNonbond_O::levelDescription()
{
  stringstream	desc;
  desc << this->FFParameter_O::levelDescription();
  desc << " nonbond type "<<this->_Type;
  return desc.str();
}


};
