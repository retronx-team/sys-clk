project('sys-clk-manager', ['c', 'cpp'],
    version: '1.0.0',
    default_options: [ 'buildtype=release', 'strip=true', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

add_global_arguments('-DAPP_TITLE="sys-clk manager"', language : 'cpp')

subdir('lib/borealis/library')

source_files = files(
    'src/main.cpp',
    'src/utils.cpp',
    'src/refresh_task.cpp',

    'src/main_frame.cpp',
    'src/logo.cpp',

    'src/status_tab.cpp',
    'src/advanced_settings_tab.cpp',
    'src/app_profiles_tab.cpp',
    'src/app_profile_frame.cpp',
    'src/cheat_sheet_tab.cpp',
    'src/about_tab.cpp'
)

ipc_files = files(
    'src/ipc/pc_shim/client.cpp',
    'src/ipc/pc_shim/ns.cpp'
)

manager_include = include_directories(
    '../common/include'
)

sys_clk_manager = executable(
    'sys-clk-manager',
    [ source_files, borealis_files, ipc_files ],
    dependencies : borealis_dependencies,
    install: true,
    include_directories: [borealis_include, manager_include],
    cpp_args: [ '-g', '-O2', '-DBOREALIS_RESOURCES="./lib/borealis/resources/"',  '-DAPP_RESOURCES="./resources/"']
)
