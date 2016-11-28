
#include <algorithm>

#include <dictdatum.h>

#include "nest_time.h"
#include "norm_controller.h"
#include "kernel_manager.h"
#include "logging.h"
#include "event_delivery_manager_impl.h"

void nest::norm_controller::calibrate() {
  /**
   * Set the current_norm_instant_ind_
   */
  Time CurrentTime = kernel().simulation_manager.get_time();

  current_norm_instant_ind_ =
      std::lower_bound(norm_instants_.begin(), norm_instants_.end(),
                       CurrentTime.get_steps())
      - norm_instants_.begin();

}

void nest::norm_controller::update(const Time &origin, const long from, const long to) {

  if(is_active) {
    while (current_norm_instant_ind_ < (long)norm_instants_.size() && origin.get_steps() + to > norm_instants_[current_norm_instant_ind_]) {
      long eventlag = norm_instants_[current_norm_instant_ind_] - origin.get_steps();
      NormEvent ne;
      ne.set_instruction(norm_instruction);
      kernel().event_delivery_manager.send( *this, ne, eventlag);

      current_norm_instant_ind_++;
    }
  }
}

bool nest::norm_controller::has_proxies() const {
  return false;
}

bool nest::norm_controller::local_receiver() const {
  return true;
}

void nest::norm_controller::set_status(const DictionaryDatum &datum) {

  // update is_active
  bool new_is_active=is_active;
  updateValue< bool >(datum, "is_active", new_is_active);

  // Creating Backup of norm_instants_
  std::vector<long> norm_instants_tmp_;
  norm_instants_.swap(norm_instants_tmp_);

  // Retrieving vector from Python
  std::vector<double> norm_instants_in_ms;
  bool is_norm_instants_set = updateValue< std::vector<double> >(datum, "norm_instants", norm_instants_in_ms);

  if (!new_is_active && is_norm_instants_set) {
    LOG(M_WARNING,
        "norm_controller::set_status",
        "You are setting the normalization instants of a norm_controller that is not active");
  }
  // Filling norm_instants
  norm_instants_.resize(norm_instants_in_ms.size());
  for(unsigned long i = 0; i < norm_instants_in_ms.size(); ++i) {
    norm_instants_[i] = static_cast<unsigned long>(Time::delay_ms_to_steps(norm_instants_in_ms[i]));
  }

  // Validating norm_instants (will throw exception if false)
  try {
    validate_norm_instants();
  }
  catch (BadProperty E) {
    norm_instants_.swap(norm_instants_tmp_);
    throw E;
  }
  catch (KernelException E) {
    norm_instants_.swap(norm_instants_tmp_);
    throw E;
  }

  is_active = new_is_active;
  updateValue<long>(datum, "norm_instr", norm_instruction);
}

void nest::norm_controller::get_status(DictionaryDatum &datum) const {

  std::vector<double> norm_instants_in_ms(norm_instants_.size());

  for(unsigned long i = 0; i < norm_instants_.size(); ++i) {
    norm_instants_in_ms[i] = Time::delay_steps_to_ms(norm_instants_[i]);
  }

  def< std::vector<double> >(datum, "norm_instants", norm_instants_in_ms);
  def< long >(datum, "norm_instr", norm_instruction);
  def< bool >(datum, "is_active", is_active);
}

void nest::norm_controller::init_state_(const Node &node) {
  /**
   * Empty norm_instants_ and reset
   */

  norm_controller proto = downcast< norm_controller >(node);
  norm_instants_ = proto.norm_instants_;
  norm_instruction = proto.norm_instruction;
  is_active = proto.is_active;
  current_norm_instant_ind_ = 0;
}

void nest::norm_controller::init_buffers_() {
  /**
   * Do exactly nothing
   */
}


