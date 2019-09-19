#include "quantum.h"

#include "debounce.h"

// #define NO_MATRIX_OUTPUT

#if (!defined(SHIFTREG_MATRIX_ROW_LATCH) || !defined(SHIFTREG_MATRIX_ROW_CLK) || !defined(SHIFTREG_MATRIX_ROW_DATA) || !defined(SHIFTREG_MATRIX_COL_LATCH) || !defined(SHIFTREG_MATRIX_COL_CLK) || !defined(SHIFTREG_MATRIX_COL_DATA))
#    error Missing shift register I/O pin definitions
#endif

// The nuber of loops we want to do for a small delay between toggling I/O pins
#ifndef SHIFTREG_MATRIX_DELAY_LOOPS
#    define SHIFTREG_MATRIX_DELAY_LOOPS 1
#endif

#define smalldelay()                                            \
    do {                                                        \
        for (int i = 0; i < SHIFTREG_MATRIX_DELAY_LOOPS; ++i) { \
            __asm__ volatile("nop\n\t"                          \
                             "nop\n\t"                          \
                             "nop\n\t");                        \
        }                                                       \
    } while (0)

#define pin_output_init(pin, init) \
    do {                           \
        setPinOutput(pin);         \
        if (init) {                \
            writePinHigh(pin);     \
        } else {                   \
            writePinLow(pin);      \
        }                          \
    } while (0)

#define pin_input(pin)    \
    do {                  \
        setPinInput(pin); \
    } while (0)

#define pin_read(pin) readPin(pin)

#define pin_high(pin)      \
    do {                   \
        writePinHigh(pin); \
    } while (0)

#define pin_low(pin)      \
    do {                  \
        writePinLow(pin); \
    } while (0)

#define delayed_pin_high(pin) \
    do {                      \
        smalldelay();         \
        pin_high(pin);        \
    } while (0)

#define delayed_pin_low(pin) \
    do {                     \
        smalldelay();        \
        pin_low(pin);        \
    } while (0)

static inline int delayed_pin_read(int pin) {
    smalldelay();
    return pin_read(pin);
}

#define delayed_pin_pulse_high(pin) \
    do {                            \
        delayed_pin_high(pin);      \
        delayed_pin_low(pin);       \
    } while (0)

#define delayed_pin_pulse_low(pin) \
    do {                           \
        delayed_pin_low(pin);      \
        delayed_pin_high(pin);     \
    } while (0)

#define clock_output_register()                          \
    do {                                                 \
        delayed_pin_pulse_high(SHIFTREG_MATRIX_ROW_CLK); \
    } while (0)

#define latch_output_register()                            \
    do {                                                   \
        delayed_pin_pulse_high(SHIFTREG_MATRIX_ROW_LATCH); \
    } while (0)

#define clock_input_register()                           \
    do {                                                 \
        delayed_pin_pulse_high(SHIFTREG_MATRIX_COL_CLK); \
    } while (0)

#define latch_input_register()                            \
    do {                                                  \
        delayed_pin_pulse_low(SHIFTREG_MATRIX_COL_LATCH); \
    } while (0)

static matrix_row_t raw_matrix[MATRIX_ROWS];
static matrix_row_t matrix[MATRIX_ROWS];

static bool is_modified = false;

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

inline uint8_t matrix_rows(void) { return MATRIX_ROWS; }

inline uint8_t matrix_cols(void) { return MATRIX_COLS; }

void matrix_init(void) {
    // Clear the stored matrix
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0x00;
    }

    pin_output_init(SHIFTREG_MATRIX_ROW_LATCH, 0);
    pin_output_init(SHIFTREG_MATRIX_ROW_CLK, 0);
    pin_output_init(SHIFTREG_MATRIX_ROW_DATA, 0);

    pin_output_init(SHIFTREG_MATRIX_COL_LATCH, 1);
    pin_output_init(SHIFTREG_MATRIX_COL_CLK, 0);
    pin_input(SHIFTREG_MATRIX_COL_DATA);

    // Clear the clock/latch pins for the shift registers
    pin_low(SHIFTREG_MATRIX_ROW_LATCH);
    pin_low(SHIFTREG_MATRIX_ROW_CLK);
    pin_high(SHIFTREG_MATRIX_COL_LATCH);
    pin_low(SHIFTREG_MATRIX_COL_CLK);

    debounce_init(MATRIX_ROWS);

    matrix_init_quantum();

    // Write zeros to the output shift register
    pin_low(SHIFTREG_MATRIX_ROW_DATA);
    for (uint8_t i = 0; i < MATRIX_ROWS; ++i) {
        clock_output_register();
    }

    latch_output_register();
}

uint8_t matrix_scan(void) {
    // Keep track of if something was modified
    is_modified = false;

#ifndef NO_MATRIX_OUTPUT
    // Write zeros to the output shift register
    pin_low(SHIFTREG_MATRIX_ROW_DATA);
    for (uint8_t i = 0; i < MATRIX_ROWS - 1; ++i) {
        clock_output_register();
    }

    // Clock a single '1' on the row driver to enable the first row
    delayed_pin_high(SHIFTREG_MATRIX_ROW_DATA);
    clock_output_register();
    delayed_pin_low(SHIFTREG_MATRIX_ROW_DATA);
    latch_output_register();

    for (int r = 0; r < MATRIX_ROWS; ++r) {
        matrix_row_t row_rx = 0;

        // Latching a '165 puts D7 onto Q7, no need to clock initially
        latch_input_register();
        for (int c = 0; c < MATRIX_COLS; ++c) {
            row_rx |= (delayed_pin_read(SHIFTREG_MATRIX_COL_DATA) ? 1 : 0) << c;
            clock_input_register();
        }

        // Shift up the output shift register now that the reads have completed (i.e. all zero)
        clock_output_register();
        latch_output_register();

        // Check for modifications and copy to the matrix
        if (raw_matrix[r] != row_rx) {
            is_modified   = true;
            raw_matrix[r] = row_rx;
        }
    }
#endif  // NO_MATRIX_OUTPUT

    debounce(raw_matrix, matrix, MATRIX_ROWS, is_modified);

    matrix_scan_quantum();

    return is_modified;
}

bool matrix_is_modified(void) {
    if (debounce_active()) return false;
    return is_modified;
}

inline bool matrix_has_ghost(void) { return false; }

bool matrix_is_on(uint8_t row, uint8_t col) { return matrix[row] & (1 << col); }

matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }

void matrix_print(void) {
    print("\nr/c");
    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
        if (col % 8 == 0) {
            print(" ");
        }
        xprintf("%c", "0123456789ABCDEF"[col % 16]);
    }
    print("\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        xprintf("%2d:", row + 1);
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            if (col % 8 == 0) {
                xprintf(" ");
            }
            xprintf("%c", matrix_is_on(row, col) ? '#' : '.');
        }
        print("\n");
    }
}

uint8_t matrix_key_count(void) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop(matrix[i]);
    }
    return count;
}
