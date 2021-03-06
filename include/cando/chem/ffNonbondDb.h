/*
    File: ffNonbondDb.h
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


/*
 *	ffNonbond.h
 *
 *	Maintains a database of nonbonded terms
 */

#ifndef FFNONBONDED_H
#define	FFNONBONDED_H
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <clasp/core/common.h>
#include <cando/chem/bond.h>
#include <cando/geom/vector3.h>
//#include	"conformation.h"
#include <cando/chem/atom.h>
#include <cando/chem/residue.h>
#include <cando/chem/ffBaseDb.h>


#include <cando/chem/chemPackage.h>


namespace chem {


    struct UnknownType {
	string	_Type;
	UnknownType(const string& t) : _Type(t) {};
    };


    SMART(FFNonbondDb);

/*! A Nonbond term, this is subclassed in AmberFunction to deal with Amber specific stuff.
 * I should combine energyFunction.cc and amberFunction.cc together.
 */
    class FFNonbondCrossTerm {
    public:
	uint		_Type1Index;
	uint		_Type2Index;
	double		_RStar;
	double		_A;
	double		_C;
    };


    SMART(FFNonbondCrossTermTable);
    class FFNonbondCrossTermTable_O : public core::CxxObject_O
    {
	LISP_CLASS(chem,ChemPkg,FFNonbondCrossTermTable_O,"FFNonbondCrossTermTable",core::CxxObject_O);

    public:
    private:
	uint				_NumberOfTypes;
        std::vector<FFNonbondCrossTerm>	_CrossTerms;
    public:
	void fillUsingFFNonbondDb(FFNonbondDb_sp);
    public:
	FFNonbondCrossTerm& nonbondCrossTermFromTypes(uint type1, uint type2 );
	FFNonbondCrossTerm& nonbondCrossTerm(uint idx);

	uint nonbondCrossTermIndexFromTypes(uint type1, uint type2 );	
	uint typeMajorIndex(uint type1);

	DEFAULT_CTOR_DTOR(FFNonbondCrossTermTable_O);
    };




//#define	FFNonbondWildCardCode	-1



    typedef	enum { daDonor, daAcceptor, daNeither } DonorAcceptorEnum ;
    typedef	enum { edConstant, edDistance } EleDielectricEnum;
    typedef	enum { vmrAverage, vmrGeometric, vmrMmff94 } VdwMixRadiusEnum;
    typedef	enum { vmwAverage, vmwMmff94 } VdwMixWellEnum;

    extern core::Symbol_sp& _sym_STARDonorAcceptorEnumConverterSTAR;
    extern core::Symbol_sp& _sym_STAREleDielectricEnumConverterSTAR;
    extern core::Symbol_sp& _sym_STARVdwMixRadiusEnumConverterSTAR;
    extern core::Symbol_sp& _sym_STARVdwMixWellEnumConverterSTAR;
};



ENUM_TRANSLATOR(chem::DonorAcceptorEnum,chem::_sym_STARDonorAcceptorEnumConverterSTAR);
ENUM_TRANSLATOR(chem::EleDielectricEnum,chem::_sym_STAREleDielectricEnumConverterSTAR);
ENUM_TRANSLATOR(chem::VdwMixRadiusEnum,chem::_sym_STARVdwMixRadiusEnumConverterSTAR);
ENUM_TRANSLATOR(chem::VdwMixWellEnum,chem::_sym_STARVdwMixWellEnumConverterSTAR);






namespace chem {

SMART(FFNonbond);
class FFNonbond_O : public FFParameter_O
{
    LISP_CLASS(chem,ChemPkg,FFNonbond_O,"FFNonbond",FFParameter_O);

public:
public:
    void initialize();
        bool fieldsp() const { return true; };
        void fields(core::Record_sp node);
private:
    core::Symbol_sp 	_Type;
	double		_Radius_Nanometers;
	double		_Epsilon_kJ;	// epsilon in AMBER
	double		_Apol;
	double		_Neff;
	double		_Mass;
        double          _Polarizability;
	double		_InitialCharge;
	double		_Fcadj;	// formal charge sharing scale factor
	double		_Pbci;	// partial bond charge increment
	DonorAcceptorEnum	_DonorAcceptor;

public:
	void setType(core::Symbol_sp s) {this->_Type=s;};
    core::Symbol_sp getType() const { return this->_Type;};
	void setPolarizability(double d) { this->_Polarizability = d;};
	void setMass(double d) { this->_Mass = d;};
	void setRadius_Angstroms(double val);
	void setRadius_Nanometers(double val);

	double getRadius_Angstroms() const;
	double getRadius_Nanometers() const;


	void setEpsilon_kJ(double val);
	void setEpsilon_kCal(double val);

	double getEpsilon_kJ() const;
	double getEpsilon_kCal() const;

virtual	string	levelDescription();
virtual ParameterType type() { return nonbond; };
DEFAULT_CTOR_DTOR(FFNonbond_O);
};




////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
SMART(FFNonbondDb);
class FFNonbondDb_O : public FFBaseDb_O
{
    LISP_CLASS(chem,ChemPkg,FFNonbondDb_O,"FFNonbondDb",FFBaseDb_O);

public:
public:
    void	initialize();
        bool fieldsp() const { return true; };
        void fields(core::Record_sp node);
public:
    typedef	gctools::Vec0<FFNonbond_sp>::iterator	FFNonbond_spIterator;
    double			_EleDielectricValue;
    double			_EleBuffer;
    double			_EleScale14;
    core::Symbol_sp		_EleChargeFcn;
    double			_VdwScale14;
    double			_VdwScaleBufferA;
    double			_VdwScaleBufferB;
    gctools::Vec0<FFNonbond_sp>	_Terms;
    core::HashTableEq_sp _Lookup;
    EleDielectricEnum	_EleDielectricCode;
    VdwMixRadiusEnum	_VdwMixRadius;
    VdwMixWellEnum		_VdwMixWell;
public:
    FFNonbond_spIterator	begin() {return this->_Terms.begin();};
    FFNonbond_spIterator	end() {return this->_Terms.end();};

    void	add( FFNonbond_sp nonbonded );
    bool    hasType( core::Symbol_sp type);
    gc::Nilable<FFNonbond_sp>   findType( core::Symbol_sp type );
    FFNonbond_sp   getFFNonbondUsingTypeIndex( uint typeIdx );
    uint findTypeIndex( core::Symbol_sp type );
    uint findTypeIndexOrThrow( core::Symbol_sp type );
    /*! Return the type index multiplied by the number
     * of type indices.  Another type index can be added
     * to this number to get an index into a Nonbond crossterm table
     */
    uint findTypeMajorIndex( core::Symbol_sp type );

    /*! Return the number of Nonbond atom types
     */
    uint  numberOfTypes();



    DEFAULT_CTOR_DTOR(FFNonbondDb_O);
};



};

TRANSLATE(chem::FFNonbondCrossTermTable_O);
TRANSLATE(chem::FFNonbond_O);
TRANSLATE(chem::FFNonbondDb_O);
#endif
