old_format() {
    cat <<__EOF__
disco_f723!alternates/disco_f723!handwired/onekey/disco_f723!console
onekey_h743!alternates/nucleo144_h743zi!handwired/onekey/nucleo144_h743zi!console
onekey_l152!alternates/nucleo64_l152re!handwired/onekey/nucleo64_l152re!console
onekey_l082!alternates/nucleo32_l082kz!handwired/onekey/nucleo32_l082!console
split_l082!alternates/nucleo32_l082kz_split!handwired/splittest/nucleo32_l082!default
test_proton_c!alternates/proton_c_test!handwired/onekey/proton_c_test!console
split_proton_c!alternates/proton_c_split!handwired/splittest/proton_c_split!default!handwired/splittest/proton_c_split/a9
split_g431!alternates/g431_split!handwired/splittest/g431_split!default
split_f072!alternates/f072_split!handwired/splittest/f072_split!default
split_f401!alternates/f401_split!handwired/splittest/f401_split!default
split_f411!alternates/f411_split!handwired/splittest/f411_split!default
spi_eeprom_promicro!alternates/spi_eeprom_test/promicro_test!handwired/onekey/spi_eeprom_test_promicro!eep_rst
spi_eeprom_proton_c!alternates/spi_eeprom_test/proton_c_test!handwired/onekey/spi_eeprom_test_proton_c!eep_rst
spi_eeprom_f103!alternates/spi_eeprom_test/f103_test!handwired/onekey/spi_eeprom_test_f103!eep_rst
spi_eeprom_f401!alternates/spi_eeprom_test/f401_test!handwired/onekey/spi_eeprom_test_f401!eep_rst
l151x8xxa!alternates/l151x8xxa!l151x8xxa!reset
f072_shiftreg!alternates/f072_shiftreg!f072_shiftreg!default
l412_shiftreg!alternates/l412_shiftreg!l412_shiftreg!default
qp_test!alternates/qp_test!qp_test!default
lvgl_test!alternates/lvgl_test!lvgl_test!default
__EOF__
}

reformat_line() {
    local name=$(echo "$1" | cut -d'!' -f1)
    local src_path=$(echo "$1" | cut -d'!' -f2)
    local tgt_path=$(echo "$1" | cut -d'!' -f3)
    local keymap=$(echo "$1" | cut -d'!' -f4)
    local build_target=$(echo "$1" | cut -d'!' -f5)
    echo $name $src_path $tgt_path $keymap $build_target
}

{
    reformat_line 'name!source!target!keymap!build'
    reformat_line '-------!-------!-------!-------!-------'
    old_format | while read line; do
        reformat_line "$line"
    done
} | column -t
