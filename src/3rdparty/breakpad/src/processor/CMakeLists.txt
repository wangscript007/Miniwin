

set(SRCS_PROC               
        address_map-inl.h           address_map.h
        basic_code_module.h         basic_code_modules.cc     
        basic_code_modules.h
        basic_source_line_resolver.cc     
        basic_source_line_resolver_types.h 
        binarystream.cc          binarystream.h 
        call_stack.cc            cfi_frame_info-inl.h 
        cfi_frame_info.cc        cfi_frame_info.h 
        contained_range_map-inl.h        contained_range_map.h 
        disassembler_x86.cc         disassembler_x86.h 
        exploitability.cc           exploitability_linux.cc 
        exploitability_linux.h      exploitability_win.cc 
        exploitability_win.h        fast_source_line_resolver.cc 
        fast_source_line_resolver_types.h 
        linked_ptr.h              logging.cc         logging.h         
        map_serializers-inl.h     map_serializers.h 
        minidump.cc               minidump_processor.cc 
        module_comparer.cc        module_comparer.h 
        module_factory.h 
        module_serializer.cc        module_serializer.h 
        pathname_stripper.cc        pathname_stripper.h 
        postfix_evaluator-inl.h     postfix_evaluator.h 
        process_state.cc 
        range_map-inl.h             range_map.h 
        simple_serializer-inl.h     simple_serializer.h 
        simple_symbol_supplier.cc   simple_symbol_supplier.h 
        source_line_resolver_base.cc 
        source_line_resolver_base_types.h 
        stack_frame_cpu.cc          stack_frame_symbolizer.cc 
        stackwalker.cc 
        stackwalker_address_list.cc stackwalker_address_list.h 
        stackwalker_amd64.cc        stackwalker_amd64.h 
        stackwalker_arm.cc          stackwalker_arm.h 
        stackwalker_arm64.cc        stackwalker_arm64.h 
        stackwalker_mips.cc         stackwalker_mips.h 
        stackwalker_ppc.cc          stackwalker_ppc.h 
        stackwalker_ppc64.cc        stackwalker_ppc64.h 
        stackwalker_selftest.cc 
        stackwalker_sparc.cc        stackwalker_sparc.h 
        stackwalker_x86.cc          stackwalker_x86.h 
        static_address_map-inl.h    static_address_map.h 
        static_contained_range_map-inl.h 
        static_contained_range_map.h 
        static_map-inl.h            static_map.h 
        static_map_iterator-inl.h   static_map_iterator.h 
        static_range_map-inl.h      static_range_map.h 
        synth_minidump.cc           synth_minidump.h 
        tokenize.cc                 tokenize.h
        windows_frame_info.h
)

add_library(processor STATIC ${SRCS_PROC})

#add_executable(minidump_dump minidump_dump.cc $<TARGET_OBJECTS:common>)
#link_libraries(minidump_dump processor )

#add_executable(minidump_stackwalk minidump_stackwalk.cc ${SRCS_PROC} $<TARGET_OBJECTS:common>)
#link_libraries(minidump_stackwalk processor)

