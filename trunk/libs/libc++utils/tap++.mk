VTABLEFIX=VTableFix.o 

prelink.elf: $(OBJECTS) $(LIBRARIES)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@ --defsym FixupVTables__Fv=0
VTableFix.o:		VTableFix.cpp
VTableFix.cpp:		prelink.elf
	@echo "// This file is automatically generated" > VTableFix.cpp
	@echo "#include <tap.h>" >> VTableFix.cpp
	@echo "#include <vtable.h>" >> VTableFix.cpp
	@echo "" >> VTableFix.cpp
	nm $^ | grep -o -G 'D _vt.*' | sed 's/D \(.*\)/extern vtable \1;/g' >> VTableFix.cpp
	nm $^ | grep -o -G 'd _vt.*' | sed 's/d \(.*\)/#error "Local vtable \1 cannot be fixed - all classes with virtual functions must be global (declared in a .h, defined in a .cpp)"/g' >> VTableFix.cpp
	@echo "" >> VTableFix.cpp
	@echo "void FixupVTables()" >> VTableFix.cpp
	@echo "{" >> VTableFix.cpp
	@nm $^ | grep -o -G 'D _vt.*' | sed 's/D \(.*\)/\tFixupVTable( \1 );/g' >> VTableFix.cpp
	@echo "}" >> VTableFix.cpp
