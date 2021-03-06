
// Marcel Timm, RhinoDevel, 2019sep12

#include "cbm_send.h"

#include "../config.h"
#include "../../lib/alloc/alloc.h"
#include "../../lib/str/str.h"
#include "../../lib/petasc/petasc.h"
#include "../tape/tape_send_params.h"
#include "../tape/tape_filetype.h"
#include "../tape/tape_send.h"
#include "../tape/tape_input.h"

#ifndef NDEBUG
    #include "../../lib/console/console.h"
#endif //NDEBUG

#include <stdint.h>
#include <stdbool.h>

void cbm_send_fill_name(
    uint8_t * const name_out, char const * const name_in)
{
    // static uint8_t const sample_name[] = {
    //     'R', 'H', 'I', 'N', 'O', 'D', 'E', 'V', 'E', 'L',
    //     0x20, 0x20, 0x20, 0x20, 0x20, 0x20
    // };

    int i = 0;
    char * const buf = alloc_alloc(str_get_len(name_in) + 1);

    str_to_upper(buf, name_in);

    while(i < MT_TAPE_INPUT_NAME_LEN && buf[i] != '\0')
    {
        name_out[i] = (uint8_t)petasc_get_petscii(buf[i], MT_PETSCII_REPLACER);
        ++i;
    }
    while(i < MT_TAPE_INPUT_NAME_LEN)
    {
        name_out[i] = 0x20;
        ++i;
    }

    alloc_free(buf);
}

bool cbm_send_data(
    struct tape_input * const data, bool (*is_stop_requested)())
{
    bool ret_val = false;
    struct tape_send_params p;
    uint32_t * const mem_addr = alloc_alloc(4 * 1024 * 1024); // Hard-coded

    p.is_stop_requested = is_stop_requested;
    p.gpio_pin_nr_read = MT_TAPE_GPIO_PIN_NR_READ;
    p.gpio_pin_nr_sense = MT_TAPE_GPIO_PIN_NR_SENSE;
    p.gpio_pin_nr_motor = MT_TAPE_GPIO_PIN_NR_MOTOR;
    p.data = data;

    ret_val = tape_send(&p, mem_addr);

    alloc_free(mem_addr);
    return ret_val;
}

bool cbm_send(
    uint8_t /*const*/ * const bytes,
    char const * const name,
    uint32_t const count,
    bool (*is_stop_requested)())
{
    bool ret_val = false;
    struct tape_input * const data = alloc_alloc(sizeof *data);

    cbm_send_fill_name(data->name, name);

    // First two bytes hold the start address:
    //
    data->addr = (((uint16_t)bytes[1]) << 8) | (uint16_t)bytes[0];

    // Hard-coded for PET PRG files. C64 (and other) machines need to load
    // PRGs that are not starting at BASIC start address / are not relocatable
    // as non-relocatable because of this (e.g.: LOAD"",1,1 on C64):
    //
    data->type = tape_filetype_relocatable;

    data->bytes = bytes + 2;
    data->len = count - 2;

    tape_input_fill_add_bytes(data->add_bytes);

    ret_val = cbm_send_data(data, is_stop_requested);

    alloc_free(data);
    return ret_val;
}
