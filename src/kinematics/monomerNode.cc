/*
    File: monomerNode.cc
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
#define	DEBUG_LEVEL_FULL

#include <clasp/core/common.h>
#include <clasp/core/environment.h>
#include <cando/adapt/symbolSet.h>
#include <cando/chem/monomer.h>
#include <cando/chem/coupling.h>
#include <cando/chem/constitution.h>
#include <cando/chem/topology.h>
#include <cando/chem/candoDatabase.h>
#include <cando/kinematics/chainNode.h>
#include <cando/kinematics/monomerNode.h>
#include <clasp/core/wrappers.h>
namespace kinematics
{

// ----------------------------------------------------------------------
//





    MonomerNode_sp MonomerNode_O::create(int monomerId)
    {
	GC_ALLOCATE(MonomerNode_O, monomerNode );
	monomerNode->_MonomerId = monomerId;
	return monomerNode;
    }

#if INIT_TO_FACTORIES

#define ARGS_MonomerNode_O_make "()"
#define DECL_MonomerNode_O_make ""
#define DOCS_MonomerNode_O_make "make MonomerNode"
  MonomerNode_sp MonomerNode_O::make()
    {
      IMPLEMENT_ME();
    };

#else
    core::T_sp MonomerNode_O::__init__(core::Function_sp exec, core::Cons_sp args, core::Environment_sp env, core::Lisp_sp lisp)
    {
//      this->Base::__init__(exec,args,env,lisp);
//      arg = translate::from_object<XXXX>::convert(env->lookup(this->Package(),"YYY"));
	return _Nil<core::T_O>();
    }
#endif

#ifdef XML_ARCHIVE
    void MonomerNode_O::archiveBase(core::ArchiveP node)
    {
        this->Base::archiveBase(node);
	// Archive other instance variables here
    }
#endif


    void MonomerNode_O::initialize()
    {_OF();
        this->Base::initialize();
    }



    void MonomerNode_O::recursivelyBuildChildren(ChainNode_sp chainNode,
						 RingClosingMonomerMap ringClosingMonomerMap,
						 gctools::Nilable<MonomerNode_sp> parent,
						 gctools::Nilable<chem::DirectionalCoupling_sp> coupling,
						 chem::Monomer_sp monomer)
    {
	LOG(BF("recursivelyBuildChildren on monomer[%s]") % monomer->getName()->__repr__() );
	this->_Parent = parent;
	if ( coupling.notnilp() )
	{
	    this->_ParentPlugName = coupling->getOutMonomerPlugName();
	    ASSERTF(chem::DirectionalCoupling_O::isInPlugName(this->_ParentPlugName),
		    BF("This has to be an inPlugName[%s] - it isnt")
		    % this->_ParentPlugName->__repr__() );
	} else
	{
          this->_ParentPlugName = _Nil<core::Symbol_O>();
	}
	this->_MonomerName = monomer->getName();
	this->_Topology = monomer->getTopology();
	this->_ConformationIndex = 0;
	for ( chem::Monomer_O::Couplings::const_iterator it=monomer->_Couplings.begin();
	      it!=monomer->_Couplings.end(); it++ )
	{
	    chem::Coupling_sp coupling = it->second;
	    if ( coupling->isRingClosing() ) continue;
	    /*! Skip InPlugs */
	    chem::DirectionalCoupling_sp dirCoupling = coupling.as<chem::DirectionalCoupling_O>();
	    if ( dirCoupling->getInMonomer() != monomer ) continue;
	    chem::Monomer_sp otherMonomer = dirCoupling->getOutMonomer();
	    MonomerNode_sp otherMonomerNode = ChainNode_O::monomerNodeFactory(chainNode,ringClosingMonomerMap,otherMonomer);
	    core::Symbol_sp outPlugName = dirCoupling->getInMonomerPlugName();
	    ASSERTF(chem::DirectionalCoupling_O::isOutPlugName(outPlugName),
		    BF("Problem - this[%s] should be an OutPlug name but it isnt")
		    %outPlugName->__repr__() );
	    this->_Children.set(outPlugName,otherMonomerNode);
	    otherMonomerNode->recursivelyBuildChildren(chainNode,
						       ringClosingMonomerMap,
						       this->sharedThis<MonomerNode_O>(),
						       dirCoupling,otherMonomer);
	}
    }


    core::List_sp MonomerNode_O::identifyConstitutionAndTopology(chem::CandoDatabase_sp db)
    {_OF();
	chem::Constitution_sp constitution = db->constitutionForNameOrPdb(this->_MonomerName);
	if ( constitution.nilp() )
	{
	    SIMPLE_ERROR(BF("Could not find Constitution for monomer[%s]")
			       % this->_MonomerName->__repr__() );
	}
	adapt::SymbolSet_sp myPlugNameSet = adapt::SymbolSet_O::create();
	if ( this->_ParentPlugName.notnilp() )
	{
	    myPlugNameSet->insert(this->_ParentPlugName);
	}
	for ( adapt::SymbolMap<MonomerNode_O>::iterator it=this->_Children.begin();
	      it!=this->_Children.end(); it++ )
	{
	    myPlugNameSet->insert(it->first);
	}
	chem::Topology_sp topology = _Nil<chem::Topology_O>();
	chem::Constitution_O::TopologyMap::iterator it;
	{_BLOCK_TRACEF(BF("Looking for Topology with plugs: %s") % myPlugNameSet->asString() );
	    for ( it= constitution->begin_Topologies();
		  it!=constitution->end_Topologies(); it++ )
	    {
		if ( (it->second)->hasMatchingPlugsWithMates(myPlugNameSet))
		{
		    topology = it->second;
		    break;
		}
	    }
	}
	if ( topology.nilp() )
	{
	    SIMPLE_ERROR(BF("No topology could be found for monomer[%s] with plugs[%s]")
			       % this->_MonomerName->__repr__() % myPlugNameSet->asString() );
	}
	return core::Cons_O::createList(constitution,topology);
    }


    void MonomerNode_O::describeRecursivelyIntoStringStream(const string& prefix, stringstream& output) const
    {_OF();
	LOG(BF("Describing %s[%s]") % this->className() % this->_MonomerName->__repr__() );
	output << prefix;
	if ( this->_ParentPlugName.notnilp() )
	{
	    output << this->_ParentPlugName->__repr__() << " ";
	}
	output << this->className()
	       << "["
	       << this->_MonomerName->__repr__()
	       << "]" << std::endl;
	for ( adapt::SymbolMap<MonomerNode_O>::const_iterator it=this->_Children.begin();
	      it!=this->_Children.end(); it++ )
	{
	    it->second->describeRecursivelyIntoStringStream(prefix+"  ",output);
	}
    }

    string MonomerNode_O::asString() const
    {_OF();
	stringstream out;
	this->describeRecursivelyIntoStringStream("",out);
	return out.str();
    }


}; /* kinematics */
