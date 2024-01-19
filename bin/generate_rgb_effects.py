#!/usr/bin/env python3
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

import datetime
import os
import re
from pathlib import Path

qmk_firmware_dir = Path(os.path.realpath(__file__)).parent.parent / 'qmk_firmware'

rgbmatrix_files = list((qmk_firmware_dir / 'quantum' / 'rgb_matrix').glob('**/*.h'))
rgblight_files = list((qmk_firmware_dir / 'quantum' / 'rgblight').glob('**/*.h'))

rgbmatrix_list = []
rgbmatrix_pattern = re.compile(r'#\s*ifdef\s+(ENABLE_([A-Z_]+))')
for f in rgbmatrix_files:
    for m in rgbmatrix_pattern.finditer(f.read_text()):
        rgbmatrix_list.append(f'#    define {m.group(1)}')
rgbmatrix_list = '\n'.join(sorted(set(rgbmatrix_list)))

rgblight_list = []
rgblight_pattern = re.compile(r'#\s*ifdef\s+(RGBLIGHT_EFFECT_([A-Z_]+))')
for f in rgblight_files:
    for m in rgblight_pattern.finditer(f.read_text()):
        rgblight_list.append(f'#    define {m.group(1)}')
rgblight_list = '\n'.join(sorted(set(rgblight_list)))

this_year = datetime.date.today().year

print(f"""\
// Copyright 2018-{this_year} Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#ifdef RGB_MATRIX_ENABLE
{rgbmatrix_list}
#endif // RGB_MATRIX_ENABLE

#ifdef RGBLIGHT_ENABLE
{rgblight_list}
#endif // RGBLIGHT_ENABLE
""")
