# Shader compilation script for BGFX
cmake_minimum_required(VERSION 3.15)

# Set the shader output directory
set(SHADER_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/shaders")

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/glsl)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/essl)
#file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/dx12)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/dx11)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/metal)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}/spirv)

find_program(SHADERC_EXECUTABLE
    NAMES shaderc
    PATHS "${CMAKE_SOURCE_DIR}/bin"
    NO_DEFAULT_PATH
)
if(NOT SHADERC_EXECUTABLE)
    message(FATAL_ERROR "shaderc not found. Make sure it's built and available in the bin directory.")
endif()

# Initialize
set(ALL_GENERATED_SHADER_FILES "")

function(compile_shader SHADER_FILE SHADER_TYPE SHADER_SHORT_TYPE)
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    set(VARYING_DEF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc")


    #TODO:
    #get_filename_component(SHADER_DIR ${SHADER_FILE} DIRECTORY)
    #set(VARYING_DEF_FILE "${SHADER_DIR}/${SHADER_NAME}.def.sc")

    if(NOT EXISTS ${VARYING_DEF_FILE})
        #message(WARNING "Varying definition file not found: ${VARYING_DEF_FILE}")
        #set(VARYING_DEF_FILE "${SHADER_DIR}/varying.def.sc")
    endif()


    set(COMMON_ARGS
        -f ${SHADER_FILE}
        -i ${BGFX_DIR}/src
        --varyingdef ${VARYING_DEF_FILE}
        --type ${SHADER_TYPE}
    )

    # Define output files with different names for vertex and fragment shaders
    set(OUTPUT_FILES
        ${SHADER_OUTPUT_DIR}/glsl/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
        ${SHADER_OUTPUT_DIR}/essl/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
        ${SHADER_OUTPUT_DIR}/dx11/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
        #${SHADER_OUTPUT_DIR}/dx12/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
        ${SHADER_OUTPUT_DIR}/metal/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
        ${SHADER_OUTPUT_DIR}/spirv/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin
    )
    add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/glsl/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform linux   --profile 420
            COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/essl/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform android --profile 100_es
            #COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/dx11/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform windows --profile s_5_0 -O 3
            #COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/dx12/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform windows --profile s_5_0 -O 3
            COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/metal/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform osx --profile metal
            COMMAND ${SHADERC_EXECUTABLE} ${COMMON_ARGS} -o ${SHADER_OUTPUT_DIR}/spirv/${SHADER_NAME}${SHADER_SHORT_TYPE}.bin --platform linux --profile spirv
            DEPENDS ${SHADER_FILE} ${VARYING_DEF_FILE}
            COMMENT "Compiling shader ${SHADER_FILE}"
            VERBATIM
        )

     # Add the output files to the global list of generated files
     set(ALL_GENERATED_SHADER_FILES ${ALL_GENERATED_SHADER_FILES} ${OUTPUT_FILES} PARENT_SCOPE)
endfunction()

# Find all shader files
file(GLOB_RECURSE SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.sc"
)

# Compile all shaders
foreach(SHADER ${SHADER_FILES})
    if(SHADER MATCHES ".*vs\\.sc$")
        compile_shader(${SHADER} "vertex" ".vs")
    elseif(SHADER MATCHES ".*fs\\.sc$")
        compile_shader(${SHADER} "fragment" ".fs")
    elseif(SHADER MATCHES ".*cs\\.sc$")
        compile_shader(${SHADER} "compute" ".cs")
    endif()
endforeach()

# Create a custom target for shader compilation
add_custom_target(
    compile_shaders ALL
    DEPENDS ${ALL_GENERATED_SHADER_FILES}
    COMMENT "Compiling all shaders"
)