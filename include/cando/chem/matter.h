       
       
//
// (C) 2004 Christian E. Schafmeister
//


/*
 *	matter.h
 *
 *	Superclass for Aggregate/Molecule/Residue/Atom
 *
 */

#ifndef Matter_H
#define	Matter_H
#include <clasp/core/common.h>

#include <iostream>
#include <vector>
#include <string>
#include <cando/geom/matrix.h>
//#include "geom/objectList.fwd.h"
#include <cando/geom/boundingBox.fwd.h>
#ifdef RENDER
//#include "geom/render.fwd.h"
#endif

#include <cando/chem/matter.fwd.h>
#include <cando/chem/atomIdMap.fwd.h>
#include <cando/chem/atomId.fwd.h>
#include <cando/chem/elements.h>
#include <clasp/core/hashTable.fwd.h>
#include <clasp/core/lispVector.h>
#include <clasp/core/holder.h>
#include <cando/chem/restraint.fwd.h>
//#include	"geom/vector3.h"


#define	MATTER_CLASS		99	//	'c'
#define	AGGREGATE_CLASS		103	//	'g'
#define	MOLECULE_CLASS		109	//	'm'
#define	RESIDUE_CLASS		114	//	'r'
#define	ATOM_CLASS		97	//	'a'
#define	VIRTUAL_ATOM_CLASS	118	//	'v'
#define	BOND_CLASS		98	//	'b'


#include <cando/chem/chemPackage.h>


namespace chem {

#define	UNDEFINED_ID -1



SMART(RestraintList);
SMART(Alias);

typedef enum	MatterIdsEnum { containerId = MATTER_CLASS,
    			aggregateId = AGGREGATE_CLASS,
			moleculeId = MOLECULE_CLASS,
			residueId = RESIDUE_CLASS,
			atomId = ATOM_CLASS,
			bondId = BOND_CLASS } MatterId;


class	Loop;

SMART(Bond);
SMART(Atom);
SMART(Residue);
SMART(Molecule);
SMART(Aggregate);
SMART(Matter);





SMART(Matter );
class Matter_O : public core::T_O
{
    friend class ConstitutionAtoms_O;
    LISP_BASE1(core::T_O);
    LISP_CLASS(chem,ChemPkg,Matter_O,"Matter");
#if INIT_TO_FACTORIES
 public:
    static Matter_sp make(string name);
#else
    DECLARE_INIT();
#endif
public:
	void initialize();
public:
	void	archiveBase(core::ArchiveP node);
//	void	serialize(serialize::SNode snode);

friend	class Loop;

public:
typedef	gctools::Vec0<Matter_sp>			MatterVector;
typedef	gctools::Vec0<Matter_sp>::iterator		contentIterator;
typedef	gctools::Vec0<Matter_sp>::const_iterator	const_contentIterator;
protected:
	int			_NextContentId;
	int			_Id;
	int			_TempFileId;	//!< Use to define temporary index while reading/writing to non XML formats
    MatterName			name;
	Matter_sp		containerContainedBy;
	MatterVector		_contents;	// KEEP THIS as a vector
						// A lot depends on residues
						// maintaining an identical
						// order of atoms
						// Through the database
						// building process
		/*! Store Symbol keyed properties of matter
		 */
    core::Cons_sp   	_Properties;
private:
	/*! Maintain a list of restraints that span this Matter_O object */
	RestraintList_sp	_Restraints;
private:

	/*! Adjust the size of the contents array */
	void resizeContents(int sz);
	/*! Put a child at a particular content index */
	void putMatter( int index, Matter_sp matter );

private:
	void accumulateRestraints(RestraintList_sp allRestraints) const;
public:

	friend class Aggregate_O;
	friend class Molecule_O;
	friend class Residue_O;
	friend class Atom_O;
	friend	Matter_sp Matter();
	friend	Matter_sp Matter(int i);

//	bool	equals(Matter_sp c);

    virtual string __repr__() const;
    virtual char getMatterType() { return MATTER_CLASS; };

	void	setId( int i );
	int	getId()	{ return(this->_Id);}


	/*! Accumulate all of the restraints in this matter and its contents into a single RestraintList */
	RestraintList_sp allRestraints() const;

