desktop_data = configuration_data()
desktop_data.set('PACKAGE_NAME', 'KISS Player')
desktop_data.set('PACKAGE', 'kissplayer')
desktop_data.set('VERSION', '$VERSION')
configure_file(input: 'kissplayer.desktop.in', output: 'kissplayer.desktop', configuration: desktop_data)

meson.add_install_script('meson_post_install.sh')

install_data('kissplayer.desktop', install_dir: 'share/applications')
install_data('icons/hicolor/48x48/apps/kissplayer.png', install_dir: 'share/icons/hicolor/48x48/apps')
install_data('icons/hicolor/64x64/apps/kissplayer.png', install_dir: 'share/icons/hicolor/64x64/apps')
install_data('icons/hicolor/scalable/apps/kissplayer.svg', install_dir: 'share/icons/hicolor/scalable/apps')

