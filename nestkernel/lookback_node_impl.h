

#ifndef LOOKBACK_INTERFACE_IMPL_H
#define LOOKBACK_INTERFACE_IMPL_H

#include "lookback_node.h"
#include "lookback_exceptions.h"
#include "target_identifier.h"

#include "../models/stdp_connection.h"
#include "connection.h"

namespace nest {

template <typename ConnectionT>
void LookBackNode::add_inc_synapse(ConnectionT *new_syn) {
    incoming_syn_ptr_set.insert(new_syn);
}

template <typename ConnectionT>
void LookBackNode::replace_inc_synapse(ConnectionT *old_syn, ConnectionT *new_syn) {

    auto old_syn_iter = incoming_syn_ptr_set.find(old_syn);

    if (old_syn_iter != incoming_syn_ptr_set.end()) {
        incoming_syn_ptr_set.erase(old_syn);
        incoming_syn_ptr_set.insert(new_syn);
    }
    else {
        Node *ThisNode = dynamic_cast<Node*>(this);
        if (ThisNode) {
            throw InvalidSynapseReplacement(ThisNode->get_gid());
        }
        else {
            throw KernelException("Invalid Incoming Synapse Pointer Replacement (Cannot ascertain neuron)");
        }
    }
}

template <typename ConnectionT>
std::set<ConnectionT*>::const_iterator LookBackNode::get_inc_syn_begin() {
    return incoming_syn_ptr_set.begin();
}

template <typename ConnectionT>
std::set<ConnectionT*>::const_iterator LookBackNode::get_inc_syn_end() {
    return incoming_syn_ptr_set.end();
}


}

#endif