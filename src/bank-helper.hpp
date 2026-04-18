#pragma once

#include "common.hpp"
#include <mapper.h>

template <typename Func>
__attribute__((noinline, section(".prg_rom_fixed.text"))) void
banked_lambda(char bank_id, Func lambda) {
  char old_bank = get_prg_8000();
  set_prg_8000(bank_id);
  lambda();
  set_prg_8000(old_bank);
}

class ScopedBank {
  u8 old_bank;

public:
  ScopedBank(u8 bank) {
    old_bank = get_prg_8000();
    set_prg_8000(bank);
  };

  ~ScopedBank() { set_prg_8000(old_bank); }
};
