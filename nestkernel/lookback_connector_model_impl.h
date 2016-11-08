#ifndef LOOKBACK_CONNECTOR_MODEL_IMPL
#define LOOKBACK_CONNECTOR_MODEL_IMPL

#include "lookback_connector_model.h"
#include "connector_model_impl.h"

#include <vector>

namespace nest
{
///////////////////////////////////////////////////////////////////////////
// LOOK BACK CONNECTOR MODEL //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

template <typename ConnectionT>
void LookBackConnectorModel::get_conn_ptrs(ConnectorBase *conn_base_in,
                                           index syn_id,
                                           std::vector<ConnectionT *> &conn_ptrs_out) {

  // Clear Empty the current connection pointers
  conn_ptrs_out.clear();

  // Extract information from 2 LSBs
  conn_base_in = validate_pointer(conn_base_in);

  if (conn_base_in->homogeneous_model()) {
    // This is the case where the ConnectorBase is a homogenous connector
    // Thus vector_like<ConnectionT> is a valid derived class to cast to.
    // Moreover, it is assumed that the synapse type of these synapses are
    // identical to the type represented by LookBackConnectorModel<ConnectionT>

    auto hom_conn_base = static_cast<vector_like<ConnectionT>* >(conn_base_in);
    for(size_t i=0; i< hom_conn_base->size(); ++i) {
      conn_ptrs_out.push_back( &(hom_conn_base->at(i)) );
    }
  }
  else {
    // This is the case where the ConnectorBase is a heterogenous connector
    // Hence, we need to iterate through the different types and search for
    // the connector with the synid matching the given synid.
    //
    // Note that the synid passed to this function represents the type of
    // the synapse inserted in add_connection, which is expected to refer to
    // the type of LookBackConnectorModel<ConnectionT>

    auto het_conn_base = static_cast<HetConnector*>(conn_base_in);
    vector_like<ConnectionT>* relevant_hom_conn_base = NULL;

    for(size_t i=0; i < het_conn_base->size(); ++i) {
      if (het_conn_base->at(i)->get_syn_id() == syn_id) {
        relevant_hom_conn_base = static_cast< vector_like<ConnectionT>* >(het_conn_base->at(i));
        break;
      }
    }

    // If there is a homogenous connector of the right type, then add the relevant pointers into the vector
    if(relevant_hom_conn_base) {
      for(size_t i=0; i < relevant_hom_conn_base->size(); ++i) {
        conn_ptrs_out.push_back( &(relevant_hom_conn_base->at(i)) );
      }
    }
  }
}

template <typename  ConnectionT>
void LookBackConnectorModel::update_conn_ptrs(const std::vector<ConnectionT *> &old_conn_ptrs,
                                              const std::vector<ConnectionT *> &new_conn_ptrs) {
  // In this function, we make use of the new pointers to determine the targets
  // to which these pointers belong. Then we perform a dynamic cast to ensure
  // that the Neuron Model is a derived class of

  // We assume that synapses are only added (ASSUMPTION) and that this function
  // is only called by add_connection after the addition of a connection. Hence
  // we will assume that all synapses in new_conn_ptrs but the last one corres-
  // pond to the synapses that were originally pointed to by the pointers in
  // old_conn_ptrs. And that the last element is the new synapse.

  // The above also means that the neuron need not be validated at this point as
  // the validatoin would already have been performed before calling add_connection

  assert (old_conn_ptrs.size() + 1 == new_conn_ptrs.size());

  if (!(old_conn_ptrs.empty() && new_conn_ptrs.empty()
        || old_conn_ptrs[0] == new_conn_ptrs[0])) {
    // First, we check if the two vectors are identical i.e. there were no
    // reallocations. This is done by checking if they are either empty or have
    // identical first elements. This is sufficient because the pointers refer
    // to synapses in the same homogenous connector and thus, are by design
    // sequential (ASSUMPTION).

    // in case they are not, we need to replace the relevant synapses across all
    // the neurons to whom these synapses point. As far as therad safety goes I
    // don't think there is a need to worry as by definition of connection_manager.connections_,
    // all the connections here point only to neurons that are accessed on the
    // same thread as the current one.

    for (auto old_iter=old_conn_ptrs.begin(), new_iter=new_conn_ptrs.end();
         old_iter != old_conn_ptrs.end();
         ++old_iter, ++new_iter) {

      // It is assumed that this will work without segfaulting (See above note
      // regarding validation of node)
      auto target_node = dynamic_cast< LookBackNode<ConnectionT>* >(
          // Get the Target of the connection that is pointed to by the (element
          // that is pointed to by new_iter)
          (*new_iter)->get_target(kernel().vp_manager.get_thread_id())
      );

      target_node->replace_inc_synapse(*old_iter, *new_iter);
    }
  }

  // Now, having assumed that the last element of new_conn_ptrs is the new connection, we
  // will perform the relevant addition to the set

  auto val_target_node = dynamic_cast< LookBackNode<ConnectionT>* >(
      // Get the Target of the connection that is pointed to by the last
      // element of new_conn_ptr
      new_conn_ptrs.back()->get_target(kernel().vp_manager.get_thread_id())
  );
  val_target_node->add_inc_synapse(new_conn_ptrs.back());

}


template <typename ConnectionT>
LookBackNode<ConnectionT>* LookBackConnectorModel::get_validated_neuron(Node *node_in) {

  auto val_target_node = dynamic_cast<LookBackNode<ConnectionT>*>(node_in);

  if (!val_target_node) {
    std::ostringstream msg;
    msg << "The node being connected to by a LookBackConnectorModel is not itself derived "
        << "from the appropriate LookBackNode";
    throw IllegalConnection(msg.str());
  }

  return val_target_node;
}


template <typename ConnectionT>
ConnectorBase* LookBackConnectorModel::add_connection(Node &src, Node &tgt, ConnectorBase *conn, synindex syn_id,
                                                      DictionaryDatum &d, double weight, double delay) {

  // First we perform target neuron validation
  get_validated_neuron(&tgt);

  // Then we read out the vector of old pointers to the connections
  std::vector<ConnectionT*> old_conn_ptrs;
  get_conn_ptrs(conn, syn_id, old_conn_ptrs);

  // Then we add synapse by calling super function from GenericConnectorModel
  ConnectorBase* new_conn_base_ptr = GenericConnectorModel<ConnectionT>::add_connection(src, tgt, conn, syn_id, d, weight, delay);

  // Then we read out the vector of new pointers to the connections
  // This includes te newly added synapse
  std::vector<ConnectionT*> new_conn_ptrs;
  get_conn_ptrs(new_conn_base_ptr, syn_id, old_conn_ptrs);

  // Now we update the connection pointers on all the relevant target neurons
  update_conn_ptrs(old_conn_ptrs, new_conn_ptrs);

  return new_conn_base_ptr;
}


template <typename ConnectionT>
ConnectorBase* LookBackConnectorModel::add_connection(Node &src, Node &tgt, ConnectorBase *conn, synindex syn_id,
                                                      double weight, double delay) {

  // First we perform target neuron validation
  get_validated_neuron(&tgt);

  // Then we read out the vector of old pointers to the connections
  std::vector<ConnectionT*> old_conn_ptrs;
  get_conn_ptrs(conn, syn_id, old_conn_ptrs);

  // Then we add synapse by calling super function from GenericConnectorModel
  ConnectorBase* new_conn_base_ptr = GenericConnectorModel<ConnectionT>::add_connection(src, tgt, conn, syn_id, weight, delay);

  // Then we read out the vector of new pointers to the connections
  // This includes te newly added synapse
  std::vector<ConnectionT*> new_conn_ptrs;
  get_conn_ptrs(new_conn_base_ptr, syn_id, old_conn_ptrs);

  // Now we update the connection pointers on all the relevant target neurons
  update_conn_ptrs(old_conn_ptrs, new_conn_ptrs);

  return new_conn_base_ptr;
}

}

#endif