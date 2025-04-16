project('raster-rise', 'cpp',
    version : '0.0',
    default_options : ['cpp_std=c++23'])

# Define project source and include directories
inc_dir = [include_directories('include')]

external_includes = [
    include_directories('external/stb', is_system : true),
    include_directories('external/xsimd/include', is_system : true),
]

sources = []

subdir('src')

# Define the executable
executable('raster-rise', sources, include_directories : inc_dir + external_includes,
)