	void	setTempFileId(int i) {this->_TempFileId = i;};
	int	getTempFileId() { return this->_TempFileId; };

public:
	/*! Return a deep copy of this Matter */
	virtual Matter_sp copy() {_OF(); SUBCLASS_MUST_IMPLEMENT(); };
protected:
	/*! Internally used to create a copy of this Matter but not redirect Atoms to their copies.
	 Use copy->redirectAtoms() to direct the new matter to its copied atoms 
	once all the atoms have been copied.*/
	virtual Matter_sp copyDontRedirectAtoms();
	/*! Redirect atoms to their copies */
	virtual void redirectAtoms();

	/*! Copy the restraints from another object but dont' redirect atoms */
	virtual void copyRestraintsDontRedirectAtoms(Matter_O const* orig);
	/*! Redirect restraint atoms */
	virtual void redirectRestraintAtoms();

public:
	contentIterator begin_contents() { return this->_contents.begin(); };
	contentIterator end_contents() { return this->_contents.end(); };
	const_contentIterator begin_contents() const { return this->_contents.begin(); };
	const_contentIterator end_contents() const { return this->_contents.end(); };

		/*! Transfer the coordinates from an equivalent (equal) Matter.
		 * If the (other) isnt equal then throw an exception.
		 */
	virtual void transferCoordinates(Matter_sp other) {_OF();SUBCLASS_MUST_IMPLEMENT();};

		/*! Return the first atom that matches the alias
		 */
	virtual Atom_sp	aliasAtomOrNil(Alias_sp alias);
		/*! Return the first residue that matches the alias
		 */
	virtual Residue_sp aliasResidueOrNil(Alias_sp alias);

		/*! Return the first atom that matches the alias
		 */
	Atom_sp	aliasAtom(Alias_sp alias);
		/*! Return the first residue that matches the alias
		 */
	Residue_sp aliasResidue(Alias_sp alias);

	void	setAllAtomMasks(int m);
	void	setAtomMasksForAllHeavyAtoms(int m);

	/*! Dump all of the properties associated with this matter */
	string	propertiesAsString() const;
	    
		/*! Remove the property from this Matters property list
		 */
	void	clearProperty(core::Symbol_sp propertySymbol);
		/*! Define/set the value of the property.
		 * If it already exists it is overwritten.
		 */
	void	setProperty(core::Symbol_sp propertySymbol, core::T_sp value);
		/*! Define/set the value of the property to TRUE.
		 * If it already exists it is overwritten.
		 */
	void	setPropertyTrue(core::Symbol_sp propertySymbol);
		/*! Return the value of the property.
		 * Throw an exception if the property isn't defined.
		 */
	core::T_sp getProperty(core::Symbol_sp propertySymbol );
		/*! Return the value of the property or the
		 * default if it isn't defined.
		 */
	core::T_sp getPropertyOrDefault(core::Symbol_sp propertySymbol, core::T_sp defVal );
		/*! Return true if the property exists.
		 */
	bool	hasProperty(core::Symbol_sp propertySymbol );

	virtual uint	numberOfAtoms() {_OF(); SUBCLASS_MUST_IMPLEMENT();};

//	void	setContainedBy(Matter_wp p){this->containerContainedBy= p;};
//	void	setContainedBy(Matter_sp p){this->containerContainedBy= (Matter_wp)p;};
	void	setContainedBy(Matter_sp p){this->containerContainedBy= p;};
    void	setContainedByNothing(){this->containerContainedBy = _Nil<Matter_O>(); };
		// Check containedByValid before touching containedBy
    bool		containedByValid() const {return (this->containerContainedBy.lock().pointerp()); };
	Matter_sp	containedBy() const	{_G();ASSERTNOTNULL(this->containerContainedBy);return this->containerContainedBy;};
	Matter_sp	containedByLock()	{_G();ASSERTNOTNULL(this->containerContainedBy); return this->containerContainedBy.lock(); };
	Matter_sp	containedByLock_const() const	{_G();ASSERTNOTNULL(this->containerContainedBy); return this->containerContainedBy.lock(); };
	bool		isContainedBy(Matter_sp matter);

	MatterVector&	getContents()	{return(this->_contents);};
	void	eraseContents(); // Empty the contents vector, don't free the
				 // memory.
	contentIterator eraseContent(contentIterator x) {return this->_contents.erase(x);};

	virtual void	makeAllAtomNamesInEachResidueUnique();
	virtual void	fillInImplicitHydrogensOnCarbon();
	virtual void	randomizeAtomPositions();
	virtual void	perturbAtomPositions(double dist);



