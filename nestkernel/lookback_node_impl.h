

#ifndef LOOKBACK_INTERFACE_IMPL_H
#define LOOKBACK_INTERFACE_IMPL_H

#include <iostream>

#include "lookback_node.h"
#include "lookback_exceptions.h"

#include "connection.h"
#include "connector_model_impl.h"

namespace nest {

template <typename ConnectionT>
void LookBackNode<ConnectionT>::add_inc_synapse(ConnectionT *new_syn) {
  // This function makes the following assumptions (same as replace_inc_synapse)
  // which is why it is private
  //
  // Assumptions:
  //
  // 1.  new_syn doesnt exists in the set and is an actual pointer
  // 2.  new_syn actually points to a valid memory location containing a
  //     connection object of type ConnectionT

  incoming_syn_ptr_set_.insert(new_syn);
}

template <typename ConnectionT>
void LookBackNode<ConnectionT>::replace_inc_synapse(ConnectionT *old_syn, ConnectionT *new_syn) {
  // This function makes the following assumptions (same as add_inc_synapse) which
  // is why it is private.
  //
  // Assumptions:
  //
  // 1.  new_syn doesnt exists in the set and is an actual pointer
  // 2.  new_syn actually points to a valid memory location containing a
  //     connection object of type ConnectionT

  auto old_syn_iter = incoming_syn_ptr_set_.find(old_syn);

  if (old_syn_iter != incoming_syn_ptr_set_.end()) {
    incoming_syn_ptr_set_.erase(old_syn);
    incoming_syn_ptr_set_.insert(new_syn);
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

}

#endif