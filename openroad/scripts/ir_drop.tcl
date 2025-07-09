source scripts/init_tech.tcl

read_db out/croc.odb
read_sdc out/croc.sdc

set_pdnsim_net_voltage -net VDD -voltage 1.2
analyze_power_grid -vsrc src/Vsrc_croc_vdd.loc -net VDD -corner tt


gui::set_display_controls "Heat Maps/IR Drop" visible true
gui::set_heatmap IRDrop Layer Metal1
gui::set_heatmap IRDrop ShowLegend 1


#set_pdnsim_net_voltage -net VSS -voltage 0
#analyze_power_grid -vsrc src/Vsrc_croc_vss.loc -net VSS -corner tt