#define	DEBUG_LEVEL_FULL

#include <cando/chem/searchStatistics.h>
//#include "core/archiveNode.h"
//#include "core/archive.h"
#
// last include is wrappers.h
#include <clasp/core/wrappers.h>


namespace chem {

#if INIT_TO_FACTORIES

#define ARGS_SearchStatistics_O_make "()"
#define DECL_SearchStatistics_O_make ""
#define DOCS_SearchStatistics_O_make "make SearchStatistics"
  SearchStatistics_sp SearchStatistics_O::make()
  {_G();
    IMPLEMENT_ME();
  };

#else

    core::T_sp 	SearchStatistics_O::__init__(core::Function_sp exec, core::Cons_sp args, core::Environment_sp env, core::Lisp_sp lisp)
{
    IMPLEMENT_ME();
}

#endif

void	SearchStatistics_O::initialize()
{
    this->Base::initialize();
    this->_ReportFrequency = UndefinedUnsignedInt;
    this->reset();
}

void	SearchStatistics_O::reset()
{
    this->_Builds = 0;
    this->_Rejects = 0;
    this->_Hits = 0;
}



#ifdef XML_ARCHIVE
    void	SearchStatistics_O::archiveBase(core::ArchiveP node)
{
    this->Base::archiveBase(node);
    node->attribute("ReportFrequency",this->_ReportFrequency);
    node->attribute("Builds",this->_Builds);
    node->attribute("Rejects",this->_Rejects);
    node->attribute("Hits",this->_Hits);
}
#endif


void	SearchStatistics_O::setReportFrequency(uint i)
{
    this->_ReportFrequency = i;
}


void	SearchStatistics_O::incrementBuilds()
{_OF();
    this->_Builds++;
    if ( this->_ReportFrequency == UndefinedUnsignedInt ) return;
    if ( ( this->_Builds % this->_ReportFrequency ) == 0 )
    {
        _lisp->print(BF("SearchStatistics_O::incrementBuilds>>%s") % this->description().c_str() );
    }
}

Bignum SearchStatistics_O::getBuilds()
{
    return this->_Builds;
}


void	SearchStatistics_O::incrementRejects()
{
    this->_Rejects++;
}

Bignum SearchStatistics_O::getRejects()
{
    return this->_Rejects;
}


void	SearchStatistics_O::incrementHits()
{
    this->_Hits++;
}

Bignum SearchStatistics_O::getHits()
{
    return this->_Hits;
}

string	SearchStatistics_O::description() const
{
    boost::format fmt("Builds[%s] Rejects[%s] Hits[%s]");
    fmt % this->_Builds % this->_Rejects % this->_Hits;
    return fmt.str();
}


    void SearchStatistics_O::exposeCando(core::Lisp_sp lisp)
{
    core::class_<SearchStatistics_O>()
	.def("getBuilds",&SearchStatistics_O::getBuilds)
	.def("getRejects",&SearchStatistics_O::getRejects)
	.def("getHits",&SearchStatistics_O::getHits)
	.def("incrementBuilds",&SearchStatistics_O::incrementBuilds)
	.def("incrementRejects",&SearchStatistics_O::incrementRejects)
	.def("incrementHits",&SearchStatistics_O::incrementHits)
    ;
}

    void SearchStatistics_O::exposePython(core::Lisp_sp lisp)
    {_G();
#ifdef USEBOOSTPYTHON
	PYTHON_CLASS(ChemPkg,SearchStatistics,"","",_lisp)
	.def("getBuilds",&SearchStatistics_O::getBuilds)
	.def("getRejects",&SearchStatistics_O::getRejects)
	.def("getHits",&SearchStatistics_O::getHits)
	.def("incrementBuilds",&SearchStatistics_O::incrementBuilds)
	.def("incrementRejects",&SearchStatistics_O::incrementRejects)
	.def("incrementHits",&SearchStatistics_O::incrementHits)
    ;
#endif
}






    EXPOSE_CLASS(chem,SearchStatistics_O);
};



