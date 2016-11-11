/*
 *  archiving_node.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file archiving_node.h
 * Definition of Archiving_Node which is capable of
 * recording and managing a spike history.
 * \author Moritz Helias, Abigail Morrison
 * \date april 2006
 */

#ifndef LOOKBACK_INTERFACE_H
#define LOOKBACK_INTERFACE_H

#include <vector>
#include <set>
#include <string>
#include <sstream>

#include "exceptions.h"
#include "connector_model.h"
#include "node.h"

namespace nest
{

template <typename ConnectionT> class LookBackConnectorModel;

template <typename ConnectionT>
class LookBackNode {
private:

  /// \brief This function adds a pointer to an incoming synapse, to the container of
  ///        lookback synapses
  ///
  /// \details
  /// \param new_syn The new pointer to insert
  ///
  inline void add_inc_synapse(ConnectionT* new_syn);

  /// \brief This function replaces an existing pointer to an incoming synapse with a new
  ///        one to an incoming synapse.
  ///
  /// \details
  /// \param old_syn The old pointer to the synapse
  /// \param new_syn The new pointer with which to replace
  ///
  /// This function is called by `LookBackConnectorModel<ConnectionT>::add_connection()`
  /// if it so happens that the synapse that was being pointed to by a specific pointer
  /// has been reallocated to a different address. The function then takes this new address
  /// and replaces the old address with the new address.
  inline void replace_inc_synapse(ConnectionT* old_syn, ConnectionT* new_syn);

  /// `std::set` used to store the pointers to incoming synapses
  std::set<ConnectionT*> incoming_syn_ptr_set;

public:

  /// \brief Virtual Destructor to make class contain vtable
  ///
  /// \details This function exists for the sole purpose of making this class a virtual base
  ///     class so that RTTI can be performed. Yeah Yeah, It's a poor design decision
  ///     yada yada Fact is, this works, and allows for some real neat type checking
  ///     (via side-casting from Node) to ensure that LookBackConnectorModel<ConnectionT>
  ///     only connects to a node model which inherits from LookBackNode<ConnectionT>
  ///     thus preventing silly mistakes from creating nasty seg-faults
  ///
  virtual ~LookBackNode() {};

  // This is so that LookBackConnectorModel<ConnectionT> can access the functions
  // add_inc_synapse and replace_inc_synapse (that are crated specifically for its
  // use)
  friend class LookBackConnectorModel<ConnectionT>;

  LookBackNode() : incoming_syn_ptr_set() {}

  /// Get a `const_iterator` to the beginning of the set of incoming synapses
  typename std::set<ConnectionT*>::const_iterator get_inc_syn_begin();

  /// Get a `const_iterator` to the end of the set of incoming synapses
  typename std::set<ConnectionT*>::const_iterator get_inc_syn_end();
}; // of LookBackNode


} // of namespace

#endif
