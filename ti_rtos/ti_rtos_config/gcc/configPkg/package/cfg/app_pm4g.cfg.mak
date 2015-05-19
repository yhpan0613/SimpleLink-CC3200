# invoke SourceDir generated makefile for app.pm4g
app.pm4g: .libraries,app.pm4g
.libraries,app.pm4g: package/cfg/app_pm4g.xdl
	$(MAKE) -f D:\TI\CC3200_git\gcc\source-1p1-rahul\ti_rtos\ti_rtos_config/src/makefile.libs

clean::
	$(MAKE) -f D:\TI\CC3200_git\gcc\source-1p1-rahul\ti_rtos\ti_rtos_config/src/makefile.libs clean

