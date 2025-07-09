########################################################################
#  multi_corner_power.tcl  –  brute-force power dump for every corner
#
#  HOW IT WORKS
#  ------------
#    • Put the desired corner names in the list  corners {}
#    • For each corner C:
#        1. delete   power_C.txt
#        2. append   report_power            -corner C
#        3. append   report_power -instances <every cell>  -corner C
#
#  USAGE
#    source multi_corner_power.tcl
#
#  NOTES
#    • Each corner gets its own file so you can parse them separately.
########################################################################

# set corners {ff hh hl lh ll mc sf ss tt} # most do not exist
set corners {ff tt}

source scripts/init_tech.tcl

# Load design and related files
read_verilog out/croc.v
link_design croc_chip
read_sdc out/croc.sdc
read_spef out/croc.spef

# Load the VCD file and define the simulation scope
read_vcd -scope tb_croc_soc/i_croc_soc ../vsim/test_idle_range.vcd

foreach corner $corners {

    set outfile "power_${corner}_idle_test.txt"
    file delete -force $outfile

    # 1) flat design power
    puts "Dumping flat power for corner $corner: $outfile"
    report_power -corner $corner >> $outfile

    # 2) per-instance power
    # puts "Dumping per-cell power for corner $corner …"

    # foreach cell [get_cells -hier *] {
    #     report_power -instances [get_property $cell name] -corner $corner >> $outfile
    # }
    # puts "finished $corner  (file: $outfile)\n"
}

puts "All requested corners processed."
