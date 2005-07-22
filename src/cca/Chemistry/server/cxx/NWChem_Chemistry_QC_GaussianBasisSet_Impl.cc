// 
// File:          NWChem_Chemistry_QC_GaussianBasisSet_Impl.cc
// Symbol:        NWChem.Chemistry_QC_GaussianBasisSet-v0.4
// Symbol Type:   class
// Babel Version: 0.10.2
// Description:   Server-side implementation for NWChem.Chemistry_QC_GaussianBasisSet
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.2
// 
#include "NWChem_Chemistry_QC_GaussianBasisSet_Impl.hh"

// DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet._includes)
// Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet._includes} (additional includes or code)
// DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet._includes)

// user-defined constructor.
void NWChem::Chemistry_QC_GaussianBasisSet_impl::_ctor() {
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet._ctor)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet._ctor} (constructor)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet._ctor)
}

// user-defined destructor.
void NWChem::Chemistry_QC_GaussianBasisSet_impl::_dtor() {
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet._dtor)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet._dtor} (destructor)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet._dtor)
}

// static class initializer.
void NWChem::Chemistry_QC_GaussianBasisSet_impl::_load() {
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet._load)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet._load} (class initialization)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet._load)
}

// user-defined static methods: (none)

// user-defined non-static methods:
/**
 * Get the user specified name.
 * @return User specified name. 
 */
::std::string
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_label ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_label)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_label} (get_label method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_label)
}

/**
 * Get the number of basis functions.
 * @return Number of basis functions. 
 */
int64_t
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_n_basis ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_n_basis)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_n_basis} (get_n_basis method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_n_basis)
}

/**
 * Get the number of shells.
 * @return Number of shells. 
 */
int64_t
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_n_shell ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_n_shell)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_n_shell} (get_n_shell method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_n_shell)
}

/**
 * Get the max angular momentum for any contraction in the basis set.
 * @return Max angular momentum value. 
 */
int64_t
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_max_angular_momentum ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_max_angular_momentum)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_max_angular_momentum} (get_max_angular_momentum method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_max_angular_momentum)
}

/**
 * Get the angular type.
 * @return enum AngularType {CARTESIAN,SPHERICAL,MIXED} 
 */
::Chemistry::QC::GaussianBasis::AngularType
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_angular_type ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_angular_type)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_angular_type} (get_angular_type method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_angular_type)
}

/**
 * Get an atomic basis set.
 * @param atomnum Atom number. 
 * @return Atomic basis set. 
 */
::Chemistry::QC::GaussianBasis::Atomic
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_atomic (
  /* in */ int64_t atomnum ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_atomic)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_atomic} (get_atomic method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_atomic)
}

/**
 * Get the molecule.
 * @return The molecule. 
 */
::Chemistry::Molecule
NWChem::Chemistry_QC_GaussianBasisSet_impl::get_molecule ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.get_molecule)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.get_molecule} (get_molecule method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.get_molecule)
}

/**
 * Print the molecular basis data. 
 */
void
NWChem::Chemistry_QC_GaussianBasisSet_impl::print_molecular ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet.print_molecular)
  // Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet.print_molecular} (print_molecular method)
  // DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet.print_molecular)
}


// DO-NOT-DELETE splicer.begin(NWChem.Chemistry_QC_GaussianBasisSet._misc)
// Insert-Code-Here {NWChem.Chemistry_QC_GaussianBasisSet._misc} (miscellaneous code)
// DO-NOT-DELETE splicer.end(NWChem.Chemistry_QC_GaussianBasisSet._misc)

