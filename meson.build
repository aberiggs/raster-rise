project('raster-rise', 'cpp',
    version : '0.0',
    default_options : ['cpp_std=c++23'])

# Define project source and include directories
inc_dir = [include_directories('include')]

external_includes = [
    include_directories('external/stb', is_system : true),
    include_directories('external/xsimd/include', is_system : true),
    include_directories('external/Tracy/public', is_system : true),
]

sources = [
    'external/tracy/public/TracyClient.cpp'
]

# Automatically gather all source files in the src directory
run_command('python3', 'scripts/gen_sources.py', check: true)
subdir('src') # This is where the generated sources will be placed

# Add the macro flag
cpp_args = ['-DTRACY_ENABLE']

# Define the executable
executable('raster-rise', sources, 
    include_directories : inc_dir + external_includes,
    cpp_args : cpp_args,
)