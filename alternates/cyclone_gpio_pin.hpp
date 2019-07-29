template <intptr_t pin_name, int delay_count>
class gpio_pin {
 public:
  static constexpr intptr_t pin = pin_name;

  __forceinline void direction_output(bool initHigh) {
    setPinOutput(pin);
    if (initHigh) {
      drive_high();
    } else {
      drive_low();
    }
  }

  __forceinline void direction_input(void) { setPinInput(pin); }

  __forceinline bool read(void) { return readPin(pin); }

  __forceinline void drive_low(void) { writePinLow(pin); }

  __forceinline void drive_high(void) { writePinHigh(pin); }

  __forceinline void delayed_drive_low(void) {
    smalldelay<delay_count>();
    drive_low();
  }

  __forceinline void delayed_drive_high(void) {
    smalldelay<delay_count>();
    drive_high();
  }

  __forceinline void pulse_low(void) {
    drive_low();
    smalldelay<delay_count>();
    drive_high();
  }

  __forceinline void pulse_high(void) {
    drive_high();
    smalldelay<delay_count>();
    drive_low();
  }

  __forceinline void delayed_pulse_low(void) {
    smalldelay<delay_count>();
    pulse_low();
  }

  __forceinline void delayed_pulse_high(void) {
    smalldelay<delay_count>();
    pulse_high();
  }
};
