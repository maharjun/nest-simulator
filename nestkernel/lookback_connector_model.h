#ifndef LOOKBACK_CONNECTOR_MODEL_H
#define LOOKBACK_CONNECTOR_MODEL_H

#include "connector_model.h"

#include <vector>

namespace nest
{

template <typename ConnectionT>
class LookBackConnectorModel : public GenericConnectorModel<ConnectionT> {
public:
  LookBackConnectorModel( const std::string name,
                          bool is_primary,
                          bool has_delay )
      : GenericConnectorModel( name, is_primary, has_delay )
  {
  }

  LookBackConnectorModel( const LookBackConnectorModel& cm,
                          const std::string name )
      : GenericConnectorModel( cm, name )
  {
  }

private:
  void get_conn_ptrs(ConnectorBase * conn_base_in, index synid, std::vector<ConnectionT*> &conn_ptrs_out);
  void update_conn_ptrs(const std::vector<ConnectionT*> &old_conn_ptrs, const std::vector<ConnectionT*> &new_conn_ptrs);
  LookBackNode<ConnectionT>* get_validated_neuron(Node *node_in);

public:
  ConnectorBase* add_connection( Node& src,
                                 Node& tgt,
                                 ConnectorBase* conn,
                                 synindex syn_id,
                                 double delay,
                                 double weight);
  ConnectorBase* add_connection( Node& src,
                                 Node& tgt,
                                 ConnectorBase* conn,
                                 synindex syn_id,
                                 DictionaryDatum& d,
                                 double delay,
                                 double weight );
};

}

#endif