## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,m4g linker.cmd package/cfg/app_pm4g.om4g

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/app_pm4g.xdl
	$(SED) 's"^\"\(package/cfg/app_pm4gcfg.cmd\)\"$""\"D:/TI/CC3200_git/gcc/source-1p1-rahul/ti_rtos/ti_rtos_config/gcc/configPkg/\1\""' package/cfg/app_pm4g.xdl > $@
	-$(SETDATE) -r:max package/cfg/app_pm4g.h compiler.opt compiler.opt.defs
