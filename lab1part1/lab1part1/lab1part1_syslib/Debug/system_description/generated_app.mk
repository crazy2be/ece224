# generated_app.mk
#
# Machine generated for a CPU named "cpu_0" as defined in:
# N:\ece224\myESystem\ANT\my_controller.ptf
#
# Generated: 2014-10-14 12:29:57.043

# DO NOT MODIFY THIS FILE
#
#   Changing this file will have subtle consequences
#   which will almost certainly lead to a nonfunctioning
#   system. If you do modify this file, be aware that your
#   changes will be overwritten and lost when this file
#   is generated again.
#
# DO NOT MODIFY THIS FILE

# assuming the Quartus project directory is the same as the PTF directory
QUARTUS_PROJECT_DIR = N:/ece224/myESystem/ANT

# the simulation directory is a subdirectory of the PTF directory
SIMDIR = $(QUARTUS_PROJECT_DIR)/my_controller_sim

DBL_QUOTE := "



verifysysid: dummy_verifysysid_file
.PHONY: verifysysid

dummy_verifysysid_file:
	nios2-download $(JTAG_CABLE)                                --sidp=0x01001150 --id=0 --timestamp=1410966645
.PHONY: dummy_verifysysid_file


generated_app_clean:
.PHONY: generated_app_clean
