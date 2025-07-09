# timing_allpaths.tcl
# ---------------------------------------------------------
# Dump **setup‑timing paths** for each PVT corner.
#    • Only paths with slack worse than `slack_limit` are kept.
#    • Path counts are capped so the run finishes in a reasonable time.
# ---------------------------------------------------------

# ⇩ Adjust these knobs to taste
set slack_limit   0.0        ;# keep paths with slack ≤ 0 ns (violations + margin)
set max_groups    20000      ;# max path groups to emit (0 = unlimited → huge!)
set max_endpoints 20000      ;# max endpoints per group (0 = unlimited)

set corners {ff tt}

source scripts/init_tech.tcl
read_db  out/croc.odb   
read_spef out/croc.spef
read_sdc  out/croc.sdc

foreach corner $corners {
    set rpt_file "allpaths_${corner}.rpt"
    file delete -force $rpt_file

    puts "Dumping all setup paths for $corner → $rpt_file"

    puts "--------------------- Short summary min ---------------------"

    report_checks -path_delay min -fields {slew cap input nets fanout} -format full_clock_expanded -corner $corner -digits 3 >> $rpt_file

    puts "--------------------- Short summary max ---------------------"

    report_checks -path_delay max -fields {slew cap input nets fanout} -format full_clock_expanded -corner $corner -digits 3 >> $rpt_file

    puts "--------------------- Detailed ---------------------"

    # Worst‑slack first, full detail, but stop when the limits are hit.
    report_checks   -path_delay  max                   \
                    -format      full_clock_expanded   \
                    -fields      {slew cap input nets fanout} \
                                        -group_count $max_groups           \
                    -endpoint_count $max_endpoints     \
                    -corner      $corner               \
                    -digits      3                     \
                    >> $rpt_file

    puts "Finished timing report for $corner (file: $rpt_file)\n"
}

puts "All timing reports completed."