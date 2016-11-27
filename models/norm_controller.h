#ifndef NORM_CONTROLLER_H

#define NORM_CONTROLLER_H

#include "node.h"
#include "nest_time.h"
#include "nest_types.h"
#include "stimulating_device.h"

#include <vector>
#include <algorithm>

namespace nest {

class norm_controller: public Node {

  /**
   * This is the vector of ntime instants where a normalization event needs
   * to be triggered
   */
  std::vector<long> norm_instants_;

  /**
   * This is the index of the element in norm_instants which will
   * be the next time to trigger a normalization event
   */
  long current_norm_instant_ind_;

  /**
   * This is the value assigned to the instruction parameter of the
   * normalization event that is generated
   */
  long norm_instruction;

  /**
   * This is the boolean flag that controls whether or not the norm_controller
   * will bother sending events
   */
  bool is_active;
public:

  norm_controller():
      Node(),
      current_norm_instant_ind_(0),
      norm_instruction(0),
      is_active(true) {}

  port send_test_event( Node&, rport, synindex, bool );

  virtual bool has_proxies() const;
  virtual bool local_receiver() const override;

  virtual void set_status(const DictionaryDatum &datum);
  virtual void get_status(DictionaryDatum &datum) const;

protected:

  virtual void calibrate();

  virtual void update(Time const &origin, const long from, const long to);

  virtual void init_state_(Node const &node);

  virtual void init_buffers_();

private:

  inline void validate_norm_instants();
  StimulatingDevice< NormEvent > device_;
};

inline port
norm_controller::send_test_event( Node& target,
                               rport receptor_type,
                               synindex syn_id,
                               bool )
{
  device_.enforce_single_syn_type( syn_id );

  NormEvent e;
  e.set_sender( *this );
  port rport_ret = target.handles_test_event( e, receptor_type );

  return rport_ret;
}

void norm_controller::validate_norm_instants() {
  /**
   * Several conditions are checked by this function
   *
   * 1.  That None of the elements are < 0
   * 2.  That the elements are in STRICTLY Ascending order
   */

  bool has_negative = std::any_of(norm_instants_.begin(), norm_instants_.end(), [=](const long &a) {return a < 0;});
  if (has_negative) {
    throw BadProperty("norm_instants cannot contain negative time instants");
  }

  bool is_strictly_ascending = std::is_sorted(norm_instants_.begin(),
                                              norm_instants_.end(),
                                              [=](const long &a, const long &b)->bool {
                                                return a <= b;
                                              });
  if (!is_strictly_ascending) {
    throw BadProperty("norm_instants must be in ascending order");
  }
}

}

#endif