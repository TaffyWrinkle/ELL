#
# cmake file for Embedded Learning Library subprojects
#

add_subdirectory(apply)
add_subdirectory(compile)
add_subdirectory(datasetFromImages)
add_subdirectory(debugCompiler)
add_subdirectory(finetune)
add_subdirectory(makeExamples)
add_subdirectory(optimizer)
add_subdirectory(pitest)
add_subdirectory(print)
add_subdirectory(profile)
add_subdirectory(pythonlibs)
add_subdirectory(pythonPlugins)
add_subdirectory(remoterun)

add_custom_target(tools)
add_dependencies(tools apply compile debugCompiler finetune print profile pythonPlugins)
