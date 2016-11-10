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
    inline void add_inc_synapse(ConnectionT* new_syn);
    inline void replace_inc_synapse(ConnectionT* old_syn, ConnectionT* new_syn);
    std::set<ConnectionT*> incoming_syn_ptr_set;

    // This function exists for the sole purpose of making this class a virtual class
    // so that RTTI can be performed
    virtual void DoNothingFunction() {}
public:
    friend class LookBackConnectorModel<ConnectionT>;

    LookBackNode();

    typename std::set<ConnectionT*>::const_iterator get_inc_syn_begin();
    typename std::set<ConnectionT*>::const_iterator get_inc_syn_end();
}; // of LookBackNode


} // of namespace

#endif