	void	setName(MatterName sName) { this->name = sName; };
    MatterName getName() const { return this->name; };
    MatterName getName_notConst() { return this->name; };

	virtual void	addMatter( Matter_sp child );
	void	addMatterRetainId( Matter_sp child );
	void	removeMatter( Matter_sp child );

	Atom_sp		firstAtomWithName(MatterName name);

	bool		hasContentWithName(MatterName sName);
	Matter_sp	contentWithName(MatterName sName);
	Matter_sp	contentWithNameOrNil(MatterName sName);
	int		contentIndexWithName(MatterName sName);

	Matter_sp	contentWithId( int lid );
	bool		hasContentWithId( int lid );
	int		contentIndexWithId( int lid );

	core::Cons_sp		contentsAsCons();
	core::Cons_sp		contents() { return this->contentsAsCons();};

	virtual bool	isAggregate() { return false;}
	virtual bool	isMolecule() { return false;}
	virtual bool	isResidue() {return false;}
	virtual bool 	isAtom() { return false;}


		/*!
		 * Connect all atoms that are within 2A of each other
		 * that aren't already bonded to each other
		 */
	void connectAllCloseAtoms();

	/*! Add a restraint to the matter */
	virtual void addRestraint(Restraint_sp restraint);

		/*! Invert the structure and any chiral or dihedral restraints
		 */
	virtual void	invertStructureAndRestraints();

	Matter_sp	contentWithStorageId( int lid );
	bool		hasContentWithStorageId( int lid );

    core::Vector_sp	allAtoms();
    core::Vector_sp	allAtomsOfElement(Element element);
	core::Cons_sp 		allAtomsOfElementAsList(Element element);

	void calculateVirtualAtomPositions();


//	void	set_StorageId(int sid) { this->_StorageId = sid; };
//	int	get_StorageId() { return this->_StorageId; };

	int		contentIndex( Matter_sp c);
	Matter_sp	contentAt( int i ) { return this->_contents[i]; };
	int		contentSize( ) { return this->_contents.size(); };

	void	translateAllAtoms(Vector3 v);

	void	setAtomAliasesForResiduesNamed(core::Cons_sp residueAliasAtoms, core::Cons_sp atomAliases );


virtual void	applyTransformToAtoms( const Matrix& m );
virtual bool	testConsistancy(Matter_sp c);
virtual	string	subMatter() {_OF(); SUBCLASS_MUST_IMPLEMENT(); };
virtual	int	totalNetResidueCharge();

virtual	string	description() const { stringstream ss; ss << "container("<<this->name<<")@"<<std::hex<<this<<std::dec; return ss.str();}
Vector3		geometricCenter();
		/*! Return a bounding box for the matter padded with (pad).
		 */
	geom::BoundingBox_sp	boundingBox(double pad);

	void		reparent(Matter_sp c);

#ifdef RENDER
	geom::Render_sp rendered(core::Cons_sp kopts);
#endif

	/*! Return all atoms as a Cons of atoms. */
	core::Cons_sp allAtomsAsCons(bool allowVirtualAtoms) const;

	/*! Return all bonds as Cons of Cons (each entry is a Bond object) */
	core::Cons_sp allBondsAsCons(bool allowVirtualAtoms) const;

	/*! Return all angles as Cons of Cons (each entry is an Angle) */
	core::Cons_sp allAnglesAsCons(bool allowVirtualAtoms) const;

	/*! Return all proper torsions as Cons of Cons (each entry is ProperTorsion) */
	core::Cons_sp allProperTorsionsAsCons(bool allowVirtualAtoms) const;

	/*! Return all improper torsions as Cons of Cons (each entry is an ImproperTorsion ) */
	core::Cons_sp allImproperTorsionsAsCons(bool allowVirtualAtoms) const;

	/*! Build a map of AtomIds to Atoms */
	virtual AtomIdToAtomMap_sp buildAtomIdMap() const;

	/*! Return the atom with the AtomId */
	virtual Atom_sp atomWithAtomId(AtomId_sp atomId) const;


	/*! Invert the stereochemistry of my immediate restraints */
	void invertStereochemistryOfRestraints();

	Matter_O(const Matter_O& c ); // copy constructor
	DEFAULT_CTOR_DTOR(Matter_O);
};

};
TRANSLATE(chem::Matter_O);
#endif