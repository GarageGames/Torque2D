include(MakeTargetDirs)
 
# copyFiles(target
#	[ALL]
#	COMMAND <cmd>
#	[ARGUMENTS <arg1> ... <argN>]
#	[OUTPUT_DIR <dir>]
#	[FORMAT <ext>]
#	FILES <source1> ... <sourceN>
#	[OUTPUT_FILES <outputVar>]
# )
function(copyFiles Target)
	CMAKE_PARSE_ARGUMENTS(
		ARGS
		"ALL"
		"OUTPUT_DIR;OUTPUT_FILES;INPUT_DIR;ARGUMENTS"
		"FILES"
		${ARGN}
	)
 
	# Argument flags.
 
	if (ARGS_ALL)
		set(ARGS_ALL "ALL")
	else()
		set(ARGS_ALL)
	endif()
 
	# Argument options.
 
	if (ARGS_OUTPUT_DIR)
		file(TO_CMAKE_PATH ${ARGS_OUTPUT_DIR} ARGS_OUTPUT_DIR)
	endif()
 
	foreach(SrcFile ${ARGS_FILES})
		set(DstFile ${SrcFile})

		# Set src filename
 
		# Set destination filename to absolute.
		if (ARGS_INPUT_DIR)
			set(RealSrcFile ${ARGS_INPUT_DIR}/${SrcFile})
		else()
			set(RealSrcFile ${SrcFile})
		endif()
 
		# Set destination filename to absolute.
		if (ARGS_OUTPUT_DIR)
			get_filename_component(DstFile ${DstFile} NAME)
			set(DstFile ${ARGS_OUTPUT_DIR}/${DstFile})
		else()
			set(DstFile ${OUTPUT_DIR}/${DstFile})
		endif()
 
		get_filename_component(SrcFileFull ${RealSrcFile} ABSOLUTE)
 
		# Command needs existence of target directories.
		makeTargetDirs(${DstFile})

		#message("COPY ${SrcFileFull} TO ${DstFile} DEPENDS ${RealSrcFile}")
 
		add_custom_command(
			OUTPUT ${DstFile}
			COMMAND cp ${SrcFileFull} ${DstFile}
			DEPENDS ${RealSrcFile}
			VERBATIM
		)
 
		list(APPEND DstFiles ${DstFile})
	endforeach()
 
	if(ARGS_OUTPUT_FILES)
		set(${ARGS_OUTPUT_FILES} ${${ARGS_OUTPUT_FILES}} ${DstFiles} PARENT_SCOPE)
	else()
		add_custom_target(${Target} ${ARGS_ALL} DEPENDS ${DstFiles})
	endif()
endfunction()
 
