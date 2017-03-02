project('kissplayer', 'c', 'cpp', version: '0.7.0')

PREFIX = get_option('prefix')
LOCALEDIR = join_paths(PREFIX, get_option('localedir'))
VERSION = meson.project_version()

cc = meson.get_compiler('cpp')

cpp_args = [
    '-DLOCALEDIR="@0@"'.format(LOCALEDIR),
    '-DVERSION="@0@"'.format(VERSION),
    '-DPACKAGE_BUGREPORT="andre42m@gmail.com"',
    '-DPACKAGE_URL="https://github.com/andreldm/kissplayer"'
]

deps = [
    cc.find_library('X11'),
    cc.find_library('Xext'),
    cc.find_library('Xft'),
    cc.find_library('Xtst'),
    cc.find_library('Xpm'),
    cc.find_library('Xinerama'),
    cc.find_library('fontconfig'),
    cc.find_library('pthread'),
    cc.find_library('dl'),
    cc.find_library('fmodex'),
    cc.find_library('z'),
    cc.find_library('png'),
    cc.find_library('sqlite3'),
    cc.find_library('tag'),
    cc.find_library('fltk'),
    cc.find_library('fltk_images'),
    cc.find_library('curl'),
    dependency('sigc++-2.0')
]

sources = [
    'src/configuration.cpp',
    'src/dao.cpp',
    'src/images.cpp',
    'src/locale.cpp',
    'src/lyrics_fetcher.cpp',
    'src/main.cpp',
    'src/music.cpp',
    'src/playlist.cpp',
    'src/signals.cpp',
    'src/sound.cpp',
    'src/sync.cpp',
    'src/tinycthread.c',
    'src/util.cpp',
    'src/window_about.cpp',
    'src/window_loading.cpp',
    'src/window_main.cpp',
    'src/window_settings.cpp',
    'src/linux/linux_specific.cpp',
    'src/widget/ksp_browser.cpp',
    'src/widget/ksp_check_button.cpp',
    'src/widget/ksp_menu_item.cpp',
    'src/widget/ksp_slider.cpp',
    'src/widget/ksp_tile.cpp',
    'src/widget/ksp_volume_controller.cpp'
]

executable('kissplayer', sources: sources, dependencies: deps, cpp_args: cpp_args, install: true)

subdir('data')
subdir('po')
